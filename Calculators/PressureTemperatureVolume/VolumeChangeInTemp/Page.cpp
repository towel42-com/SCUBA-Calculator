#include "Page.h"
#include "ui_Page.h"

CPage::CPage( CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    //addWidget( false, fImpl->v2 );
    //addWidgets( true, { fImpl->t2, fImpl->v1, fImpl->t1 } );
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

    //fImpl->v1Label->setText( tr( "(%1)" ).arg( NUtilities::NUnitStrings::volumeUnit( imperial(), false ) ) );
    //fImpl->v2Label->setText( tr( "(%1)" ).arg( NUtilities::NUnitStrings::volumeUnit( imperial(), false ) ) );

    //if ( triggerWidget == nullptr )
    //    return;

    //auto t1 = NUtilities::getValue( fImpl->t1->text() );
    //auto t2 = NUtilities::getValue( fImpl->t2->text() );
    //auto v1 = NUtilities::getValue( fImpl->v1->text() );
    //auto v2 = NUtilities::getValue( fImpl->v2->text() );

    //if ( NUtilities::numEmpty( { t1, t2, v1, v2 } ) == 0 )
    //{
    //    if ( triggerWidget == fImpl->v2 )
    //        v1.reset();
    //    else
    //        v2.reset();
    //}

    //auto newValues = calculator()->compute( { t1, t2, v1, v2 } );
    //if ( !newValues.has_value() || ( newValues.value().size() != 4 ) )
    //    return;

    //NUtilities::setValue( fImpl->t1, t1, newValues.value()[ 0 ], 2 );
    //NUtilities::setValue( fImpl->t2, t2, newValues.value()[ 1 ], 2 );
    //NUtilities::setValue( fImpl->v1, v1, newValues.value()[ 2 ], 2 );
    //NUtilities::setValue( fImpl->v2, v2, newValues.value()[ 3 ], 2 );
}
