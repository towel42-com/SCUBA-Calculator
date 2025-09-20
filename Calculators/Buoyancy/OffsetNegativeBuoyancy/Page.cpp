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

    addWidget( false, fImpl->volumeDisplaced );
    addWidgets( true, { fImpl->negativeBuoyancy } );
}

CPage::~CPage()
{
}

void CPage::updateValues( QWidget * changedWidget )
{
    auto weightOfWaterString = weightOfWater( fImpl->saltwater->isChecked() );
    fImpl->weightOfWater->setText( "x " + weightOfWaterString );

    QString labelText = tr( "%1 required = (%2 of negative buoyancy) / %3" ).arg( volumeUnit( false ) ).arg( weightUnit( false ) ).arg( weightOfWaterString );
    fImpl->eq->setText( labelText );

    fImpl->negativeBuoyancy->setPlaceholderText( tr( "Negative Buyoyancy (%1)" ).arg( weightUnit( false ) ) );
    fImpl->volumeDisplaced->setPlaceholderText( tr( "Volume (%1)" ).arg( volumeUnit( false ) ) );

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

    auto newValues = calculator()->compute( { fImpl->saltwater->isChecked() ? 1.0 : 0.0, volumeDisplaced, negativeBuoyancy } );
    if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
        return;

    if ( changedWidget != fImpl->volumeDisplaced )
        setValue( fImpl->volumeDisplaced, newValues.value()[ 0 ] );
    if ( changedWidget != fImpl->negativeBuoyancy )
        setValue( fImpl->negativeBuoyancy, newValues.value()[ 1 ] );
}
