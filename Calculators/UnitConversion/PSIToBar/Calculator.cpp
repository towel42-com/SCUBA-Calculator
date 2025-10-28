#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "PSIToBar" ); }
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
    return { tr( "PSI" ), tr( "BAR" ) };
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
            std::make_shared< CVariableInfo >( "psi", tr( "Pressure" ), EVariableType::eVariable, EVariableLoc::eRHS, EUnit::ePressure, true ),   //
            std::make_shared< CVariableInfo >( "bar", tr( "Pressure" ), EVariableType::eVariable, EVariableLoc::eLHS, EUnit::ePressure, false),   //
            std::make_shared< CVariableInfo >( EVariableType::ePSIToBarConst ),   //
        } );

    return retVal;
}

std::optional< QString > CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return NUtilities::NConversions::psiToBarFormula( "psi", "bar" );
}

std::optional< QString > CCalculator::myReversedBaseFormula() const
{
    return NUtilities::NConversions::barToPSIFormula( "psi", "bar" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    auto psi = getVariable( "psi" );
    auto bar = getVariable( "bar" );

    if ( unsetVar->name() == "bar" )
    {
        return NUtilities::NConversions::psiToBarFormula( "psi", "bar" );
    }
    else if ( unsetVar->name() == "psi" )
    {
        return NUtilities::NConversions::barToPSIFormula( "psi", "bar" );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto psi = getVariable( "psi" );
    auto bar = getVariable( "bar" );

    if ( unsetVar == bar )
    {
        bar->setValue( NUtilities::NConversions::psiToBar( psi->value() ) );
    }
    else if ( unsetVar == psi )
    {
        psi->setValue( NUtilities::NConversions::barToPSI( bar->value() ) );
    }
}