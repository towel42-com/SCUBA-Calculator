#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    addWidget( false, fImpl->buoyancy );
    addWidgets( true, { fImpl->volumeDisplaced, fImpl->weightOfObject } );
}

CPage::~CPage()
{
}

void CPage::updateValuesInternal( QWidget *changedWidget )
{
    fImpl->buoyancy->setPlaceholderText( tr( "Buoyancy (%1)" ).arg( weightUnit( false, false ) ) );
    fImpl->buoyuancyUnits->setText( weightUnit( false, false ) );
    fImpl->weightOfObject->setPlaceholderText( tr( "Weight (%1)" ).arg( weightUnit( false, false ) ) );
    fImpl->weightUnits->setText( weightUnit( false, false ) );
    fImpl->volumeDisplaced->setPlaceholderText( tr( "Volume (%1)" ).arg( volumeUnit( false, false ) ) );
    fImpl->volumeDisplacedUnits->setText( volumeUnit( false, false ) );

    auto weightOfObject = getValue( fImpl->weightOfObject->text() );
    auto volumeDisplaced = getValue( fImpl->volumeDisplaced->text() );
    auto buoyancy = getValue( fImpl->buoyancy->text() );

    std::size_t triggerPos = -1;
    if ( changedWidget == fImpl->buoyancy )
        triggerPos = 1;
    else if ( changedWidget == fImpl->weightOfObject )
        triggerPos = 2;
    else if ( changedWidget == fImpl->volumeDisplaced )
        triggerPos = 3;

    auto newValues = calculator()->setupValues( updateFromRHS(), triggerPos, { buoyancy, weightOfObject, volumeDisplaced } );
    if ( !newValues.has_value() || ( newValues.value().size() != 3 ) )
        return;
    
    newValues.value().insert( newValues.value().begin(), saltWater() );
    newValues = calculator()->compute( newValues.value() );
    if ( !newValues.has_value() || ( newValues.value().size() != 3 ) )
        return;

    setValue( fImpl->buoyancy, buoyancy, newValues.value()[ 0 ] );
    setValue( fImpl->weightOfObject, weightOfObject, newValues.value()[ 1 ] );
    setValue( fImpl->volumeDisplaced, volumeDisplaced, newValues.value()[ 2 ] );
}

