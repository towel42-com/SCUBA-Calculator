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
    return tr( "Depth Freshwater to Seawater" );
}

QString CCalculator::myReversedCalculatorName() const
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
            std::make_shared< CVariableInfo >( "seaWater", tr( "Seawater" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "freshWater", tr( "Freshwater" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::eFreshWaterToSeaWaterConst ),   //
        } );

    return retVal;
}

QString CCalculator::myBaseFormula() const
{
    return NUtilities::depthFreshwaterToSeawaterFormula( "freshWater", "seaWater" );
}

QString CCalculator::myReversedBaseFormula() const
{
    return NUtilities::depthSeawaterToFreshwaterFormula( "freshWater", "seaWater" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar ) const
{
    if ( unsetVar->name() == "seaWater" )
    {
        return NUtilities::depthFreshwaterToSeawaterFormula( "freshWater", "seaWater" );
    }
    else if ( unsetVar->name() == "freshWater" )
    {
        return NUtilities::depthSeawaterToFreshwaterFormula( "freshWater", "seaWater" );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto freshWater = getVariable( "freshWater" );
    auto seaWater = getVariable( "seaWater" );

    if ( unsetVar == seaWater )
    {
        seaWater->setValue( NUtilities::depthFreshwaterToSeawater( freshWater->value() ) );
    }
    else if ( unsetVar == freshWater )
    {
        freshWater->setValue( NUtilities::depthSeawaterToFreshwater( freshWater->value() ) );
    }
}