#include "Page.h"
#include "ui_Page.h"

CPage::CPage( CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    //addWidgets( false, { fImpl->p1, fImpl->v1 } );
    //addWidgets( true, { fImpl->p2, fImpl->v2 } );
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

    //auto volumeLabel = tr( "(%1)" ).arg( NUtilities::NUnitStrings::volumeUnit( imperial(), false ) );
    //fImpl->v1Label->setText( volumeLabel );
    //fImpl->v2Label->setText( volumeLabel );

    //if ( triggerWidget == nullptr )
    //    return;

    //auto p1 = NUtilities::getValue( fImpl->p1->text() );
    //auto v1 = NUtilities::getValue( fImpl->v1->text() );
    //auto p2 = NUtilities::getValue( fImpl->p2->text() );
    //auto v2 = NUtilities::getValue( fImpl->v2->text() );

    //if ( NUtilities::numEmpty( { p1, v1, p2, v2 } ) == 0 )
    //{
    //    if ( triggerWidget == fImpl->v1 )
    //        p1.reset();
    //    else if ( triggerWidget == fImpl->p1 )
    //        v1.reset();
    //    else if ( triggerWidget == fImpl->v2 )
    //        p2.reset();
    //    else if ( triggerWidget == fImpl->p2 )
    //        v2.reset();
    //}

    //auto newValues = calculator()->compute( { p1, v1, p2, v2 } );
    //if ( !newValues.has_value() || ( newValues.value().size() != 4 ) )
    //    return;

    //NUtilities::setValue( fImpl->p1, p1, newValues.value()[ 0 ], 2 );
    //NUtilities::setValue( fImpl->v1, v1, newValues.value()[ 1 ], 2 );
    //NUtilities::setValue( fImpl->p2, p2, newValues.value()[ 2 ], 2 );
    //NUtilities::setValue( fImpl->v2, v2, newValues.value()[ 3 ], 2 );
}
