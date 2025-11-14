#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "PressureToDepth" ); }
    virtual ~CCalculator() override {}

    virtual bool isReversible() const override { return true; }
    virtual std::optional< std::pair< QString, QString > > fromToLabels() const override;

    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual std::optional< TFormulaList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaList > myReversedBaseFormulas( bool imperial, bool seaWater ) const override;
    virtual std::optional< TFormulaList > getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
    virtual bool isWaterTypeBased() const override { return true; }
};

extern "C" CCalculatorBase *instantiateCalculator()
{
    return new CCalculator;
}

std::optional< std::pair< QString, QString > > CCalculator::fromToLabels() const
{
    return std::make_pair(  tr( "Pressure" ), tr( "Depth" ) );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure and Depth Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "depth", tr( "depth" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "pressure", tr( "Pressure" ), EVariableType::eVariable, EUnit::eAtmospheres, EVariableLoc::eRHS ),   //
        };
}

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { NUtilities::NConversions::ataToDepthFormula( getVariable( "pressure" ), getVariable( "depth" ) ) } );
}

std::optional< TFormulaList > CCalculator::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { NUtilities::NConversions::depthToATAFormula( getVariable( "pressure" ), getVariable( "depth" ) ) } );
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "depth" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "pressure" )
    {
        return myReversedBaseFormulas( imperial, seaWater );
    }
    return {};
}

void CCalculator::computeVariableValues()
{
    auto pressure = getVariable( "pressure" );
    auto depth = getVariable( "depth" );

    if ( !depth->has_value() && depth->dependenciesSatisfied() )
    {
        depth->setValue( NUtilities::NConversions::ataToDepth( imperial(), seaWater(), pressure->value() ) );
    }
    
    if ( !pressure->has_value() && pressure->dependenciesSatisfied() )
    {
        pressure->setValue( NUtilities::NConversions::depthToATA( imperial(), seaWater(), depth->value() ) );
    }
}