#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Calculators/Core/SCUBACalculator.h"
#include "Calculators/Core/SCUBACalculatorPage.h"
#include "Calculators/Core/Formula.h"

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
        fImpl->baseFormulaGroupBox, &NTowel42::CMathJaxQt6GroupBox::sigErrorMessage,
        [ = ]( const QString &msg )
        {
            fImpl->baseFormulaGroupBox->setMathJaxVisible( false );
            QMessageBox::critical( this, tr( "Error in MathJax Engine" ), msg );
        } );

    connect(
        fImpl->currFormulaGroupBox, &NTowel42::CMathJaxQt6GroupBox::sigErrorMessage,
        [ = ]( const QString &msg )
        {
            fImpl->currFormulaGroupBox->setMathJaxVisible( false );
            QMessageBox::critical( this, tr( "Error in MathJax Engine" ), msg );
        } );

    connect(
        fImpl->currFormulaValueGroupBox, &NTowel42::CMathJaxQt6GroupBox::sigErrorMessage,
        [ = ]( const QString &msg )
        {
            fImpl->currFormulaValueGroupBox->setMathJaxVisible( false );
            QMessageBox::critical( this, tr( "Error in MathJax Engine" ), msg );
        } );

    connect( fImpl->reset, &QPushButton::clicked, this, &CMainWindow::slotResetCurrentPage );
    loadSettings();
    loadCalculators();

    connect( fImpl->whichCalculator, &QTreeWidget::currentItemChanged, this, &CMainWindow::slotSelectCalculator );
    connect( fImpl->imperial, &QRadioButton::toggled, this, &CMainWindow::slotUnitsChanged );
    connect( fImpl->metric, &QRadioButton::toggled, this, &CMainWindow::slotUnitsChanged );
    connect( fImpl->seaWater, &QRadioButton::toggled, this, &CMainWindow::slotWaterChanged );
    connect( fImpl->freshWater, &QRadioButton::toggled, this, &CMainWindow::slotWaterChanged );

    connect( fImpl->actionGenerateAllFormulas, &QAction::triggered, this, &CMainWindow::slotGenerateAllFormulas );
    connect( fImpl->actionGenerateUpdatedFormulas, &QAction::triggered, this, &CMainWindow::slotGenerateUpdatedFormulas );

    slotSelectCalculator( nullptr );
    QTimer::singleShot( 100, [ = ] { loadCache(); } );
}

void CMainWindow::initMathJaxWidgets()
{
    fRenderingEngine = fImpl->baseFormulaGroupBox->engine();
    fImpl->baseFormulaGroupBox->setTitle( tr( "Base Formula" ) );

    fImpl->baseFormulaGroupBox->slotSetAutoUpdateMinimumParentHeight( true );
    fImpl->baseFormulaGroupBox->slotHideEmptyOrInvalid( true );
    fImpl->baseFormulaGroupBox->slotSetAutoSizeToParentWidth( true );
    fImpl->baseFormulaGroupBox->updateMathJaxWidgetName();

    fImpl->currFormulaGroupBox->setEngine( fRenderingEngine );
    fImpl->currFormulaGroupBox->setSubordinateTo( fImpl->baseFormulaGroupBox );
    fImpl->currFormulaGroupBox->setTitle( tr( "Current Formula" ) );
    fImpl->currFormulaGroupBox->updateMathJaxWidgetName();

    fImpl->currFormulaGroupBox->slotSetAutoUpdateMinimumParentHeight( true );
    fImpl->currFormulaGroupBox->slotHideEmptyOrInvalid( true );
    fImpl->currFormulaGroupBox->slotSetAutoSizeToParentWidth( true );

    fImpl->currFormulaValueGroupBox->setEngine( fRenderingEngine );
    fImpl->currFormulaValueGroupBox->setSubordinateTo( std::list< NTowel42::CMathJaxQt6GroupBox * >( { fImpl->baseFormulaGroupBox, fImpl->currFormulaGroupBox } ) );
    fImpl->currFormulaValueGroupBox->setTitle( tr( "Formula with Values" ) );
    fImpl->currFormulaValueGroupBox->updateMathJaxWidgetName();

    fImpl->currFormulaValueGroupBox->slotSetAutoUpdateMinimumParentHeight( true );
    fImpl->currFormulaValueGroupBox->slotHideEmptyOrInvalid( true );
    fImpl->currFormulaValueGroupBox->slotSetAutoSizeToParentWidth( true );
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

        if ( calculator->isReversable() )
        {
            auto reversedCalc = constructor();
            reversedCalc->setIsReversed( true );
            addCalculator( reversedCalc );
        }
    }
    fImpl->whichCalculator->expandAll();
    fImpl->whichCalculator->sortByColumn( 0, Qt::SortOrder::AscendingOrder );
    fImpl->whichCalculator->resizeColumnToContents( 0 );
    auto colWidth = fImpl->whichCalculator->columnWidth( 0 );
    fImpl->whichCalculator->setMinimumWidth( colWidth + 20 );
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

    calculator->setUpdateFormulaFunc( [ = ]( CSCUBACalculatorPage *calcPage, const QString &formula, EFormulaType formulaType, bool finished )   //
                                      {   //
                                          this->setFormulaForPage( calcPage, formula, formulaType, finished );
                                      } );
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
        fImpl->stackedWidget->setCurrentWidget( page );
        showUnits = page->property( "showUnits" ).toBool();
        isWaterTypeBased = page->property( "isWaterTypeBased" ).toBool();
    }

    if ( page && initPage )
    {
        auto calc = getCalculator( item );
        if ( !calc )
            return;
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
    fImpl->currFormulaValueGroupBox->setMathJaxVisible( visible );
    fImpl->currFormulaGroupBox->setMathJaxVisible( visible );
    fImpl->baseFormulaGroupBox->setMathJaxVisible( visible );
}

