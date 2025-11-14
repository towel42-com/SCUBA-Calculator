#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/Formula.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "IdealGasLaw" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

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
    return tr( "Ideal Gas Law" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure, Temperature and Volume Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "p", tr( "Pressure" ), EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "v", tr( "Volume" ), EUnit::eVolume, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "numMoles", tr( "Number of Moles" ), EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "t", tr( "Temperature" ), EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
        };
}

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { std::make_shared< CFormula >( TVariableInfo(), QString( R"__(<p> \times <v> = <numMoles> \times %1 \times <t>)__" ).arg( NUtilities::fieldNameForType( EConstantType::eIdealGasConst ) ) ) } );
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar == "p" )
    {
        // p = nrt/v
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(\frac{<numMoles> \times %1 \times (<t> + %2)}{<v>})__" ).arg( NUtilities::fieldNameForType( EConstantType::eIdealGasConst ) ).arg( NUtilities::fieldNameForType( EConstantType::eAbsZeroOffsetConst ) ) ) } );
    }
    else if ( unsetVar == "v" )
    {
        // v = nrt/p
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(\frac{<numMoles> \times %1 \times (<t> + %2)}{<p>})__" ).arg( NUtilities::fieldNameForType( EConstantType::eIdealGasConst ) ).arg( NUtilities::fieldNameForType( EConstantType::eAbsZeroOffsetConst ) ) ) } );
    }
    else if ( unsetVar == "numMoles" )
    {
        // n = pv/rt
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(\frac{<p> \times <v>}{%1 \times (<t> + %2)})__" ).arg( NUtilities::fieldNameForType( EConstantType::eIdealGasConst ) ).arg( NUtilities::fieldNameForType( EConstantType::eAbsZeroOffsetConst ) ) ) } );
    }
    else if ( unsetVar == "t" )
    {
        // t = pv/nr
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__((\frac{<p> \times <v>}{%1 \times <numMoles>}) - %2)__" ).arg( NUtilities::fieldNameForType( EConstantType::eIdealGasConst ) ).arg( NUtilities::fieldNameForType( EConstantType::eAbsZeroOffsetConst ) ) ) } );
    }

    return {};
}

void CCalculator::computeVariableValues()
{
    auto p = getVariable( "p" );
    auto v = getVariable( "v" );
    auto numMoles = getVariable( "numMoles" );
    auto t = getVariable( "t" );

    if ( !p->has_value() && p->dependenciesSatisfied() )
    {
        // p = nrt/v
        p->setValue( numMoles->value() * NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t->value() ) / v->value() );
    }
    
    if ( !v->has_value() && v->dependenciesSatisfied() )
    {
        // v = nrt/p
        v->setValue( numMoles->value() * NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t->value() ) / p->value() );
    }
    
    if ( !numMoles->has_value() && numMoles->dependenciesSatisfied() )
    {
        // n = pv/rt
        numMoles->setValue( ( p->value() * v->value() ) / ( NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::NConversions::toAbsZeroBasedTemp( imperial(), t->value() ) ) );
    }
    
    if ( !t->has_value() && t->dependenciesSatisfied() )
    {
        // t = pv/nr
        t->setValue( NUtilities::NConversions::fromAbsZeroBasedTemp( imperial(), ( p->value() * v->value() ) / ( NUtilities::NConstants::idealGasConstant( imperial() ) * numMoles->value() ) ) );
    }
}