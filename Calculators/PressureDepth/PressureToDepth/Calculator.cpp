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

    virtual std::list< std::shared_ptr< SVariableInfo > > getMyVariables() const override;

    virtual QString getBaseFormula() const override;   // for descriptive purposes
    virtual std::optional< QString > getCurrentFormula() const override;   // returns the current formula in use

    virtual void computeValues() const override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return tr( "Pressure to Depth" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Pressure and Depth Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< SVariableInfo >( "depth", tr( "depth" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "pressure", tr( "Pressure" ), EVariableType::eVariable, EUnit::eAtmospheres, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "depthToSingleAtmosphere", tr( "Depth to Single Atmosphere" ), EVariableType::eDepthToSingleAtmosphereConstant, EUnit::eLength, EVariableLoc::eRHS ),   //
        };
}

QString CCalculator::getBaseFormula() const
{
    return NUtilities::pressureToDepthFormula( "pressure", "depth", "depthToSingleAtmosphere" );
}

QString CCalculator::computeAndGenerateFormula( bool &isBaseFormula ) const
{
    auto pressure = getVariable( "pressure" );
    auto depth = getVariable( "depth" );

    QString formula;
    bool aOK = numUnsetVariables() == 1;
    auto depthToSingleATM = NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() );
    isBaseFormula = false;
    if ( !aOK )
    {
        formula = getBaseFormula();
        isBaseFormula = true;
    }
    else if ( !depth->has_value() )
    {
        depth->setValue( NUtilities::pressureToDepth( imperial(), seaWater(), pressure->value() ) );
        formula = getBaseFormula();
    }
    else if ( !pressure->has_value() )
    {
        pressure->setValue( NUtilities::depthToPressure( imperial(), seaWater(), pressure->value() ) );
        formula = NUtilities::depthToPressureFormula( "pressure", "depth", "depthToSingleAtmosphere" );
    }
    return formula;
}
