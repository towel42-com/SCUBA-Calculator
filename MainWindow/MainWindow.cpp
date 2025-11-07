#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Calculators/Core/SCUBACalculator.h"
#include "Calculators/Core/SCUBACalculatorPage.h"
#include "Calculators/Core/Formula.h"
#include "Calculators/Core/GeneratedFormulaData.h"

#include "T42-MathJaxQt6/include/MathJaxQt6.h"
#include "SABUtils/utils.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSettings>
#include <QMessageBox>
#include <QButtonGroup>
#include <QResizeEvent>
#include <QRegularExpression>
#include <QFileDialog>
#include <QProgressDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QTimer>

#include <set>
#include <libloaderapi.h>
#include "SABUtils/FileUtils.h"

static QString toString( EFormulaType formulaType )
{
    if ( formulaType == EFormulaType::eBaseFormula )
        return QObject::tr( "BaseFormula" );
    else if ( formulaType == EFormulaType::eCurrentFormula )
        return QObject::tr( "CurrentFormula" );
    else if ( formulaType == EFormulaType::eCurrentValueFormula )
        return QObject::tr( "CurrentValueFormula" );
    return QObject::tr( "Unknown" );
}

CMainWindow::CMainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    fImpl( new Ui::CMainWindow )
{
    setWindowIcon( QIcon( ":/resources/scubacalc.png" ) );
    setAttribute( Qt::WA_DeleteOnClose );

    fImpl->setupUi( this );
    initMathJaxWidgets();

    fBlankPage = new QWidget;
    fImpl->stackedWidget->addWidget( fBlankPage );
    fImpl->stackedWidget->installEventFilter( this );

    connect(
        fImpl->formulaGroupBox, &NTowel42::CMathJaxQt6GroupBox::sigErrorMessage,
        [ = ]( const QString &msg )
        {
            fImpl->formulaGroupBox->setMathJaxVisible( false );
            QMessageBox::critical( this, tr( "Error in MathJax Engine" ), msg );
        } );

    connect( fImpl->reset, &QPushButton::clicked, this, &CMainWindow::slotResetCurrentPage );
    loadSettings();

    connect( fImpl->whichCalculator, &QTreeWidget::currentItemChanged, this, &CMainWindow::slotSelectCalculator );
    connect( fImpl->imperial, &QRadioButton::toggled, this, &CMainWindow::slotUnitsChanged );
    connect( fImpl->metric, &QRadioButton::toggled, this, &CMainWindow::slotUnitsChanged );
    connect( fImpl->seaWater, &QRadioButton::toggled, this, &CMainWindow::slotWaterChanged );
    connect( fImpl->freshWater, &QRadioButton::toggled, this, &CMainWindow::slotWaterChanged );

    connect(
        fImpl->menuFile, &QMenu::aboutToShow,   //
        [ this ]()   //
        {
#ifndef _DEBUG
            fImpl->actionGenerateAllFormulas->setVisible( false );
            fImpl->actionGenerateUpdatedFormulas->setVisible( false );
            //fImpl->menuSeparator->setVisible( false );
#endif
        } );

    connect( fImpl->actionGenerateAllFormulas, &QAction::triggered, this, &CMainWindow::slotGenerateAllFormulas );
    connect( fImpl->actionGenerateUpdatedFormulas, &QAction::triggered, this, &CMainWindow::slotGenerateUpdatedFormulas );

    QTimer::singleShot( 100, [ = ] { loadCalculators(); } );
}

void CMainWindow::initMathJaxWidgets()
{
    fRenderingEngine = fImpl->formulaGroupBox->engine();
    fImpl->formulaGroupBox->setTitle( {} );

    fImpl->formulaGroupBox->slotSetAutoUpdateMinimumParentHeight( true );
    fImpl->formulaGroupBox->slotHideEmptyOrInvalid( true );
    fImpl->formulaGroupBox->slotSetAutoSizeToParentWidth( true );
    fImpl->formulaGroupBox->updateMathJaxWidgetName();
}

