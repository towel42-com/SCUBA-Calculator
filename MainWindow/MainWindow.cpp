
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
#include <QResizeEvent>
#include <QRegularExpression>

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
    bg->addButton( fImpl->seaWater );
    bg->addButton( fImpl->freshWater );

    setWindowIcon( QIcon( ":/resources/scubacalc.png" ) );
    setAttribute( Qt::WA_DeleteOnClose );
    fBlankPage = new QWidget;
    fImpl->stackedWidget->addWidget( fBlankPage );
    fImpl->stackedWidget->installEventFilter( this );

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
    connect( fImpl->seaWater, &QRadioButton::toggled, this, &CMainWindow::slotWaterChanged );
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
    auto func = getSetSeaWaterFunc( item );
    if ( !func )
        return;

    func( calc, fImpl->seaWater->isChecked() );
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

        initFunc( calc, fImpl->imperial->isChecked(), fImpl->seaWater->isChecked() );
    }

    loadFormulasForPage( page );
    this->showUnits( showUnits );
    this->showWaterType( isWaterTypeBased );
    fImpl->reset->setVisible( page != nullptr );
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
            continue;

        auto constructor = (TInstantiateCalcFunc)GetProcAddress( hLib, kInstantiateCalcFuncName );
        auto initFunc = (TInitFunc)GetProcAddress( hLib, kInitFuncName );
        if ( !constructor || !initFunc )
            continue;
        auto getPageFunc = (TGetPageFunc)GetProcAddress( hLib, kGetPageFuncName );
        auto setImperial = (TSetBoolFunc)GetProcAddress( hLib, kSetImperialFuncName );
        auto setSeaWater = (TSetBoolFunc)GetProcAddress( hLib, kSetSeaWaterFuncName );
        auto setUpdateFormulaFunc = (TSetUpdateFormulaFunc)GetProcAddress( hLib, kSetUpdateFormulaFuncName );

        auto calculator = (CSCUBACalculator *)constructor();
        addCalculator( calculator, getPageFunc, setImperial, setSeaWater, setUpdateFormulaFunc, initFunc );
    }
    fImpl->whichCalculator->expandAll();
    fImpl->whichCalculator->sortByColumn( 0, Qt::SortOrder::AscendingOrder );
    fImpl->whichCalculator->resizeColumnToContents( 0 );
    auto colWidth = fImpl->whichCalculator->columnWidth( 0 );
    fImpl->whichCalculator->setMinimumWidth( colWidth + 20 );
}

void CMainWindow::addCalculator( CSCUBACalculator *calculator, TGetPageFunc getPageFunc, TSetBoolFunc setImperialFunc, TSetBoolFunc setSeaWater, TSetUpdateFormulaFunc setUpdateFormulaFunc, TInitFunc initFunc )
{
    auto path = calculator->calculatorPath();
    if ( path.isEmpty() )
        return;

    auto calculatorName = calculator->calculatorName();

    path.push_back( calculatorName );
    auto leaf = findItem( fImpl->whichCalculator->invisibleRootItem(), path, true );
    fCalculators[ leaf ] = { calculator, initFunc, getPageFunc, setImperialFunc, setSeaWater };

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
        fImpl->formulaFrame->setVisible( renderSVG( formula.value() ) );
    else
        fImpl->formulaFrame->setVisible( false );

    formula = formulaForPage( page, false );
    if ( formula.has_value() )
        currentCalculator()->svgFrame()->setVisible( renderSVG( formula.value() ) );
    else if ( currentCalculator() )
        currentCalculator()->svgFrame()->setVisible( false );
}

bool CMainWindow::renderSVG( const QString &formula )
{
    auto pos2 = fFormulaToSVGMap.find( formula );
    if ( pos2 != fFormulaToSVGMap.end() )
    {
        auto svg = ( *pos2 ).second;
        loadSVG( formula, svg );
        return true;
    };
    fRenderingEngine->renderSVG( formula );
    return false;
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
        else if ( currentCalculator()  )
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
    fImpl->seaWater->setVisible( show );
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

TSetBoolFunc CMainWindow::getSetSeaWaterFunc( QTreeWidgetItem *leaf ) const
{
    auto pos = fCalculators.find( leaf );
    if ( pos != fCalculators.end() )
        return ( *pos ).second.fSetSeaWaterFunc;
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
        if ( isBaseFormula )
            fCurrFormulas.first = formula;
        else
            fCurrFormulas.second = formula;

        frame->setVisible( !svg.isEmpty() );

        if ( !svg.isEmpty() )
        {
            svgWidget->load( svg );
            if ( svgWidget->renderer()->isValid() )
            {
                updateSVGSize( isBaseFormula );
            }
            else
            {
                QMessageBox::critical( this, tr( "Error Loading SVG File" ), tr( "SVG was generated but could not be loaded" ) );
                frame->setVisible( false );
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

    updateSVGSize( true );
    updateSVGSize( false );
}

void CMainWindow::updateSVGSize( bool isBaseFormula )
{
    auto widget = isBaseFormula ? fImpl->formulaWidget : currentCalculator()->svgWidget();
    auto frame = isBaseFormula ? fImpl->formulaFrame : currentCalculator()->svgFrame();
    auto formula = isBaseFormula ? fCurrFormulas.first : fCurrFormulas.second;
    auto maxWidth = ( frame->size() * 0.9 ).width();

    NTowel42::updateSVGSize( widget, formula, maxWidth, true );
}
