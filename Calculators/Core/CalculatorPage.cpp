#include "CalculatorPage.h"
#include "CalculatorBase.h"
#include "VariableInfo.h"
#include "T42-Utils/WidgetChanged.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>

#include <list>
#include <utility>

CCalculatorPage::CCalculatorPage( CCalculatorBase *calculator, QWidget *parent ) :
    QWidget( parent ),
    fCalculator( calculator )
{
    connect( this, &CCalculatorPage::sigUpdateValues, [ = ]() { updateValues( nullptr ); } );
}

CCalculatorPage::~CCalculatorPage()
{
}

void CCalculatorPage::init( bool imperial, bool seaWater )
{
    fImperial = imperial;
    fSeaWater = seaWater;

    updateValues( nullptr );
}

void CCalculatorPage::setImperial( bool imperial )
{
    fImperial = imperial;
    updateValues( nullptr );
}

void CCalculatorPage::setSeaWater( bool seaWater )
{
    fSeaWater = seaWater;
    emit sigUpdateValues();
}

void CCalculatorPage::updateValues( QWidget *triggerWidget )
{
    fNeedsInit = false;
    calculator()->compute( updateFromSide(), triggerWidget );
}

void CCalculatorPage::addWidgets( EVariableLoc side, const std::list< QWidget * > &widgets )
{
    for ( auto &&ii : widgets )
    {
        addWidget( side, ii );
    }
}

void CCalculatorPage::addWidget( EVariableLoc side, QWidget *widget )
{
    fVariables[ widget ] = side;

    NTowel42Utils::setupWidgetChanged( widget, [ = ]( QObject *object ) { slotWidgetChanged( dynamic_cast< QWidget * >( object ) ); } );
}

bool CCalculatorPage::showUnits() const
{
    return fCalculator->showUnits();
}

bool CCalculatorPage::showWaterType() const
{
    return fCalculator->showWaterType();
}

bool CCalculatorPage::needsInit() const
{
    return fNeedsInit;
}

void CCalculatorPage::setUpdateFromSide( EVariableLoc updateFromSide )
{
    fUpdateFromSide = updateFromSide;
}

void CCalculatorPage::slotWidgetChanged( QWidget *widget )
{
    auto pos = fVariables.find( widget );
    if ( pos == fVariables.end() )
        return;
    setUpdateFromSide( ( *pos ).second );
    updateValues( widget );
}

std::tuple< CCalculatorPage *, std::size_t > CCalculatorPage::constructPage( CCalculatorBase *calculator, QWidget *parent )
{
    if ( !calculator )
        return { nullptr, 0 };
    std::size_t numVariables = 0;

    auto retVal = new CCalculatorPage( calculator, parent );
    auto formLayout = new QVBoxLayout( retVal );
    formLayout->setSpacing( 0 );
    formLayout->setContentsMargins( 0, 0, 0, 0 );

    auto &&[ groupBox, currNumVariables ] = loadVariables( tr( "LHS" ), calculator->getLHSVariables(), retVal );
    numVariables += currNumVariables;
    if ( groupBox )
        formLayout->addWidget( groupBox );

    std::tie( groupBox, currNumVariables ) = loadVariables( tr( "RHS" ), calculator->getRHSVariables(), retVal );
    numVariables += currNumVariables;
    if ( groupBox )
        formLayout->addWidget( groupBox );

    return { retVal, numVariables };
}

std::pair< QGroupBox *, std::size_t > CCalculatorPage::loadVariables( const QString &name, const TVariableInfoList &variables, CCalculatorPage *page )
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