CMainWindow::~CMainWindow()
{
}

void CMainWindow::loadSettings()
{
    QSettings settings;
    if ( settings.value( "ImperialUnits", true ).toBool() )
        fImpl->imperial->setChecked( true );
    else
        fImpl->metric->setChecked( true );
    if ( settings.value( "SeaWater", true ).toBool() )
        fImpl->seaWater->setChecked( true );
    else
        fImpl->freshWater->setChecked( true );
}

void CMainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue( "ImperialUnits", fImpl->imperial->isChecked() );
    settings.setValue( "SeaWater", fImpl->seaWater->isChecked() );
}

void CMainWindow::loadCalculators()
{
    auto calcDir = QApplication::applicationDirPath() + "/Calculators";

    auto ii = QDirIterator( calcDir, QStringList() << "*.dll" );
    while ( ii.hasNext() )
    {
        auto dllName = ii.next();
        auto fi = QFileInfo( dllName );
        auto baseName = fi.baseName();
        bool isDebugDLL = baseName.endsWith( "d" );
#ifdef _DEBUG
        if ( !isDebugDLL )
#else
        if ( isDebugDLL )
#endif
            continue;

        if ( !QFileInfo( dllName ).isFile() )
            continue;
        auto hLib = ::LoadLibrary( (LPCWSTR)dllName.utf16() );
        if ( !hLib )
        {
            auto lastError = NSABUtils::getLastError();   // windows only
            QMessageBox::critical( this, tr( "Could not load Calculator" ), tr( "Loading calculator '%1' failed with error:<br/> %2" ).arg( dllName ).arg( lastError ) );
            continue;
        }

        auto constructor = (TInstantiateCalcFunc)GetProcAddress( hLib, kInstantiateCalcFuncName );
        if ( !constructor )
            continue;

        auto calculator = constructor();
        addCalculator( calculator );

        if ( calculator->isReversible() )
        {
            auto reversedCalc = constructor();
            reversedCalc->setIsReversed( calculator, true );
            addCalculator( reversedCalc );
        }
    }
    fImpl->whichCalculator->expandAll();
    fImpl->whichCalculator->sortByColumn( 0, Qt::SortOrder::AscendingOrder );
    fImpl->whichCalculator->resizeColumnToContents( 0 );
    auto colWidth = fImpl->whichCalculator->columnWidth( 0 );
    fImpl->whichCalculator->setMinimumWidth( colWidth + 20 );

    slotSelectCalculator( nullptr );
}

void CMainWindow::addCalculator( CSCUBACalculator *calculator )
{
    auto path = calculator->calculatorPath();
    if ( path.isEmpty() )
        return;

    calculator->initResources();

    auto calculatorName = calculator->calculatorName();
    path.push_back( calculatorName );
    auto leaf = findItem( fImpl->whichCalculator->invisibleRootItem(), path, true );
    fCalculators[ leaf ] = calculator;

    auto page = calculator->getPage( nullptr );
    if ( !page )
    {
        //qCDebug( ScubaCalculator ).noquote().nospace() << "No widget for page :" << path;
        return;
    }

    fImpl->stackedWidget->addWidget( page );
    fPageToItem[ page ] = leaf;

    calculator->setUpdateFormulaFunc( [ = ]( CSCUBACalculatorPage *calcPage, const QString &formula, bool finished )   //
                                      {   //
                                          this->setFormulaForPage( calcPage, formula, finished );
                                      } );
}

QString pathForItem( QTreeWidgetItem *rootItem, QTreeWidgetItem *item )
{
    if ( !item )
        return {};

    auto parentItem = item->parent();

    QString retVal;
    if ( parentItem && ( parentItem != rootItem ) )
        retVal = pathForItem( rootItem, parentItem );

    if ( !retVal.isEmpty() )
        retVal += ".";
    retVal += item->text( 0 );
    return retVal;
}

