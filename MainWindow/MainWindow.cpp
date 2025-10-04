
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "SCUBACalculator.h"
#include "T42-Qt6MathJax/include/Qt6MathJax.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSettings>
#include <QMessageBox>
#include <QSvgRenderer>
#include <QButtonGroup>
#include <libloaderapi.h>

CMainWindow::CMainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    fImpl( new Ui::CMainWindow )
{
    fImpl->setupUi( this );

    auto bg = new QButtonGroup( this );
    bg->addButton( fImpl->imperial );
    bg->addButton( fImpl->metric );

    bg = new QButtonGroup( this );
    bg->addButton( fImpl->saltWater );
    bg->addButton( fImpl->freshWater );

    setWindowIcon( QIcon( ":/resources/scubacalc.png" ) );
    setAttribute( Qt::WA_DeleteOnClose );
    fBlankPage = new QWidget;
    fImpl->stackedWidget->addWidget( fBlankPage );

    fRenderingEngine = new NTowel42::CQt6MathJax( this );
    connect( fRenderingEngine, &NTowel42::CQt6MathJax::sigSVGRendered, this, &CMainWindow::slotFormulaRendered );
    connect(
        fRenderingEngine, &NTowel42::CQt6MathJax::sigErrorMessage,
        [ = ]( const QString msg )
        {
            fImpl->formulaFrame->setVisible( false );
            QMessageBox::critical( this, tr( "Error in MathJax Engine" ), msg );
        } );

    loadSettings();
    loadCalculators();

    connect( fImpl->whichCalculator, &QTreeWidget::currentItemChanged, this, &CMainWindow::slotSelectCalculator );
    connect( fImpl->imperial, &QRadioButton::toggled, this, &CMainWindow::slotUnitsChanged );
    connect( fImpl->metric, &QRadioButton::toggled, this, &CMainWindow::slotUnitsChanged );
    connect( fImpl->saltWater, &QRadioButton::toggled, this, &CMainWindow::slotWaterChanged );
    connect( fImpl->freshWater, &QRadioButton::toggled, this, &CMainWindow::slotWaterChanged );

    slotSelectCalculator( nullptr );
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
    if ( settings.value( "SaltWater", true ).toBool() )
        fImpl->saltWater->setChecked( true );
    else
        fImpl->freshWater->setChecked( true );
}

void CMainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue( "ImperialUnits", fImpl->imperial->isChecked() );
    settings.setValue( "SaltWater", fImpl->saltWater->isChecked() );
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
    auto func = getSetImperialFunc( item );
    if ( !func )
        return;

    func( calc, fImpl->imperial->isChecked() );
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
    auto func = getSetSaltWaterFunc( item );
    if ( !func )
        return;

    func( calc, fImpl->saltWater->isChecked() );
}

void CMainWindow::slotSelectCalculator( QTreeWidgetItem *item )
{
    auto pageFunc = getGetPageFunc( item );
    auto calculator = getCalculator( item );
    CSCUBACalculatorPage *page = nullptr;
    bool needsInit = false;
    if ( pageFunc && calculator )
    {
        page = pageFunc( calculator, nullptr, &needsInit );
    }

    if ( calculator && page )
    {
        fImpl->pageName->setText( calculator->calculatorName() );
    }
    else
    {
        fImpl->pageName->setText( "Please Select a Calculator" );
    }

    setCurrentPage( item, page, needsInit );
}

void CMainWindow::setCurrentPage( QTreeWidgetItem * item, CSCUBACalculatorPage *page, bool initPage )
{
    bool showUnits = page != nullptr;
    bool showWaterType = page != nullptr;
    if ( page == nullptr )
    {
        fImpl->stackedWidget->setCurrentWidget( fBlankPage );
    }
    else
    {
        fImpl->stackedWidget->setCurrentWidget( page );
        showUnits = page->property( "showUnits" ).toBool();
        showWaterType = page->property( "showWaterType" ).toBool();
    }

    if ( page && initPage )
    {
        auto calc = getCalculator( item );
        if ( !calc )
            return;
        auto func = getInitFunc( item );
        if ( !func )
            return;

        func( calc, fImpl->imperial->isChecked(), fImpl->saltWater->isChecked() );
    }

    loadFormulaForPage( page );
    this->showUnits( showUnits );
    this->showWaterType( showWaterType );
}

void CMainWindow::loadCalculators()
{
    auto calcDir = QApplication::applicationDirPath();

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
            continue;

        auto constructor = (TInstantiateCalcFunc)GetProcAddress( hLib, kInstantiateCalcFuncName );
        auto initFunc = (TInitFunc)GetProcAddress( hLib, kInitFuncName );
        if ( !constructor || !initFunc )
            continue;
        auto getPageFunc = (TGetPageFunc)GetProcAddress( hLib, kGetPageFuncName );
        auto setImperial = (TSetBoolFunc)GetProcAddress( hLib, kSetImperialFuncName );
        auto setSaltWater = (TSetBoolFunc)GetProcAddress( hLib, kSetSaltWaterFuncName );
        auto setUpdateFormulaFunc = (TSetUpdateFormulaFunc)GetProcAddress( hLib, kSetUpdateFormulaFuncName );

        auto calculator = (CSCUBACalculator *)constructor();
        addCalculator( calculator, getPageFunc, setImperial, setSaltWater, setUpdateFormulaFunc, initFunc );
    }
    fImpl->whichCalculator->expandAll();
    fImpl->whichCalculator->sortByColumn( 0, Qt::SortOrder::AscendingOrder );
    fImpl->whichCalculator->resizeColumnToContents( 0 );
    auto colWidth = fImpl->whichCalculator->columnWidth( 0 );
    fImpl->whichCalculator->setMinimumWidth( colWidth + 10 );
}

