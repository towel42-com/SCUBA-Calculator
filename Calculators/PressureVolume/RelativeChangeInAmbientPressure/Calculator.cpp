#include "Calculator.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "RelativeChangeinAmbientPressure" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual TVariableInfoList getMyVariables() const override;

    virtual std::optional< QString > myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo & unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Calculating Relative Change in Ambient Pressure" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure and Volume Conversions" ) };
}


TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "relChange", tr( "Relative Change in Ambient Pressure" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "p2", tr( "Pressure 2" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "p1", tr( "Pressure 1" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
        };
}

std::optional< QString > CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return R"__(<relChange> = \frac{<p2>}{<p1>})__";
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "relChange" )
    {
        return R"__(<relChange> = \frac{<p2>}{<p1>})__";
    }
    else if ( unsetVar->name() == "p1" )
    {
        return R"__(<p1> = \frac{<p2>}{<relChange>})__";
    }
    else if ( unsetVar->name() == "p2" )
    {
        return R"__(<p2> = <p2> \times <relChange>)__";
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo & unsetVar )
{
    auto relChange = getVariable( "relChange" );
    auto p1 = getVariable( "p1" );
    auto p2 = getVariable( "p2" );

    if ( unsetVar->name() == "relChange" )
    {
        relChange->setValue( p2->value() / p1->value() );
    }
    else if ( unsetVar->name() == "p1" )
    {
        p1->setValue( p2->value() / relChange->value() );
    }
    else if ( unsetVar->name() == "p2" )
    {
        p2->setValue( relChange->value() * p1->value() );
    }
}