QTreeWidgetItem *CMainWindow::findItem( QTreeWidgetItem *parent, const QStringList &path, bool createIfNecessary )
{
    if ( !parent )
        return nullptr;
    QTreeWidgetItem *foundChild = nullptr;
    for ( int ii = 0; !foundChild && ( ii < parent->childCount() ); ++ii )
    {
        auto child = parent->child( ii );
        if ( !child )
            continue;
        if ( child->text( 0 ) == path.front() )
            foundChild = child;
    }
    if ( !foundChild )
    {
        if ( createIfNecessary )
        {
            foundChild = new QTreeWidgetItem( parent );
            foundChild->setText( 0, path.front() );
            qDebug() << "Creating Item: " << pathForItem( fImpl->whichCalculator->invisibleRootItem(), foundChild );
        }
    }
    if ( path.length() == 1 )
        return foundChild;

    return findItem( foundChild, path.mid( 1 ), createIfNecessary );
}

CSCUBACalculator *CMainWindow::getCalculator( QTreeWidgetItem *leaf ) const
{
    auto pos = fCalculators.find( leaf );
    if ( pos != fCalculators.end() )
        return ( *pos ).second;
    return nullptr;
}

CSCUBACalculator *CMainWindow::getCalculator( QWidget *page ) const
{
    auto pos = fPageToItem.find( page );
    if ( pos == fPageToItem.end() )
        return nullptr;
    return getCalculator( ( *pos ).second );
}

CSCUBACalculator *CMainWindow::currentCalculator() const
{
    auto page = fImpl->stackedWidget->currentWidget();
    if ( page == fBlankPage )
        return nullptr;

    return getCalculator( page );
}

CSCUBACalculatorPage *CMainWindow::currentCalculatorPage() const
{
    auto page = fImpl->stackedWidget->currentWidget();
    if ( page == fBlankPage )
        return nullptr;

    return dynamic_cast< CSCUBACalculatorPage * >( page );
}

void CMainWindow::slotUnitsChanged()
{
    auto page = fImpl->stackedWidget->currentWidget();
    if ( page == fBlankPage )
        return;

    saveSettings();
    auto item = getItemForPage( page );
    if ( !item )
        return;

    auto calc = getCalculator( item );
    if ( !calc )
        return;

    calc->setImperial( fImpl->imperial->isChecked() );
}

void CMainWindow::slotWaterChanged()
{
    auto page = fImpl->stackedWidget->currentWidget();
    if ( page == fBlankPage )
        return;

    saveSettings();
    auto item = getItemForPage( page );
    if ( !item )
        return;

    auto calc = getCalculator( item );
    if ( !calc )
        return;
    calc->setSeaWater( fImpl->seaWater->isChecked() );
}

void CMainWindow::slotSelectCalculator( QTreeWidgetItem *item )
{
    auto calculator = getCalculator( item );
    auto page = dynamic_cast< CSCUBACalculatorPage * >( calculator ? calculator->getPage( nullptr ) : nullptr );
    bool needsInit = page ? page->needsInit() : false;

    if ( page )
    {
        fImpl->pageName->setText( calculator->calculatorName() );
    }
    else
    {
        fImpl->pageName->setText( "Please Select a Calculator" );
    }

    setCurrentPage( item, page, needsInit );
}

