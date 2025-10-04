#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    addWidgets( false, { fImpl->p1, fImpl->v1, fImpl->t1 } );
    addWidgets( true, { fImpl->p2, fImpl->v2, fImpl->t2 } );
}

CPage::~CPage()
{
}

void CPage::updateValuesInternal( QWidget *changedWidget )
{
    auto tempLabel = tr( "(%1) + %2" ).arg( tempUnit( false, false ) ).arg( doubleToString( calculator()->absZero(), 1 ) );
    fImpl->t1Label->setText( tempLabel );
    fImpl->t2Label->setText( tempLabel );

    fImpl->p1Label->setText( tr( "(%1)" ).arg( pressureUnit( false ) ) );
    fImpl->p2Label->setText( tr( "(%1)" ).arg( pressureUnit( false ) ) );

    fImpl->v1Label->setText( tr( "(%1)" ).arg( volumeUnit( false, false ) ) );
    fImpl->v2Label->setText( tr( "(%1)" ).arg( volumeUnit( false, false ) ) );

    if ( changedWidget == nullptr )
        return;

    auto t1 = getValue( fImpl->t1->text() );
    auto t2 = getValue( fImpl->t2->text() );
    auto v1 = getValue( fImpl->v1->text() );
    auto v2 = getValue( fImpl->v2->text() );
    auto p1 = getValue( fImpl->p1->text() );
    auto p2 = getValue( fImpl->p2->text() );

    if ( calculator()->numEmpty( { p1, p2, t1, t2, v1, v2 } ) == 0 )
    {
        if ( changedWidget == fImpl->t1 )
            t2.reset();
        else if ( changedWidget == fImpl->t2 )
            t1.reset();

        if ( changedWidget == fImpl->p1 )
            p2.reset();
        else if ( changedWidget == fImpl->p2 )
            p1.reset();

        if ( changedWidget == fImpl->v1 )
            v2.reset();
        else if ( changedWidget == fImpl->v2 )
            v1.reset();
    }

    auto newValues = calculator()->compute( { t1, t2, v1, v2, p1, p2 } );
    if ( !newValues.has_value() || ( newValues.value().size() != 6 ) )
        return;

    setValue( fImpl->t1, t1, newValues.value()[ 0 ], 2 );
    setValue( fImpl->t2, t2, newValues.value()[ 1 ], 2 );
    setValue( fImpl->v1, v1, newValues.value()[ 2 ], 2 );
    setValue( fImpl->v2, v2, newValues.value()[ 3 ], 2 );
    setValue( fImpl->p1, p1, newValues.value()[ 4 ], 2 );
    setValue( fImpl->p2, p2, newValues.value()[ 5 ], 2 );
}
