#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "PressureToDepth" ); }
    virtual ~CCalculator() override {}

    virtual bool isReversible() const override { return true; }
    virtual std::pair< QString, QString > fromToLabels() const override;

    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables() const override;

    virtual std::optional< QStringList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QStringList > myReversedBaseFormulas( bool imperial, bool seaWater ) const override;
    virtual std::optional< QStringList > getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
    virtual bool isWaterTypeBased() const override { return true; }
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

std::pair< QString,QString > CCalculator::fromToLabels() const
{
    return { tr( "Pressure" ), tr( "Depth" ) };
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure and Depth Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "depth", tr( "depth" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "pressure", tr( "Pressure" ), EVariableType::eVariable, EUnit::eAtmospheres, EVariableLoc::eRHS ),   //
        };
}

std::optional< QStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return QStringList() << NUtilities::NConversions::ataToDepthFormula( "pressure", "depth" );
}

std::optional< QStringList > CCalculator::myReversedBaseFormulas( bool imperial, bool seaWater ) const
{
    return QStringList() << NUtilities::NConversions::depthToATAFormula( "pressure", "depth" );
}

std::optional< QStringList > CCalculator::getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar->name() == "depth" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar->name() == "pressure" )
    {
        return myReversedBaseFormulas( imperial, seaWater );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto pressure = getVariable( "pressure" );
    auto depth = getVariable( "depth" );

    if ( unsetVar == depth )
    {
        depth->setValue( NUtilities::NConversions::ataToDepth( imperial(), seaWater(), pressure->value() ) );
    }
    else if ( unsetVar == pressure )
    {
        pressure->setValue( NUtilities::NConversions::depthToATA( imperial(), seaWater(), depth->value() ) );
    }
}