void CMainWindow::setCurrentPage( QTreeWidgetItem *item, CSCUBACalculatorPage *page, bool initPage )
{
    bool showUnits = page != nullptr;
    bool isWaterTypeBased = page != nullptr;
    if ( page == nullptr )
    {
        fImpl->stackedWidget->setCurrentWidget( fBlankPage );
    }
    else
    {
        if ( !initPage )
            fImpl->stackedWidget->setCurrentWidget( page );
        showUnits = page->property( "showUnits" ).toBool();
        isWaterTypeBased = page->property( "isWaterTypeBased" ).toBool();
    }

    if ( page && initPage )
    {
        auto calc = getCalculator( item );
        if ( !calc )
            return;
        loadCacheForCalc( calc );
        fImpl->stackedWidget->setCurrentWidget( page );
        calc->init( fImpl->imperial->isChecked(), fImpl->seaWater->isChecked() );
    }

    this->showUnits( showUnits );
    this->showWaterType( isWaterTypeBased );
    fImpl->reset->setVisible( page != nullptr );
    loadFormulasForPage( page );
}

void CMainWindow::showUnits( bool show )
{
    fImpl->unitGroupBox->setVisible( show );
}

void CMainWindow::showWaterType( bool show )
{
    fImpl->waterGroupBox->setVisible( show );
}

QTreeWidgetItem *CMainWindow::getItemForPage( QWidget *page ) const
{
    auto pos = fPageToItem.find( page );
    if ( pos != fPageToItem.end() )
        return ( *pos ).second;
    return nullptr;
}

void CMainWindow::setMathJaxWidgetsVisible( bool visible )
{
    fImpl->formulaGroupBox->setMathJaxVisible( visible );
}

void CMainWindow::setFormulaForPage( CSCUBACalculatorPage *page, const QString &formula, bool finished )
{
    //Q_ASSERT( !NUtilities::hasVariable( formula ) );

    auto widget = mathJaxGoupBox();
    Q_ASSERT( widget );
    auto pos = this->fPageToFormulasMap.find( page );
    if ( pos != fPageToFormulasMap.end() )
    {
        if ( ( *pos ).second == formula )
        {
            if ( finished )
                loadFormulasForPage( page );
            return;
        }
    }
    else
    {
        pos = fPageToFormulasMap.insert( { page, std::optional< QString >() } ).first;
    }
    ( *pos ).second = formula;
    if ( finished )
        loadFormulasForPage( page );
}

void CMainWindow::loadFormulasForPage( CSCUBACalculatorPage *page )
{
    if ( !page )
    {
        setMathJaxWidgetsVisible( false );
        return;
    }

    for ( auto &&formulaType : { EFormulaType::eBaseFormula /*, EFormulaType::eCurrentFormula, EFormulaType::eCurrentValueFormula*/ } )
    {
        auto formula = formulaForPage( page );
        mathJaxGoupBox()->setFormula( formula );
    }
}

NTowel42::CMathJaxQt6GroupBox *CMainWindow::mathJaxGoupBox() const
{
    return fImpl->formulaGroupBox;
}

std::optional< QString > CMainWindow::formulaForPage( QWidget *page )
{
    auto pos = fPageToFormulasMap.find( page );

    auto calculator = getCalculator( page );

    if ( pos == fPageToFormulasMap.end() )
    {
        //qCDebug( ScubaCalculator ).noquote().nospace() << "Page: '" << calculator->calculatorName() << "' has no " << toString( formulaType ) << " formula.";
        return {};
    }
    auto retVal = ( *pos ).second;
    //qCDebug( ScubaCalculator ).noquote().nospace() << "Page: '" << calculator->calculatorName() << "' " << toString( formulaType ) << " formula is '" << retVal << "'";
    return retVal;
}

void CMainWindow::slotResetCurrentPage()
{
    auto page = fImpl->stackedWidget->currentWidget();
    if ( page == fBlankPage )
        return;

    auto calc = getCalculator( page );
    if ( !calc )
        return;
    calc->resetVariables();
}

void CMainWindow::slotGenerateAllFormulas()
{
    generateFormulas( false );
}

void CMainWindow::slotGenerateUpdatedFormulas()
{
    generateFormulas( true );
}

static constexpr int kMAX_TO_GENERATE = -1;

