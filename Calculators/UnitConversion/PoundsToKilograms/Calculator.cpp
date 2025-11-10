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
    virtual std::optional< std::pair< QString, QString > > fromToLabels() const override;

    virtual QStringList myCalculatorPath() const override;
    virtual QStringList myReversedCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual bool showUnits() const { return false; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual std::optional< TFormulaList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaList > myReversedBaseFormulas( bool imperial, bool seaWater ) const override;
    virtual std::optional< TFormulaList > getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

std::optional< std::pair< QString, QString > > CCalculator::fromToLabels() const
{
    return std::make_pair( tr( "Pounds" ), tr( "Kilograms" ) );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Unit Conversion" ), tr( "Imperial to Metric" ) };
}

QStringList CCalculator::myReversedCalculatorPath() const
{
    return { tr( "Unit Conversion" ), tr( "Metric to Imperial" ) };
}

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< CVariableInfo >( "pounds", tr( "Weight" ), EVariableType::eVariable, EVariableLoc::eRHS, EUnit::eWeight, true ),   //
            std::make_shared< CVariableInfo >( "kiloGrams", tr( "Weight" ), EVariableType::eVariable, EVariableLoc::eLHS, EUnit::eWeight, false ),   //
        } );

    return retVal;
}

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { NUtilities::NConversions::lbsToKGsFormula( getVariable( "pounds" ), getVariable( "kiloGrams" ) ) } );
}

std::optional< TFormulaList > CCalculator::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { NUtilities::NConversions::kgsToLbsFormula( getVariable( "pounds" ), getVariable( "kiloGrams" ) ) } );
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "kiloGrams" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "pounds" )
    {
        return myReversedBaseFormulas( imperial, seaWater );
    }
    return {};
}

void CCalculator::computeVariableValues()
{
    auto pounds = getVariable( "pounds" );
    auto kiloGrams = getVariable( "kiloGrams" );

    if ( kiloGrams->has_value() && !kiloGrams->dependenciesSatisfied() )
    {
        kiloGrams->setValue( NUtilities::NConversions::lbsToKGs( pounds->value() ) );
    }
    
    if ( !pounds->has_value() && !pounds->dependenciesSatisfied() )
    {
        pounds->setValue( NUtilities::NConversions::kgsToLbs( kiloGrams->value() ) );
    }
}