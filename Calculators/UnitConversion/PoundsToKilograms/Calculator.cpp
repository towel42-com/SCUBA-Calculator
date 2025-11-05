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
        } );

    return retVal;
}

std::optional< TFormulaStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { NUtilities::NConversions::lbsToKGsFormula( "pounds", "kiloGrams" ) } );
}

std::optional< TFormulaStringList > CCalculator::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { NUtilities::NConversions::kgsToLbsFormula( "pounds", "kiloGrams" ) } );
}

std::optional< TFormulaStringList > CCalculator::getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar->name() == "kiloGrams" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar->name() == "pounds" )
    {
        return myReversedBaseFormulas( imperial, seaWater );
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