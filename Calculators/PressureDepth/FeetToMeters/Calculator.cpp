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

    virtual bool showUnits() const { return false; }

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
    return tr( "Feet to Meters" );
}

QString CCalculator::myReversedCalculatorName() const
{
    return tr( "Meters to Feet" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Pressure and Depth Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< CVariableInfo >( "meters", tr( "Meters" ), EVariableType::eVariable, EUnit::eNone, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "feet", tr( "Feet" ), EVariableType::eVariable, EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::eMetersToFeetConst ),   //
        } );

    retVal.front()->setUnitLabel( NUtilities::NUnitStrings::lengthUnit( false, true, false ) );
    ( *std::next( retVal.begin() ) )->setUnitLabel( NUtilities::NUnitStrings::lengthUnit( true, true, false ) );
    return retVal;
}

QString CCalculator::myBaseFormula() const
{
    return NUtilities::NConversions::feetToMetersFormula( "feet", "meters" );
}

QString CCalculator::myReversedBaseFormula() const
{
    return NUtilities::NConversions::metersToFeetFormula( "feet", "meters" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar ) const
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