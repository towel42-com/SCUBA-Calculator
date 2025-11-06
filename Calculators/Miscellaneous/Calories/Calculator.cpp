#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

////https://swimmingcalculators.com/scuba-diving-calculator/

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "Calories" ); }
    virtual ~CCalculator() override {}

    virtual bool isWaterTypeBased() const override { return true; }

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables() const override;
    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior ) override;

    virtual std::optional< TFormulaStringList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaStringList > getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

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

QStringList CCalculator::myCalculatorPath() const
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
            std::make_shared< CVariableInfo >( "weightM", tr( "Weight (%1)" ).arg( NUtilities::NUnitStrings::weightUnit( false, true, false ) ), EVariableType::eIntermediate, EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "depthMS", tr( "Depth (%1)" ).arg( NUtilities::NUnitStrings::depthUnit( false, true, true, false ) ), EVariableType::eIntermediate, EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "tempM", tr( "Temperature (%1)" ).arg( NUtilities::NUnitStrings::tempUnit( false, true, false ) ), EVariableType::eIntermediate, EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >(
                "activityLevel", tr( "Activity Level Adjustment" ), EUnit::eLargePercent, EVariableLoc::eRHS,
                SBaseInfo< TNamedValueItemList >(
                    {}, {},
                    TNamedValueItemList( {
                        //
                        std::make_pair( tr( "Leisurely (minimal effort)" ), 100.0 ),   //
                        std::make_pair( tr( "Recreational (moderate effort)" ), 120 ),   //
                        std::make_pair( tr( "Active (strong currents)" ), 150.0 ),   //
                        std::make_pair( tr( "Intensive (rescue, heavy gear)" ), 180.0 ),   //
                        std::make_pair( tr( "Custom" ), TOptionalDouble() )   //
                    } ) ) ),   //
        } );
    return retVal;
}

std::optional< TFormulaStringList > CCalculator::myBaseFormulas( bool imperial, bool seaWater ) const
{
    return TFormulaStringList( { NUtilities::NConversions::NCaloriesComputer::computeCaloriesFormula( imperial, seaWater, getVariable( "calories" ), getVariable( "weight" ), getVariable( "depth" ), getVariable( "temp" ), getVariable( "activityLevel" ), getVariable( "duration" ) ) } );
}

TVariableInfo CCalculator::determineVariableToUnset( EVariableLoc updateFromSide, QWidget * /*triggerWidget*/, bool /*preDefaultBehavior*/ )
{
    if ( updateFromSide == EVariableLoc::eRHS )
        return getVariable( "calories" );
    else
        return getVariable( "duration" );
}

std::optional< TFormulaStringList > CCalculator::getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar->name() == "calories" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar->name() == "duration" )
    {
        return TFormulaStringList( { NUtilities::NConversions::NCaloriesComputer::computeDurationFormula( imperial, seaWater, getVariable( "calories" ), getVariable( "weight" ), getVariable( "depth" ), getVariable( "temp" ), getVariable( "activityLevel" ), getVariable( "duration" ) ) } );
    }
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
