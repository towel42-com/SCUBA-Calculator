
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "SCUBACalculator.h"
#include "SCUBACalculatorPage.h"

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

    fImpl->formulaWidget->renderer()->setAspectRatioMode( Qt::AspectRatioMode::KeepAspectRatioByExpanding );

    fRenderingEngine = new NTowel42::CQt6MathJax( this );
    connect( fRenderingEngine, &NTowel42::CQt6MathJax::sigSVGRendered, this, &CMainWindow::slotFormulaRendered );
    connect(
        fRenderingEngine, &NTowel42::CQt6MathJax::sigErrorMessage,
        [ = ]( const QString &msg )
        {
            if ( currentCalculator() && currentCalculator()->svgFrame() )
                currentCalculator()->svgFrame()->setVisible( false );
            fImpl->formulaFrame->setVisible( false );
            QMessageBox::critical( this, tr( "Error in MathJax Engine" ), msg );
        } );

    connect( fImpl->reset, &QPushButton::clicked, this, &CMainWindow::slotResetCurrentPage );
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

CSCUBACalculator *CMainWindow::currentCalculator() const
{
    auto page = fImpl->stackedWidget->currentWidget();
    if ( page == fBlankPage )
        return nullptr;

    return getCalculator( page );
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
        auto initFunc = getInitFunc( item );
        if ( !initFunc )
            return;

        initFunc( calc, fImpl->imperial->isChecked(), fImpl->saltWater->isChecked() );
    }

    loadFormulasForPage( page );
    this->showUnits( showUnits );
    this->showWaterType( isWaterTypeBased );
    fImpl->reset->setVisible( page != nullptr );
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
    fImpl->whichCalculator->setMinimumWidth( colWidth + 20 );
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
            calculator, [ = ]( CSCUBACalculatorPage *calcPage, const QString &formula, bool baseFormula )   //
            {   //
                this->setFormulaForPage( calcPage, formula, baseFormula );
            } );
    }
}

void CMainWindow::loadFormulasForPage( CSCUBACalculatorPage *page )
{
    if ( !page )
    {
        fImpl->formulaFrame->setVisible( false );
        if ( currentCalculator() && currentCalculator()->svgFrame() )
            currentCalculator()->svgFrame()->setVisible( false );
        return;
    }

    auto formula = formulaForPage( page, true );
    if ( formula.has_value() )
        renderSVG( formula.value() );
    else
        fImpl->formulaFrame->setVisible( false );

    formula = formulaForPage( page, false );
    if ( formula.has_value() )
        renderSVG( formula.value() );
    else if ( currentCalculator() && currentCalculator()->svgFrame() )
        currentCalculator()->svgFrame()->setVisible( false );
}

void CMainWindow::renderSVG( const QString &formula )
{
    auto pos2 = fFormulaToSVGMap.find( formula );
    if ( pos2 != fFormulaToSVGMap.end() )
    {
        auto svg = ( *pos2 ).second;
        loadSVG( formula, svg );
        return;
    };
    fRenderingEngine->renderSVG( formula );
}

std::optional< QString > CMainWindow::formulaForPage( QWidget *page, bool baseFormula )
{
    auto &&map = baseFormula ? fPageToBaseFormulaMap : fPageToResultFormulaMap;
    auto pos = map.find( page );

    auto calculator = getCalculator( page );

    if ( pos == map.end() )
    {
        qDebug().noquote().nospace() << "Page: '" << calculator->calculatorName() << "' has no " << ( baseFormula ? "base" : "result" ) << " formula.";
        return {};
    }
    qDebug().noquote().nospace() << "Page: '" << calculator->calculatorName() << "' " << ( baseFormula ? "base" : "result" ) << " formula is '" << ( *pos ).second << "'";
    return ( *pos ).second;
}

void CMainWindow::setFormulaForPage( CSCUBACalculatorPage *page, const QString &formula, bool baseFormula )
{
    if ( formula.isEmpty() )
    {
        if ( baseFormula )
            fImpl->formulaFrame->setVisible( false );
        else if ( currentCalculator() && currentCalculator()->svgFrame() )
            currentCalculator()->svgFrame()->setVisible( false );
        return;
    }

    auto &&map = baseFormula ? fPageToBaseFormulaMap : fPageToResultFormulaMap;
    auto pos = map.find( page );
    if ( pos != map.end() )
    {
        if ( ( *pos ).second == formula )
        {
            loadFormulasForPage( page );
            return;
        }
        map.erase( pos );
    }
    map[ page ] = formula;
    loadFormulasForPage( page );
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

CSCUBACalculator *CMainWindow::getCalculator( QWidget *page ) const
{
    auto leaf = getItemForPage( page );
    return getCalculator( leaf );
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

void CMainWindow::loadSVG( const QString &formula, const QByteArray &svg )
{
    fFormulaToSVGMap[ formula ] = svg;

    auto currPage = fImpl->stackedWidget->currentWidget();
    if ( currPage == fBlankPage )
        return;

    auto baseFormula = formulaForPage( currPage, true );
    auto resultFormula = formulaForPage( currPage, false );

    if ( baseFormula.has_value() && resultFormula.has_value() && ( baseFormula.value() == resultFormula.value() ) )
        resultFormula.reset();

    if ( baseFormula.has_value() && ( baseFormula.value() != formula ) )
    {
        baseFormula.reset();
    }

    if ( resultFormula.has_value() && ( resultFormula.value() != formula ) )
    {
        resultFormula.reset();
    }

    for ( auto &&isBaseFormula : { false, true } )
    {
        if ( isBaseFormula && !baseFormula.has_value() )
            continue;
        if ( !isBaseFormula && !resultFormula.has_value() )
            continue;

        auto frame = isBaseFormula ? fImpl->formulaFrame : ( currentCalculator() ? currentCalculator()->svgFrame() : nullptr );
        auto svgWidget = isBaseFormula ? fImpl->formulaWidget : ( currentCalculator() ? currentCalculator()->svgWidget() : nullptr );

        frame->setVisible( !svg.isEmpty() );
        if ( !svg.isEmpty() )
        {
            svgWidget->load( svg );
            if ( !svgWidget->renderer()->isValid() )
            {
                QMessageBox::critical( this, tr( "Error Loading SVG File" ), tr( "SVG was generated but could not be loaded" ) );
                frame->setVisible( false );
            }
            else
            {
                auto maxSize = fImpl->whichCalculator->size();
                maxSize.setHeight( 200 );
                maxSize.setWidth( maxSize.width() * 0.75 );
                auto sz = svgWidget->sizeHint().scaled( maxSize, Qt::KeepAspectRatio );
                svgWidget->setMaximumSize( sz );
                svgWidget->setMinimumSize( sz );
            }
        }
    }
}

void CMainWindow::slotFormulaRendered( const QString &formula, const QByteArray &svg )
{
    loadSVG( formula, svg );
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
