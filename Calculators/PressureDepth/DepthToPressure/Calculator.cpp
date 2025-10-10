#include "Calculator.h"
#include "VariableInfo.h"
#include "Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() {}
    virtual ~CCalculator() override {}

    virtual QString calculatorName() const override;
    virtual QStringList calculatorPath() const override;

    virtual bool isWaterTypeBased() const override { return true; }

    virtual void resetVariables() override { CSCUBACalculator::resetVariables(); }
    virtual QFrame *svgFrame() const override { return CSCUBACalculator::svgFrame(); }
    virtual QSvgWidget *svgWidget() const override { return CSCUBACalculator::svgWidget(); }

    virtual TVariableInfoList getMyVariables() const override;

    virtual QString getBaseFormula() const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo & unsetVar ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo & unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
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
            std::make_shared< SVariableInfo >( "pressure", tr( "Pressure" ), EVariableType::eVariable, EUnit::eAtmospheres, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "depth", tr( "depth" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "depthToSingleAtmosphere", tr( "Depth to Single Atmosphere" ), EVariableType::eDepthToSingleAtmosphereConstant, EUnit::eLength, EVariableLoc::eRHS ),   //
        };
}

QString CCalculator::getBaseFormula() const
{
    return NUtilities::depthToPressureFormula( "pressure", "depth", "depthToSingleAtmosphere" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo & unsetVar ) const
{
    if ( unsetVar->name() == "pressure" )
    {
        return getBaseFormula();
    }
    else if ( unsetVar->name() == "depth" )
    {
        return NUtilities::pressureToDepthFormula( "pressure", "depth", "depthToSingleAtmosphere" );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo & unsetVar )
{
    auto pressure = getVariable( "pressure" );
    auto depth = getVariable( "depth" );

    if ( unsetVar == pressure )
    {
        pressure->setValue( NUtilities::depthToPressure( imperial(), seaWater(), depth->value() ) );
    }
    else if ( unsetVar == depth )
    {
        pressure->setValue( NUtilities::pressureToDepth( imperial(), seaWater(), pressure->value() ) );
    }
}