#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "PressureChangeInTemp" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

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
    return tr( "Calculating Pressure Following a Change in Temperature" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure, Temperature and Volume Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "p1", tr( "Pressure 1" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "t1", tr( "Temperature 1" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "p2", tr( "Pressure 2" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "t2", tr( "Temperature 2" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::eAbsZeroOffsetConst ),   //
            std::make_shared< CVariableInfo >( EVariableType::ePressureAtSurfaceConst ),   //
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
    }
    else if ( updateFromSide == EVariableLoc::eRHS )
    {
        if ( getVariable( "p2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "p1" );
        else if ( getVariable( "t2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "t1" );
    }
    else
        retVal = CSCUBACalculator::determineVariableToUnset( updateFromSide, triggerWidget, preDefaultBehavior );
    return retVal;
}

std::optional< QString > CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return QString( R"__(<p2> = [<t2> \times \frac{(<p1> + <%2>)}{(<t1>}] - <%2>)__" ).arg( NUtilities::NConstants::kPressureAtSurfaceConstFieldName );
    ;
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "p1" )
    {
        // p1 = p2 * ( t1/t2 );
        return QString( R"__(<p1> = [(<t1> + <%1>) \times \frac{(<p2> + <%2>)}{(<t2> + <%1>}] - <%2>)__" ).arg( NUtilities::NConstants::kAbsZeroOffsetConstFieldName ).arg( NUtilities::NConstants::kPressureAtSurfaceConstFieldName );
        ;
    }
    else if ( unsetVar->name() == "p2" )
    {
        // p2 = p1 * ( t2/t1 );
        return QString( R"__(<p2> = [(<t2> + <%1>) \times \frac{(<p1> + <%2>)}{(<t1> + <%1>}] - <%2>)__" ).arg( NUtilities::NConstants::kAbsZeroOffsetConstFieldName ).arg( NUtilities::NConstants::kPressureAtSurfaceConstFieldName );
        ;
    }
    else if ( unsetVar->name() == "t1" )
    {
        // T1 = t2*(t1/t2)
        return QString( R"__(<t1> = [\frac{(<p1> + <%2>) \times (<t2> + <%1>)}{<p2> + <%2>}] - <%1>)__" ).arg( NUtilities::NConstants::kAbsZeroOffsetConstFieldName ).arg( NUtilities::NConstants::kPressureAtSurfaceConstFieldName );
        ;
    }
    else if ( unsetVar->name() == "t2" )
    {
        // T2 = t1*(t2/t1)
        return QString( R"__(<t2> = [\frac{(<p2> + <%2>) \times (<t1> + <%1>)}{<p1> + <%2>}] - <%1>)__" ).arg( NUtilities::NConstants::kAbsZeroOffsetConstFieldName ).arg( NUtilities::NConstants::kPressureAtSurfaceConstFieldName );
        ;
    }

    return {};
}

void CCalculator::computeValueForVar( TVariableInfo & unsetVar )
{
    auto p1 = getVariable( "p1" );
    auto t1 = getVariable( "t1" );

    auto p2 = getVariable( "p2" );
    auto t2 = getVariable( "t2" );

    if ( unsetVar == p1 )
    {
        // p1 = p2 * ( t1/t2 );
        p1->setValue( ( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) * ( ( p2->value() + NUtilities::NConstants::pressureAtSurface( imperial() ) ) / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) ) ) - NUtilities::NConstants::pressureAtSurface( imperial() ) );
    }
    else if ( unsetVar == p2 )
    {
        // p2 = p1 * ( t2/t1 );
        p2->setValue( ( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) * ( ( p1->value() + NUtilities::NConstants::pressureAtSurface( imperial() ) ) / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) ) ) - NUtilities::NConstants::pressureAtSurface( imperial() ) );
    }
    else if ( unsetVar == t1 )
    {
        // T1 = t2*(t1/t2)
        t1->setValue( NUtilities::NConversions::fromAbsZeroBasedTemp( imperial(), ( ( p1->value() + NUtilities::NConstants::pressureAtSurface( imperial() ) ) * NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) ) / ( p2->value() + NUtilities::NConstants::pressureAtSurface( imperial() ) ) ) );
    }
    else if ( unsetVar == t2 )
    {
        // T2 = t1*(t2/t1)
        t2->setValue( NUtilities::NConversions::fromAbsZeroBasedTemp( imperial(), ( ( p2->value() + NUtilities::NConstants::pressureAtSurface( imperial() ) ) * NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) ) / ( p1->value() + NUtilities::NConstants::pressureAtSurface( imperial() ) ) ) );
    }
}