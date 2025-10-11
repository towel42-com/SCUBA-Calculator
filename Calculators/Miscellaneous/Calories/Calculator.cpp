#include "Calculator.h"
#include "VariableInfo.h"
#include "Utilities.h"

////https://swimmingcalculators.com/scuba-diving-calculator/

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() {}
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList calculatorPath() const override;

    virtual TVariableInfoList getMyVariables() const override;

    virtual QString myBaseFormula() const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar ) const override;   // returns the current formula in use

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
            std::make_shared< CVariableInfo >( "met", tr( "Base Met Value" ), EVariableType::eBaseMETValueConst, EUnit::eTemperature, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "activityLevel", tr( "Activity Level" ), EVariableType::eVariable, EUnit::eNone, EVariableLoc::eRHS ),   //
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

QString CCalculator::myBaseFormula() const
{
    return R"__(<calories>)__";
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar ) const
{
    (void)unsetVar;
    return R"__(<calories>)__";
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
        calories->setValue( NUtilities::computeCalories( imperial(), weight->value(), depth->value(), temp->value(), activityLevel->value(), duration->value() ) );
    }
}

