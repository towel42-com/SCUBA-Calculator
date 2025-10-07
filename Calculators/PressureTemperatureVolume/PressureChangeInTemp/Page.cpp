#include "Page.h"
#include "ui_Page.h"

CPage::CPage( CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    //addWidget( false, fImpl->p2 );
    //addWidgets( true, { fImpl->t2, fImpl->p1, fImpl->t1 } );
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

    //if ( triggerWidget == nullptr )
    //    return;

    //auto t1 = NUtilities::getValue( fImpl->t1->text() );
    //auto t2 = NUtilities::getValue( fImpl->t2->text() );
    //auto p1 = NUtilities::getValue( fImpl->p1->text() );
    //auto p2 = NUtilities::getValue( fImpl->p2->text() );

    //if ( NUtilities::numEmpty( { t1, t2, p1, p2 } ) == 0 )
    //{
    //    if ( triggerWidget == fImpl->p2 )
    //        p1.reset();
    //    else
    //        p2.reset();
    //}

    //auto newValues = calculator()->compute( { t1, t2, p1, p2 } );
    //if ( !newValues.has_value() || ( newValues.value().size() != 4 ) )
    //    return;

    //NUtilities::setValue( fImpl->t1, t1, newValues.value()[ 0 ], 2 );
    //NUtilities::setValue( fImpl->t2, t2, newValues.value()[ 1 ], 2 );
    //NUtilities::setValue( fImpl->p1, p1, newValues.value()[ 2 ], 2 );
    //NUtilities::setValue( fImpl->p2, p2, newValues.value()[ 3 ], 2 );
}
