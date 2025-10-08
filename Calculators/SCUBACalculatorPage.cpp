#include "SCUBACalculatorPage.h"
#include "SCUBACalculator.h"
#include "VariableInfo.h"
#include "SABUtils/WidgetChanged.h"

#include <QFormLayout>
#include <QFrame>
#include <QSvgWidget>
#include <QSvgRenderer>

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

void CSCUBACalculatorPage::init( bool imperial, bool saltWater )
{
    fImperial = imperial;
    fSaltWater = saltWater;

    calculator()->renderDefaultFormulas();

    updateValues( nullptr );
}

void CSCUBACalculatorPage::setImperial( bool imperial )
{
    fImperial = imperial;
    updateValues( nullptr );
}

void CSCUBACalculatorPage::setSaltWater( bool saltWater )
{
    fSaltWater = saltWater;
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

    auto &&variables = calculator->getVariables();
    for ( auto &&curr : variables )
    {
        if ( curr->createWidgets( retVal, formLayout ) )
            numVariables++;
    }

    auto svgFrame = new QFrame( retVal );
    QSizePolicy sizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred );
    sizePolicy.setHorizontalStretch( 0 );
    sizePolicy.setVerticalStretch( 1 );
    sizePolicy.setHeightForWidth( svgFrame->sizePolicy().hasHeightForWidth() );
    svgFrame->setSizePolicy( sizePolicy );

    svgFrame->setFrameShape( QFrame::Shape::StyledPanel );
    svgFrame->setFrameShadow( QFrame::Shadow::Raised );
    auto hBoxLayout = new QHBoxLayout( svgFrame );

    auto svgWidget = new QSvgWidget( svgFrame );
    svgWidget->setMinimumSize( QSize( 0, 10 ) );

    hBoxLayout->addWidget( svgWidget, 0, Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter );

    formLayout->addRow( svgFrame );

    auto spacerItem = new QSpacerItem( 20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::MinimumExpanding );
    formLayout->addItem( spacerItem );

    return { retVal, svgFrame, svgWidget, numVariables };
}