std::pair< std::size_t, std::size_t > CMainWindow::computeTotals( QProgressDialog *progress, TFormulaMap &allFormulas ) const
{
    std::size_t numToBeRendered{ 0 };
    std::size_t totalFormulas{ 0 };
    for ( auto &&ii : fCalculators )
    {
        if ( progress->wasCanceled() )
            break;

        auto label = QString( "Generating SVGs for calculator:<br/>%1<br/>Calculator %2 of %3" ).arg( ii.second->calculatorName() ).arg( progress->value() + 1 ).arg( fCalculators.size() );
        progress->setLabelText( label );
        progress->setValue( progress->value() + 1 );
        qApp->processEvents();

        const auto &formulaData = ii.second->getAllFormulas( [ = ]( const QString &formula ) { return fRenderingEngine->beenCreated( formula ); } );
        if ( !formulaData )
            continue;

        allFormulas[ ii.second ] = formulaData;

        numToBeRendered += formulaData->numToRender();
        totalFormulas += formulaData->numTotal();
    }

    return { totalFormulas, numToBeRendered };
}

std::size_t CMainWindow::generateSVGs( QProgressDialog *progress, const TFormulaMap &allFormulas, std::size_t totalFormulas, std::size_t numToBeRendered ) const
{
    progress->setLabelText( tr( "Generating SVGs" ) );
    progress->setValue( 0 );
    progress->setMaximum( (int)numToBeRendered );

    std::size_t numErrors = 0;
    fRenderingEngine->blockSignals( true );
    int formulaNum = 0;
    for ( auto &&currFormulaData : allFormulas )
    {
        if ( progress->wasCanceled() )
            break;
        for ( auto &&ii : currFormulaData.second->formulaList() )
        {
            if ( progress->wasCanceled() )
                break;

            bool beenCreated = fRenderingEngine->beenCreated( ii->formula() );
            if ( !beenCreated )
            {
                progress->setValue( progress->value() + 1 );
                qDebug() << "Equation: " << ii->name() << " requires rendering.";
            }
            qApp->processEvents();

            QJsonObject obj;
            obj.insert( "name", QJsonValue::fromVariant( ii->name() ) );
            obj.insert( "formula", QJsonValue::fromVariant( ii->formula() ) );

            auto label = QString( "Generating SVG for formula:<br/>%1<br/>Rendering Formula %2 of %3<br/>Current Formula: %4 of %5<br/>Number of Errors: %6" ).arg( ii->name() ).arg( progress->value() ).arg( numToBeRendered ).arg( ++formulaNum ).arg( totalFormulas ).arg( numErrors );
            progress->setLabelText( label );

            obj.insert( "num", QJsonValue::fromVariant( formulaNum ) );

            std::optional< QByteArray > objSVG;

            if ( ( kMAX_TO_GENERATE == -1 ) || progress->value() <= kMAX_TO_GENERATE )
            {
                bool hasError = false;
                fRenderingEngine->renderSVG(
                    ii->formula(),   //
                    [ =, &obj, &objSVG ]( const QString &texCode, const std::optional< QByteArray > &svg )   //
                    {
                        if ( ii->formula() != texCode )   // only do it once, ignore the cleanedMessage
                            return;

                        if ( !svg.has_value() )
                        {
                            obj.insert( "error", QJsonValue::fromVariant( "ERROR: Unknown error occurred in the SVG generation" ) );
                            return;
                        }

                        objSVG = svg;
                    },   //
                    [ =, &obj, &hasError ]( const QString &errorMessage )
                    {
                        //qCDebug( ScubaCalculator ).noquote().nospace() << tr( "Error Generating SVG: %1: %2" ).arg( ii->name() ).arg( errorMessage );
                        obj.insert( "error", QJsonValue::fromVariant( "ERROR: " + errorMessage ) );
                        hasError = true;
                    }   //
                );
                if ( hasError )
                    numErrors++;

                currFormulaData.second->addSVG( obj, objSVG, fRenderingEngine->renderedDate( ii->formula() ) );
            }
        }
    }
    fRenderingEngine->blockSignals( false );
    return numErrors;
}

