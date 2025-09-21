#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    fImpl->saltwater->setChecked( true );
    QObject::connect( fImpl->saltwater, &QRadioButton::clicked, [ = ]() { updateValues( nullptr ); } );
    QObject::connect( fImpl->freshWater, &QRadioButton::clicked, [ = ]() { updateValues( nullptr ); } );

    addWidget( false, fImpl->ead );
    addWidgets( true, { fImpl->fn2, fImpl->depth } );
}

CPage::~CPage()
{
}

void CPage::updateValues( QWidget *changedWidget )
{
    auto depthString = tr( "(%1) + %2) - %2" ).arg( lengthUnit( false ) ).arg( doubleToString( calculator()->depthToSingleAtmosphere( fImpl->saltwater->isChecked() ), 1 ) );
    fImpl->depthLabel->setText( depthString );
    fImpl->eadLabel->setText( tr( "(%1)" ).arg( lengthUnit( false ) ) );
    fImpl->fn2Label->setText( tr( "FN2%/%1%)" ).arg( doubleToString( calculator()->percentN2AtSurface(), 2 ) ) );
    if ( changedWidget == nullptr )
        return;

    auto ead = getValue( fImpl->ead->text() );
    auto fn2 = getValue( fImpl->fn2->text() );
    auto depth = getValue( fImpl->depth->text() );

    if ( calculator()->numEmpty( { ead, fn2, depth } ) == 0 )
    {
        if ( updateFromRHS() )
            ead.reset();
        else
            depth.reset();
    }

    auto newValues = calculator()->compute( { ( fImpl->saltwater->isChecked() ? 1 : 0 ), ead, fn2, depth } );
    if ( !newValues.has_value() || ( newValues.value().size() != 3 ) )
        return;

    setValue( fImpl->ead, ead, newValues.value()[ 0 ], 2 );
    setValue( fImpl->fn2, fn2, newValues.value()[ 1 ], 2 );
    setValue( fImpl->depth, depth, newValues.value()[ 2 ], 2 );
}
