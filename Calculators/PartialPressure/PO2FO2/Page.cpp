#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    addWidget( false, fImpl->po2 );
    addWidgets( true, { fImpl->p } );
}

CPage::~CPage()
{
}

void CPage::updateValues( QWidget *changedWidget )
{
    fImpl->pLabel->setText( tr( "(%1)" ).arg( pressureUnit() ) );
    fImpl->fo2Label->setText( tr( "%1%" ).arg( calculator()->percentO2AtSurface() ) );

    if ( changedWidget == nullptr )
        return;

    auto po2 = getValue( fImpl->po2->text() );
    auto p = getValue( fImpl->p->text() );

    if ( calculator()->numEmpty( { po2, p } ) == 0 )
    {
        if ( changedWidget == fImpl->po2 )
            p.reset();
        else
            po2.reset();
    }

    auto newValues = calculator()->compute( { po2, p } );
    if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
        return;

    if ( changedWidget != fImpl->po2 )
        setValue( fImpl->po2, newValues.value()[ 0 ], 2 );
    if ( changedWidget != fImpl->p )
        setValue( fImpl->p, newValues.value()[ 1 ], 2 );
}
