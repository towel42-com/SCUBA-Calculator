
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "SCUBACalculator.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSettings>
#include <libloaderapi.h>

CMainWindow::CMainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    fImpl( new Ui::CMainWindow )
{
    fImpl->setupUi( this );
    setWindowIcon( QIcon( ":/resources/scubacalc.png" ) );
    setAttribute( Qt::WA_DeleteOnClose );
    fBlankPage = new QWidget;
    fImpl->stackedWidget->addWidget( fBlankPage );

    loadCalculators();

    connect( fImpl->whichCalculator, &QTreeWidget::currentItemChanged, this, &CMainWindow::slotSelectCalculator );
    connect(
        fImpl->imperial, &QRadioButton::toggled,
        [ = ]()
        {
            for ( auto ii : fCalculators )
            {
                auto func = getSetImperialFunc( ii.first );
                if ( !func )
                    continue;
                func( std::get< 0 >( ii.second ), true );
            }
        } );
    connect(
        fImpl->metric, &QRadioButton::toggled,
        [ = ]()
        {
            for ( auto ii : fCalculators )
            {
                auto func = getSetMetricFunc( ii.first );
                if ( !func )
                    continue;
                func( std::get< 0 >( ii.second ), true );
            }
        } );

    slotSelectCalculator( nullptr );

    QSettings settings;
    fImpl->imperial->setChecked( settings.value( "ImperialUnits", true ).toBool() );
}

CMainWindow::~CMainWindow()
{
}

void CMainWindow::slotSelectCalculator( QTreeWidgetItem *item )
{
    auto pageFunc = getGetPageFunc( item );
    auto calculator = getCalculator( item );
    CSCUBACalculatorPage *page = nullptr;
    if ( pageFunc && calculator )
    {
        page = pageFunc( calculator, nullptr );
    }

    bool showUnits = page != nullptr;
    if ( calculator && page )
    {
        fImpl->pageName->setText( calculator->calculatorName() );
        showUnits = page->property( "showUnits" ).toBool();
        fImpl->stackedWidget->setCurrentWidget( page );
    }
    else
    {
        fImpl->stackedWidget->setCurrentWidget( fBlankPage );
        fImpl->pageName->setText( "Please Select a Calculator" );
    }
    fImpl->imperial->setVisible( showUnits );
    fImpl->metric->setVisible( showUnits );
}

void CMainWindow::loadCalculators()
{
    auto calcDir = QDir( QApplication::applicationDirPath() ).absoluteFilePath( "Calculators" );

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

        auto constructor = (TInstantiateCalcFunc)GetProcAddress( hLib, "instantiateCalculator" );
        auto getPageFunc = (TGetPageFunc)GetProcAddress( hLib, "getPage" );
        auto setImperial = (TSetImperialFunc)GetProcAddress( hLib, "setImperial" );
        auto setMetric = (TSetMetricFunc)GetProcAddress( hLib, "setMetric" );
        if ( !constructor )
            continue;

        auto calculator = (CSCUBACalculator *)constructor();
        addCalculator( calculator, getPageFunc, setImperial, setMetric );
    }
    fImpl->whichCalculator->expandAll();
}

void CMainWindow::addCalculator( CSCUBACalculator *calculator, TGetPageFunc getPageFunc, TSetImperialFunc setImperialFunc, TSetMetricFunc setMetricFunc )
{
    auto path = calculator->calculatorPath();
    if ( path.isEmpty() )
        return;

    auto calculatorName = calculator->calculatorName();

    path.push_back( calculatorName );
    auto leaf = findItem( fImpl->whichCalculator->invisibleRootItem(), path, true );
    fCalculators[ leaf ] = { calculator, getPageFunc, setImperialFunc, setMetricFunc };

    if ( !getGetPageFunc( leaf ) )
    {
        qDebug() << "No widget for page :" << path;
        return;
    }
    
    auto page = getPageFunc( calculator, nullptr );
    if ( !page )
    {
        qDebug() << "No widget for page :" << path;
        return;
    }
    fImpl->stackedWidget->addWidget( page );
}

void CMainWindow::hideUnits( bool hide )
{
    fImpl->imperial->setHidden( hide );
    fImpl->metric->setHidden( hide );
}

CSCUBACalculator *CMainWindow::getCalculator( QTreeWidgetItem *leaf ) const
{
    auto pos = fCalculators.find( leaf );
    if ( pos != fCalculators.end() )
        return std::get< 0 >( ( *pos ).second );
    return nullptr;
}

TGetPageFunc CMainWindow::getGetPageFunc( QTreeWidgetItem *leaf ) const
{
    auto pos = fCalculators.find( leaf );
    if ( pos != fCalculators.end() )
        return std::get< 1 >( ( *pos ).second );
    return nullptr;
}

TSetImperialFunc CMainWindow::getSetImperialFunc( QTreeWidgetItem *leaf ) const
{
    auto pos = fCalculators.find( leaf );
    if ( pos != fCalculators.end() )
        return std::get< 2 >( ( *pos ).second );
    return nullptr;
}

TSetMetricFunc CMainWindow::getSetMetricFunc( QTreeWidgetItem *leaf ) const
{
    auto pos = fCalculators.find( leaf );
    if ( pos != fCalculators.end() )
        return std::get< 3 >( ( *pos ).second );
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
