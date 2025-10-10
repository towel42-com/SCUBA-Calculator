#include "Calculator.h"
#include "VariableInfo.h"
#include "Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() {}
    virtual ~CCalculator() override {}

    virtual bool isReversable() const override { return true; }
    virtual QString myReversedCalculatorName() const override;
    virtual QString myReversedBaseFormula() const override;

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
    return tr( "Quick Estimate for Pressure when Temperature Changes" );
}

QString CCalculator::myReversedCalculatorName() const
{
    return tr( "Quick Estimate for Temperature when Pressure Changes" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Quick Estimates" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "p1", tr( "Pressure Change" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "t1", tr( "Temperature Change" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "pressurePerDegree", tr( "Pressure Change Per Degree" ), EVariableType::ePressurePerDegreeConst, EUnit::eNone, EVariableLoc::eRHS ),   //
        };
}

QString CCalculator::myBaseFormula() const
{
    return NUtilities::quickDegreeChangeToPressureFormula( "t1", "p1", "pressurePerDegree" );
}

QString CCalculator::myReversedBaseFormula() const
{
    return NUtilities::quickPressureChangeToDegreeFormula( "t1", "p1", "pressurePerDegree" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar ) const
{
    if ( unsetVar->name() == "p1" )
    {
        return NUtilities::quickDegreeChangeToPressureFormula( "t1", "p1", "pressurePerDegree" );
    }
    else if ( unsetVar->name() == "t1" )
    {
        return NUtilities::quickPressureChangeToDegreeFormula( "t1", "p1", "pressurePerDegree" );
    }

    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto p1 = getVariable( "p1" );
    auto t1 = getVariable( "t1" );

    if ( unsetVar == p1 )
    {
        p1->setValue( NUtilities::quickDegreeChangeToPressure( imperial(), t1->value() ) );
    }
    else if ( unsetVar == t1 )
    {
        t1->setValue( NUtilities::quickPressureChangeToDegree( imperial(), p1->value() ) );
    }
}