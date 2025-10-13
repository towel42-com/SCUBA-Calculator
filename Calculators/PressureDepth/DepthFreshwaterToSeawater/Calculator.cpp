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

    virtual QString myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

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
            std::make_shared< CVariableInfo >( "seaWater", tr( "Seawater" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "freshWater", tr( "Freshwater" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::eFreshWaterToSeaWaterConst ),   //
        } );

    return retVal;
}

QString CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return NUtilities::NConversions::depthFreshwaterToSeawaterFormula( "freshWater", "seaWater" );
}

QString CCalculator::myReversedBaseFormula() const
{
    return NUtilities::NConversions::depthSeawaterToFreshwaterFormula( "freshWater", "seaWater" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "seaWater" )
    {
        return NUtilities::NConversions::depthFreshwaterToSeawaterFormula( "freshWater", "seaWater" );
    }
    else if ( unsetVar->name() == "freshWater" )
    {
        return NUtilities::NConversions::depthSeawaterToFreshwaterFormula( "freshWater", "seaWater" );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto freshWater = getVariable( "freshWater" );
    auto seaWater = getVariable( "seaWater" );

    if ( unsetVar == seaWater )
    {
        seaWater->setValue( NUtilities::NConversions::depthFreshwaterToSeawater( freshWater->value() ) );
    }
    else if ( unsetVar == freshWater )
    {
        freshWater->setValue( NUtilities::NConversions::depthSeawaterToFreshwater( seaWater->value() ) );
    }
}