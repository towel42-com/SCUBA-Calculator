#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    addWidgets( false, { fImpl->p1, fImpl->t1 } );
    addWidgets( true, { fImpl->p2, fImpl->t2 } );
}

CPage::~CPage()
{
}

void CPage::updateValues( QWidget *changedWidget )
{
    auto pressureLabel = tr( "(%1)" ).arg( pressureUnit() );
    fImpl->p1Label->setText( pressureLabel );
    fImpl->p2Label->setText( pressureLabel );
    if ( changedWidget == nullptr )
        return;
    auto p1 = getValue( fImpl->p1->text() );
    auto t1 = getValue( fImpl->t1->text() );
    auto p2 = getValue( fImpl->p2->text() );
    auto t2 = getValue( fImpl->t2->text() );

    if ( calculator()->numEmpty( { p1, t1, p2, t2 } ) == 0 )
    {
        if ( changedWidget == fImpl->t1 )
            p1.reset();
        else if ( changedWidget == fImpl->p1 )
            t1.reset();
        else if ( changedWidget == fImpl->t2 )
            p2.reset();
        else if ( changedWidget == fImpl->p2 )
            t2.reset();
    }

    auto newValues = calculator()->compute( { p1, t1, p2, t2 } );
    if ( !newValues.has_value() || ( newValues.value().size() != 4 ) )
        return;

    setValue( fImpl->p1, p1, newValues.value()[ 0 ], 2 );
    setValue( fImpl->t1, t1, newValues.value()[ 1 ], 2 );
    setValue( fImpl->p2, p2, newValues.value()[ 2 ], 2 );
    setValue( fImpl->t2, t2, newValues.value()[ 3 ], 2 );
}
