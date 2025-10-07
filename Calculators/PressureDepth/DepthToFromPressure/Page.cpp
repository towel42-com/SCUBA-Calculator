#include "Page.h"
#include "ui_Page.h"

CPage::CPage( CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    fImpl->saltwater->setChecked( true );
    QObject::connect( fImpl->saltwater, &QRadioButton::clicked, [ = ]() { updateValues( nullptr ); } );
    QObject::connect( fImpl->freshWater, &QRadioButton::clicked, [ = ]() { updateValues( nullptr ); } );

    //addWidget( false, fImpl->pressure );
    //addWidget( true, fImpl->depth );
}

CPage::~CPage()
{
}

void CPage::updateValuesInternal( QWidget *triggerWidget )
{
    (void)triggerWidget;
    //auto pressureString = tr( "Pressure (%1) = (" ).arg( NUtilities::NUnitStrings::pressureUnit( imperial(), false ) );
    //fImpl->pressureLabel->setText( pressureString );

    //auto depthString = tr( "(%1) + %2)/%2" ).arg( NUtilities::NUnitStrings::lengthUnit( imperial(), false, false ) ).arg( NUtilities::doubleToString( NUtilities::NConstants::depthToSingleAtmosphere( imperial(), saltWater() ), 1 ) );
    //fImpl->depthLabel->setText( depthString );

    //auto pressure = ( triggerWidget == fImpl->depth ) ? TOptionalDouble() : NUtilities::getValue( fImpl->pressure->text() );
    //auto depth = ( triggerWidget == fImpl->pressure ) ? TOptionalDouble() : NUtilities::getValue( fImpl->depth->text() );

    //if ( NUtilities::numEmpty( { pressure, depth } ) == 0 )
    //{
    //    if ( updateFromRHS() )
    //        depth.reset();
    //    else
    //        pressure.reset();
    //}

    //auto newValues = calculator()->compute( { saltWater(), pressure, depth } );
    //if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
    //    return;

    //NUtilities::setValue( fImpl->pressure, pressure, newValues.value()[ 0 ], 1 );
    //NUtilities::setValue( fImpl->depth, depth, newValues.value()[ 1 ], 1 );
}