void CMainWindow::setFormulaForPage( CSCUBACalculatorPage *page, const QString &formula, EFormulaType formulaType, bool finished )
{
    auto regEx = QRegularExpression( R"__(\<[A-Za-z]+\>)__" );
    Q_ASSERT( !regEx.match( formula ).hasMatch() );

    auto widget = mathJaxForFormulaType( formulaType );
    Q_ASSERT( widget );
    auto pos = this->fPageToFormulasMap.find( page );
    if ( pos != fPageToFormulasMap.end() )
    {
        if ( ( *pos ).second.formula( formulaType ) == formula )
        {
            if ( finished )
                loadFormulasForPage( page );
            return;
        }
    }
    else
    {
        pos = fPageToFormulasMap.insert( { page, SFormulas() } ).first;
    }
    ( *pos ).second.setFormula( formula, formulaType );
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

    for ( auto &&formulaType : { EFormulaType::eBaseFormula, EFormulaType::eCurrentFormula, EFormulaType::eCurrentValueFormula } )
    {
        auto formula = formulaForPage( page, formulaType );
        mathJaxForFormulaType( formulaType )->setFormula( formula );
    }
}

std::optional< QString > CMainWindow::formulaForFormulaType( EFormulaType formulaType ) const
{
    auto currPage = currentCalculatorPage();
    if ( !currPage )
        return {};

    auto pos = fPageToFormulasMap.find( currPage );
    if ( pos == fPageToFormulasMap.end() )
        return {};

    return ( *pos ).second.formula( formulaType );
}

NTowel42::CMathJaxQt6GroupBox *CMainWindow::mathJaxForFormulaType( EFormulaType formulaType ) const
{
    if ( formulaType == EFormulaType::eBaseFormula )
        return fImpl->baseFormulaGroupBox;
    else if ( formulaType == EFormulaType::eCurrentFormula )
        return fImpl->currFormulaGroupBox;
    else if ( formulaType == EFormulaType::eCurrentValueFormula )
        return fImpl->currFormulaValueGroupBox;

    return nullptr;
}

