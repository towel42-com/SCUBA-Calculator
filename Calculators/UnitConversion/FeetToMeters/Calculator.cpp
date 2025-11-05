#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "FeetToMeters" ); }
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
    return { tr( "Feet" ), tr( "Meters" ) };
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
            std::make_shared< CVariableInfo >( "feet", tr( "Length" ), EVariableType::eVariable, EVariableLoc::eRHS, EUnit::eLength, true ),   //
            std::make_shared< CVariableInfo >( "meters", tr( "Length" ), EVariableType::eVariable, EVariableLoc::eLHS, EUnit::eLength, false ),   //
        } );

    return retVal;
}

std::optional< TFormulaStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { NUtilities::NConversions::feetToMetersFormula( "feet", "meters" ) } );
}

std::optional< TFormulaStringList > CCalculator::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { NUtilities::NConversions::metersToFeetFormula( "feet", "meters" ) } );
}

std::optional< TFormulaStringList > CCalculator::getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar->name() == "meters" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar->name() == "feet" )
    {
        return myReversedBaseFormulas( imperial, seaWater );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto feet = getVariable( "feet" );
    auto meters = getVariable( "meters" );

    if ( unsetVar == meters )
    {
        meters->setValue( NUtilities::NConversions::feetToMeters( feet->value() ) );
    }
    else if ( unsetVar == feet )
    {
        feet->setValue( NUtilities::NConversions::metersToFeet( meters->value() ) );
    }
}