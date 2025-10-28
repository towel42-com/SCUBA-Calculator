#include "Calculator.h"
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
            std::make_shared< CVariableInfo >( EVariableType::eFeetToMetersConst ),   //
            std::make_shared< CVariableInfo >( EVariableType::eMetersToFeetConst ),   //
        } );

    return retVal;
}

std::optional< QString > CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return NUtilities::NConversions::feetToMetersFormula( "feet", "meters" );
}

std::optional< QString > CCalculator::myReversedBaseFormula() const
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