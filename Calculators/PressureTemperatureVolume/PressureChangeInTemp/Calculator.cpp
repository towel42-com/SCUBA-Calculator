#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/FormulaString.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "PressureChangeInTemp" ); }
    virtual ~CCalculator() override {}

    virtual bool isReversible() const override { return true; }

    virtual QString myCalculatorName() const override;
    virtual QString myReversedCalculatorName() const override;

    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;
    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior );

    virtual std::optional< TFormulaStringList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaStringList > myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const override;
    virtual std::optional< TFormulaStringList > getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Calculating Pressure Following a Change in Temperature" );
}

QString CCalculator::myReversedCalculatorName() const
{
    return tr( "Calculating Temperature Following a Change in Pressure" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure, Temperature and Volume Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables( bool *preReversed ) const
{
    *preReversed = true;

    auto retVal = TVariableInfoList( {
        std::make_shared< CVariableInfo >( "p1", tr( "Pressure 1" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
        std::make_shared< CVariableInfo >( "t1", tr( "Temperature 1" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
        std::make_shared< CVariableInfo >( "p2", tr( "Pressure 2" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
        std::make_shared< CVariableInfo >( "t2", tr( "Temperature 2" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
    } );

    if ( isReversed() )
    {
        for(auto && ii : retVal)
        {
            if ( ii->name() == "t2" )
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
    else if ( getVariable( "t1" )->isWidget( triggerWidget ) )
        retVal = getVariable( "t2" );
    else if ( getVariable( "p2" )->isWidget( triggerWidget ) )
        retVal = getVariable( "p1" );
    else if ( getVariable( "t2" )->isWidget( triggerWidget ) )
        retVal = getVariable( "t1" );
    else
        retVal = CSCUBACalculator::determineVariableToUnset( updateFromSide, triggerWidget, preDefaultBehavior );
    return retVal;
}

std::optional< TFormulaStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( "p2" ), QString( R"__([(<t2> + %1) \times \frac{<p1> + %2}{(<t1> + %1}] - %2)__" ).arg( NUtilities::fieldNameForType( EVariableType::eAbsZeroOffsetConst ) ).arg( NUtilities::fieldNameForType( EVariableType::ePressureAtSurfaceConst ) ) ) } );
}

std::optional< TFormulaStringList > CCalculator::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( "t2" ), QString( R"__([\frac{(<p2> + %2) \times (<t1> + %1)}{<p1> + %2}] - %1)__" ).arg( NUtilities::fieldNameForType( EVariableType::eAbsZeroOffsetConst ) ).arg( NUtilities::fieldNameForType( EVariableType::ePressureAtSurfaceConst ) ) ) } );
}

std::optional< TFormulaStringList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "p2" )
    {
        // p2 = p1 * ( t2/t1 );
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "t2" )
    {
        // T2 = t1*(t2/t1)
        return myReversedBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "p1" )
    {
        // p1 = p2 * ( t1/t2 );
        return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( unsetVar ), QString( R"__([(<t1> + %1) \times \frac{<p2> + %2}{(<t2> + %1}] - %2)__" ).arg( NUtilities::fieldNameForType( EVariableType::eAbsZeroOffsetConst ) ).arg( NUtilities::fieldNameForType( EVariableType::ePressureAtSurfaceConst ) ) ) } );
    }
    else if ( unsetVar == "t1" )
    {
        // T1 = t2*(t1/t2)
        return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( unsetVar ), QString( R"__([\frac{(<p1> + %2) \times (<t2> + %1)}{<p2> + %2}] - %1)__" ).arg( NUtilities::fieldNameForType( EVariableType::eAbsZeroOffsetConst ) ).arg( NUtilities::fieldNameForType( EVariableType::ePressureAtSurfaceConst ) ) ) } );
    }

    return {};
}

void CCalculator::computeVariableValues()
{
    auto p1 = getVariable( "p1" );
    auto t1 = getVariable( "t1" );

    auto p2 = getVariable( "p2" );
    auto t2 = getVariable( "t2" );

    if ( !p1->has_value() && t1->has_value() && p2->has_value() && t2->has_value() )
    {
        // p1 = p2 * ( t1/t2 );
        p1->setValue( ( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) * ( ( p2->value() + NUtilities::NConstants::pressureAtSurface( imperial() ) ) / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) ) ) - NUtilities::NConstants::pressureAtSurface( imperial() ) );
    }

    if ( p1->has_value() && t1->has_value() && !p2->has_value() && t2->has_value() )
    {
        // p2 = p1 * ( t2/t1 );
        p2->setValue( ( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) * ( ( p1->value() + NUtilities::NConstants::pressureAtSurface( imperial() ) ) / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) ) ) - NUtilities::NConstants::pressureAtSurface( imperial() ) );
    }

    if ( p1->has_value() && !t1->has_value() && p2->has_value() && t2->has_value() )
    {
        // T1 = t2*(t1/t2)
        t1->setValue( NUtilities::NConversions::fromAbsZeroBasedTemp( imperial(), ( ( p1->value() + NUtilities::NConstants::pressureAtSurface( imperial() ) ) * NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) ) / ( p2->value() + NUtilities::NConstants::pressureAtSurface( imperial() ) ) ) );
    }

    if ( p1->has_value() && t1->has_value() && p2->has_value() && !t2->has_value() )
    {
        // T2 = t1*(t2/t1)
        t2->setValue( NUtilities::NConversions::fromAbsZeroBasedTemp( imperial(), ( ( p2->value() + NUtilities::NConstants::pressureAtSurface( imperial() ) ) * NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) ) / ( p1->value() + NUtilities::NConstants::pressureAtSurface( imperial() ) ) ) );
    }
}