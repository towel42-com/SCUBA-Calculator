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

    addWidget( false, fImpl->volumeDisplaced );
    addWidgets( true, { fImpl->negativeBuoyancy } );
}

CPage::~CPage()
{
}

void CPage::updateValues( QWidget * /*changedWidget*/ )
{
    auto volumeUnitSingular = imperial() ? tr( "cu ft" ) : tr( "liter" );
    auto volumeUnitPlural = imperial() ? tr( "cu ft" ) : tr( "liters" );
    auto weightUnit = imperial() ? tr( "lb" ) : tr( "kg" );

    auto weightLabel = imperial() ? ( fImpl->saltwater->isChecked() ? tr( "64" ) : tr( "62.4" ) ) : ( fImpl->saltwater->isChecked() ? tr( "1.0" ) : tr( "1.03" ) );
    bool aOK;
    auto weightOfWater = weightLabel.toDouble( &aOK );
    Q_ASSERT( aOK );

    auto weightOfWaterLabel = tr( "(%3 %4/%5 of water)" ).arg( weightLabel ).arg( weightUnit ).arg( volumeUnitSingular );
    fImpl->weightOfWater->setText( weightOfWaterLabel );

    QString labelText = tr( "%1 required = (%2s of negative buoyancy) / %3" ).arg( volumeUnitPlural ).arg( weightUnit ).arg( weightOfWaterLabel );
    fImpl->eq->setText( labelText );

    fImpl->negativeBuoyancy->setPlaceholderText( tr( "Negative Buyoyancy (%1)" ).arg( weightUnit ) );
    fImpl->volumeDisplaced->setPlaceholderText( tr( "Volume (%1)" ).arg( volumeUnitPlural ) );

    auto negativeBuoyancy = getValue( fImpl->negativeBuoyancy->text() );
    auto volumeDisplaced = getValue( fImpl->volumeDisplaced->text() );

    int numEmpty = negativeBuoyancy.has_value() ? 0 : 1;
    numEmpty += volumeDisplaced.has_value() ? 0 : 1;
    if ( numEmpty == 0 )
    {
        if ( updateFromRHS() )
        {
            volumeDisplaced.reset();
            numEmpty = 1;
        }
        else
        {
            negativeBuoyancy.reset();
            numEmpty = 1;
        }
    }

    if ( numEmpty != 1 )
        return;

    if ( !volumeDisplaced.has_value() )
    {
        volumeDisplaced = negativeBuoyancy.value() / weightOfWater;
        fImpl->volumeDisplaced->setText( QString( "%1" ).arg( volumeDisplaced.value(), 0, 'f', 1 ) );
    }
    else if ( !negativeBuoyancy.has_value() )
    {
        negativeBuoyancy = volumeDisplaced.value() * weightOfWater;
        fImpl->negativeBuoyancy->setText( QString( "%1" ).arg( negativeBuoyancy.value(), 0, 'f', 1 ) );
    }
}
