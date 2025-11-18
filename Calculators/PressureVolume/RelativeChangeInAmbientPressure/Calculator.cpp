#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Formula.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "RelativeChangeinAmbientPressure" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual TOptionalFormulaList myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual TOptionalFormulaList getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
};

extern "C" CCalculatorBase *instantiateCalculator()
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
            CVariableInfo::create( "relChange", tr( "Relative Change in Ambient Pressure" ), EUnit::ePercent, EVariableLoc::eLHS ),   //
            CVariableInfo::create( "p2", tr( "Pressure 2" ), EUnit::ePressure, EVariableLoc::eRHS ),   //
            CVariableInfo::create( "p1", tr( "Pressure 1" ), EUnit::ePressure, EVariableLoc::eRHS ),   //
        };
}

TOptionalFormulaList CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { std::make_shared< CFormula >( getVariable( "relChange" ), QString( R"__(\frac{<p2>}{<p1>})__" ) ) } );
}

TOptionalFormulaList CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "relChange" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "p1" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(\frac{<p2>}{<relChange>})__" ) ) } );
    }
    else if ( unsetVar == "p2" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(<p2> \times <relChange>)__" ) ) } );
    }
    return {};
}

void CCalculator::computeVariableValues()
{
    auto relChange = getVariable( "relChange" );
    auto p1 = getVariable( "p1" );
    auto p2 = getVariable( "p2" );

    if ( !relChange->has_value() && relChange->dependenciesSatisfied() )
    {
        relChange->setValue( p2->value() / p1->value() );
    }

    if ( !p1->has_value() && p1->dependenciesSatisfied() )
    {
        p1->setValue( p2->value() / relChange->value() );
    }

    if ( !p2->has_value() && p2->dependenciesSatisfied() )
    {
        p2->setValue( relChange->value() * p1->value() );
    }
}
