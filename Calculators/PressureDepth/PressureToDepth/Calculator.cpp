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

    virtual bool isWaterTypeBased() const override { return true; }

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
    return tr( "Pressure to Depth" );
}

QString CCalculator::myReversedCalculatorName() const
{
    return tr( "Depth to Pressure" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Pressure and Depth Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "depth", tr( "depth" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "pressure", tr( "Pressure" ), EVariableType::eVariable, EUnit::eAtmospheres, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "depthToSingleAtmosphere", tr( "Depth to Single Atmosphere" ), EVariableType::eDepthToSingleAtmosphereConstant, EUnit::eLength, EVariableLoc::eRHS ),   //
        };
}

QString CCalculator::myBaseFormula() const
{
    return NUtilities::pressureToDepthFormula( "pressure", "depth", "depthToSingleAtmosphere" );
}

QString CCalculator::myReversedBaseFormula() const
{
    return NUtilities::depthToPressureFormula( "pressure", "depth", "depthToSingleAtmosphere" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar ) const
{
    if ( unsetVar->name() == "depth" )
    {
        return getBaseFormula();
    }
    else if ( unsetVar->name() == "pressure" )
    {
        return NUtilities::depthToPressureFormula( "pressure", "depth", "depthToSingleAtmosphere" );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto pressure = getVariable( "pressure" );
    auto depth = getVariable( "depth" );

    if ( unsetVar == depth )
    {
        depth->setValue( NUtilities::pressureToDepth( imperial(), seaWater(), pressure->value() ) );
    }
    else if ( unsetVar == pressure )
    {
        pressure->setValue( NUtilities::depthToPressure( imperial(), seaWater(), pressure->value() ) );
    }
}