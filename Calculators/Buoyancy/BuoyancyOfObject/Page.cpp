#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    fImpl->saltwater->setChecked( true );
    QObject::connect( fImpl->saltwater, &QRadioButton::clicked, [ = ]() { updateValues( nullptr ); } );
    QObject::connect( fImpl->freshWater, &QRadioButton::clicked, [ = ]() { updateValues( nullptr ); } );

    addWidget( false, fImpl->buoyancy );
    addWidgets( true, { fImpl->volumeDisplaced, fImpl->weightOfObject } );
}

CPage::~CPage()
{
}

void CPage::updateValues( QWidget *changedWidget )
{
    auto weightOfWaterString = weightOfWater( fImpl->saltwater->isChecked() );
    fImpl->weightOfWater->setText( "x " + weightOfWaterString );

    QString labelText = tr( "Buoyancy = weight of object - (%1 displaced) x %2" ).arg( volumeUnit( false ) ).arg( weightOfWaterString );
    fImpl->eq->setText( labelText );

    fImpl->buoyancy->setPlaceholderText( tr( "Buoyancy (%1)" ).arg( weightUnit( false ) ) );
    fImpl->weightOfObject->setPlaceholderText( tr( "Weight (%1)" ).arg( weightUnit( false ) ) );
    fImpl->volumeDisplaced->setPlaceholderText( tr( "Volume (%1)" ).arg( volumeUnit( false ) ) );

    auto weightOfObject = getValue( fImpl->weightOfObject->text() );
    auto volumeDisplaced = getValue( fImpl->volumeDisplaced->text() );
    auto buoyancy = getValue( fImpl->buoyancy->text() );

    int numEmpty = weightOfObject.has_value() ? 0 : 1;
    numEmpty += volumeDisplaced.has_value() ? 0 : 1;
    numEmpty += buoyancy.has_value() ? 0 : 1;
    if ( numEmpty == 0 )
    {
        if ( updateFromRHS() )
        {
            buoyancy.reset();
            numEmpty = 1;
        }
        else
        {
            if ( changedWidget == fImpl->weightOfObject )
            {
                volumeDisplaced.reset();
                numEmpty = 1;
            }
            else
            {
                buoyancy.reset();
                numEmpty = 1;
            }
        }
    }

    auto newValues = calculator()->compute( { fImpl->saltwater->isChecked() ? 1.0 : 0.0, buoyancy, weightOfObject, volumeDisplaced } );
    if ( !newValues.has_value() || ( newValues.value().size() != 3 ) )
        return;

    setValue( fImpl->buoyancy, buoyancy, newValues.value()[ 0 ] );
    setValue( fImpl->weightOfObject, weightOfObject, newValues.value()[ 1 ] );
    setValue( fImpl->volumeDisplaced, volumeDisplaced, newValues.value()[ 2 ] );
}
