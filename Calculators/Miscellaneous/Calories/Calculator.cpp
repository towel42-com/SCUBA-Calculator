#include "Calculator.h"
#include "VariableInfo.h"
#include "Utilities.h"

////https://swimmingcalculators.com/scuba-diving-calculator/

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() {}
    virtual ~CCalculator() override {}

    virtual bool isWaterTypeBased() const override { return true; }

    virtual QString myCalculatorName() const override;
    virtual QStringList calculatorPath() const override;

    virtual TVariableInfoList getMyVariables() const override;
    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior ) override;

    virtual QString myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Calories burned while Diving" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Miscellaneous" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< CVariableInfo >( "calories", tr( "Calories" ), EVariableType::eVariable, EUnit::eCalories, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "weight", tr( "Weight" ), EVariableType::eVariable, EUnit::eWeight, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "duration", tr( "Bottom Time" ), EVariableType::eVariable, EUnit::eTime, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "depth", tr( "Average Depth" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "temp", tr( "Temperature" ), EVariableType::eVariable, EUnit::eTemperature, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::eBaseMETofSCUBAConst ),   //
            std::make_shared< CVariableInfo >( EVariableType::eFreshWaterToSeaWaterConst ),   //
            std::make_shared< CVariableInfo >( "weightC", tr( "Weight (%1)" ).arg( NUtilities::NUnitStrings::weightUnit( false, true, false ) ), EVariableType::eIntermediate, EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "depthC", tr( "Depth (%1)" ).arg( NUtilities::NUnitStrings::depthUnit( false, true, true, false ) ), EVariableType::eIntermediate, EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "tempC", tr( "Temperature (%1)" ).arg( NUtilities::NUnitStrings::tempUnit( false, true, false ) ), EVariableType::eIntermediate, EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "activityLevel", tr( "Activity Level Adjustment" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS ),   //
        } );
    retVal.back()->setValues(   //
        TOptionalNamedValueItemList( {
            //
            std::make_pair( tr( "Leisurely (minimal effort)" ), 1.0 ),   //
            std::make_pair( tr( "Recreational (moderate effort)" ), 1.2 ),   //
            std::make_pair( tr( "Active (strong currents)" ), 1.5 ),   //
            std::make_pair( tr( "Intensive (rescue, heavy gear)" ), 1.8 ),   //
            std::make_pair( tr( "Custom" ), TOptionalDouble() )   //
        } ) );   //
    return retVal;
}

QString CCalculator::myBaseFormula( bool imperial, bool seaWater ) const
{
    return NUtilities::NConversions::NCaloriesComputer::computeCaloriesFormula( imperial, seaWater, "calories", "weight", "depth", { getVariable( "temp" )->optValue(), "temp" }, "activityLevel", "duration" );
}

TVariableInfo CCalculator::determineVariableToUnset( EVariableLoc updateFromSide, QWidget * /*triggerWidget*/, bool /*preDefaultBehavior*/ )
{
    if ( updateFromSide == EVariableLoc::eRHS )
        return getVariable( "calories" );
    else
        return getVariable( "duration" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar->name() == "calories" )
        return NUtilities::NConversions::NCaloriesComputer::computeCaloriesFormula( imperial, seaWater, "calories", "weight", "depth", { getVariable( "temp" )->optValue(), "temp" }, "activityLevel", "duration" );
    else if ( unsetVar->name() == "duration" )
        return NUtilities::NConversions::NCaloriesComputer::computeDurationFormula( imperial, seaWater, "calories", "weight", "depth", { getVariable( "temp" )->optValue(), "temp" }, "activityLevel", "duration" );

    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto calories = getVariable( "calories" );

    auto weight = getVariable( "weight" );
    auto duration = getVariable( "duration" );
    auto depth = getVariable( "depth" );
    auto temp = getVariable( "temp" );
    auto activityLevel = getVariable( "activityLevel" );

    if ( unsetVar == calories )
    {
        calories->setValue( NUtilities::NConversions::NCaloriesComputer::computeCalories( imperial(), seaWater(), weight->value(), depth->value(), temp->value(), activityLevel->value(), duration->value() ) );
    }
    else if ( unsetVar == duration )
    {
        duration->setValue( NUtilities::NConversions::NCaloriesComputer::computeDuration( imperial(), seaWater(), calories->value(), weight->value(), depth->value(), temp->value(), activityLevel->value() ) );
    }
}
