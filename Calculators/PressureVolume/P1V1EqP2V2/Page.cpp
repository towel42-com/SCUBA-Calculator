#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    addWidgets( false, { fImpl->p1, fImpl->v1 } );
    addWidgets( true, { fImpl->p2, fImpl->v2 } );
}

CPage::~CPage()
{
}

void CPage::updateValues( QWidget *changedWidget )
{
    auto pressureLabel = tr( "(%1)" ).arg( pressureUnit() );
    fImpl->p1Label->setText( pressureLabel );
    fImpl->p2Label->setText( pressureLabel );

    auto volumeLabel = tr( "(%1)" ).arg( volumeUnit( false ) );
    fImpl->v1Label->setText( volumeLabel );
    fImpl->v2Label->setText( volumeLabel );

    if ( changedWidget == nullptr )
        return;

    auto p1 = getValue( fImpl->p1->text() );
    auto v1 = getValue( fImpl->v1->text() );
    auto p2 = getValue( fImpl->p2->text() );
    auto v2 = getValue( fImpl->v2->text() );

    if ( calculator()->numEmpty( { p1, v1, p2, v2 } ) == 0 )
    {
        if ( changedWidget == fImpl->v1 )
            p1.reset();
        else if ( changedWidget == fImpl->p1 )
            v1.reset();
        else if ( changedWidget == fImpl->v2 )
            p2.reset();
        else if ( changedWidget == fImpl->p2 )
            v2.reset();
    }

    auto newValues = calculator()->compute( { p1, v1, p2, v2 } );
    if ( !newValues.has_value() || ( newValues.value().size() != 4 ) )
        return;

    if ( changedWidget != fImpl->p1 )
        setValue( fImpl->p1, newValues.value()[ 0 ], 2 );
    if ( changedWidget != fImpl->v1 )
        setValue( fImpl->v1, newValues.value()[ 1 ], 2 );
    if ( changedWidget != fImpl->p2 )
        setValue( fImpl->p2, newValues.value()[ 2 ], 2 );
    if ( changedWidget != fImpl->v2 )
        setValue( fImpl->v2, newValues.value()[ 3 ], 2 );
}
