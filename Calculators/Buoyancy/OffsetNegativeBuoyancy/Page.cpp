#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    addWidget( false, fImpl->volumeDisplaced );
    addWidgets( true, { fImpl->negativeBuoyancy } );
}

CPage::~CPage()
{
}

void CPage::updateValuesInternal( QWidget *changedWidget )
{
    fImpl->volumeDisplaced->setPlaceholderText( tr( "Volume (%1)" ).arg( volumeUnit( false, false ) ) );
    fImpl->volumeDisplacedUnits->setText( volumeUnit( false, false ) );

    fImpl->negativeBuoyancy->setPlaceholderText( tr( "Negative Buoyancy (%1)" ).arg( weightUnit( false, false ) ) );
    fImpl->negativeBuoyancyUnits->setText( weightUnit( false, false ) );

    auto volumeDisplaced = getValue( fImpl->volumeDisplaced->text() );
    auto negativeBuoyancy = getValue( fImpl->negativeBuoyancy->text() );

    std::size_t triggerPos = -1;
    if ( changedWidget == fImpl->volumeDisplaced )
        triggerPos = 0;
    else if ( changedWidget == fImpl->negativeBuoyancy )
        triggerPos = 1;

    auto newValues = calculator()->setupValues( updateFromRHS(), triggerPos, { volumeDisplaced, negativeBuoyancy } );
    if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
        return;

    newValues = calculator()->compute( newValues.value() );
    if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
        return;

    setValue( fImpl->volumeDisplaced, volumeDisplaced, newValues.value()[ 0 ] );
    setValue( fImpl->negativeBuoyancy, negativeBuoyancy, newValues.value()[ 1 ] );
}
