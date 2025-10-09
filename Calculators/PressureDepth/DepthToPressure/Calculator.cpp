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

    virtual void resetVariables() override { CSCUBACalculator::resetVariables(); }
    virtual QFrame *svgFrame() const override { return CSCUBACalculator::svgFrame(); }
    virtual QSvgWidget *svgWidget() const override { return CSCUBACalculator::svgWidget(); }

    virtual bool isWaterTypeBased() const override { return true; }

    virtual std::list< std::shared_ptr< SVariableInfo > > getMyVariables() const override;
    virtual QString getDefaultFormula() const override;
    virtual QString computeAndGenerateFormula( bool &isBaseFormula ) const override;
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Depth to Pressure";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure and Depth Conversions" };
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

QString CCalculator::getDefaultFormula() const
{
    return NUtilities::depthToPressureFormula( "pressure", "depth", "depthToSingleAtmosphere" );
}

QString CCalculator::computeAndGenerateFormula( bool &isBaseFormula ) const
{
    auto pressure = getVariable( "pressure" );
    auto depth = getVariable( "depth" );

    QString formula;
    bool aOK = numUnsetVariables() == 1;
    isBaseFormula = false;
    if ( !aOK )
    {
        formula = getDefaultFormula();
        isBaseFormula = true;
    }
    else if ( !pressure->has_value() )
    {
        pressure->setValue( NUtilities::depthToPressure( imperial(), seaWater(), depth->value() ) );
        formula = getDefaultFormula();
    }
    else if ( !depth->has_value() )
    {
        pressure->setValue( NUtilities::pressureToDepth( imperial(), seaWater(), pressure->value() ) );
        formula = NUtilities::pressureToDepthFormula( "pressure", "depth", "depthToSingleAtmosphere" );
    }
    return formula;
}
