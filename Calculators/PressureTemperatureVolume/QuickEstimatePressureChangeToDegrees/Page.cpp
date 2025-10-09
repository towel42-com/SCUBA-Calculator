#include "Page.h"
#include "ui_Page.h"

CPage::CPage( CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    //addWidgets( false, { fImpl->t } );
    //addWidgets( true, { fImpl->p } );
}

CPage::~CPage()
{
}

void CPage::updateValuesInternal( QWidget *triggerWidget )
{
    (void)triggerWidget;
    //fImpl->tLabel->setText( tr( "(%1)" ).arg( NUtilities::NUnitStrings::tempUnit( imperial(), false ) ) );
    //fImpl->pLabel->setText( tr( "(%1)" ).arg( NUtilities::NUnitStrings::pressureUnit( imperial(), false ) ) );
    //fImpl->PperT->setText( tr( "%1" ).arg( NUtilities::NUnitStrings::pressurePerTemp( imperial(), false ) ) );
    //if ( triggerWidget == nullptr )
    //    return;

    //auto t = NUtilities::getValue( fImpl->t->text() );
    //auto p = NUtilities::getValue( fImpl->p->text() );

    //if ( NUtilities::numEmpty( { t, p } ) == 0 )
    //{
    //    if ( triggerWidget == fImpl->t )
    //        p.reset();
    //    else if ( triggerWidget == fImpl->p )
    //        t.reset();
    //}
    //auto newValues = calculator()->compute( { t, p } );
    //if ( !newValues.has_value() || ( newValues.value().size() != 2 ) )
    //    return;

    //NUtilities::setValue( fImpl->t, t, newValues.value()[ 0 ], 2 );
    //NUtilities::setValue( fImpl->p, p, newValues.value()[ 1 ], 2 );
}
