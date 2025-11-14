#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/Formula.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "VolumeChangeInTemp" ); }
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
    return tr( "Calculating Volume Following a Change in Temperature" );
}

QString CCalculator::myReversedCalculatorName() const
{
    return tr( "Calculating Temperature Following a Change in Volume" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure, Temperature and Volume Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables( bool *preReversed ) const
{
    *preReversed = true;

    auto retVal = TVariableInfoList( {
        std::make_shared< CVariableInfo >( "v1", tr( "Volume 1" ), EUnit::eVolume, EVariableLoc::eRHS ),   //
        std::make_shared< CVariableInfo >( "t1", tr( "Temperature 1" ), EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
        std::make_shared< CVariableInfo >( "v2", tr( "Volume 2" ), EUnit::eVolume, EVariableLoc::eLHS ),   //
        std::make_shared< CVariableInfo >( "t2", tr( "Temperature 2" ), EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
    } );

    if ( isReversed() )
    {
        for ( auto &&ii : retVal )
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

    if ( getVariable( "t1" )->isWidget( triggerWidget ) )
        retVal = getVariable( "t2" );
    else if ( getVariable( "v1" )->isWidget( triggerWidget ) )
        retVal = getVariable( "v2" );
    else if ( getVariable( "t2" )->isWidget( triggerWidget ) )
        retVal = getVariable( "t1" );
    else if ( getVariable( "v2" )->isWidget( triggerWidget ) )
        retVal = getVariable( "v1" );
    else
        retVal = CCalculatorBase::determineVariableToUnset( updateFromSide, triggerWidget, preDefaultBehavior );
    return retVal;
}

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { std::make_shared< CFormula >( getVariable( "v2" ), QString( R"__(<v1> \times \frac{<t2>}{<t1>})__" ) ) } );
}

std::optional< TFormulaList > CCalculator::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { std::make_shared< CFormula >( getVariable( "t2" ), QString( R"__([(<t1> + %1) \times \frac{<v2>}{<v1>}] - %1)__" ).arg( NUtilities::fieldNameForType( EConstantType::eAbsZeroOffsetConst ) ) ) } );
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "v2" )
    {
        // V2 = V1 * ( t2/t1 );
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "v1" )
    {
        // V1 = V2 * ( t1/t2 );
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(<v2> \times \frac{<t1> + %1}{<t2> + %1})__" ).arg( NUtilities::fieldNameForType( EConstantType::eAbsZeroOffsetConst ) ) ) } );
    }
    else if ( unsetVar == "t1" )
    {
        // T1 = t2*(V1/v2)
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__([(<t2> + %1) \times \frac{<v1>}{<v2>}] - %1)__" ).arg( NUtilities::fieldNameForType( EConstantType::eAbsZeroOffsetConst ) ) ) } );
    }
    else if ( unsetVar == "t2" )
    {
        // T2 = t1*(V2/v1)
        return myReversedBaseFormulas( imperial, seaWater );
    }

    return {};
}

void CCalculator::computeVariableValues()
{
    auto v1 = getVariable( "v1" );
    auto t1 = getVariable( "t1" );

    auto v2 = getVariable( "v2" );
    auto t2 = getVariable( "t2" );

    if ( !v1->has_value() && v1->dependenciesSatisfied() )
    {
        // V1 = V2 * ( t1/t2 );
        v1->setValue( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) * ( v2->value() / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) ) );
    }

    if ( !v2->has_value() && v2->dependenciesSatisfied() )
    {
        // V2 = V1 * ( t2/t1 );
        v2->setValue( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) * ( v1->value() / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) ) );
    }

    if ( !t1->has_value() && t1->dependenciesSatisfied() )
    {
        // T1 = t2*(V1/v2)
        t1->setValue( NUtilities::NConversions::fromAbsZeroBasedTemp( imperial(), NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) * ( v1->value() / v2->value() ) ) );
    }

    if ( !t2->has_value() && t2->dependenciesSatisfied() )
    {
        // T2 = t1*(V2/v1)
        t2->setValue( NUtilities::NConversions::fromAbsZeroBasedTemp( imperial(), NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) * ( v2->value() / v1->value() ) ) );
    }
}