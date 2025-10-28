#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "CubicFeetToLiters" ); }
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
    return { tr( "Cubic Feet" ), tr( "Liters" ) };
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
            std::make_shared< CVariableInfo >( "cubicfeet", tr( "Volume" ), EVariableType::eVariable, EVariableLoc::eRHS, EUnit::eVolume, true ),   //
            std::make_shared< CVariableInfo >( "liters", tr( "Volume" ), EVariableType::eVariable, EVariableLoc::eLHS, EUnit::eVolume, false ),   //
            std::make_shared< CVariableInfo >( EVariableType::eCubicFeetToLitersConst ),   //
            std::make_shared< CVariableInfo >( EVariableType::eLitersToCubicFeetConst ),   //
        } );

    return retVal;
}

std::optional< QString > CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return NUtilities::NConversions::cubicFeetToLitersFormula( "cubicfeet", "liters" );
}

std::optional< QString > CCalculator::myReversedBaseFormula() const
{
    return NUtilities::NConversions::litersToCubicFeetFormula( "cubicfeet", "liters" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "liters" )
    {
        return NUtilities::NConversions::cubicFeetToLitersFormula( "cubicfeet", "liters" );
    }
    else if ( unsetVar->name() == "cubicfeet" )
    {
        return NUtilities::NConversions::litersToCubicFeetFormula( "cubicfeet", "liters" );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto cubicfeet = getVariable( "cubicfeet" );
    auto liters = getVariable( "liters" );

    if ( unsetVar == liters )
    {
        liters->setValue( NUtilities::NConversions::cubicFeetToLiters( cubicfeet->value() ) );
    }
    else if ( unsetVar == cubicfeet )
    {
        cubicfeet->setValue( NUtilities::NConversions::litersToCubicFeet( liters->value() ) );
    }
}