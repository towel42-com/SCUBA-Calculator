#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    addWidget( false, fImpl->psi );
    addWidget( true, fImpl->bar );
}

CPage::~CPage()
{
}

void CPage::updateValues( QWidget *changedWidget )
{
    auto psi = ( changedWidget == fImpl->bar ) ? std::optional< double >() : getValue( fImpl->psi->text() );
    auto bar = ( changedWidget == fImpl->psi ) ? std::optional< double >() : getValue( fImpl->bar->text() );

    auto newValues = calculator()->compute( { psi, bar } );
    if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
        return;

    if ( changedWidget != fImpl->psi )
        setValue( fImpl->psi, newValues.value()[ 0 ] );
    if ( changedWidget != fImpl->bar )
        setValue( fImpl->bar, newValues.value()[ 1 ] );
}
