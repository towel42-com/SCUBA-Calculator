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
    virtual std::optional< std::pair< QString, QString > > fromToLabels() const override;

    virtual QStringList myCalculatorPath() const override;
    virtual QStringList myReversedCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual bool showUnits() const { return false; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual std::optional< TFormulaStringList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaStringList > myReversedBaseFormulas( bool imperial, bool seaWater ) const override;
    virtual std::optional< TFormulaStringList > getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

std::optional< std::pair< QString, QString > > CCalculator::fromToLabels() const
{
    return std::make_pair( tr( "Cubic Feet" ), tr( "Liters" ) );
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
            std::make_shared< CVariableInfo >( "cubicfeet", tr( "Volume" ), EVariableType::eVariable, EVariableLoc::eRHS, EUnit::eVolume, true ),   //
            std::make_shared< CVariableInfo >( "liters", tr( "Volume" ), EVariableType::eVariable, EVariableLoc::eLHS, EUnit::eVolume, false ),   //
        } );

    return retVal;
}

std::optional< TFormulaStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { NUtilities::NConversions::cubicFeetToLitersFormula( getVariable( "cubicfeet" ), getVariable( "liters" ) ) } );
}

std::optional< TFormulaStringList > CCalculator::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { NUtilities::NConversions::litersToCubicFeetFormula( getVariable( "cubicfeet" ), getVariable( "liters" ) ) } );
}

std::optional< TFormulaStringList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "liters" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "cubicfeet" )
    {
        return myReversedBaseFormulas( imperial, seaWater );
    }
    return {};
}

void CCalculator::computeVariableValues()
{
    auto cubicFeet = getVariable( "cubicfeet" );
    auto liters = getVariable( "liters" );

    if ( !liters->has_value() && liters->dependenciesSatisfied() )
    {
        liters->setValue( NUtilities::NConversions::cubicFeetToLiters( cubicFeet->value() ) );
    }

    if ( !cubicFeet->has_value() && cubicFeet->dependenciesSatisfied() )
    {
        cubicFeet->setValue( NUtilities::NConversions::litersToCubicFeet( liters->value() ) );
    }
}