#include "Page.h"
#include "ui_Page.h"

#include <QButtonGroup>
CPage::CPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );
    addWidgets( false, { fImpl->pressureForDepth, fImpl->partialPressure } );
    addWidgets( true, { fImpl->depthForPressure, fImpl->surfacePercentage } );

    auto bg = new QButtonGroup( this );
    bg->addButton( fImpl->saltwater );
    bg->addButton( fImpl->freshWater );

    bg = new QButtonGroup( this );
    bg->addButton( fImpl->oxygen );
    bg->addButton( fImpl->nitrogen );
    bg->addButton( fImpl->other );

    fImpl->saltwater->setChecked( true );
    fImpl->oxygen->setChecked( true );
    updateSurfacePercentage();

    QObject::connect( fImpl->oxygen, &QRadioButton::clicked, [ = ]() { updateSurfacePercentage(); } );
    QObject::connect( fImpl->nitrogen, &QRadioButton::clicked, [ = ]() { updateSurfacePercentage(); } );
    QObject::connect( fImpl->other, &QRadioButton::clicked, [ = ]() { updateSurfacePercentage(); } );

    QObject::connect( fImpl->saltwater, &QRadioButton::clicked, [ = ]() { updateValues( nullptr ); } );
    QObject::connect( fImpl->freshWater, &QRadioButton::clicked, [ = ]() { updateValues( nullptr ); } );
}

CPage::~CPage()
{
}

void CPage::updateSurfacePercentage()
{
    if ( fImpl->oxygen->isChecked() )
    {
        setValue( fImpl->surfacePercentage, {}, 0.21, 2 );
        fImpl->surfacePercentageLabel->setText( tr( "FO2 (%)" ) );
        fImpl->partialPressureLabel->setText( tr( "PO2 (%)" ) );
        fImpl->surfacePercentage->setReadOnly( true );
        fImpl->surfacePercentage->setEnabled( false );
    }
    else if ( fImpl->nitrogen->isChecked() )
    {
        setValue( fImpl->surfacePercentage, {}, 0.79, 2 );
        fImpl->surfacePercentageLabel->setText( tr( "FN2 (%)" ) );
        fImpl->partialPressureLabel->setText( tr( "PN2 (%)" ) );
        fImpl->surfacePercentage->setReadOnly( true );
        fImpl->surfacePercentage->setEnabled( false );
    }
    else   // if ( fImpl->other->isChecked() )
    {
        fImpl->surfacePercentageLabel->setText( tr( "(%)" ) );
        fImpl->partialPressureLabel->setText( tr( "(%)" ) );
        fImpl->surfacePercentage->setReadOnly( false );
        fImpl->surfacePercentage->setEnabled( true );
    }
    updateValues( fImpl->surfacePercentage );
}

void CPage::updateValuesInternal( QWidget *changedWidget )
{
    auto pressureString = tr( "Pressure (%1)" ).arg( pressureUnit( false ) );
    fImpl->pressureLabel->setText( pressureString );
    fImpl->pLabel->setText( pressureString );

    auto depthString = tr( "(%1) + %2)/%2" ).arg( lengthUnit( false, false ) ).arg( doubleToString( calculator()->depthToSingleAtmosphere( saltWater() ), 1 ) );
    fImpl->depthLabel->setText( depthString );

    fImpl->depthLabel->setText( tr( "(%1)" ).arg( lengthUnit( false, false ) ) );
    if ( changedWidget == nullptr )
        return;

    auto pressureForDepth = getValue( fImpl->pressureForDepth->text() );
    auto depthForPressure = getValue( fImpl->depthForPressure->text() );
    auto partialPressure = getValue( fImpl->partialPressure->text() );
    auto surfacePercentage = getValue( fImpl->surfacePercentage->text() );

    if ( calculator()->numEmpty( { pressureForDepth, depthForPressure } ) == 0 )
    {
        if ( changedWidget == fImpl->pressureForDepth )
        {
            depthForPressure.reset();
            partialPressure.reset();
        }
        else if ( changedWidget == fImpl->depthForPressure )
        {
            pressureForDepth.reset();
            partialPressure.reset();
        }
    }

    if ( calculator()->numEmpty( { pressureForDepth, depthForPressure, partialPressure, surfacePercentage } ) == 0 )
    {
        if ( ( changedWidget == fImpl->pressureForDepth ) || ( changedWidget == fImpl->depthForPressure ) )
            partialPressure.reset();
        else if ( changedWidget == fImpl->partialPressure )
        {
            depthForPressure.reset();
            pressureForDepth.reset();
        }
        else if ( changedWidget == fImpl->surfacePercentage )
            partialPressure.reset();
    }

    auto newValues = calculator()->compute( { saltWater(), pressureForDepth, depthForPressure, partialPressure, surfacePercentage } );
    if ( !newValues.has_value() || ( newValues.value().size() != 4 ) )
        return;

    setValue( fImpl->pressureForDepth, pressureForDepth, newValues.value()[ 0 ], 2 );
    setValue( fImpl->pressure, pressureForDepth, newValues.value()[ 0 ], 2 );
    setValue( fImpl->depthForPressure, depthForPressure, newValues.value()[ 1 ], 2 );
    setValue( fImpl->partialPressure, partialPressure, newValues.value()[ 2 ], 2 );
    setValue( fImpl->surfacePercentage, surfacePercentage, newValues.value()[ 3 ], 2 );
}
