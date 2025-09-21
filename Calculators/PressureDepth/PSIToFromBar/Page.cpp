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

    setValue( fImpl->psi, psi, newValues.value()[ 0 ] );
    setValue( fImpl->bar, bar, newValues.value()[ 1 ] );
}
