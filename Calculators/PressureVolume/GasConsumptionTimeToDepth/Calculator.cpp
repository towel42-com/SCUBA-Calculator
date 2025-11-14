#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/Formula.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "GasConsumptionTimeToDepth" ); }
    virtual ~CCalculator() override {}

    virtual bool isReversible() const override { return true; }
    virtual std::optional< std::pair< QString, QString > > fromToLabels() const override;

    virtual QString myCalculatorName() const override;
    virtual QString myReversedCalculatorName() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual QStringList myCalculatorPath() const override;

    virtual TVariableInfoList getMyVariables( bool *preReversed ) const override;

    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior );

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
    return std::make_pair( tr( "Gas Used" ), tr( "Gas Consumption Time" ) );
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Calculating Gas Consumption Time Based on Known Consumption at a Specific Depth" );
}

QString CCalculator::myReversedCalculatorName() const
{
    return tr( "Calculating Gas Consumption Used based on Known Consumption at a Specific Depth" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure and Volume Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables( bool *preReversed ) const
{
    *preReversed = true;
    auto retVal = TVariableInfoList( {
        std::make_shared< CVariableInfo >( "p1", tr( "Gas Consumption 1" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
        std::make_shared< CVariableInfo >( "m1", tr( "Time 1" ), EVariableType::eVariable, EUnit::eTime, EVariableLoc::eRHS ),   //
    } );
    if ( isReversed() )
    {
        retVal.push_back( std::make_shared< CVariableInfo >( "m2", tr( "Time 2" ), EVariableType::eVariable, EUnit::eTime, EVariableLoc::eRHS ) );
        retVal.push_back( std::make_shared< CVariableInfo >( "p2", tr( "Gas Consumption 2" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ) );
    }
    else
    {
        retVal.push_back( std::make_shared< CVariableInfo >( "m2", tr( "Time 2" ), EVariableType::eVariable, EUnit::eTime, EVariableLoc::eLHS ) );
        retVal.push_back( std::make_shared< CVariableInfo >( "p2", tr( "Gas Consumption 2" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ) );
    }
    return retVal;
}

TVariableInfo CCalculator::determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior )
{
    TVariableInfo retVal;

    if ( getVariable( "p1" )->isWidget( triggerWidget ) )
        retVal = getVariable( "p2" );
    else if ( getVariable( "m1" )->isWidget( triggerWidget ) )
        retVal = getVariable( "m2" );
    else if ( getVariable( "p2" )->isWidget( triggerWidget ) )
        retVal = getVariable( "p1" );
    else if ( getVariable( "m2" )->isWidget( triggerWidget ) )
        retVal = getVariable( "m1" );
    else
        retVal = CCalculatorBase::determineVariableToUnset( updateFromSide, triggerWidget, preDefaultBehavior );
    return retVal;
}

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { std::make_shared< CFormula >( getVariable( "m2" ), QString( R"__(<m1> \times \frac{<p1>}{<p2>})__" ) ) } );
}

std::optional< TFormulaList > CCalculator::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { std::make_shared< CFormula >( getVariable( "p2" ), QString( R"__(<p1> \times \frac{<m1>}{<m2>})__" ) ) } );
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "m2" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "p2" )
    {
        return myReversedBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "m1" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(<m2> \times \frac{<p2>}{<p1>})__" ) ) } );
    }
    else if ( unsetVar == "p1" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(<p2> \times \frac{<m2>}{<m1>})__" ) ) } );
    }
    return {};
}

void CCalculator::computeVariableValues()
{
    auto p1 = getVariable( "p1" );
    auto m1 = getVariable( "m1" );

    auto p2 = getVariable( "p2" );
    auto m2 = getVariable( "m2" );

    if ( !p1->has_value() && p1->dependenciesSatisfied() )
    {
        p1->setValue( p2->value() * ( m2->value() / m1->value() ) );
    }

    if ( !p2->has_value() && p2->dependenciesSatisfied() )
    {
        p2->setValue( p1->value() * ( m1->value() / m2->value() ) );
    }

    if ( !m1->has_value() && m1->dependenciesSatisfied() )
    {
        m1->setValue( m2->value() * ( p2->value() / p1->value() ) );
    }

    if ( !m2->has_value() && m2->dependenciesSatisfied() )
    {
        m2->setValue( m1->value() * ( p1->value() / p2->value() ) );
    }
}
