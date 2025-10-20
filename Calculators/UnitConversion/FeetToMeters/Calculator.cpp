#include "Calculator.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

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

    virtual bool showUnits() const { return false; }

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
    return tr( "Feet to Meters" );
}

QString CCalculator::myReversedCalculatorName() const
{
    return tr( "Meters to Feet" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Unit Conversion" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< CVariableInfo >( "meters", tr( "Length" ), EVariableType::eVariable, EUnit::eNone, EVariableLoc::eLHS, NUtilities::NUnitStrings::lengthUnit( false, true, false ) ),   //
            std::make_shared< CVariableInfo >( "feet", tr( "Length" ), EVariableType::eVariable, EUnit::eNone, EVariableLoc::eRHS, NUtilities::NUnitStrings::lengthUnit( true, true, false ) ),   //
            std::make_shared< CVariableInfo >( EVariableType::eFeetToMetersConst ),   //
            std::make_shared< CVariableInfo >( EVariableType::eMetersToFeetConst ),   //
        } );

    return retVal;
}

QString CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return NUtilities::NConversions::feetToMetersFormula( "feet", "meters" );
}

QString CCalculator::myReversedBaseFormula() const
{
    return NUtilities::NConversions::metersToFeetFormula( "feet", "meters" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "meters" )
    {
        return NUtilities::NConversions::feetToMetersFormula( "feet", "meters" );
    }
    else if ( unsetVar->name() == "feet" )
    {
        return NUtilities::NConversions::metersToFeetFormula( "feet", "meters" );
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