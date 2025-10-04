#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    addWidget( false, fImpl->p2 );
    addWidgets( true, { fImpl->t2, fImpl->p1, fImpl->t1 } );
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

    if ( changedWidget == nullptr )
        return;

    auto t1 = getValue( fImpl->t1->text() );
    auto t2 = getValue( fImpl->t2->text() );
    auto p1 = getValue( fImpl->p1->text() );
    auto p2 = getValue( fImpl->p2->text() );

    if ( calculator()->numEmpty( { t1, t2, p1, p2 } ) == 0 )
    {
        if ( changedWidget == fImpl->p2 )
            p1.reset();
        else
            p2.reset();
    }

    auto newValues = calculator()->compute( { t1, t2, p1, p2 } );
    if ( !newValues.has_value() || ( newValues.value().size() != 4 ) )
        return;

    setValue( fImpl->t1, t1, newValues.value()[ 0 ], 2 );
    setValue( fImpl->t2, t2, newValues.value()[ 1 ], 2 );
    setValue( fImpl->p1, p1, newValues.value()[ 2 ], 2 );
    setValue( fImpl->p2, p2, newValues.value()[ 3 ], 2 );
}
