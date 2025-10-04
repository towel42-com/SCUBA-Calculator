#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    addWidget( false, fImpl->v2 );
    addWidgets( true, { fImpl->t2, fImpl->v1, fImpl->t1 } );
}

CPage::~CPage()
{
}

void CPage::updateValuesInternal( QWidget *changedWidget )
{
    auto tempLabel = tr( "(%1) + %2" ).arg( tempUnit( false, false ) ).arg( doubleToString( calculator()->absZero(), 1 ) );
    fImpl->t1Label->setText( tempLabel );
    fImpl->t2Label->setText( tempLabel );

    fImpl->v1Label->setText( tr( "(%1)" ).arg( volumeUnit( false, false ) ) );
    fImpl->v2Label->setText( tr( "(%1)" ).arg( volumeUnit( false, false ) ) );

    if ( changedWidget == nullptr )
        return;

    auto t1 = getValue( fImpl->t1->text() );
    auto t2 = getValue( fImpl->t2->text() );
    auto v1 = getValue( fImpl->v1->text() );
    auto v2 = getValue( fImpl->v2->text() );

    if ( calculator()->numEmpty( { t1, t2, v1, v2 } ) == 0 )
    {
        if ( changedWidget == fImpl->v2 )
            v1.reset();
        else
            v2.reset();
    }

    auto newValues = calculator()->compute( { t1, t2, v1, v2 } );
    if ( !newValues.has_value() || ( newValues.value().size() != 4 ) )
        return;

    setValue( fImpl->t1, t1, newValues.value()[ 0 ], 2 );
    setValue( fImpl->t2, t2, newValues.value()[ 1 ], 2 );
    setValue( fImpl->v1, v1, newValues.value()[ 2 ], 2 );
    setValue( fImpl->v2, v2, newValues.value()[ 3 ], 2 );
}
