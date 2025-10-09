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
    return "Depth Seawater to Freshwater";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure and Depth Conversions" };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< SVariableInfo >( "freshWater", tr( "Freshwater" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "seaWater", tr( "Seawater" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "freshWaterToSeaWater", tr( "Freshwater to Seawater" ), EVariableType::eSeaWaterToFreshWaterConst, EUnit::eNone, EVariableLoc::eRHS ),   //
        } );

    return retVal;
}

QString CCalculator::getDefaultFormula() const
{
    return NUtilities::depthSeawaterToFreshwaterFormula( "freshWater", "seaWater", "seaWaterToFreshWater" );
}

QString CCalculator::computeAndGenerateFormula() const
{
    auto freshWater = getVariable( "freshWater" );
    auto seaWater = getVariable( "seaWater" );

    QString formula;
    bool aOK = numUnsetVariables() == 1;
    if ( !aOK || !freshWater->has_value() )
    {
        if ( aOK )
            freshWater->setValue( NUtilities::depthSeawaterToFreshwater( freshWater->value() ) );
        formula = getDefaultFormula();
    }
    else if ( !seaWater->has_value() )
    {
        seaWater->setValue( NUtilities::depthFreshwaterToSeawater( freshWater->value() ) );
        formula = NUtilities::depthFreshwaterToSeawaterFormula( "freshWater", "seaWater", "seaWaterToFreshWater" );
    }
    return formula;
}