std::optional< QString > CMainWindow::formulaForPage( QWidget *page, EFormulaType formulaType )
{
    auto pos = fPageToFormulasMap.find( page );

    auto calculator = getCalculator( page );

    if ( pos == fPageToFormulasMap.end() )
    {
        //qCDebug( ScubaCalculator ).noquote().nospace() << "Page: '" << calculator->calculatorName() << "' has no " << toString( formulaType ) << " formula.";
        return {};
    }
    auto retVal = ( *pos ).second.formula( formulaType );
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

void CMainWindow::generateFormulas( bool needUpdatingOnly )
{
    bool first = true;
    if ( first )
    {
        auto defaultDir = R"(C:\Users\scott.TOWEL42\Dropbox\home\sb\SCUBA-Calculator\Calculators)";
        if ( QDir::current() != QDir( defaultDir ) )
            QDir::setCurrent( defaultDir );
        first = false;
    }

    auto dir = QFileDialog::getExistingDirectory( this, tr( "Select Target Directory:" ) );
    if ( dir.isEmpty() )
        return;

    using TFormulaMap = std::unordered_map< CSCUBACalculator *, std::shared_ptr< SGeneratedFormulaData > >;
    TFormulaMap allFormulas;

    for ( auto &&ii : fCalculators )
    {
        const auto &formulaData = ii.second->getAllFormulas();
        allFormulas[ ii.second ] = formulaData;
    }

    int numToBeRendered = 0;
    int totalFormulas = 0;

    for ( auto &&ii : allFormulas )
    {
        ii.second->sortByName();

        auto &&[ lclNumTotal, lclToRender ] = ii.second->formulaCounts( [ = ]( const QString &formula ) { return fRenderingEngine->beenCreated( formula ); } );
        ii.second->fUpdated = ( lclToRender != 0 );

        numToBeRendered += lclToRender;
        totalFormulas += lclNumTotal;
    }

    fRenderingEngine->blockSignals( true );
    auto progress = new QProgressDialog( tr( "Generating SVGs" ), tr( "Abort Generation" ), 0, numToBeRendered, this );
    progress->setMinimumDuration( 1000 );
    for ( auto &&currFormulaData : allFormulas )
    {
        for ( auto &&ii : currFormulaData.second->fByNameList )
        {
            bool beenCreated = fRenderingEngine->beenCreated( ii->formula() );
            if ( !beenCreated )
            {
                progress->setMinimumDuration( 0 );
                progress->setValue( progress->value() + 1 );
            }
            if ( progress->wasCanceled() )
                break;

            QJsonObject obj;
            obj.insert( "name", QJsonValue::fromVariant( ii->name() ) );
            obj.insert( "formula", QJsonValue::fromVariant( ii->formula() ) );
            auto cleanedFormula = ii->cleanedFormula();
            if ( cleanedFormula != ii->formula() )
                obj.insert( "cleanedFormula", QJsonValue::fromVariant( cleanedFormula ) );

            auto label = QString( "Generating SVG for formula:<br/>%1<br/>%2 of %3 to be Rendered<br/>Total Formulas: %4" ).arg( ii->name() ).arg( progress->value() + 1 ).arg( numToBeRendered ).arg( totalFormulas );
            progress->setLabelText( label );

            fRenderingEngine->renderSVG(
                ii->formula(),   //
                [ =, &obj ]( const QString &texCode, const std::optional< QByteArray > &svg )   //
                {
                    if ( !svg.has_value() )
                        return;

                    if ( ii->formula() != texCode )   // only do it once, ignore the cleanedMessage
                        return;

                    obj.insert( "svg", QJsonValue::fromVariant( svg.value().toBase64() ) );
                },   //
                [ =, &obj ]( const QString &errorMessage )
                {
                    //qCDebug( ScubaCalculator ).noquote().nospace() << tr( "Error Generating SVG: %1: %2" ).arg( ii->name() ).arg( errorMessage );
                    obj.insert( "error", QJsonValue::fromVariant( "ERROR: " + errorMessage ) );
                }   //
            );

            currFormulaData.second->fJsonArray.append( obj );
        }
    }
    delete progress;
    progress = nullptr;

    for ( auto &&ii : allFormulas )
    {
        auto calc = ii.first;
        if ( needUpdatingOnly && !ii.second->fUpdated )
            continue;

        auto jsonFileName = QDir( dir ).absoluteFilePath( QString( "%1-formulas.json" ).arg( calc->calculatorName() ) );
        QFile jsonFile( jsonFileName );
        if ( !jsonFile.open( QFile::WriteOnly | QFile::Text | QFile::Truncate ) )
        {
            QMessageBox::critical( this, tr( "Could not open file" ), tr( "Error opening file: %1<br/>%2" ).arg( jsonFileName ).arg( jsonFile.errorString() ) );
            return;
        }
        QJsonDocument doc;
        doc.setArray( ii.second->fJsonArray );

        jsonFile.write( doc.toJson( QJsonDocument::Indented ) );
        jsonFile.close();
    }
    fRenderingEngine->blockSignals( false );

    QMessageBox::information( this, tr( "Finished generating SVGs" ), tr( "Total Number of Formulas: %1<br>Number needing Rendering: %2" ).arg( totalFormulas ).arg( numToBeRendered ) );
}

void CMainWindow::loadCache()
{
    QDirIterator ii( ":/formulas", QStringList() << "*.json" );
    while ( ii.hasNext() )
    {
        // Advance the iterator to the next entry and print its path.
        auto fileName = ii.next();

        QFile fi( fileName );
        if ( !fi.open( QFile::Text | QFile::ReadOnly ) )
        {
            QMessageBox::critical( this, tr( "Could not open cache" ), tr( "Error: Problem opening cache file<br/>%1" ).arg( fi.errorString() ) );
            return;
        }

        QJsonParseError parseError;
        auto doc = QJsonDocument::fromJson( fi.readAll(), &parseError );
        if ( ( parseError.error != QJsonParseError::NoError ) || doc.isNull() )
        {
            QMessageBox::critical( this, tr( "Error Reading JSON" ), tr( "Error: Error in Cache File @%1:<br/> %2" ).arg( parseError.offset ).arg( parseError.errorString() ) );
            return;
        }

        if ( !doc.isArray() )
        {
            QMessageBox::critical( this, tr( "Error Reading JSON" ), tr( "Error: Invalid Format expected Array" ) );
            return;
        }

        auto svgCacheArray = doc.array();

        for ( auto &&svgItem : svgCacheArray )
        {
            if ( !svgItem.isObject() )
                continue;

            auto svgCacheObject = svgItem.toObject();

            if ( !svgCacheObject.contains( "formula" ) || !svgCacheObject.contains( "name" ) )
            {
                QMessageBox::critical( this, tr( "Error Reading JSON" ), tr( "Error: Invalid Format expected 'formula' and 'name' fields" ) );
                continue;
            }

            if ( !svgCacheObject.contains( "svg" ) && !svgCacheObject.contains( "error" ) )
            {
                QMessageBox::critical( this, tr( "Error Reading JSON" ), tr( "Error: Invalid Format expected svg or error fields" ) );
                continue;
            }

            auto name = svgCacheObject[ "name" ].toString();
            auto formula = svgCacheObject[ "formula" ].toString();
            std::optional< QString > cleanedFormula;
            if ( svgCacheObject.contains( "cleanedFormula" ) )
                cleanedFormula = svgCacheObject[ "cleanedFormula" ].toString();

            if ( svgCacheObject.contains( "error" ) )
            {
                //qCDebug( ScubaCalculator ).noquote().nospace() << "Skipping cache item: name '" << name << "' tex '" << formula << "'";
                continue;
            }

            auto svg = svgCacheObject[ "svg" ].toString().toUtf8();
            if ( auto result = QByteArray::fromBase64Encoding( svg ) )
            {
                fRenderingEngine->addToCache( formula, cleanedFormula, *result );
            }
            else
            {
                QMessageBox::critical( this, tr( "Error Reading JSON" ), tr( "Error: Invalid SVG base 64" ) );
                continue;
            }
        }
    }
}

std::optional< QString > SFormulas::formula( EFormulaType formulaType ) const
{
    if ( formulaType == EFormulaType::eBaseFormula )
        return fBaseFormula;
    else if ( formulaType == EFormulaType::eCurrentFormula )
        return fCurrFormula;
    else if ( formulaType == EFormulaType::eCurrentValueFormula )
        return fCurrValueFormula;
    else
        return {};
}

void SFormulas::setFormula( const QString &formula, EFormulaType formulaType )
{
    if ( formulaType == EFormulaType::eBaseFormula )
        fBaseFormula = formula;
    else if ( formulaType == EFormulaType::eCurrentFormula )
        fCurrFormula = formula;
    else if ( formulaType == EFormulaType::eCurrentValueFormula )
        fCurrValueFormula = formula;
}
