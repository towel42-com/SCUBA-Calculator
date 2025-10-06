#include "SCUBACalculatorPage.h"
#include "SCUBACalculator.h"
#include "VariableInfo.h"
#include "SABUtils/WidgetChanged.h"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
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

void CSCUBACalculatorPage::addWidgets( bool rhs, const std::list< QWidget * > &widgets )
{
    for ( auto &&ii : widgets )
    {
        addWidget( rhs, ii );
    }
}

void CSCUBACalculatorPage::updateValues( QWidget *triggerWidget )
{
    fNeedsInit = false;
    calculator()->compute( updateFromRHS(), triggerWidget );
}

void CSCUBACalculatorPage::addWidget( bool rhs, QWidget *widget )
{
    fWidgets[ widget ] = rhs;

    NSABUtils::setupWidgetChanged(
        widget,
        [ = ]( QObject * )
        {
            auto pos = fWidgets.find( widget );
            if ( pos == fWidgets.end() )
                return;
            setUpdateFromRHS( ( *pos ).second );
            updateValues( widget );
        } );
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

void CSCUBACalculatorPage::setUpdateFromRHS( bool updateFromRHS )
{
    fUpdateFromRHS = updateFromRHS;
}

void CSCUBACalculatorPage::slotWidgetChanged( QWidget *widget )
{
    auto pos = fWidgets.find( widget );
    if ( pos == fWidgets.end() )
        return;
    setUpdateFromRHS( ( *pos ).second );
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
        if ( curr->fType != EVariableType::eVariable )
            continue;
        numVariables++;
        curr->fLabel = new QLabel( retVal );
        curr->fLabel->setText( QString( "%1:" ).arg( curr->fDescription ) );

        curr->fField = new QLineEdit( retVal );
        curr->fField->setObjectName( curr->fName.data() );

        curr->fUnitLabel = new QLabel( retVal );

        auto hLayout = new QHBoxLayout();
        hLayout->addWidget( curr->fField );
        hLayout->addWidget( curr->fUnitLabel );

        formLayout->addRow( curr->fLabel, hLayout );
        retVal->addWidget( curr->fRHSVariable, curr->fField );

        curr->updateLabels( calculator->imperial(), calculator->saltWater() );
    }

    auto svgFrame = new QFrame( retVal );
    svgFrame->setFrameShape( QFrame::Shape::StyledPanel );
    svgFrame->setFrameShadow( QFrame::Shadow::Raised );
    auto hBoxLayout = new QHBoxLayout( svgFrame );
    hBoxLayout->addItem( new QSpacerItem( 40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum ) );

    auto svgWidget = new QSvgWidget( svgFrame );
    svgWidget->setMinimumSize( QSize( 0, 10 ) );
    svgWidget->renderer()->setAspectRatioMode( Qt::AspectRatioMode::KeepAspectRatioByExpanding );

    hBoxLayout->addWidget( svgWidget, 0, Qt::AlignCenter );

    hBoxLayout->addItem( new QSpacerItem( 40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum ) );

    formLayout->addRow( svgFrame );
    formLayout->setItem( formLayout->rowCount(), QFormLayout::LabelRole, new QSpacerItem( 20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding ) );

    return { retVal, svgFrame, svgWidget, numVariables };
}
