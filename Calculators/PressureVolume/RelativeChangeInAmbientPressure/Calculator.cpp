#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/FormulaString.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "RelativeChangeinAmbientPressure" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual std::optional< TFormulaStringList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaStringList > getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
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

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "relChange", tr( "Relative Change in Ambient Pressure" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "p2", tr( "Pressure 2" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "p1", tr( "Pressure 1" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
        };
}

std::optional< TFormulaStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( "relChange>" ), QString( R"__(\frac{<p2>}{<p1>})__" ) ) } );
}

std::optional< TFormulaStringList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "relChange" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "p1" )
    {
        TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( unsetVar ), QString( R"__(\frac{<p2>}{<relChange>})__" ) ) } );
    }
    else if ( unsetVar == "p2" )
    {
        TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( unsetVar ), QString( R"__(<p2> \times <relChange>)__" ) ) } );
    }
    return {};
}

void CCalculator::computeVariableValues()
{
    auto relChange = getVariable( "relChange" );
    auto p1 = getVariable( "p1" );
    auto p2 = getVariable( "p2" );

    if ( !relChange->has_value() && p1->has_value() && p2->has_value() )
    {
        relChange->setValue( p2->value() / p1->value() );
    }
    
    if ( relChange->has_value() && !p1->has_value() && p2->has_value() )
    {
        p1->setValue( p2->value() / relChange->value() );
    }
    
    if ( relChange->has_value() && p1->has_value() && !p2->has_value() )
    {
        p2->setValue( relChange->value() * p1->value() );
    }
}
