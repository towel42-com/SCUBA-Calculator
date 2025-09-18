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
    auto weightOfWaterString = this->weightOfWaterString( fImpl->saltwater->isChecked() );
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

    auto values = std::vector< std::optional< double > >( { fImpl->saltwater->isChecked() ? 1.0 : 0.0, buoyancy, weightOfObject, volumeDisplaced } );

    auto aOK = calculator()->compute( values );
    if ( !aOK )
        return;

    fImpl->weightOfObject->setText( QString( "%1" ).arg( weightOfObject.value(), 0, 'f', 1 ) );
    fImpl->volumeDisplaced->setText( QString( "%1" ).arg( volumeDisplaced.value(), 0, 'f', 1 ) );
    fImpl->buoyancy->setText( QString( "%1" ).arg( buoyancy.value(), 0, 'f', 1 ) );
}
