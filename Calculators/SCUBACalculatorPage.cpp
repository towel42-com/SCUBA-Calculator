#include "SCUBACalculatorPage.h"
#include "SCUBACalculator.h"
#include "VariableInfo.h"
#include "SABUtils/WidgetChanged.h"

#include <QFormLayout>
#include <QFrame>
#include <QSvgWidget>
#include <QSvgRenderer>
#include <QGroupBox>

#include <list>
#include <utility>

CSCUBACalculatorPage::CSCUBACalculatorPage( CSCUBACalculator *calculator, QWidget *parent ) :
    QWidget( parent ),
    fCalculator( calculator )
{
    connect( this, &CSCUBACalculatorPage::sigUpdateValues, [ = ]() { updateValues( nullptr ); } );
}

CSCUBACalculatorPage::~CSCUBACalculatorPage()
{
}

void CSCUBACalculatorPage::init( bool imperial, bool seaWater )
{
    fImperial = imperial;
    fSeaWater = seaWater;

    calculator()->renderDefaultFormulas();

    updateValues( nullptr );
}

void CSCUBACalculatorPage::setImperial( bool imperial )
{
    fImperial = imperial;
    updateValues( nullptr );
}

void CSCUBACalculatorPage::setSeaWater( bool seaWater )
{
    fSeaWater = seaWater;
    emit sigUpdateValues();
}

void CSCUBACalculatorPage::updateValues( QWidget *triggerWidget )
{
    fNeedsInit = false;
    calculator()->compute( updateFromSide(), triggerWidget );
}

void CSCUBACalculatorPage::addWidgets( EVariableLoc side, const std::list< QWidget * > &widgets )
{
    for ( auto &&ii : widgets )
    {
        addWidget( side, ii );
    }
}

void CSCUBACalculatorPage::addWidget( EVariableLoc side, QWidget *widget )
{
    fVariables[ widget ] = side;

    NSABUtils::setupWidgetChanged( widget, [ = ]( QObject *object ) { slotWidgetChanged( dynamic_cast< QWidget * >( object ) ); } );
}

bool CSCUBACalculatorPage::showUnits() const
{
    return fCalculator->showUnits();
}

bool CSCUBACalculatorPage::isWaterTypeBased() const
{
    return fCalculator->isWaterTypeBased();
}

bool CSCUBACalculatorPage::needsInit() const
{
    return fNeedsInit;
}

void CSCUBACalculatorPage::setUpdateFromSide( EVariableLoc updateFromSide )
{
    fUpdateFromSide = updateFromSide;
}

void CSCUBACalculatorPage::slotWidgetChanged( QWidget *widget )
{
    auto pos = fVariables.find( widget );
    if ( pos == fVariables.end() )
        return;
    setUpdateFromSide( ( *pos ).second );
    updateValues( widget );
}

std::tuple< CSCUBACalculatorPage *, QFrame *, QSvgWidget *, std::size_t > CSCUBACalculatorPage::constructPage( CSCUBACalculator *calculator, QWidget *parent )
{
    if ( !calculator )
        return { nullptr, nullptr, nullptr, 0 };
    std::size_t numVariables = 0;

    auto retVal = new CSCUBACalculatorPage( calculator, parent );
    auto formLayout = new QFormLayout( retVal );

    auto &&[ groupBox, currNumVariables ] = loadVariables( tr( "Global" ), calculator->getGlobalVariables(), retVal );
    numVariables += currNumVariables;
    if ( groupBox )
        formLayout->addRow( groupBox );

    std::tie( groupBox, currNumVariables ) = loadVariables( tr( "LHS" ), calculator->getLHSVariables(), retVal );
    numVariables += currNumVariables;
    if ( groupBox )
        formLayout->addRow( groupBox );

    std::tie( groupBox, currNumVariables ) = loadVariables( tr( "RHS" ), calculator->getRHSVariables(), retVal );
    numVariables += currNumVariables;
    if ( groupBox )
        formLayout->addRow( groupBox );

    //auto svgFrame = new QFrame( retVal );
    //QSizePolicy sizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred );
    //sizePolicy.setHorizontalStretch( 0 );
    //sizePolicy.setVerticalStretch( 1 );
    //sizePolicy.setHeightForWidth( svgFrame->sizePolicy().hasHeightForWidth() );
    //svgFrame->setSizePolicy( sizePolicy );

    //svgFrame->setFrameShape( QFrame::Shape::StyledPanel );
    //svgFrame->setFrameShadow( QFrame::Shadow::Raised );
    //auto hBoxLayout = new QHBoxLayout( svgFrame );

    //auto svgWidget = new QSvgWidget( svgFrame );
    //svgWidget->setMinimumSize( QSize( 0, 10 ) );

    //hBoxLayout->addWidget( svgWidget, 0, Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter );

    //formLayout->addRow( svgFrame );

    //auto spacerItem = new QSpacerItem( 20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::MinimumExpanding );
    //formLayout->addItem( spacerItem );

    return { retVal, nullptr, nullptr, numVariables };
}

std::pair< QGroupBox *, std::size_t > CSCUBACalculatorPage::loadVariables( const QString &name, const TVariableInfoList &variables, CSCUBACalculatorPage *page )
{
    std::size_t numVariables = 0;
    for ( auto &&curr : variables )
    {
        if ( curr->isVariable() )
            numVariables++;
    }
    if ( numVariables == 0 )
        return { nullptr, 0 };

    auto groupBox = new QGroupBox( tr( "%1 Variables:" ).arg( name ), page );
    auto formLayout = new QFormLayout( groupBox );

    for ( auto &&curr : variables )
    {
        curr->createWidgets( page, formLayout );
    }
    return { groupBox, numVariables };
}

//QFrame *line;
//line = new QFrame( layoutWidget );
//line->setObjectName( "line" );
//line->setFrameShape( QFrame::Shape::HLine );
//line->setFrameShadow( QFrame::Shadow::Sunken );
//
//gridLayout->addWidget( line, 2, 0, 1, 1 );
