#include "Calculator.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "VolumeChangeInTemp" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual TVariableInfoList getMyVariables() const override;
    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior );

    virtual std::optional< QString > myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo & unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Calculating Volume Following a Change in Temperature" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure, Temperature and Volume Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "v1", tr( "Volume 1" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "t1", tr( "Temperature 1" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "v2", tr( "Volume 2" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "t2", tr( "Temperature 2" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::eAbsZeroOffsetConst ),   //
        };
}

TVariableInfo CCalculator::determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior )
{
    TVariableInfo retVal;

    if ( updateFromSide == EVariableLoc::eLHS )
    {
        if ( getVariable( "t1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "t2" );
        else if ( getVariable( "v1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "v2" );
    }
    else if ( updateFromSide == EVariableLoc::eRHS )
    {
        if ( getVariable( "t2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "t1" );
        else if ( getVariable( "v2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "v1" );
    }
    else
        retVal = CSCUBACalculator::determineVariableToUnset( updateFromSide, triggerWidget, preDefaultBehavior );
    return retVal;
}

std::optional< QString > CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return QString( R"__(<v2> = <v1> \times \frac{<t2>}{<t1>})__" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "v1" )
    {
        // V1 = V2 * ( t1/t2 );
        return QString( R"__(<v1> = <v2> \times \frac{<t1> + <%1>}{<t2> + <%1>})__" ).arg( NUtilities::NConstants::kAbsZeroOffsetConstFieldName );
    }
    else if ( unsetVar->name() == "v2" )
    {
        // V2 = V1 * ( t2/t1 );
        return QString( R"__(<v2> = <v1> \times \frac{<t2> + <%1>}{<t1> + <%1>})__" ).arg( NUtilities::NConstants::kAbsZeroOffsetConstFieldName );
    }
    else if ( unsetVar->name() == "t1" )
    {
        // T1 = t2*(V1/v2)
        return QString( R"__(<t1> = [(<t2> + <%1>) \times \frac{<v1>}{<v2>}] - <%1>)__" ).arg( NUtilities::NConstants::kAbsZeroOffsetConstFieldName );
    }
    else if ( unsetVar->name() == "t2" )
    {
        // T2 = t1*(V2/v1)
        return QString( R"__(<t2> = [(<t1> + <%1>) \times \frac{<v2>}{<v1>}] - <%1>)__" ).arg( NUtilities::NConstants::kAbsZeroOffsetConstFieldName );
    }

    return {};
}

void CCalculator::computeValueForVar( TVariableInfo & unsetVar )
{
    auto v1 = getVariable( "v1" );
    auto t1 = getVariable( "t1" );

    auto v2 = getVariable( "v2" );
    auto t2 = getVariable( "t2" );

    if ( unsetVar == v1 )
    {
        // V1 = V2 * ( t1/t2 );
        v1->setValue( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) * ( v2->value() / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) ) );
    }
    else if ( unsetVar == v2 )
    {
        // V2 = V1 * ( t2/t1 );
        v2->setValue( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) * ( v1->value() / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) ) );
    }
    else if ( unsetVar == t1 )
    {
        // T1 = t2*(V1/v2)
        t1->setValue( NUtilities::NConversions::fromAbsZeroBasedTemp( imperial(), NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) * ( v1->value() / v2->value() ) ) );
    }
    else if ( unsetVar == t2 )
    {
        // T2 = t1*(V2/v1)
        t2->setValue( NUtilities::NConversions::fromAbsZeroBasedTemp( imperial(), NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) * ( v2->value() / v1->value() ) ) );
    }
}