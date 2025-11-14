#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/Formula.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "PressureChangeinVolume" ); }
    virtual ~CCalculator() override {}

    virtual bool isReversible() const override { return true; }

    virtual QString myCalculatorName() const override;
    virtual QString myReversedCalculatorName() const override;

    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;
    
    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior );

    virtual std::optional< TFormulaList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaList > myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const override;
    virtual std::optional< TFormulaList > getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
};

extern "C" CCalculatorBase *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Calculating Pressure Following a Change in Volume" );
}

QString CCalculator::myReversedCalculatorName() const
{
    return tr( "Calculating Volume Following a Change in Pressure" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure, Temperature and Volume Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables( bool *preReversed ) const
{
    *preReversed = true;

    auto retVal = TVariableInfoList( {
        std::make_shared< CVariableInfo >( "p1", tr( "Pressure 1" ), EUnit::ePressure, EVariableLoc::eRHS ),   //
        std::make_shared< CVariableInfo >( "v1", tr( "Volume 1" ), EUnit::eVolume, EVariableLoc::eRHS ),   //
        std::make_shared< CVariableInfo >( "p2", tr( "Pressure 2" ), EUnit::ePressure, EVariableLoc::eLHS ),   //
        std::make_shared< CVariableInfo >( "v2", tr( "Volume 2" ), EUnit::eVolume, EVariableLoc::eRHS )   //
    } );

    if ( isReversed() )
    {
        for ( auto &&ii : retVal )
        {
            if ( ii->name() == "v2" )
                ii->setVariableLoc( EVariableLoc::eLHS );
            else
                ii->setVariableLoc( EVariableLoc::eRHS );
        }
    }
    return retVal;
}

TVariableInfo CCalculator::determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior )
{
    TVariableInfo retVal;

    if ( getVariable( "p1" )->isWidget( triggerWidget ) )
        retVal = getVariable( "p2" );
    else if ( getVariable( "v1" )->isWidget( triggerWidget ) )
        retVal = getVariable( "v2" );
    else if ( getVariable( "p2" )->isWidget( triggerWidget ) )
        retVal = getVariable( "p1" );
    else if ( getVariable( "v2" )->isWidget( triggerWidget ) )
        retVal = getVariable( "v1" );
    else
        retVal = CCalculatorBase::determineVariableToUnset( updateFromSide, triggerWidget, preDefaultBehavior );

    return retVal;
}

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { std::make_shared< CFormula >( getVariable( "p2" ), QString( R"__(<p1> \times \frac{<v1>}{<v2>})__" ) ) } );
}

std::optional< TFormulaList > CCalculator::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { std::make_shared< CFormula >( getVariable( "v2" ), QString( R"__(<v1> \times \frac{<p1>}{<p2>})__" ) ) } );
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "p2" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "p1" )
    {
        // p1 = p2 * (v2/v1);
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(<p2> \times \frac{<v2>}{<v1>})__" ) ) } );
    }
    else if ( unsetVar == "v2" )
    {
        return myReversedBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "v1" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(<v2> \times \frac{<p2>}{<p1>})__" ) ) } );
    }

    return {};
}

void CCalculator::computeVariableValues()
{
    auto p1 = getVariable( "p1" );
    auto v1 = getVariable( "v1" );

    auto p2 = getVariable( "p2" );
    auto v2 = getVariable( "v2" );

    if ( !p1->has_value() && p1->dependenciesSatisfied() )
    {
        p1->setValue( p2->value() * ( v2->value() / v1->value() ) );
    }

    if ( !p2->has_value() && p2->dependenciesSatisfied() )
    {
        p2->setValue( p1->value() * ( v1->value() / v2->value() ) );
    }

    if ( !v1->has_value() && v1->dependenciesSatisfied() )
    {
        // v1 = v2 * (p2/p1)
        v1->setValue( v2->value() * ( p2->value() / p1->value() ) );
    }

    if ( !v2->has_value() && v2->dependenciesSatisfied() )
    {
        // v2 = v1 * (p1/p2)
        v2->setValue( v1->value() * ( p1->value() / p2->value() ) );
    }
}