void CMainWindow::addCalculator( CSCUBACalculator *calculator, TGetPageFunc getPageFunc, TSetBoolFunc setImperialFunc, TSetBoolFunc setSaltWater, TSetUpdateFormulaFunc setUpdateFormulaFunc, TInitFunc initFunc )
{
    auto path = calculator->calculatorPath();
    if ( path.isEmpty() )
        return;

    auto calculatorName = calculator->calculatorName();

    path.push_back( calculatorName );
    auto leaf = findItem( fImpl->whichCalculator->invisibleRootItem(), path, true );
    fCalculators[ leaf ] = { calculator, initFunc, getPageFunc, setImperialFunc, setSaltWater };

    if ( !getGetPageFunc( leaf ) )
    {
        qDebug() << "No widget for page :" << path;
        return;
    }

    auto page = getPageFunc( calculator, nullptr, nullptr );
    if ( !page )
    {
        qDebug() << "No widget for page :" << path;
        return;
    }

    fImpl->stackedWidget->addWidget( page );
    fPageToItem[ page ] = leaf;

    if ( setUpdateFormulaFunc )
    {
        setUpdateFormulaFunc(
            calculator, [ = ]( CSCUBACalculatorPage *calcPage, const QString &formula )   //
            {   //
                this->setFormulaForCalculator( calcPage, formula );
            } );
    }
}

void CMainWindow::loadSVG( const QString &formula, const QByteArray &svg )
{
    fFormulaToSVGMap[ formula ] = svg;

    auto currWidget = fImpl->stackedWidget->currentWidget();
    if ( currWidget == fBlankPage )
        return;

    auto pos = fPageToFormulaMap.find( currWidget );
    Q_ASSERT( pos != fPageToFormulaMap.end() );
    if ( pos == fPageToFormulaMap.end() )
        return;

    if ( ( *pos ).second != formula )
        return;

    fImpl->formulaFrame->setVisible( !svg.isEmpty() );
    if ( !svg.isEmpty() )
    {
        fImpl->formulaWidget->load( svg );
        if ( !fImpl->formulaWidget->renderer()->isValid() )
        {
            QMessageBox::critical( this, tr( "Error Loading SVG File" ), tr( "SVG was generated but could not be loaded" ) );
            fImpl->formulaFrame->setVisible( false );
        }
    }
}

void CMainWindow::slotFormulaRendered( const QString &formula, const QByteArray &svg )
{
    loadSVG( formula, svg );
}

void CMainWindow::loadFormulaForPage( CSCUBACalculatorPage *page )
{
    if ( !page )
    {
        fImpl->formulaFrame->setVisible( false );
        return;
    }
    auto pos = fPageToFormulaMap.find( page );
    if ( pos == fPageToFormulaMap.end() )
    {
        fImpl->formulaFrame->setVisible( false );
        return;
    }

    auto formula = ( *pos ).second;

    auto pos2 = fFormulaToSVGMap.find( formula );
    if ( pos2 != fFormulaToSVGMap.end() )
    {
        auto svg = ( *pos2 ).second;
        loadSVG( formula, svg );
        return;
    };
    fRenderingEngine->renderSVG( formula );
}

void CMainWindow::setFormulaForCalculator( CSCUBACalculatorPage *page, const QString &formula )
{
    fImpl->formulaFrame->setVisible( false );
    if ( formula.isEmpty() )
        return;

    auto pos = fPageToFormulaMap.find( page );
    if ( pos != fPageToFormulaMap.end() )
    {
        if ( ( *pos ).second == formula )
        {
            loadFormulaForPage( page );
            return;
        }
        fPageToFormulaMap.erase( pos );
    }
    fPageToFormulaMap[ page ] = formula;
    loadFormulaForPage( page );
}

void CMainWindow::showUnits( bool show )
{
    fImpl->imperial->setVisible( show );
    fImpl->metric->setVisible( show );
}

void CMainWindow::showWaterType( bool show )
{
    fImpl->saltWater->setVisible( show );
    fImpl->freshWater->setVisible( show );
}

CSCUBACalculator *CMainWindow::getCalculator( QTreeWidgetItem *leaf ) const
{
    auto pos = fCalculators.find( leaf );
    if ( pos != fCalculators.end() )
        return ( *pos ).second.fCalculator;
    return nullptr;
}

TInitFunc CMainWindow::getInitFunc( QTreeWidgetItem *leaf ) const
{
    auto pos = fCalculators.find( leaf );
    if ( pos != fCalculators.end() )
        return ( *pos ).second.fInitFunc;
    return nullptr;
}

QTreeWidgetItem *CMainWindow::getItemForPage( QWidget *page ) const
{
    auto pos = fPageToItem.find( page );
    if ( pos != fPageToItem.end() )
        return ( *pos ).second;
    return nullptr;
}

TGetPageFunc CMainWindow::getGetPageFunc( QTreeWidgetItem *leaf ) const
{
    auto pos = fCalculators.find( leaf );
    if ( pos != fCalculators.end() )
        return ( *pos ).second.fGetPageFunc;
    return nullptr;
}

TSetBoolFunc CMainWindow::getSetImperialFunc( QTreeWidgetItem *leaf ) const
{
    auto pos = fCalculators.find( leaf );
    if ( pos != fCalculators.end() )
        return ( *pos ).second.fSetImperialFunc;
    return nullptr;
}

TSetBoolFunc CMainWindow::getSetSaltWaterFunc( QTreeWidgetItem *leaf ) const
{
    auto pos = fCalculators.find( leaf );
    if ( pos != fCalculators.end() )
        return ( *pos ).second.fSetSaltWaterFunc;
    return nullptr;
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
