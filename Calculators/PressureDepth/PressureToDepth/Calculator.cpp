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

    virtual TOptionalFormulaList myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual TOptionalFormulaList myReversedBaseFormulas( bool imperial, bool seaWater ) const override;
    virtual TOptionalFormulaList getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
    virtual bool showWaterType() const override { return true; }
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
            CVariableInfo::create( "depth", tr( "depth" ), EUnit::eDepth, EVariableLoc::eLHS ),   //
            CVariableInfo::create( "pressure", tr( "Pressure" ), EUnit::eAtmospheres, EVariableLoc::eRHS ),   //
        };
}

TOptionalFormulaList CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { NUtilities::NConversions::ataToDepthFormula( getVariable( "pressure" ), getVariable( "depth" ) ) } );
}

TOptionalFormulaList CCalculator::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { NUtilities::NConversions::depthToATAFormula( getVariable( "pressure" ), getVariable( "depth" ) ) } );
}

TOptionalFormulaList CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
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