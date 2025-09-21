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

void CPage::updateValues( QWidget *changedWidget )
{
    auto pressureString = tr( "Pressure (%1) = (" ).arg( pressureUnit() );
    fImpl->pressureLabel->setText( pressureString );

    auto depthString = tr( "(%1) + %2)/%2" ).arg( lengthUnit( false ) ).arg( doubleToString( calculator()->depthToSingleAtmosphere( fImpl->saltwater->isChecked() ), 1 ) );
    fImpl->depthLabel->setText( depthString );

    auto pressure = ( changedWidget == fImpl->depth ) ? std::optional< double >() : getValue( fImpl->pressure->text() );
    auto depth = ( changedWidget == fImpl->pressure ) ? std::optional< double >() : getValue( fImpl->depth->text() );

    if ( calculator()->numEmpty( { pressure, depth } ) == 0 )
    {
        if ( updateFromRHS() )
            depth.reset();
        else
            pressure.reset();
    }

    auto newValues = calculator()->compute( { ( fImpl->saltwater->isChecked() ? 1 : 0 ), pressure, depth } );
    if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
        return;

    if ( changedWidget != fImpl->pressure )
        setValue( fImpl->pressure, newValues.value()[ 0 ], 1 );
    if ( changedWidget != fImpl->depth )
        setValue( fImpl->depth, newValues.value()[ 1 ], 1 );
}
