#include "Page.h"
#include "ui_Page.h"

CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );
    addWidgets( false, { fImpl->mod, fImpl->ambientPressure } );
    addWidgets( true, { fImpl->pressureForDepth, fImpl->po2 } );

    fImpl->saltwater->setChecked( true );
    QObject::connect( fImpl->saltwater, &QRadioButton::clicked, [ = ]() { updateValues( nullptr ); } );
    QObject::connect( fImpl->freshWater, &QRadioButton::clicked, [ = ]() { updateValues( nullptr ); } );
    QObject::connect( fImpl->maxPO2, &QDoubleSpinBox::valueChanged, [ = ]() { updateValues( fImpl->maxPO2 ); } );
}

CPage::~CPage()
{
}

void CPage::updateValues( QWidget *changedWidget )
{
    auto pressureString = tr( "Total Pressure (%1)" ).arg( pressureUnit() );
    fImpl->pressureLabel->setText( pressureString );
    fImpl->ambientPressureLabel->setText( pressureString );

    auto depthString = tr( "(%1) + %2)/%2" ).arg( lengthUnit( false ) ).arg( doubleToString( calculator()->depthToSingleAtmosphere( fImpl->saltwater->isChecked() ), 1 ) );
    fImpl->modLabel->setText( depthString );
    if ( changedWidget == nullptr )
        return;

    auto ambientPressure = getValue( fImpl->ambientPressure->text() );
    auto mod = getValue( fImpl->mod->text() );
    auto po2 = getValue( fImpl->po2->text() );

    if ( calculator()->numEmpty( { ambientPressure, po2 } ) == 0 )
    {
        if ( changedWidget == fImpl->ambientPressure )
        {
            mod.reset();
            po2.reset();
        }
        else if ( ( changedWidget == fImpl->po2 ) || ( changedWidget == fImpl->maxPO2 ) )
        {
            mod.reset();
            ambientPressure.reset();
        }
    }

    if ( calculator()->numEmpty( { ambientPressure, mod, po2 } ) == 0 )
    {
        if ( ( changedWidget == fImpl->ambientPressure ) || ( changedWidget == fImpl->po2 ) )
            mod.reset();
        else if ( changedWidget == fImpl->mod )
        {
            ambientPressure.reset();
            po2.reset();
        }
    }

    auto newValues = calculator()->compute( { fImpl->saltwater->isChecked() ? 1.0 : 0.0, fImpl->maxPO2->value(), ambientPressure, po2, mod } );
    if ( !newValues.has_value() || ( newValues.value().size() != 3 ) )
        return;

    setValue( fImpl->ambientPressure, ambientPressure, newValues.value()[ 0 ], 2 );
    setValue( fImpl->pressureForDepth, ambientPressure, newValues.value()[ 0 ], 2 );
    setValue( fImpl->po2, po2, newValues.value()[ 1 ], 2 );
    setValue( fImpl->mod, mod, newValues.value()[ 2 ], 2 );
}
