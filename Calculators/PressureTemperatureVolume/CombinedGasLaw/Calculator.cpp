#include "Calculator.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "CombinedGasLaw" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual TVariableInfoList getMyVariables() const override;
    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior );

    virtual QString myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo & unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Combined Gas Law" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure, Temperature and Volume Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "p1", tr( "Pressure 1" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "v1", tr( "Volume 1" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "t1", tr( "Temperature 1" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "p2", tr( "Pressure 2" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "v2", tr( "Volume 2" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "t2", tr( "Temperature 2" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::eAbsZeroOffsetConst ),   //
        };
}

TVariableInfo CCalculator::determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior )
{
    TVariableInfo retVal;

    if ( updateFromSide == EVariableLoc::eLHS )
    {
        if ( getVariable( "p1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "p2" );
        else if ( getVariable( "t1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "t2" );
        else if ( getVariable( "v1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "v2" );
    }
    else if ( updateFromSide == EVariableLoc::eRHS )
    {
        if ( getVariable( "p2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "p1" );
        else if ( getVariable( "t2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "t1" );
        else if ( getVariable( "v2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "v1" );
    }
    else
        retVal = CSCUBACalculator::determineVariableToUnset( updateFromSide, triggerWidget, preDefaultBehavior );
    return retVal;
}

QString CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return R"__(\frac{<p1> \times <v1>}{<t1>} = \frac{<p2> \times <v2>}{<t2>})__";
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "p1" )
    {
        return R"__(<p1> = <p2> \times \frac{<v2>}{<v1>} \times \frac{<t1> + <absOffset>}{<t2> + <absOffset>})__";
    }
    else if ( unsetVar->name() == "p2" )
    {
        return R"__(<p2> = <p1> \times \frac{<v1>}{<v2>} \times \frac{<t2> + <absOffset>}{<t1> + <absOffset>})__";
    }
    else if ( unsetVar->name() == "v1" )
    {
        return R"__(<v1> = <v2> \times \frac{<t1> + <absOffset>}{<t2> + <absOffset>} \times \frac{<p2>}{<p1>})__";
    }
    else if ( unsetVar->name() == "v2" )
    {
        return R"__(<v2> = <v1> \times \frac{<t2> + <absOffset>}{<t1> + <absOffset>} \times \frac{<p1>}{<p2>})__";
    }
    else if ( unsetVar->name() == "t1" )
    {
        return R"__(<t1> = <t2> \times \frac{<p1>}{<p2>} \times \frac{<v1>}{<v2>})__";
    }
    else if ( unsetVar->name() == "t2" )
    {
        return R"__(<t2> = <t1> \times \frac{<p2>}{<p1>} \times \frac{<v2>}{<v1>})__";
    }

    return {};
}

void CCalculator::computeValueForVar( TVariableInfo & unsetVar )
{
    auto p1 = getVariable( "p1" );
    auto v1 = getVariable( "v1" );
    auto t1 = getVariable( "t1" );

    auto p2 = getVariable( "p2" );
    auto v2 = getVariable( "v2" );
    auto t2 = getVariable( "t2" );

    if ( unsetVar == p1 )
    {
        p1->setValue( p2->value() * ( v2->value() / v1->value() ) * ( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) ) );
    }
    else if ( unsetVar == p2 )
    {
        p2->setValue( p1->value() * ( v1->value() / v2->value() ) * ( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) ) );
    }
    else if ( unsetVar == v1 )
    {
        v1->setValue( v2->value() * ( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) ) * ( p2->value() / p1->value() ) );
    }
    else if ( unsetVar == v2 )
    {
        v2->setValue( v1->value() * ( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) ) * ( p1->value() / p2->value() ) );
    }
    else if ( unsetVar == t1 )
    {
        t1->setValue( t2->value() * ( p1->value() / p2->value() ) * ( v1->value() / v2->value() ) );
    }
    else if ( unsetVar == t2 )
    {
        t2->setValue( t1->value() * ( p2->value() / p1->value() ) * ( v2->value() / v1->value() ) );
    }
}