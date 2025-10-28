#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "PoundsToKilograms" ); }
    virtual ~CCalculator() override {}

    virtual bool isReversible() const override { return true; }
    virtual std::pair< QString, QString > fromToLabels() const override;

    virtual QStringList myCalculatorPath() const override;
    virtual QStringList myReversedCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual bool showUnits() const { return false; }

    virtual TVariableInfoList getMyVariables() const override;

    virtual std::optional< QString > myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QString > myReversedBaseFormula() const override;
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

std::pair< QString, QString > CCalculator::fromToLabels() const
{
    return { tr( "Pounds" ), tr( "Kilograms" ) };
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Unit Conversion" ), tr( "Imperial to Metric" ) };
}

QStringList CCalculator::myReversedCalculatorPath() const
{
    return { tr( "Unit Conversion" ), tr( "Metric to Imperial" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< CVariableInfo >( "pounds", tr( "Weight" ), EVariableType::eVariable, EVariableLoc::eRHS, EUnit::eWeight, true ),   //
            std::make_shared< CVariableInfo >( "kiloGrams", tr( "Weight" ), EVariableType::eVariable, EVariableLoc::eLHS, EUnit::eWeight, false ),   //
            std::make_shared< CVariableInfo >( EVariableType::eLbsPerKgsConst ),   //
            std::make_shared< CVariableInfo >( EVariableType::eKgsPerLbsConst )   //
        } );

    return retVal;
}

std::optional< QString > CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return NUtilities::NConversions::lbsToKGsFormula( "pounds", "kiloGrams" );
}

std::optional< QString > CCalculator::myReversedBaseFormula() const
{
    return NUtilities::NConversions::kgsToLbsFormula( "pounds", "kiloGrams" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "kiloGrams" )
    {
        return NUtilities::NConversions::lbsToKGsFormula( "pounds", "kiloGrams" );
    }
    else if ( unsetVar->name() == "pounds" )
    {
        return NUtilities::NConversions::kgsToLbsFormula( "pounds", "kiloGrams" );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto pounds = getVariable( "pounds" );
    auto kiloGrams = getVariable( "kiloGrams" );

    if ( unsetVar == kiloGrams )
    {
        kiloGrams->setValue( NUtilities::NConversions::lbsToKGs( pounds->value() ) );
    }
    else if ( unsetVar == pounds )
    {
        pounds->setValue( NUtilities::NConversions::kgsToLbs( kiloGrams->value() ) );
    }
}