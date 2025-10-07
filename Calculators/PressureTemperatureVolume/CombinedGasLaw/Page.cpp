#include "Page.h"
#include "ui_Page.h"

CPage::CPage( CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    //addWidgets( false, { fImpl->p1, fImpl->v1, fImpl->t1 } );
    //addWidgets( true, { fImpl->p2, fImpl->v2, fImpl->t2 } );
}

CPage::~CPage()
{
}

void CPage::updateValuesInternal( QWidget *triggerWidget )
{
    (void)triggerWidget;
    //auto tempLabel = tr( "(%1) + %2" ).arg( NUtilities::NUnitStrings::tempUnit( imperial(), false ) ).arg( NUtilities::doubleToString( NUtilities::NConstants::absZero( imperial() ), 1 ) );
    //fImpl->t1Label->setText( tempLabel );
    //fImpl->t2Label->setText( tempLabel );

    //fImpl->p1Label->setText( tr( "(%1)" ).arg( NUtilities::NUnitStrings::pressureUnit( imperial(), false ) ) );
    //fImpl->p2Label->setText( tr( "(%1)" ).arg( NUtilities::NUnitStrings::pressureUnit( imperial(), false ) ) );

    //fImpl->v1Label->setText( tr( "(%1)" ).arg( NUtilities::NUnitStrings::volumeUnit( imperial(), false ) ) );
    //fImpl->v2Label->setText( tr( "(%1)" ).arg( NUtilities::NUnitStrings::volumeUnit( imperial(), false ) ) );

    //if ( triggerWidget == nullptr )
    //    return;

    //auto t1 = NUtilities::getValue( fImpl->t1->text() );
    //auto t2 = NUtilities::getValue( fImpl->t2->text() );
    //auto v1 = NUtilities::getValue( fImpl->v1->text() );
    //auto v2 = NUtilities::getValue( fImpl->v2->text() );
    //auto p1 = NUtilities::getValue( fImpl->p1->text() );
    //auto p2 = NUtilities::getValue( fImpl->p2->text() );

    //if ( NUtilities::numEmpty( { p1, p2, t1, t2, v1, v2 } ) == 0 )
    //{
    //    if ( triggerWidget == fImpl->t1 )
    //        t2.reset();
    //    else if ( triggerWidget == fImpl->t2 )
    //        t1.reset();

    //    if ( triggerWidget == fImpl->p1 )
    //        p2.reset();
    //    else if ( triggerWidget == fImpl->p2 )
    //        p1.reset();

    //    if ( triggerWidget == fImpl->v1 )
    //        v2.reset();
    //    else if ( triggerWidget == fImpl->v2 )
    //        v1.reset();
    //}

    //auto newValues = calculator()->compute( { t1, t2, v1, v2, p1, p2 } );
    //if ( !newValues.has_value() || ( newValues.value().size() != 6 ) )
    //    return;

    //NUtilities::setValue( fImpl->t1, t1, newValues.value()[ 0 ], 2 );
    //NUtilities::setValue( fImpl->t2, t2, newValues.value()[ 1 ], 2 );
    //NUtilities::setValue( fImpl->v1, v1, newValues.value()[ 2 ], 2 );
    //NUtilities::setValue( fImpl->v2, v2, newValues.value()[ 3 ], 2 );
    //NUtilities::setValue( fImpl->p1, p1, newValues.value()[ 4 ], 2 );
    //NUtilities::setValue( fImpl->p2, p2, newValues.value()[ 5 ], 2 );
}
