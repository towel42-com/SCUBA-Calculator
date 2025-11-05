#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "IdealGasLaw" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables() const override;

    virtual std::optional< TFormulaStringList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaStringList > getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Ideal Gas Law" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure, Temperature and Volume Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "p", tr( "Pressure" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "v", tr( "Volume" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "numMoles", tr( "Number of Moles" ), EVariableType::eVariable, EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "t", tr( "Temperature" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
        };
}

std::optional< TFormulaStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { TFormulaString( R"__(<p> \times <v>)__", QString( R"__(<numMoles> \times <%1> \times <t>)__" ).arg( NUtilities::fieldNameForType( EVariableType::eIdealGasConst ) ) ) } );
}

std::optional< TFormulaStringList > CCalculator::getFormulasForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "p" )
    {
        // p = nrt/v
        return TFormulaStringList( { TFormulaString( "<p>)__", QString( R"__(\frac{<numMoles> \times <%1> \times (<t> + <%2>)}{<v>})__" ).arg( NUtilities::fieldNameForType( EVariableType::eIdealGasConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eAbsZeroOffsetConst ) ) ) } );
    }
    else if ( unsetVar->name() == "v" )
    {
        // v = nrt/p
        return TFormulaStringList( { TFormulaString( "<v>", QString( R"__(\frac{<numMoles> \times <%1> \times (<t> + <%2>)}{<p>})__" ).arg( NUtilities::fieldNameForType( EVariableType::eIdealGasConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eAbsZeroOffsetConst ) ) ) } );
    }
    else if ( unsetVar->name() == "numMoles" )
    {
        // n = pv/rt
        return TFormulaStringList( { TFormulaString( "<numMoles>", QString( R"__(\frac{<p> \times <v>}{<%1> \times (<t> + <%2>)})__" ).arg( NUtilities::fieldNameForType( EVariableType::eIdealGasConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eAbsZeroOffsetConst ) ) ) } );
    }
    else if ( unsetVar->name() == "t" )
    {
        // t = pv/nr
        return TFormulaStringList( { TFormulaString( "<t>", QString( R"__((\frac{<p> \times <v>}{<%1> \times <numMoles>}) - <%2>)__" ).arg( NUtilities::fieldNameForType( EVariableType::eIdealGasConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eAbsZeroOffsetConst ) ) ) } );
    }

    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto p = getVariable( "p" );
    auto v = getVariable( "v" );
    auto numMoles = getVariable( "numMoles" );
    auto t = getVariable( "t" );

    if ( unsetVar == p )
    {
        // p = nrt/v
        p->setValue( numMoles->value() * NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t->value() ) / v->value() );
    }
    else if ( unsetVar == v )
    {
        // v = nrt/p
        v->setValue( numMoles->value() * NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t->value() ) / p->value() );
    }
    else if ( unsetVar == numMoles )
    {
        // n = pv/rt
        numMoles->setValue( ( p->value() * v->value() ) / ( NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t->value() ) ) );
    }
    else if ( unsetVar == t )
    {
        // t = pv/nr
        t->setValue( NUtilities::NConversions::fromAbsZeroBasedTemp( imperial(), ( p->value() * v->value() ) / ( NUtilities::NConstants::idealGasConstant( imperial() ) * numMoles->value() ) ) );
    }
}