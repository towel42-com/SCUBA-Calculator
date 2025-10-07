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
    virtual QString computeAndGenerateFormula() const override;
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Maximum Operating Depth (MOD)";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Partial Pressure Calculations" };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< SVariableInfo >( "mod", tr( "Maximum Operating Depth (MOD)" ), EVariableType::eVariable, EUnit::eLength, ESide::eLHS ),   //
            std::make_shared< SVariableInfo >( "maxPO2", tr( "Maximum PO2" ), EVariableType::eVariable, EUnit::ePercent, ESide::eRHS ),   //
            std::make_shared< SVariableInfo >( "fo2", tr( "FO2" ), EVariableType::eVariable, EUnit::ePercent, ESide::eRHS ),   //
            std::make_shared< SVariableInfo >( "depthToSingleAtmosphere", tr( "Depth to Single Atmosphere" ), EVariableType::eDepthToSingleAtmosphereConst, EUnit::eLength, ESide::eRHS ),   //
        } );
    ( *std::next( retVal.begin() ) )->fRange = { 0.21, 2.0, 1.4, 0.1 };
    return retVal;
}

QString CCalculator::getDefaultFormula() const
{
    return tr( R"__(<mod>=[(\frac{<maxPO2>}{<fo2>})-1]\times<depthToSingleAtmosphere>)__" );
}

QString CCalculator::computeAndGenerateFormula() const
{
    auto mod = getVariable( "mod" );
    auto maxPO2 = getVariable( "maxPO2" );
    auto fo2 = getVariable( "fo2" );

    QString formula;
    bool aOK = numUnsetVariables() == 1;
    if ( !aOK || !mod->has_value() )
    {
        if ( aOK )
        {
            mod->setValue( ( ( maxPO2->value() / fo2->value() ) - 1 ) * NUtilities::NConstants::depthToSingleAtmosphere( imperial(), saltWater() ) );
        }
        formula = getDefaultFormula();
    }
    else if ( !maxPO2->has_value() )
    {
        maxPO2->setValue( fo2->value() * ( ( mod->value() / NUtilities::NConstants::depthToSingleAtmosphere( imperial(), saltWater() ) ) + 1 ) );
        formula = tr( R"__(<maxPO2>=<fo2>\times[(\frac{<mod>}{<depthToSingleAtmosphere>})+1])__" );
    }
    else if ( !fo2->has_value() )
    {
        fo2->setValue( maxPO2->value() / ( ( mod->value() / NUtilities::NConstants::depthToSingleAtmosphere( imperial(), saltWater() ) ) + 1 ) );
        formula = tr( R"__(<fo2>=\frac{<maxPO2>}[(\frac{<mod>}{<depthToSingleAtmosphere>})+1])__" );
    }
    return formula;
}
