#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "SAC, RMV and Gas Consumption" ); }
    virtual ~CCalculator() override {}
    virtual bool isWaterTypeBased() const override { return true; }
    virtual bool valuesSetProperly() const override
    {
        auto unsetVars = unsetVariables();
        if ( unsetVars.size() == 1 )
            return true;
        if ( ( unsetVars.size() == 2 ) || ( unsetVars.size() == 3 ) )
        {
            auto rmv = getVariable( "rmv" );
            auto sac = getVariable( "sac" );
            auto gasConsumed = getVariable( "gasConsumed" );
            auto pressureUsed = getVariable( "pressureUsed" );

            for ( auto &&ii : unsetVars )
            {
                if ( ( ii != rmv ) && ( ii != sac ) && ( ii != gasConsumed ) && ( ii != pressureUsed ) )
                    return false;
            }
            return true;
        }
        return false;
    }

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables() const override;

    virtual std::optional< QStringList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QStringList > getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior ) override;
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "SAC, RMV and Gas Consumption" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Miscellaneous" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal =   //
        TVariableInfoList( {
            std::make_shared< CVariableInfo >( "depth", tr( "Average Depth" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "time", tr( "Time at Average Depth" ), EVariableType::eVariable, EUnit::eTime, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "pressureUsed", tr( "Pressure used" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "tankVolume", tr( "Tank Volume" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "tankPressure", tr( "Tank Pressure Rating" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "sac", tr( "SAC" ), EVariableType::eVariable, EUnit::ePressurePerMinute, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "rmv", tr( "RMV" ), EVariableType::eVariable, EUnit::eVolumePerMinute, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "gasConsumed", tr( "Gas Consumed" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "ata", tr( "Absolute Pressure at Depth" ), EVariableType::eIntermediate, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "psiPerMin", tr( "PSI Per Minute" ), EVariableType::eIntermediate, EUnit::ePressurePerMinute, EVariableLoc::eRHS ),   //

        } );
    return retVal;
}

/*
    if ( ( !sac->has_value() || !rmv->has_value() ) && pressureUsed->has_value() )
    {
        auto ata = NUtilities::NConversions::depthToATA( imperial(), seaWater(), depth->value() );
        auto psiPerMin = pressureUsed->value() / time->value();
        auto sacValue = psiPerMin / ata;

        sac->setValue( sacValue );
        rmv->setValue( NUtilities::NConversions::sacToRMV( sacValue, tankVolume->value(), tankPressure->value() ) );
    }
*/
std::optional< QStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    QStringList formulas;

    formulas << NUtilities::NConversions::depthToATAFormula( "ata", "depth" );
    formulas << R"__(<psiPerMin> = \frac{<pressureUsed>}{<time>})__";
    formulas << R"__(<sac> = \frac{<psiPerMin>}{<ata>})__";
    formulas << NUtilities::NConversions::sacToRMVFormula( "sac", "rmv", "tankVolume", "tankPressure" );
    formulas << R"__(<gasConsumed> = <pressureUsed> \times \frac{<tankVolume>}{<tankPressure>})__";

    return formulas;
}

std::optional< QStringList > CCalculator::getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const
{
    auto depth = getVariable( "depth" );
    auto time = getVariable( "time" );
    auto pressureUsed = getVariable( "pressureUsed" );
    auto tankVolume = getVariable( "tankVolume" );
    auto tankPressure = getVariable( "tankPressure" );
    auto sac = getVariable( "sac" );
    auto rmv = getVariable( "rmv" );
    auto gasConsumed = getVariable( "gasConsumed" );

    QStringList formulas;

    if ( ( unsetVar == sac ) || ( unsetVar == rmv ) )
    {
        return myBaseFormulas( imperial, seaWater );
    }

    return formulas;
}

void CCalculator::computeValueForVar( TVariableInfo & /*unsetVar*/ )
{
    auto depth = getVariable( "depth" );
    auto time = getVariable( "time" );
    auto pressureUsed = getVariable( "pressureUsed" );
    auto tankVolume = getVariable( "tankVolume" );
    auto tankPressure = getVariable( "tankPressure" );
    auto sac = getVariable( "sac" );
    auto rmv = getVariable( "rmv" );
    auto gasConsumed = getVariable( "gasConsumed" );
    auto psiPerMin = getVariable( "psiPerMin" );
    auto ata = getVariable( "ata" );

    if ( tankVolume->has_value() && tankPressure->has_value() )
    {
        if ( sac->has_value() && !rmv->has_value() )
        {
            rmv->setValue( NUtilities::NConversions::sacToRMV( sac->value(), tankVolume->value(), tankPressure->value() ) );
        }
        else if ( !sac->has_value() && rmv->has_value() )
        {
            sac->setValue( NUtilities::NConversions::rmvToSAC( rmv->value(), tankVolume->value(), tankPressure->value() ) );
        }
    }

    if ( !pressureUsed->has_value() && gasConsumed->has_value() )
    {
        pressureUsed->setValue( gasConsumed->value() * tankPressure->value() / tankVolume->value() );
    }

    if ( pressureUsed->has_value() && !gasConsumed->has_value() )
    {
        gasConsumed->setValue( ( pressureUsed->value() * tankVolume->value() ) / tankPressure->value() );
    }

    if ( ( !sac->has_value() || !rmv->has_value() ) && pressureUsed->has_value() )
    {
        ata->setValue( NUtilities::NConversions::depthToATA( imperial(), seaWater(), depth->value() ) );
        psiPerMin->setValue( pressureUsed->value() / time->value() );

        sac->setValue( psiPerMin->value() / ata->value() );
        rmv->setValue( NUtilities::NConversions::sacToRMV( sac->value(), tankVolume->value(), tankPressure->value() ) );
    }
}

TVariableInfo CCalculator::determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool /*preDefaultBehavior*/ )
{
    if ( updateFromSide == EVariableLoc::eRHS )
    {
        getVariable( "rmv" )->resetValue( imperial(), seaWater(), false, false );
        getVariable( "gasConsumed" )->resetValue( imperial(), seaWater(), false, false );
        return getVariable( "sac" );
    }
    if ( getVariable( "rmv" )->isWidget( triggerWidget ) )
    {
        getVariable( "sac" )->resetValue( imperial(), seaWater(), false, false );
        getVariable( "gasConsumed" )->resetValue( imperial(), seaWater(), false, false );
    }
    if ( getVariable( "sac" )->isWidget( triggerWidget ) )
    {
        getVariable( "rmv" )->resetValue( imperial(), seaWater(), false, false );
        getVariable( "gasConsumed" )->resetValue( imperial(), seaWater(), false, false );
    }
    if ( getVariable( "gasConsumed" )->isWidget( triggerWidget ) )
    {
        getVariable( "sac" )->resetValue( imperial(), seaWater(), false, false );
        getVariable( "rmv" )->resetValue( imperial(), seaWater(), false, false );
    }

    return getVariable( "pressureUsed" );
}
