#include "Page.h"
#include "ui_Page.h"

CPage::CPage( CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    //addWidgets( false, { fImpl->p, fImpl->v } );
    //addWidgets( true, { fImpl->numMoles, fImpl->t } );
}

CPage::~CPage()
{
}

void CPage::updateValuesInternal( QWidget *triggerWidget )
{
    (void)triggerWidget;
    //auto tempLabel = tr( "(%1) + %2" ).arg( NUtilities::NUnitStrings::tempUnit( imperial(), false ) ).arg( NUtilities::doubleToString( NUtilities::NConstants::absZero( imperial() ), 1 ) );
    //fImpl->tLabel->setText( tempLabel );
    //fImpl->pLabel->setText( tr( "(%1)" ).arg( NUtilities::NUnitStrings::pressureUnit( imperial(), false ) ) );
    //fImpl->vLabel->setText( tr( "(%1)" ).arg( NUtilities::NUnitStrings::volumeUnit( imperial(), false ) ) );
    //fImpl->idealGasConstant->setText( NUtilities::NUnitStrings::idealGasConstant( imperial(), false ) );

    //if ( triggerWidget == nullptr )
    //    return;

    //auto p = NUtilities::getValue( fImpl->p->text() );
    //auto v = NUtilities::getValue( fImpl->v->text() );
    //auto numMoles = NUtilities::getValue( fImpl->numMoles->text() );
    //auto t = NUtilities::getValue( fImpl->t->text() );

    //if ( NUtilities::numEmpty( { p, v, numMoles, t } ) == 0 )
    //{
    //    if ( triggerWidget == fImpl->p )
    //        v.reset();
    //    else if ( triggerWidget == fImpl->v )
    //        t.reset();
    //    else if ( triggerWidget == fImpl->numMoles )
    //        t.reset();
    //    else if ( triggerWidget == fImpl->t )
    //        numMoles.reset();
    //}

    //auto newValues = calculator()->compute( { p, v, numMoles, t } );
    //if ( !newValues.has_value() || ( newValues.value().size() != 4 ) )
    //    return;

    //NUtilities::setValue( fImpl->p, p, newValues.value()[ 0 ], 2 );
    //NUtilities::setValue( fImpl->v, v, newValues.value()[ 1 ], 2 );
    //NUtilities::setValue( fImpl->numMoles, numMoles, newValues.value()[ 2 ], 2 );
    //NUtilities::setValue( fImpl->t, t, newValues.value()[ 3 ], 2 );
}
