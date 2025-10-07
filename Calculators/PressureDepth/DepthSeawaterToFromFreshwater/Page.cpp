#include "Page.h"
#include "ui_Page.h"

CPage::CPage( CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    //addWidget( false, fImpl->depthFreshWater );
    //addWidget( true, fImpl->depthSeaWater );
}

CPage::~CPage()
{
}

void CPage::updateValuesInternal( QWidget *triggerWidget )
{
    (void)triggerWidget;
    //auto freshWaterString = tr( "%1 fresh water = 1.03 x" ).arg( NUtilities::NUnitStrings::lengthUnit( imperial(), false, false ) );
    //fImpl->freshWaterLabel->setText( freshWaterString );

    //auto saltWaterString = tr( "%1 salt water" ).arg( NUtilities::NUnitStrings::lengthUnit( imperial(), false, false ) );
    //fImpl->saltWaterLabel->setText( saltWaterString );

    //auto depthFreshWater = ( triggerWidget == fImpl->depthSeaWater ) ? TOptionalDouble() : NUtilities::getValue( fImpl->depthFreshWater->text() );
    //auto depthSeaWater = ( triggerWidget == fImpl->depthFreshWater ) ? TOptionalDouble() : NUtilities::getValue( fImpl->depthSeaWater->text() );

    //auto newValues = calculator()->compute( { depthFreshWater, depthSeaWater } );
    //if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
    //    return;

    //NUtilities::setValue( fImpl->depthFreshWater, depthFreshWater, newValues.value()[ 0 ], 2 );
    //NUtilities::setValue( fImpl->depthSeaWater, depthSeaWater, newValues.value()[ 1 ], 2 );
}
