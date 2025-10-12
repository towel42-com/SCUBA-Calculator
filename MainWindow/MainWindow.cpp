#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "SCUBACalculator.h"
#include "SCUBACalculatorPage.h"
#include "T42-Qt6MathJax/include/Qt6MathJax.h"
#include "SABUtils/utils.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSettings>
#include <QMessageBox>
#include <QSvgRenderer>
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
    fImpl->setupUi( this );

    auto bg = new QButtonGroup( this );
    bg->addButton( fImpl->imperial );
    bg->addButton( fImpl->metric );

    bg = new QButtonGroup( this );
    bg->addButton( fImpl->seaWater );
    bg->addButton( fImpl->freshWater );

    setWindowIcon( QIcon( ":/resources/scubacalc.png" ) );
    setAttribute( Qt::WA_DeleteOnClose );
    fBlankPage = new QWidget;
    fImpl->stackedWidget->addWidget( fBlankPage );
    fImpl->stackedWidget->installEventFilter( this );

    fRenderingEngine = new NTowel42::CQt6MathJax( this );
    fImpl->baseFormulaWidget->setEngine( fRenderingEngine );
    fImpl->currFormulaWidget->setEngine( fRenderingEngine );
    fImpl->currFormulaWidget->setSubordinateTo( fImpl->baseFormulaWidget );
    fImpl->currFormulaValueWidget->setEngine( fRenderingEngine );
    fImpl->currFormulaValueWidget->setSubordinateTo( { fImpl->baseFormulaWidget, fImpl->currFormulaWidget } );

    connect(
        fImpl->baseFormulaWidget, &NTowel42::CMathJaxWidget::sigErrorMessage,
        [ = ]( const QString &msg )
        {
            fImpl->baseFormulaWidget->setVisible( false );
            QMessageBox::critical( this, tr( "Error in MathJax Engine" ), msg );
        } );

    connect(
        fImpl->currFormulaWidget, &NTowel42::CMathJaxWidget::sigErrorMessage,
        [ = ]( const QString &msg )
        {
            fImpl->currFormulaWidget->setVisible( false );
            QMessageBox::critical( this, tr( "Error in MathJax Engine" ), msg );
        } );

    connect(
        fImpl->currFormulaValueWidget, &NTowel42::CMathJaxWidget::sigErrorMessage,
        [ = ]( const QString &msg )
        {
            fImpl->currFormulaValueWidget->setVisible( false );
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
    slotSelectCalculator( nullptr );
    QTimer::singleShot( 100, [ = ] { loadCache(); } );
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

    auto calculatorName = calculator->calculatorName();

    path.push_back( calculatorName );
    auto leaf = findItem( fImpl->whichCalculator->invisibleRootItem(), path, true );
    fCalculators[ leaf ] = calculator;

    auto page = calculator->getPage( nullptr );
    if ( !page )
    {
        qDebug() << "No widget for page :" << path;
        return;
    }

    fImpl->stackedWidget->addWidget( page );
    fPageToItem[ page ] = leaf;

    calculator->setUpdateFormulaFunc( [ = ]( CSCUBACalculatorPage *calcPage, const QString &formula, EFormulaType formulaType )   //
                                      {   //
                                          this->setFormulaForPage( calcPage, formula, formulaType );
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
    auto page = calculator ? calculator->getPage( nullptr ) : nullptr;
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
    fImpl->imperial->setVisible( show );
    fImpl->metric->setVisible( show );
}

void CMainWindow::showWaterType( bool show )
{
    fImpl->seaWater->setVisible( show );
    fImpl->freshWater->setVisible( show );
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
    fImpl->currFormulaValueWidget->setVisible( visible );
    fImpl->currFormulaWidget->setVisible( visible );
    fImpl->baseFormulaWidget->setVisible( visible );
}

void CMainWindow::setFormulaForPage( CSCUBACalculatorPage *page, const QString &formula, EFormulaType formulaType )
{
    auto widget = mathJaxForFormulaType( formulaType );
    Q_ASSERT( widget );
    if ( formula.isEmpty() )
    {
        widget->clear();
        return;
    }

    auto pos = this->fPageToFormulasMap.find( page );
    if ( pos != fPageToFormulasMap.end() )
    {
        if ( ( *pos ).second.formula( formulaType ) == formula )
        {
            loadFormulasForPage( page );
            return;
        }
    }
    else
    {
        pos = fPageToFormulasMap.insert( { page, SFormulas() } ).first;
    }
    ( *pos ).second.setFormula( formula, formulaType );
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

NTowel42::CMathJaxWidget *CMainWindow::mathJaxForFormulaType( EFormulaType formulaType ) const
{
    if ( formulaType == EFormulaType::eBaseFormula )
        return fImpl->baseFormulaWidget;
    else if ( formulaType == EFormulaType::eCurrentFormula )
        return fImpl->currFormulaWidget;
    else if ( formulaType == EFormulaType::eCurrentValueFormula )
        return fImpl->currFormulaValueWidget;

    return nullptr;
}

std::optional< QString > CMainWindow::formulaForPage( QWidget *page, EFormulaType formulaType )
{
    auto pos = fPageToFormulasMap.find( page );

    auto calculator = getCalculator( page );

    if ( pos == fPageToFormulasMap.end() )
    {
        qDebug().noquote().nospace() << "Page: '" << calculator->calculatorName() << "' has no " << toString( formulaType ) << " formula.";
        return {};
    }
    auto retVal = ( *pos ).second.formula( formulaType );
    qDebug().noquote().nospace() << "Page: '" << calculator->calculatorName() << "' " << toString( formulaType ) << " formula is '" << retVal << "'";
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

bool CMainWindow::eventFilter( QObject *obj, QEvent *event )
{
    if ( ( obj == fImpl->stackedWidget ) && ( event->type() == QEvent::Resize ) )
    {
        updateSVGSizes();
    }
    return QObject::eventFilter( obj, event );
}

void CMainWindow::updateSVGSizes()
{
    if ( !currentCalculator() )
        return;

    updateSVGSize( EFormulaType::eBaseFormula );
    updateSVGSize( EFormulaType::eCurrentFormula );
    updateSVGSize( EFormulaType::eCurrentValueFormula );
}

void CMainWindow::updateSVGSize( EFormulaType formulaType )
{
    auto widget = mathJaxForFormulaType( formulaType );
    widget->updateSVGSize();
}

void CMainWindow::slotGenerateAllFormulas()
{
    auto dir = QFileDialog::getExistingDirectory( this, tr( "Select Target Directory:" ) );
    if ( dir.isEmpty() )
        return;

    // formula -> formula name, filename
    std::unordered_map< QString, std::pair< QString, QString > > allFormulas;

    for ( auto &&ii : fCalculators )
    {
        // map formula name -> formula
        auto formulas = ii.second->getAllFormulas();
        for ( auto &&jj : formulas )
        {
            auto formulaName = jj.first;
            auto tex = jj.second;
            auto pos = allFormulas.find( tex );
            if ( pos != allFormulas.end() )
                continue;
            allFormulas[ tex ] = { formulaName, QString( "formula-%1.svg" ).arg( (int)allFormulas.size() + 1, 2, 10, QChar( '0' ) ) };
        }
    }

    QJsonArray svgArray;

    fRenderingEngine->blockSignals( true );
    QProgressDialog progress( tr( "Generating SVGs" ), tr( "Abort Generation" ), 0, (int)allFormulas.size(), this );
    progress.setMinimumDuration( 0 );
    for ( auto &&ii : allFormulas )
    {
        progress.setValue( progress.value() + 1 );
        if ( progress.wasCanceled() )
            break;

        //if ( progress.value() == 3 )
        //    break;

        auto tex = ii.first;
        auto formulaName = ii.second.first;
        auto fileName = ii.second.second;
        auto path = QDir( dir ).absoluteFilePath( fileName );

        QJsonObject obj;
        obj.insert( "name", QJsonValue::fromVariant( formulaName ) );
        obj.insert( "tex", QJsonValue::fromVariant( tex ) );

        fRenderingEngine->renderSVG(
            tex,   //
            [ =, &obj ]( const QString &texCode, const std::optional< QByteArray > &svg )   //
            {
                if ( !svg.has_value() )
                    return;

                if ( tex != texCode )
                    return;

                obj.insert( "svg", QJsonValue::fromVariant( svg.value().toBase64() ) );
            },   //
            [ =, &obj ]( const QString &errorMessage )
            {
                qDebug().noquote().nospace() << tr( "Error Generating SVG: %1: %2" ).arg( formulaName ).arg( errorMessage );
                obj.insert( "error", QJsonValue::fromVariant( "ERROR: " + errorMessage ) );
                if ( QFileInfo( path ).exists() )
                    QFile::remove( path );
            }   //
        );

        svgArray.append( obj );
    }
    progress.setValue( (int)allFormulas.size() );

    auto jsonFileName = QDir( dir ).absoluteFilePath( "formulas.json" );
    QFile jsonFile( jsonFileName );
    if ( !jsonFile.open( QFile::WriteOnly | QFile::Text | QFile::Truncate ) )
    {
        QMessageBox::critical( this, tr( "Could not open file" ), tr( "Error opening file: %1<br/>%2" ).arg( jsonFileName ).arg( jsonFile.errorString() ) );
        return;
    }
    QJsonDocument doc;
    doc.setArray( svgArray );

    jsonFile.write( doc.toJson( QJsonDocument::Indented ) );
    jsonFile.close();

    fRenderingEngine->blockSignals( false );
}

void CMainWindow::loadCache()
{
    QFile fi( ":/resources/formulas.json" );
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

        if ( !svgCacheObject.contains( "tex" ) || !svgCacheObject.contains( "name" ) )
        {
            QMessageBox::critical( this, tr( "Error Reading JSON" ), tr( "Error: Invalid Format expected tex and name keys" ) );
            continue;
        }

        if ( !svgCacheObject.contains( "svg" ) && !svgCacheObject.contains( "error" ) )
        {
            QMessageBox::critical( this, tr( "Error Reading JSON" ), tr( "Error: Invalid Format expected svg or error keys" ) );
            continue;
        }

        auto name = svgCacheObject[ "name" ].toString();
        auto tex = svgCacheObject[ "tex" ].toString();

        if ( svgCacheObject.contains( "error" ) )
        {
            qDebug().noquote().nospace() << "Skipping cache item: name '" << name << "' tex '" << tex << "'";
            continue;
        }
        if ( auto result = QByteArray::fromBase64Encoding( svgCacheObject[ "svg" ].toString().toUtf8() ) )
            fRenderingEngine->addToCache( tex, *result );
        else
        {
            QMessageBox::critical( this, tr( "Error Reading JSON" ), tr( "Error: Invalid SVG base 64" ) );
            continue;
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
