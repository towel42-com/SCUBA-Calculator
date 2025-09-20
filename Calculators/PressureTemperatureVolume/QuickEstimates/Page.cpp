#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    addWidgets( false, { fImpl->t } );
    addWidgets( true, { fImpl->p } );
}

CPage::~CPage()
{
}

void CPage::updateValues( QWidget *changedWidget )
{
    fImpl->tLabel->setText( tr( "(%1)" ).arg( tempUnit( false ) ) );
    fImpl->pLabel->setText( tr( "(%1)" ).arg( pressureUnit() ) );
    fImpl->PperT->setText( tr( "%1" ).arg( pressurePerTemp() ) );
    if ( changedWidget == nullptr )
        return;

    auto t = getValue( fImpl->t->text() );
    auto p = getValue( fImpl->p->text() );

    if ( calculator()->numEmpty( { t, p } ) == 0 )
    {
        if ( changedWidget == fImpl->t )
            p.reset();
        else if ( changedWidget == fImpl->p )
            t.reset();
    }
    auto newValues = calculator()->compute( { t, p } );
    if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
        return;

    if ( changedWidget != fImpl->t )
        setValue( fImpl->t, newValues.value()[ 0 ], 2 );
    if ( changedWidget != fImpl->p )
        setValue( fImpl->p, newValues.value()[ 1 ], 2 );
}
