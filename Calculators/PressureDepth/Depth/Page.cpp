#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    addWidget( false, fImpl->feet );
    addWidget( true, fImpl->meters );
}

CPage::~CPage()
{
}

void CPage::updateValues( QWidget *changedWidget )
{
    auto feet = ( changedWidget == fImpl->meters ) ? std::optional< double >() : getValue( fImpl->feet->text() );
    auto meters = ( changedWidget == fImpl->feet ) ? std::optional< double >() : getValue( fImpl->meters->text() );

    auto newValues = calculator()->compute( { feet, meters } );
    if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
        return;

    setValue( fImpl->feet, feet, newValues.value()[ 0 ] );
    setValue( fImpl->meters, meters, newValues.value()[ 1 ] );
}
