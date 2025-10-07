#include "Page.h"
#include "ui_Page.h"

CPage::CPage( CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    //addWidget( false, fImpl->psi );
    //addWidget( true, fImpl->bar );
}

CPage::~CPage()
{
}

void CPage::updateValuesInternal( QWidget *triggerWidget )
{
    (void)triggerWidget;
    //auto psi = ( triggerWidget == fImpl->bar ) ? TOptionalDouble() : NUtilities::getValue( fImpl->psi->text() );
    //auto bar = ( triggerWidget == fImpl->psi ) ? TOptionalDouble() : NUtilities::getValue( fImpl->bar->text() );

    //auto newValues = calculator()->compute( { psi, bar } );
    //if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
    //    return;

    //NUtilities::setValue( fImpl->psi, psi, newValues.value()[ 0 ] );
    //NUtilities::setValue( fImpl->bar, bar, newValues.value()[ 1 ] );
}
