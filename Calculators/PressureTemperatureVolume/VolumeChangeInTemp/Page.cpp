#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator * calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );

    fImpl->saltwater->setChecked( true );
    QObject::connect( fImpl->saltwater, &QRadioButton::clicked, [ = ]() { updateValues( nullptr ); } );
    QObject::connect( fImpl->freshWater, &QRadioButton::clicked, [ = ]() { updateValues( nullptr ); } );

    //addWidget( false, fImpl->buoyancy );
    //addWidgets( true, { fImpl->volumeDisplaced, fImpl->weightOfObject } );
}

CPage::~CPage()
{
}

void CPage::updateValues( QWidget *changedWidget )
{
    (void)changedWidget;
}
    