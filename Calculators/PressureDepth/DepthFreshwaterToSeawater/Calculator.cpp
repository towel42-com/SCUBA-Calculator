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
    return tr( "Depth Freshwater to Seawater" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Pressure and Depth Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< SVariableInfo >( "seaWater", tr( "Seawater" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "freshWater", tr( "Freshwater" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "seaWaterToFreshWater", tr( "Seawater to Freshwater" ), EVariableType::eSeaWaterToFreshWaterConstant, EUnit::eNone, EVariableLoc::eRHS ),   //
        } );

    return retVal;
}

QString CCalculator::getBaseFormula() const
{
    return NUtilities::depthFreshwaterToSeawaterFormula( "freshWater", "seaWater", "seaWaterToFreshWater" );
}

QString CCalculator::computeAndGenerateFormula( bool &isBaseFormula ) const
{
    auto freshWater = getVariable( "freshWater" );
    auto seaWater = getVariable( "seaWater" );

    QString formula;
    bool aOK = numUnsetVariables() == 1;
    isBaseFormula = false;
    if ( !aOK )
    {
        formula = getBaseFormula();
        isBaseFormula = true;
    }
    else if ( !seaWater->has_value() )
    {
        seaWater->setValue( NUtilities::depthFreshwaterToSeawater( freshWater->value() ) );
        formula = getBaseFormula();
    }
    else if ( !freshWater->has_value() )
    {
        freshWater->setValue( NUtilities::depthSeawaterToFreshwater( freshWater->value() ) );
        formula = NUtilities::depthSeawaterToFreshwaterFormula( "freshWater", "seaWater", "seaWaterToFreshWater" );
    }
    return formula;
}