void CMainWindow::saveJSONFiles( QProgressDialog *progress, const QDir &dir, const TFormulaMap &allFormulas, bool needUpdatingOnly ) const
{
    auto regExp = QRegularExpression( R"__([\/\\\?\*\:\"\<\>\|])__" );
    std::size_t numFiles = 0;
    for ( auto &&ii : allFormulas )
    {
        auto calc = ii.first;
        if ( needUpdatingOnly && !ii.second->updated() )
            continue;

        std::size_t num = 1;
        auto numArrays = ii.second->jsonArrays().size();
        for ( auto &&jj : ii.second->jsonArrays() )
        {
            numFiles++;
        }
    }

    progress->setLabelText( tr( "Generating JSON Files" ) );
    progress->setValue( 0 );
    progress->setMaximum( (int)numFiles );
    for ( auto &&ii : allFormulas )
    {
        auto calc = ii.first;
        if ( needUpdatingOnly && !ii.second->updated() )
            continue;

        std::size_t num = 1;
        auto numArrays = ii.second->jsonArrays().size();
        for ( auto &&jj : ii.second->jsonArrays() )
        {
            auto jsonFileNameBase = QString( "%1-formulas" ).arg( calc->calculatorName() );

            if ( numArrays > 1 )
                jsonFileNameBase += QString( "-%1" ).arg( (int)num++, 2, 10, QChar( '0' ) );
            jsonFileNameBase += ".json";

            jsonFileNameBase.replace( regExp, "_" );

            auto subDir = QDir( QDir( dir ).absoluteFilePath( QString( "%1/%2/formulas" ).arg( ii.first->calculatorGroupName() ).arg( ii.first->calculatorProjectName() ) ) );
            if ( !subDir.exists() )
            {
                if ( !subDir.mkpath( "." ) )
                {
                    QMessageBox::critical( const_cast< CMainWindow * >( this ), tr( "Could not create Directory" ), tr( "Could not create directory %1" ).arg( subDir.absolutePath() ) );
                    return;
                }
            }

            auto jsonFileName = subDir.absoluteFilePath( jsonFileNameBase );

            auto label = QString( "Generating JSON file:<br/>%1<br/>Generating file %2 of %3" ).arg( jsonFileName ).arg( progress->value() + 1 ).arg( numFiles );
            progress->setLabelText( label );
            progress->setValue( progress->value() + 1 );
            qApp->processEvents();

            QFile jsonFile( jsonFileName );
            if ( !jsonFile.open( QFile::WriteOnly | QFile::Text | QFile::Truncate ) )
            {
                QMessageBox::critical( const_cast< CMainWindow * >( this ), tr( "Could not open file" ), tr( "Error opening file: %1<br/>%2" ).arg( jsonFileName ).arg( jsonFile.errorString() ) );
                return;
            }
            QJsonDocument doc;
            doc.setArray( jj );

            jsonFile.write( doc.toJson( QJsonDocument::Indented ) );
            jsonFile.close();
        }
    }
}

