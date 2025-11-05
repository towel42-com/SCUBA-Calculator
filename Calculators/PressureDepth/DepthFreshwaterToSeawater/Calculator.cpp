#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "DepthFreshwaterToSeawater" ); }
    virtual ~CCalculator() override {}

    virtual bool isReversible() const override { return true; }
    virtual std::pair< QString, QString > fromToLabels() const override;

    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables() const override;

    virtual std::optional< TFormulaStringList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaStringList > myReversedBaseFormulas( bool imperial, bool seaWater ) const override;
    virtual std::optional< TFormulaStringList > getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

std::pair< QString, QString > CCalculator::fromToLabels() const
{
    return { tr( "Depth Freshwater" ), tr( "Depth Seawater" ) };
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure and Depth Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< CVariableInfo >( "seaWater", tr( "Seawater" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "freshWater", tr( "Freshwater" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eRHS ),   //
        } );

    return retVal;
}

std::optional< TFormulaStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { NUtilities::NConversions::depthFreshwaterToSeawaterFormula( "freshWater", "seaWater" ) } );
}

std::optional< TFormulaStringList > CCalculator::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { NUtilities::NConversions::depthSeawaterToFreshwaterFormula( "freshWater", "seaWater" ) } );
}

std::optional< TFormulaStringList > CCalculator::getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar->name() == "seaWater" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar->name() == "freshWater" )
    {
        return myReversedBaseFormulas( imperial, seaWater );
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