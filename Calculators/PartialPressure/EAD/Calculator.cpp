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
    return tr( "Equivalent Air Depth (EAD)" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Partial Pressure Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< SVariableInfo >( "ead", tr( "Equivalent Air Depth EAD" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "fn2", tr( "FN2" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "depth", tr( "Depth" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "fn2AtSurface", tr( "FN2 @ Surface" ), EVariableType::eFN2AtSurfaceConstant, EUnit::ePercent, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "depthToSingleAtmosphere", tr( "Depth to Single Atmosphere" ), EVariableType::eDepthToSingleAtmosphereConstant, EUnit::eLength, EVariableLoc::eRHS ),   //
        };
}

QString CCalculator::getBaseFormula() const
{
    return R"__(<ead> = [(\frac{<fn2>}{<fn2AtSurface>}) \times (<depth> + <depthToSingleAtmosphere>)] - <depthToSingleAtmosphere>)__";
}

QString CCalculator::computeAndGenerateFormula( bool &isBaseFormula ) const
{
    auto ead = getVariable( "ead" );
    auto fn2 = getVariable( "fn2" );
    auto depth = getVariable( "depth" );

    QString formula;
    bool aOK = numUnsetVariables() == 1;
    isBaseFormula = false;
    if ( !aOK )
    {
        formula = getBaseFormula();
        isBaseFormula = true;
    }
    else if ( !ead->has_value() )
    {
        ead->setValue( ( ( fn2->value() / NUtilities::NConstants::percentN2AtSurface() ) * ( depth->value() + NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) ) - NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) );
        formula = getBaseFormula();
    }
    else if ( !fn2->has_value() )
    {
        fn2->setValue( ( NUtilities::NConstants::percentN2AtSurface() * ( ead->value() + NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) ) / ( depth->value() + NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) );
        formula = tr( R"__(<fn2> = \frac{[<fn2AtSurface> \times (<ead>+<depthToSingleAtmosphere>)]}{(<depth>+<depthToSingleAtmosphere>)})__" );
    }
    else if ( !depth->has_value() )
    {
        depth->setValue( ( ( ead->value() + NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) / ( fn2->value() / NUtilities::NConstants::percentN2AtSurface() ) ) - NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) );
        formula = tr( R"__(<depth> = [\frac{(<ead>+<depthToSingleAtmosphere>)}{\frac{<fn2>}{<fn2AtSurface>}]-<depthToSingleAtmosphere>)__" );
    }
    return formula;
}