void CMainWindow::generateFormulas( bool needUpdatingOnly )
{
    (void)needUpdatingOnly;
    //loadCache();

    //bool first = true;
    //if ( first )
    //{
    //    auto defaultDir = R"(C:\Users\scott.TOWEL42\Dropbox\home\sb\SCUBA-Calculator\Calculators)";
    //    if ( !QDir( defaultDir ).exists() )
    //    {
    //        QDir( defaultDir ).mkpath( "." );
    //    }
    //    if ( QDir::current() != QDir( defaultDir ) )
    //    {
    //        QDir::setCurrent( defaultDir );
    //    }
    //    first = false;
    //}

    //auto dir = QFileDialog::getExistingDirectory( this, tr( "Select Target Directory:" ) );
    //if ( dir.isEmpty() )
    //    return;

    //TFormulaMap allFormulas;

    //auto progress = std::make_unique< QProgressDialog >( tr( "Generating SVGs" ), tr( "Abort Generation" ), 0, (int)fCalculators.size(), this );
    //progress->setAutoClose( false );
    //progress->setAutoReset( false );
    //progress->setMinimumDuration( 0 );

    //auto &&[ totalFormulas, numToBeRendered ] = computeTotals( progress.get(), allFormulas );
    //if ( progress->wasCanceled() )
    //    return;

    //auto numErrors = generateSVGs( progress.get(), allFormulas, totalFormulas, numToBeRendered );
    //if ( !progress->wasCanceled() && ( numErrors != 0 ) )
    //{
    //    auto retVal = QMessageBox::warning( this, tr( "Errors while Generating SVGs" ), tr( "There were %1 errors while generating the SVGs, would you like to save the non-error formulas?" ).arg( numErrors ), QMessageBox::Yes, QMessageBox::No );
    //    if ( retVal == QMessageBox::No )
    //        return;
    //}

    //if ( progress->wasCanceled() )
    //{
    //    auto retVal = QMessageBox::warning( this, tr( "Generation of SVGs Cancelled" ), tr( "Would you like to save the JSON files that finished?" ), QMessageBox::Yes, QMessageBox::No );
    //    if ( retVal == QMessageBox::No )
    //        return;
    //}

    //saveJSONFiles( progress.get(), dir, allFormulas, needUpdatingOnly );
    //if ( progress->wasCanceled() )
    //    return;

    //QMessageBox::information( this, tr( "Finished generating SVGs" ), tr( "Total Number of Formulas: %1<br/>Number needing Rendering: %2<br/>Number of Errors: %3" ).arg( totalFormulas ).arg( numToBeRendered ).arg( numErrors ) );
}

void CMainWindow::loadCacheForCalc( CSCUBACalculator *calc )
{
    if ( !calc )
        return;

    auto wildCard = QString( "%1*.json" ).arg( calc->calculatorProjectName() );
    loadCache( wildCard );
}

void CMainWindow::loadCache( std::optional< QString > wildCard )
{
    if ( !wildCard.has_value() )
        wildCard = "*.json";

    QStringList fileNames;
    QDirIterator ii( ":/formulas", QStringList() << wildCard.value() );
    while ( ii.hasNext() )
    {
        fileNames << ii.next();
    }
    fileNames.sort();
    //if ( fileNames.empty() )
    //{
    //    auto files = NSABUtils::NFileUtils::dumpResources( true );
    //    qDebug() << "Could not find JSON file:" << wildCard.value();
    //    for ( auto &&ii : files )
    //    {
    //        qDebug() << ii;
    //    }
    //}

    loadCacheFiles( fileNames );
}

void CMainWindow::loadCacheFiles( const QStringList &fileNames )
{
    if ( fileNames.isEmpty() )
        return;

    auto progress = std::make_unique< QProgressDialog >( tr( "Loading SVG Cache" ), tr( "Abort Cache Load" ), 0, fileNames.size(), this );
    progress->setAutoClose( false );
    progress->setAutoReset( false );
    progress->setMinimumDuration( 0 );
    std::size_t currSVGNum = 0;
    std::size_t totalKnownSVG = 0;
    for ( auto &&fileName : fileNames )
    {
        if ( fLoadedCacheFiles.find( fileName ) != fLoadedCacheFiles.end() )
            continue;
        fLoadedCacheFiles.insert( fileName );
        currSVGNum = ( totalKnownSVG += loadCacheFile( progress.get(), fileName, fileNames.size(), currSVGNum, totalKnownSVG ) );
    }

    qDebug() << "Total SVG Loaded:" << currSVGNum;
}

