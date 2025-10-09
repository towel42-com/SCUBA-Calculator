#include "Page.h"
#include "ui_Page.h"

CPage::CPage( CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    //addWidgets( false, { fImpl->p1, fImpl->t1 } );
    //addWidgets( true, { fImpl->p2, fImpl->t2 } );
}

CPage::~CPage()
{
}

void CPage::updateValuesInternal( QWidget *triggerWidget )
{
    (void)triggerWidget;
    //auto pressureLabel = tr( "(%1)" ).arg( NUtilities::NUnitStrings::pressureUnit( imperial(), false ) );
    //fImpl->p1Label->setText( pressureLabel );
    //fImpl->p2Label->setText( pressureLabel );
    //if ( triggerWidget == nullptr )
    //    return;
    //auto p1 = NUtilities::getValue( fImpl->p1->text() );
    //auto t1 = NUtilities::getValue( fImpl->t1->text() );
    //auto p2 = NUtilities::getValue( fImpl->p2->text() );
    //auto t2 = NUtilities::getValue( fImpl->t2->text() );

    //if ( NUtilities::numEmpty( { p1, t1, p2, t2 } ) == 0 )
    //{
    //    if ( triggerWidget == fImpl->t1 )
    //        p1.reset();
    //    else if ( triggerWidget == fImpl->p1 )
    //        t1.reset();
    //    else if ( triggerWidget == fImpl->t2 )
    //        p2.reset();
    //    else if ( triggerWidget == fImpl->p2 )
    //        t2.reset();
    //}

    //auto newValues = calculator()->compute( { p1, t1, p2, t2 } );
    //if ( !newValues.has_value() || ( newValues.value().size() != 4 ) )
    //    return;

    //NUtilities::setValue( fImpl->p1, p1, newValues.value()[ 0 ], 2 );
    //NUtilities::setValue( fImpl->t1, t1, newValues.value()[ 1 ], 2 );
    //NUtilities::setValue( fImpl->p2, p2, newValues.value()[ 2 ], 2 );
    //NUtilities::setValue( fImpl->t2, t2, newValues.value()[ 3 ], 2 );
}
