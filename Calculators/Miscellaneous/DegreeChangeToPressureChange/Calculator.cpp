#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "DegreeChangeToPressureChange" ); }
    virtual ~CCalculator() override {}

    virtual bool isReversible() const override { return true; }
    virtual std::optional< std::pair< QString, QString > > fromToLabels() const override;

    virtual QString myCalculatorName() const override;
    virtual QString myReversedCalculatorName() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual QStringList myCalculatorPath() const override;

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual std::optional< TFormulaList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaList > myReversedBaseFormulas( bool imperial, bool seaWater ) const override;
    virtual std::optional< TFormulaList > getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
};

extern "C" CCalculatorBase *instantiateCalculator()
{
    return new CCalculator;
}

std::optional< std::pair< QString, QString > > CCalculator::fromToLabels() const
{
    return std::make_pair( tr( "Pressure Change" ), tr( "Temperature Change" ) );
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Quick Estimate for Pressure when Temperature Changes" );
}

QString CCalculator::myReversedCalculatorName() const
{
    return tr( "Quick Estimate for Temperature when Pressure Changes" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Miscellaneous" ) };
}

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "p1", tr( "Pressure Change" ), EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "t1", tr( "Temperature Change" ), EUnit::eTemperature, EVariableLoc::eRHS ),   //
        };
}

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { NUtilities::NConversions::pressureChangeForDegreeChangeFormula( getVariable( "t1" ), getVariable( "p1" ) ) } );
}

std::optional< TFormulaList > CCalculator::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { NUtilities::NConversions::degreeChangeForPressureChangeFormula( getVariable( "t1" ), getVariable( "p1" ) ) } );
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "p1" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "t1" )
    {
        return myReversedBaseFormulas( imperial, seaWater );
    }

    return {};
}

void CCalculator::computeVariableValues()
{
    auto p1 = getVariable( "p1" );
    auto t1 = getVariable( "t1" );

    if ( !p1->has_value() && p1->dependenciesSatisfied() )
    {
        p1->setValue( NUtilities::NConversions::pressureChangeForDegreeChange( imperial(), t1->value() ) );
    }
    if ( !t1->has_value() && t1->dependenciesSatisfied() )
    {
        t1->setValue( NUtilities::NConversions::degreeChangeForPressureChange( imperial(), p1->value() ) );
    }
}