std::size_t CMainWindow::loadCacheFile( QProgressDialog *progress, const QString &fileName, std::size_t numFiles, std::size_t currSVGNum, std::size_t currTotal )
{
    if ( progress->wasCanceled() )
        return 0;
    qDebug() << "Loading into Cache: " << fileName;
    QFile fi( fileName );
    if ( !fi.open( QFile::Text | QFile::ReadOnly ) )
    {
        QMessageBox::critical( this, tr( "Could not open cache" ), tr( "Error: Problem opening cache file<br/>%1" ).arg( fi.errorString() ) );
        return 0;
    }

    QJsonParseError parseError;
    auto doc = QJsonDocument::fromJson( fi.readAll(), &parseError );
    if ( ( parseError.error != QJsonParseError::NoError ) || doc.isNull() )
    {
        QMessageBox::critical( this, tr( "Error Reading JSON" ), tr( "Error: Error in Cache File @%1:<br/> %2" ).arg( parseError.offset ).arg( parseError.errorString() ) );
        return 0;
    }

    if ( !doc.isArray() )
    {
        QMessageBox::critical( this, tr( "Error Reading JSON" ), tr( "Error: Invalid Format expected Array" ) );
        return 0;
    }

    progress->setValue( progress->value() + 1 );
    qApp->processEvents();

    auto svgCacheArray = doc.array();

    currTotal += svgCacheArray.count();
    std::size_t numLoaded = 0;

    for ( auto &&svgItem : svgCacheArray )
    {
        if ( progress->wasCanceled() )
            break;

        if ( !svgItem.isObject() )
            continue;

        currSVGNum++;
        numLoaded++;

        auto label = QString( "Loading Cache:<br/>%1<br/>Cache File %2 of %3<br/>SVG %4 of %5" ).arg( fileName ).arg( progress->value() + 1 ).arg( numFiles ).arg( currSVGNum ).arg( currTotal );
        progress->setLabelText( label );
        qApp->processEvents();

        auto svgCacheObject = svgItem.toObject();

        if ( !svgCacheObject.contains( "formula" ) || !svgCacheObject.contains( "name" ) )
        {
            QMessageBox::critical( this, tr( "Error Reading JSON" ), tr( "Error: Invalid Format expected 'formula' and 'name' fields" ) );
            return numLoaded;
        }

        if ( !svgCacheObject.contains( "svg" ) && !svgCacheObject.contains( "error" ) )
        {
            QMessageBox::critical( this, tr( "Error Reading JSON" ), tr( "Error: Invalid Format expected svg or error fields" ) );
            return numLoaded;
        }

        auto name = svgCacheObject[ "name" ].toString();
        auto formula = svgCacheObject[ "formula" ].toString();
        auto num = svgCacheObject[ "num" ].toInt();
        auto renderDate = svgCacheObject[ "renderDate" ].toVariant().toDateTime();
        std::optional< QString > cleanedFormula;
        if ( svgCacheObject.contains( "cleanedFormula" ) )
            cleanedFormula = svgCacheObject[ "cleanedFormula" ].toString();

        qDebug() << "Loading cache item" << name;
        if ( svgCacheObject.contains( "error" ) )
        {
            //qCDebug( ScubaCalculator ).noquote().nospace() << "Skipping cache item: name '" << name << "' tex '" << formula << "'";
            continue;
        }

        fRenderingEngine->addToCache( formula, cleanedFormula, renderDate, svgCacheObject[ "svg" ].toString().toUtf8(), true );
        //auto svg = svgCacheObject[ "svg" ].toString().toUtf8();
        //if ( auto result = QByteArray::fromBase64Encoding( svg ) )
        //{
        //    fRenderingEngine->addToCache( formula, cleanedFormula, renderDate, *result );
        //}
        //else
        //{
        //    QMessageBox::critical( this, tr( "Error Reading JSON" ), tr( "Error: Invalid SVG base 64" ) );
        //    return numLoaded;
        //}
    }
    return numLoaded;
}
