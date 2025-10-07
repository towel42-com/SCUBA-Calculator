#include "Page.h"
#include "ui_Page.h"
#include "Utilities.h"

#include <QButtonGroup>
CPage::CPage( CSCUBACalculator *calculator, QWidget *parent ) :
    CSCUBACalculatorPage( calculator, parent ),
    fImpl( new Ui::CPage )
{
    fImpl->setupUi( this );
    //addWidgets( false, { fImpl->pressureForDepth, fImpl->partialPressure } );
    //addWidgets( true, { fImpl->depthForPressure, fImpl->surfacePercentage } );

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
        NUtilities::setValue( fImpl->surfacePercentage, {}, 0.21, 2, false );
        fImpl->surfacePercentageLabel->setText( tr( "FO2 (%)" ) );
        fImpl->partialPressureLabel->setText( tr( "PO2 (%)" ) );
        fImpl->surfacePercentage->setReadOnly( true );
        fImpl->surfacePercentage->setEnabled( false );
    }
    else if ( fImpl->nitrogen->isChecked() )
    {
        NUtilities::setValue( fImpl->surfacePercentage, {}, 0.79, 2, false );
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

void CPage::updateValuesInternal( QWidget *triggerWidget )
{
    (void)triggerWidget;
    //auto pressureString = tr( "Pressure (%1)" ).arg( NUtilities::NUnitStrings::pressureUnit( imperial(), false ) );
    //fImpl->pressureLabel->setText( pressureString );
    //fImpl->pLabel->setText( pressureString );

    //auto depthString = tr( "(%1) + %2)/%2" ).arg( NUtilities::NUnitStrings::lengthUnit( imperial(), false, false ) ).arg( NUtilities::doubleToString( NUtilities::NConstants::depthToSingleAtmosphere( imperial(), saltWater() ), 1 ) );
    //fImpl->depthLabel->setText( depthString );

    //fImpl->depthLabel->setText( tr( "(%1)" ).arg( NUtilities::NUnitStrings::lengthUnit( imperial(), false, false ) ) );
    //if ( triggerWidget == nullptr )
    //    return;

    //auto pressureForDepth = NUtilities::getValue( fImpl->pressureForDepth->text() );
    //auto depthForPressure = NUtilities::getValue( fImpl->depthForPressure->text() );
    //auto partialPressure = NUtilities::getValue( fImpl->partialPressure->text() );
    //auto surfacePercentage = NUtilities::getValue( fImpl->surfacePercentage->text() );

    //if ( NUtilities::numEmpty( { pressureForDepth, depthForPressure } ) == 0 )
    //{
    //    if ( triggerWidget == fImpl->pressureForDepth )
    //    {
    //        depthForPressure.reset();
    //        partialPressure.reset();
    //    }
    //    else if ( triggerWidget == fImpl->depthForPressure )
    //    {
    //        pressureForDepth.reset();
    //        partialPressure.reset();
    //    }
    //}

    //if ( NUtilities::numEmpty( { pressureForDepth, depthForPressure, partialPressure, surfacePercentage } ) == 0 )
    //{
    //    if ( ( triggerWidget == fImpl->pressureForDepth ) || ( triggerWidget == fImpl->depthForPressure ) )
    //        partialPressure.reset();
    //    else if ( triggerWidget == fImpl->partialPressure )
    //    {
    //        depthForPressure.reset();
    //        pressureForDepth.reset();
    //    }
    //    else if ( triggerWidget == fImpl->surfacePercentage )
    //        partialPressure.reset();
    //}

    //auto newValues = calculator()->compute( { saltWater(), pressureForDepth, depthForPressure, partialPressure, surfacePercentage } );
    //if ( !newValues.has_value() || ( newValues.value().size() != 4 ) )
    //    return;

    //NUtilities::setValue( fImpl->pressureForDepth, pressureForDepth, newValues.value()[ 0 ], 2 );
    //NUtilities::setValue( fImpl->pressure, pressureForDepth, newValues.value()[ 0 ], 2 );
    //NUtilities::setValue( fImpl->depthForPressure, depthForPressure, newValues.value()[ 1 ], 2 );
    //NUtilities::setValue( fImpl->partialPressure, partialPressure, newValues.value()[ 2 ], 2 );
    //NUtilities::setValue( fImpl->surfacePercentage, surfacePercentage, newValues.value()[ 3 ], 2 );
}
