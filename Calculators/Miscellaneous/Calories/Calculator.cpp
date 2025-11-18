#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

////https://swimmingcalculators.com/scuba-diving-calculator/

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "Calories" ); }
    virtual ~CCalculator() override {}

    virtual bool showWaterType() const override { return true; }

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior ) override;

    virtual TOptionalFormulaList myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual TOptionalFormulaList getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
    virtual void setupCustomDependencies() override;
};

extern "C" CCalculatorBase *instantiateCalculator()
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

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
{
    auto retVal = TVariableInfoList(   //
        {
            CVariableInfo::create( "calories", tr( "Calories" ), EUnit::eCalories, EVariableLoc::eLHS ),   //
            CVariableInfo::create( "weight", tr( "Weight" ), EUnit::eWeight, EVariableLoc::eRHS ),   //
            CVariableInfo::create( "duration", tr( "Bottom Time" ), EUnit::eTime, EVariableLoc::eRHS ),   //
            CVariableInfo::create( "depth", tr( "Average Depth" ), EUnit::eDepth, EVariableLoc::eRHS ),   //
            CVariableInfo::create( "temp", tr( "Temperature" ), EUnit::eTemperature, EVariableLoc::eRHS ),   //
            CVariableInfo::create( "weightM", tr( "Weight (%1)" ).arg( NUtilities::NUnitStrings::weightUnit( false, true, false ) ), EUnit::eNone, EVariableLoc::eRHS ),   //
            CVariableInfo::create( "depthMS", tr( "Depth (%1)" ).arg( NUtilities::NUnitStrings::depthUnit( false, true, true, false ) ), EUnit::eNone, EVariableLoc::eRHS ),   //
            CVariableInfo::create( "tempM", tr( "Temperature (%1)" ).arg( NUtilities::NUnitStrings::tempUnit( false, true, false ) ), EUnit::eNone, EVariableLoc::eRHS ),   //
            CVariableInfo::create(
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

    auto pos = std::prev( std::prev( std::prev( std::prev( retVal.end() ) ) ) );
    ( *pos )->setIsIntermediate( true );
    pos++;
    ( *pos )->setIsIntermediate( true );
    pos++;
    ( *pos )->setIsIntermediate( true );

    return retVal;
}

TOptionalFormulaList CCalculator::myBaseFormulas( bool imperial, bool seaWater ) const
{
    return TFormulaList( { NUtilities::NConversions::NCaloriesComputer::computeCaloriesFormula( imperial, seaWater, getVariable( "calories" ), getVariable( "weight" ), getVariable( "depth" ), getVariable( "temp" ), getVariable( "activityLevel" ), getVariable( "duration" ) ) } );
}

TVariableInfo CCalculator::determineVariableToUnset( EVariableLoc updateFromSide, QWidget * /*triggerWidget*/, bool /*preDefaultBehavior*/ )
{
    if ( updateFromSide == EVariableLoc::eRHS )
        return getVariable( "calories" );
    else
        return getVariable( "duration" );
}

TOptionalFormulaList CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "calories" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "duration" )
    {
        return TFormulaList( { NUtilities::NConversions::NCaloriesComputer::computeDurationFormula( imperial, seaWater, getVariable( "calories" ), getVariable( "weight" ), getVariable( "depth" ), getVariable( "temp" ), getVariable( "activityLevel" ), getVariable( "duration" ) ) } );
    }
    return {};
}

void CCalculator::setupCustomDependencies()
{
    setDependencies( "calories", { "weight", "depth", "temp", "activityLevel", "duration" } );
    setDependencies( "duration", { "weight", "depth", "temp", "activityLevel", "calories" } );
}

void CCalculator::computeVariableValues()
{
    auto calories = getVariable( "calories" );

    auto weight = getVariable( "weight" );
    auto duration = getVariable( "duration" );
    auto depth = getVariable( "depth" );
    auto temp = getVariable( "temp" );
    auto activityLevel = getVariable( "activityLevel" );

    if ( !calories->has_value() && calories->dependenciesSatisfied() )
    {
        calories->setValue( NUtilities::NConversions::NCaloriesComputer::computeCalories( imperial(), seaWater(), weight->value(), depth->value(), temp->value(), activityLevel->value() / 100.0, duration->value() ) );
    }

    if ( !duration->has_value() && duration->dependenciesSatisfied() )
    {
        duration->setValue( NUtilities::NConversions::NCaloriesComputer::computeDuration( imperial(), seaWater(), calories->value(), weight->value(), depth->value(), temp->value(), activityLevel->value() / 100.0 ) );
    }
}
