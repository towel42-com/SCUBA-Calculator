#include "Calculator.h"
#include "VariableInfo.h"

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

    virtual TVariableInfoList getMyVariables() const override;
    virtual TVariableInfoList getMyVariables( bool *preReversed ) const override;
    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior );

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
    return tr( "Calculating Gas Consumption Time Based on Known Consumption at a Specific Depth" );
}

QString CCalculator::myReversedCalculatorName() const
{
    return tr( "Calculating Gas Consumption Used based on Known Consumption at a Specific Depth" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Pressure and Volume Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    bool preReversed = true;
    return getMyVariables( &preReversed );
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

    if ( updateFromSide == EVariableLoc::eLHS )
    {
        if ( getVariable( "p1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "p2" );
        else if ( getVariable( "m1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "m2" );
    }
    else if ( updateFromSide == EVariableLoc::eRHS )
    {
        if ( getVariable( "p2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "p1" );
        else if ( getVariable( "m2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "m1" );
    }
    else
        retVal = CSCUBACalculator::determineVariableToUnset( updateFromSide, triggerWidget, preDefaultBehavior );
    return retVal;
}

QString CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return R"__(<m2> = <m1> \times \frac{<p1>}{<p2>})__";
}

QString CCalculator::myReversedBaseFormula() const
{
    return R"__(<p2> = <p1> \times \frac{<m1>}{<m2>})__";
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "p1" )
    {
        return R"__(<p1> = <p2> \times \frac{<m2>}{<m1>})__";
    }
    else if ( unsetVar->name() == "p2" )
    {
        return R"__(<p2> = <p1> \times \frac{<m1>}{<m2>})__";
    }
    else if ( unsetVar->name() == "m1" )
    {
        return R"__(<m1> = <m2> \times \frac{<p2>}{<p1>})__";
    }
    else if ( unsetVar->name() == "m2" )
    {
        return R"__(<m2> = <m1> \times \frac{<p1>}{<p2>})__";
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto p1 = getVariable( "p1" );
    auto m1 = getVariable( "m1" );

    auto p2 = getVariable( "p2" );
    auto m2 = getVariable( "m2" );

    if ( unsetVar->name() == "p1" )
    {
        p1->setValue( p2->value() * ( m2->value() / m1->value() ) );
    }
    else if ( unsetVar->name() == "p2" )
    {
        p2->setValue( p1->value() * ( m1->value() / m2->value() ) );
    }
    else if ( unsetVar->name() == "m1" )
    {
        m1->setValue( m2->value() * ( p2->value() / p1->value() ) );
    }
    else if ( unsetVar->name() == "m2" )
    {
        m2->setValue( m1->value() * ( p1->value() / p2->value() ) );
    }
}
