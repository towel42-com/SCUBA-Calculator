#include "Page.h"
#include "ui_Page.h"

CPage::CPage( CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    //addWidget( false, fImpl->feet );
    //addWidget( true, fImpl->meters );
}

CPage::~CPage()
{
}

void CPage::updateValuesInternal( QWidget *triggerWidget )
{
    (void)triggerWidget;
    //auto feet = ( triggerWidget == fImpl->meters ) ? TOptionalDouble() : NUtilities::getValue( fImpl->feet->text() );
    //auto meters = ( triggerWidget == fImpl->feet ) ? TOptionalDouble() : NUtilities::getValue( fImpl->meters->text() );

    //auto newValues = calculator()->compute( { feet, meters } );
    //if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
    //    return;

    //NUtilities::setValue( fImpl->feet, feet, newValues.value()[ 0 ] );
    //NUtilities::setValue( fImpl->meters, meters, newValues.value()[ 1 ] );
}
