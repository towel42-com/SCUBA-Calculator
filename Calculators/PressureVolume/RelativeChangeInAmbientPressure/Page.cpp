#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    addWidget( false, fImpl->relChange );
    addWidgets( true, { fImpl->p2, fImpl->p1 } );
}

CPage::~CPage()
{
}

void CPage::updateValuesInternal( QWidget *changedWidget )
{
    auto pressureLabel = tr( "(%1)" ).arg( pressureUnit( false ) );
    fImpl->p1Label->setText( pressureLabel );
    fImpl->p2Label->setText( pressureLabel );

    auto relChange = getValue( fImpl->relChange->text() );
    auto p1 = getValue( fImpl->p1->text() );
    auto p2 = getValue( fImpl->p2->text() );

    if ( changedWidget == nullptr )
        return;
    if ( calculator()->numEmpty( { relChange, p1, p2 } ) == 0 )
    {
        if ( changedWidget == fImpl->relChange )
            p2.reset();
        else
            relChange.reset();
    }

    auto newValues = calculator()->compute( { relChange, p1, p2 } );
    if ( !newValues.has_value() || ( newValues.value().size() != 3 ) )
        return;

    setValue( fImpl->relChange, relChange, newValues.value()[ 0 ], 2 );
    setValue( fImpl->p1, p1, newValues.value()[ 1 ], 2 );
    setValue( fImpl->p2, p2, newValues.value()[ 2 ], 2 );
}