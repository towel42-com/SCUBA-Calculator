#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    addWidget( false, fImpl->depthFreshWater );
    addWidget( true, fImpl->depthSeaWater );
}

CPage::~CPage()
{
}

void CPage::updateValues( QWidget *changedWidget )
{
    auto freshWaterString = tr( "%1 fresh water = 1.03 x" ).arg( lengthUnit( false ) );
    fImpl->freshWaterLabel->setText( freshWaterString );

    auto saltWaterString = tr( "%1 salt water" ).arg( lengthUnit( false ) );
    fImpl->saltWaterLabel->setText( saltWaterString );

    auto depthFreshWater = ( changedWidget == fImpl->depthSeaWater ) ? std::optional< double >() : getValue( fImpl->depthFreshWater->text() );
    auto depthSeaWater = ( changedWidget == fImpl->depthFreshWater ) ? std::optional< double >() : getValue( fImpl->depthSeaWater->text() );

    auto newValues = calculator()->compute( { depthFreshWater, depthSeaWater } );
    if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
        return;

    if ( changedWidget != fImpl->depthFreshWater )
        setValue( fImpl->depthFreshWater, newValues.value()[ 0 ], 2 );
    if ( changedWidget != fImpl->depthSeaWater )
        setValue( fImpl->depthSeaWater, newValues.value()[ 1 ], 2 );
}
