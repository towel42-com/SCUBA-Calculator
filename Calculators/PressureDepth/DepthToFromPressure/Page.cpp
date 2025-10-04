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

    addWidget( false, fImpl->pressure );
    addWidget( true, fImpl->depth );
}

CPage::~CPage()
{
}

void CPage::updateValuesInternal( QWidget *changedWidget )
{
    auto pressureString = tr( "Pressure (%1) = (" ).arg( pressureUnit( false ) );
    fImpl->pressureLabel->setText( pressureString );

    auto depthString = tr( "(%1) + %2)/%2" ).arg( lengthUnit( false, false ) ).arg( doubleToString( calculator()->depthToSingleAtmosphere( saltWater() ), 1 ) );
    fImpl->depthLabel->setText( depthString );

    auto pressure = ( changedWidget == fImpl->depth ) ? TOptionalVariant() : getValue( fImpl->pressure->text() );
    auto depth = ( changedWidget == fImpl->pressure ) ? TOptionalVariant() : getValue( fImpl->depth->text() );

    if ( calculator()->numEmpty( { pressure, depth } ) == 0 )
    {
        if ( updateFromRHS() )
            depth.reset();
        else
            pressure.reset();
    }

    auto newValues = calculator()->compute( { saltWater(), pressure, depth } );
    if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
        return;

    setValue( fImpl->pressure, pressure, newValues.value()[ 0 ], 1 );
    setValue( fImpl->depth, depth, newValues.value()[ 1 ], 1 );
}
