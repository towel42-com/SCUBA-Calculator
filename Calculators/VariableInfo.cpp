#include "VariableInfo.h"
#include "Utilities.h"
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QLabel>

SVariableInfo::SVariableInfo( const QString &name, const QString &desc, EVariableType type, EUnit unitType, ESide variableLocation ) :
    fName( name ),
    fDescription( desc ),
    fType( type ),
    fUnit( unitType ),
    fVariableLocation( variableLocation )
{
}

void SVariableInfo::updateLabels( bool imperial, bool /*saltWater*/ )
{
    if ( fType != EVariableType::eVariable )
        return;

    Q_ASSERT( fField && fUnitLabel );

    auto unitText = NUtilities::NUnitStrings::getUnitLabel( imperial, fUnit, false );
    if ( fField )
    {
        if ( dynamic_cast< QLineEdit * >( fField ) )
            dynamic_cast< QLineEdit * >( fField )->setPlaceholderText( QString( "%1 (%2)" ).arg( fDescription ).arg( unitText ) );

    }
    if ( fUnitLabel )
        fUnitLabel->setText( unitText );
}

void SVariableInfo::resetValue( bool updateUI, bool notifyUI )
{
    fValue.reset();
    if ( !updateUI )
        return;
    updateFieldFromValue( notifyUI );
}

void SVariableInfo::updateValueFromField()
{
    if ( fType != EVariableType::eVariable )
        return;

    fValue.reset();
    Q_ASSERT( fField );
    if ( !fField )
        return;

    if ( dynamic_cast< QLineEdit * >( fField ) )
        fValue = NUtilities::getValue( dynamic_cast< QLineEdit * >( fField )->text() );
    else if ( dynamic_cast< QDoubleSpinBox * >( fField ) )
        fValue = dynamic_cast< QDoubleSpinBox * >( fField )->value();
}

void SVariableInfo::updateFormula( bool imperial, bool saltWater, QString &formula, bool defaultFormula ) const
{
    QString value;
    QString format;
    if ( !defaultFormula && fValue.has_value() )
    {
        value = NUtilities::doubleToString( fValue.value(), 2 );
        format = QString( "%1%2" );
    }
    else
    {
        switch ( fType )
        {
            case EVariableType::eVariable:
                {
                    value = fDescription;
                    format = QString( "%1 (%2)" );
                    break;
                }
            case EVariableType::ePressurePerTempConst:
                {
                    value = NUtilities::NUnitStrings::pressurePerTemp( imperial, true );
                    format = "%1";
                }
                break;
            case EVariableType::eWeightOfWaterConst:
                {
                    value = NUtilities::NUnitStrings::weightOfWater( imperial, saltWater, true );
                    format = "%1";
                }
                break;
            case EVariableType::eIdealGasConst:
                {
                    value = NUtilities::NUnitStrings::idealGasConstant( imperial, true );
                    format = "%1";
                }
                break;
            case EVariableType::eFN2AtSurfaceConst:
                {
                    value = NUtilities::NUnitStrings::percentN2AtSurface( true );
                    format = "%1";
                }
                break;
            case EVariableType::eFO2AtSurfaceConst:
                {
                    value = NUtilities::NUnitStrings::percentO2AtSurface( true );
                    format = "%1";
                }
                break;
            case EVariableType::eDepthToSingleAtmosphereConst:
                {
                    value = NUtilities::NUnitStrings::depthToSingleAtmosphere( imperial, saltWater, true );
                    format = "%1";
                }
                break;
        };
    }

    auto newString = QString( format ).arg( value );
    if ( fType == EVariableType::eVariable )
        newString = newString.arg( NUtilities::NUnitStrings::getUnitLabel( imperial, fUnit, true ) );
    auto token = QString( "<%1>" ).arg( fName );
    formula = formula.replace( token, newString );
}

void SVariableInfo::updateFieldFromValue( bool notifyUI )
{
    if ( fType != EVariableType::eVariable )
        return;

    Q_ASSERT( fField );
    if ( !fField )
        return;

    TOptionalDouble currValue;
    if ( dynamic_cast< QLineEdit * >( fField ) )
        currValue = NUtilities::getValue( dynamic_cast< QLineEdit * >( fField )->text() );
    else if ( dynamic_cast< QDoubleSpinBox * >( fField ) )
        currValue = dynamic_cast< QDoubleSpinBox * >( fField )->value();

    if ( !fValue.has_value() )
    {
        if ( !notifyUI )
            fField->blockSignals( true );
        if ( dynamic_cast< QLineEdit * >( fField ) )
            dynamic_cast< QLineEdit * >( fField )->clear();
        else if ( dynamic_cast< QDoubleSpinBox * >( fField ) )
        {
            dynamic_cast< QDoubleSpinBox * >( fField )->setValue( fRange.value().fDefaultValue );
            fValue = fRange.value().fDefaultValue;
        }
        if ( !notifyUI )
            fField->blockSignals( false );
    }

    NUtilities::setValue( fField, currValue, fValue, 2, notifyUI );
}
