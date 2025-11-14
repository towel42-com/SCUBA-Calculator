#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/Formula.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "CombinedGasLaw" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;
    
    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior );

    virtual std::optional< TFormulaList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaList > getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
};

extern "C" CCalculatorBase *instantiateCalculator()
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

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "p1", tr( "Pressure 1" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "v1", tr( "Volume 1" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "t1", tr( "Temperature 1" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "p2", tr( "Pressure 2" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "v2", tr( "Volume 2" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "t2", tr( "Temperature 2" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
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
        retVal = CCalculatorBase::determineVariableToUnset( updateFromSide, triggerWidget, preDefaultBehavior );
    return retVal;
}

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { std::make_shared< CFormula >( TVariableInfo(), QString( R"__(\frac{\frac{<p1> \times <v1>}{<t1>} = <p2> \times <v2>}{<t2>})__" ) ) } );
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar == "p1" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(<p2> \times \frac{<v2>}{<v1>} \times \frac{<t1> + %1}{<t2> + %1})__" ).arg( NUtilities::fieldNameForType( EVariableType::eAbsZeroOffsetConst ) ) ) } );
    }
    else if ( unsetVar == "p2" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(<p1> \times \frac{<v1>}{<v2>} \times \frac{<t2> + %1}{<t1> + %1})__" ).arg( NUtilities::fieldNameForType( EVariableType::eAbsZeroOffsetConst ) ) ) } );
    }
    else if ( unsetVar == "v1" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(<v2> \times \frac{<t1> + %1}{<t2> + %1} \times \frac{<p2>}{<p1>})__" ).arg( NUtilities::fieldNameForType( EVariableType::eAbsZeroOffsetConst ) ) ) } );
    }
    else if ( unsetVar == "v2" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(<v1> \times \frac{<t2> + %1}{<t1> + %1} \times \frac{<p1>}{<p2>})__" ).arg( NUtilities::fieldNameForType( EVariableType::eAbsZeroOffsetConst ) ) ) } );
    }
    else if ( unsetVar == "t1" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(<t2> \times \frac{<p1>}{<p2>} \times \frac{<v1>}{<v2>})__" ) ) } );
    }
    else if ( unsetVar == "t2" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(<t1> \times \frac{<p2>}{<p1>} \times \frac{<v2>}{<v1>})__" ) ) } );
    }

    return {};
}

void CCalculator::computeVariableValues()
{
    auto p1 = getVariable( "p1" );
    auto v1 = getVariable( "v1" );
    auto t1 = getVariable( "t1" );

    auto p2 = getVariable( "p2" );
    auto v2 = getVariable( "v2" );
    auto t2 = getVariable( "t2" );

    if ( !p1->has_value() && p1->dependenciesSatisfied() )
    {
        p1->setValue( p2->value() * ( v2->value() / v1->value() ) * ( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) ) );
    }
    
    if ( !p2->has_value() && p2->dependenciesSatisfied() )
    {
        p2->setValue( p1->value() * ( v1->value() / v2->value() ) * ( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) ) );
    }
    
    if ( !v1->has_value() && v1->dependenciesSatisfied() )
    {
        v1->setValue( v2->value() * ( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) ) * ( p2->value() / p1->value() ) );
    }
    
    if ( !v2->has_value() && v2->dependenciesSatisfied() )
    {
        v2->setValue( v1->value() * ( NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t2->value() ) / NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t1->value() ) ) * ( p1->value() / p2->value() ) );
    }
    
    if ( !t1->has_value() && t1->dependenciesSatisfied() )
    {
        t1->setValue( t2->value() * ( p1->value() / p2->value() ) * ( v1->value() / v2->value() ) );
    }
    
    if ( !t2->has_value() && t2->dependenciesSatisfied() )
    {
        t2->setValue( t1->value() * ( p2->value() / p1->value() ) * ( v2->value() / v1->value() ) );
    }
}