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
    return std::make_pair( tr( "PSI" ), tr( "BAR" ) );
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
            std::make_shared< CVariableInfo >( "psi", tr( "Pressure" ), EVariableType::eVariable, EVariableLoc::eRHS, EUnit::ePressure, true ),   //
            std::make_shared< CVariableInfo >( "bar", tr( "Pressure" ), EVariableType::eVariable, EVariableLoc::eLHS, EUnit::ePressure, false ),   //
        } );
    return retVal;
}

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { NUtilities::NConversions::psiToBarFormula( getVariable( "psi" ), getVariable( "bar" ) ) } );
}

std::optional< TFormulaList > CCalculator::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { NUtilities::NConversions::barToPSIFormula( getVariable( "psi" ), getVariable( "bar" ) ) } );
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "bar" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "psi" )
    {
        return myReversedBaseFormulas( imperial, seaWater );
    }
    return {};
}

void CCalculator::computeVariableValues()
{
    auto psi = getVariable( "psi" );
    auto bar = getVariable( "bar" );

    if ( !bar->has_value() && bar->dependenciesSatisfied() )
    {
        bar->setValue( NUtilities::NConversions::psiToBar( psi->value() ) );
    }

    if ( !psi->has_value() && psi->dependenciesSatisfied() )
    {
        psi->setValue( NUtilities::NConversions::barToPSI( bar->value() ) );
    }
}