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
    return tr( "Depth Seawater to Freshwater" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Pressure and Depth Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< SVariableInfo >( "freshWater", tr( "Freshwater" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "seaWater", tr( "Seawater" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "freshWaterToSeaWater", tr( "Freshwater to Seawater" ), EVariableType::eSeaWaterToFreshWaterConstant, EUnit::eNone, EVariableLoc::eRHS ),   //
        } );

    return retVal;
}

QString CCalculator::getBaseFormula() const
{
    return NUtilities::depthSeawaterToFreshwaterFormula( "freshWater", "seaWater", "seaWaterToFreshWater" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo & unsetVar ) const
{
    if ( unsetVar->name() == "freshWater" )
    {
        return getBaseFormula();
    }
    else if ( unsetVar->name() == "seaWater" )
    {
        return NUtilities::depthFreshwaterToSeawaterFormula( "freshWater", "seaWater", "seaWaterToFreshWater" );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo & unsetVar )
{
    auto freshWater = getVariable( "freshWater" );
    auto seaWater = getVariable( "seaWater" );

    if ( unsetVar == freshWater )
    {
        freshWater->setValue( NUtilities::depthSeawaterToFreshwater( freshWater->value() ) );
    }
    else if ( unsetVar == seaWater )
    {
        seaWater->setValue( NUtilities::depthFreshwaterToSeawater( freshWater->value() ) );
    }
}
