#include "Page.h"
#include "ui_Page.h"

CPage::CPage( QWidget *parent ) :
    CSCUBACalculatorPage( parent ),
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
    auto volumeUnitSingular = imperial() ? tr( "cu ft" ) : tr( "liter" );
    auto volumeUnitPlural = imperial() ? tr( "cu ft" ) : tr( "liters" );
    auto weightUnit = imperial() ? tr( "lb" ) : tr( "kg" );

    auto weightLabel = imperial() ? ( fImpl->saltwater->isChecked() ? tr( "64" ) : tr( "62.4" ) ) : ( fImpl->saltwater->isChecked() ? tr( "1.0" ) : tr( "1.03" ) );
    bool aOK;
    auto weightOfWater = weightLabel.toDouble( &aOK );
    Q_ASSERT( aOK );

    auto weightOfWaterLabel = tr( "(%3 %4/%5 of water)" ).arg( weightLabel ).arg( weightUnit ).arg( volumeUnitSingular );
    fImpl->weightOfWater->setText( "x " + weightOfWaterLabel );

    QString labelText = tr( "Buoyancy = weight of object - (%1 displaced) x %2" ).arg( volumeUnitPlural ).arg( weightOfWaterLabel );
    fImpl->eq->setText( labelText );

    fImpl->buoyancy->setPlaceholderText( tr( "Buoyancy (%1)" ).arg( weightUnit ) );
    fImpl->weightOfObject->setPlaceholderText( tr( "Weight (%1)" ).arg( weightUnit ) );
    fImpl->volumeDisplaced->setPlaceholderText( tr( "Volume (%1)" ).arg( volumeUnitPlural ) );

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

    if ( numEmpty != 1 )
        return;

    if ( !weightOfObject.has_value() )
    {
        weightOfObject = buoyancy.value() + ( volumeDisplaced.value() * weightOfWater );
        fImpl->weightOfObject->setText( QString( "%1" ).arg( weightOfObject.value(), 0, 'f', 1 ) );
    }
    else if ( !volumeDisplaced.has_value() )
    {
        volumeDisplaced = ( weightOfObject.value() - buoyancy.value() ) / weightOfWater;
        fImpl->volumeDisplaced->setText( QString( "%1" ).arg( volumeDisplaced.value(), 0, 'f', 1 ) );
    }
    else if ( !buoyancy.has_value() )
    {
        buoyancy = weightOfObject.value() - ( volumeDisplaced.value() * weightOfWater );
        fImpl->buoyancy->setText( QString( "%1" ).arg( buoyancy.value(), 0, 'f', 1 ) );
    }
}
