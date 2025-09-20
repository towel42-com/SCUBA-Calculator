#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    addWidgets( false, { fImpl->p, fImpl->v } );
    addWidgets( true, { fImpl->numMoles, fImpl->t } );
}

CPage::~CPage()
{
}

void CPage::updateValues( QWidget *changedWidget )
{
    auto tempLabel = tr( "(%1) + %2" ).arg( tempUnit( false ) ).arg( doubleToString( calculator()->absZero(), 1 ) );
    fImpl->tLabel->setText( tempLabel );
    fImpl->pLabel->setText( tr( "(%1)" ).arg( pressureUnit() ) );
    fImpl->vLabel->setText( tr( "(%1)" ).arg( volumeUnit( false ) ) );
    fImpl->idealGasConstant->setText( idealGasConstant() );

    if ( changedWidget == nullptr )
        return;

    auto p = getValue( fImpl->p->text() );
    auto v = getValue( fImpl->v->text() );
    auto numMoles = getValue( fImpl->numMoles->text() );
    auto t = getValue( fImpl->t->text() );

    if ( calculator()->numEmpty( { p, v, numMoles, t } ) == 0 )
    {
        if ( changedWidget == fImpl->p )
            v.reset();
        else if ( changedWidget == fImpl->v )
            t.reset();
        else if ( changedWidget == fImpl->numMoles )
            t.reset();
        else if ( changedWidget == fImpl->t )
            numMoles.reset();
    }

    auto newValues = calculator()->compute( { p, v, numMoles, t } );
    if ( !newValues.has_value() || ( newValues.value().size() != 4 ) )
        return;

    if ( changedWidget != fImpl->p )
        setValue( fImpl->p, newValues.value()[ 0 ], 2 );
    if ( changedWidget != fImpl->v )
        setValue( fImpl->v, newValues.value()[ 1 ], 2 );
    if ( changedWidget != fImpl->numMoles )
        setValue( fImpl->numMoles, newValues.value()[ 2 ], 2 );
    if ( changedWidget != fImpl->t )
        setValue( fImpl->t, newValues.value()[ 3 ], 2 );
}
