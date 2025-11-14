#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/Formula.h"

#include <memory>
#include <cmath>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "O2MixCalculator" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;
    virtual void setupCustomDependencies() override;

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
    return tr( "Oxygen Mix Calculator" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Miscellaneous" ) };
}

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
{
    auto retVal =   //
        TVariableInfoList( {
            std::make_shared< CVariableInfo >( "mix1", tr( "Starting Mix" ), EUnit::ePercent, EVariableLoc::eRHS, SBaseInfo< SRange >( {}, {}, SRange( { NUtilities::NConstants::percentO2AtSurface(), 0.40, {}, 0.01 } ) ) ),   //
            std::make_shared< CVariableInfo >( "p1", tr( "Starting Pressure" ), EUnit::ePressure, EVariableLoc::eRHS, SBaseInfo< SRange >( true, {}, SRange( { 0.0, 4000, {}, 10 } ) ) ),   //
            std::make_shared< CVariableInfo >( "mix2", tr( "Final Mix" ), EUnit::ePercent, EVariableLoc::eRHS, SBaseInfo< SRange >( {}, {}, SRange( { NUtilities::NConstants::percentO2AtSurface(), 0.40, {}, 0.01 } ) ) ),   //
            std::make_shared< CVariableInfo >( "p2", tr( "Final Pressure" ), EUnit::ePressure, EVariableLoc::eRHS, SBaseInfo< SRange >( true, {}, SRange( { 0.0, 4000, {}, 10 } ) ) ),   //
            std::make_shared< CVariableInfo >( "o2_p", tr( "Fill with 100% O2 to Pressure" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "o2_t", tr( "Time to fill with 100% O2" ), EVariableType::eIntermediate, EUnit::eTime, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "air_t", tr( "Fill Time with Air (%1%)" ).arg( NUtilities::doubleToString( NUtilities::NConstants::percentO2AtSurface(), 3 ) ), EVariableType::eIntermediate, EUnit::eTime, EVariableLoc::eLHS ),   //
        } );
    auto pos = std::next( retVal.begin() );
    ( *pos )->addRange( false, {}, SRange( { 0.0, NUtilities::NConversions::psiToBar( 4000 ), {}, 1 } ) );
    pos++;
    pos++;
    ( *pos )->addRange( false, {}, SRange( { NUtilities::NConversions::psiToBar( 1000 ), NUtilities::NConversions::psiToBar( 4000 ), {}, 1 } ) );
    return retVal;
}

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    TFormulaList formulas;

    formulas.emplace_back( std::make_shared< CFormula >( getVariable( "o2_p" ), QString( R"__(\frac{(<p2> \times (<mix2> - %1) ) - ( <p1> \times ( <mix1> - %1 ) )}{%2} - <p1>)__" ).arg( NUtilities::fieldNameForType( EVariableType::eFO2AtSurfaceConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eFN2AtSurfaceConst ) ) ) );
    formulas.emplace_back( std::make_shared< CFormula >( getVariable( "o2_t" ), QString( R"__(\frac{<o2_p> - <p1>}{%1})__" ).arg( NUtilities::fieldNameForType( EVariableType::eFillRateO2Const ) ) ) );
    formulas.emplace_back( std::make_shared< CFormula >( getVariable( "air_t" ), QString( R"__(\frac{<p2>-<o2_p>}{%1})__" ).arg( NUtilities::fieldNameForType( EVariableType::eFillRateAirConst ) ) ) );

    return formulas;
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    TFormulaList formulas;
    if ( unsetVar == "o2_p" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "mix1" )
    {
        formulas.emplace_back( std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(\frac{( <p2> \times (<mix2> - %1) ) - ((<o2_p> + <p1>) \times %2)}{<p1>} + %1)__" ).arg( NUtilities::fieldNameForType( EVariableType::eFO2AtSurfaceConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eFN2AtSurfaceConst ) ) ) );
    }
    else if ( unsetVar == "p1" )
    {
        formulas.emplace_back( std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(\frac{<p2>(<mix2> - %1) - %2 \times <o2_p>}{(<mix1> - %1) + %2})__" ).arg( NUtilities::fieldNameForType( EVariableType::eFO2AtSurfaceConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eFN2AtSurfaceConst ) ) ) );
    }
    else if ( unsetVar == "mix2" )
    {
        formulas.emplace_back( std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(\frac{((<o2_p> + <p1>) \times %2) + ( <p1> \times ( <mix1> - %1 ) )}{<p2>} + %1)__" ).arg( NUtilities::fieldNameForType( EVariableType::eFO2AtSurfaceConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eFN2AtSurfaceConst ) ) ) );
    }
    else if ( unsetVar == "p2" )
    {
        formulas.emplace_back( std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(\frac{((<o2_p> + <p1>) \times %2) + ( <p1> \times ( <mix1> - %1 ) )}{<mix2> - %1})__" ).arg( NUtilities::fieldNameForType( EVariableType::eFO2AtSurfaceConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eFN2AtSurfaceConst ) ) ) );
    }
    if ( !formulas.empty() )
    {
        formulas.emplace_back( std::make_shared< CFormula >( getVariable( "o2_t" ), QString( R"__(\frac{<o2_p> - <p1>}{%1})__" ).arg( NUtilities::fieldNameForType( EVariableType::eFillRateO2Const ) ) ) );
        formulas.emplace_back( std::make_shared< CFormula >( getVariable( "air_t" ), QString( R"__(\frac{<p2>-<o2_p>}{%1})__" ).arg( NUtilities::fieldNameForType( EVariableType::eFillRateAirConst ) ) ) );
    }
    return formulas;
}

/**
// https://scuba.garykessler.net/EANcalculator/EAN_psi.html
// mixing O2 + Air for proper Nitrox

function check()
{
    // Get user input
    var strt_mix = parseFloat( strt_mix_field.value );
    var strt_press = parseInt( strt_press_field.value );
    var end_mix = parseFloat( end_mix_field.value );
    var end_press = parseInt( end_press_field.value );

    // Determine the required 100% O2 press and amount of time

    O2_press = Math.round( ( ( ( end_press * ( end_mix - 20.9 ) ) - ( strt_press * ( strt_mix - 20.9 ) ) ) / 79.1 ) + strt_press + 0.5 );
    O2_minutes = Math.round( ( ( O2_press - strt_press ) / 60 ) + 0.5 );

    printed_answer = printed_answer + "Your starting pressure is " + strt_press + " psi</br />";
    printed_answer = printed_answer + "Fill with 100% O2 to " + O2_press + " psi (~" + O2_minutes + " minutes at 60 psi/min)<br />";
    printed_answer = printed_answer + "Then top-off with air to " + end_press + " psi</br />";

    output_div.innerHTML = printed_answer;
    output_div.className = "result";
}
*/

void CCalculator::setupCustomDependencies()
{
    setDependencies( "mix1", { "p1", "p2", "mix2", "o2_p" } );
    setDependencies( "p1", { "p2", "mix1", "mix2", "o2_p" } );
    setDependencies( "mix2", { "p1", "p2", "mix1", "o2_p" } );
    setDependencies( "p2", { "p1", "mix1", "mix2", "o2_p" } );
    setDependencies( "o2_p", { "p1", "p2", "mix1", "mix2" } );
    setDependencies( "o2_t", { "p1", "o2_p" } );
    setDependencies( "air_t", { "p2", "o2_p" } );
}

void CCalculator::computeVariableValues()
{
    auto mix1 = getVariable( "mix1" );
    auto p1 = getVariable( "p1" );
    auto mix2 = getVariable( "mix2" );
    auto p2 = getVariable( "p2" );

    auto o2_p = getVariable( "o2_p" );
    auto o2_t = getVariable( "o2_t" );
    auto air_t = getVariable( "air_t" );

    if ( !o2_p->has_value() && o2_p->dependenciesSatisfied() )
    {
        if ( ( mix1->value() == mix2->value() ) && ( mix1->value() == NUtilities::NConstants::percentO2AtSurface() ) )
        {
            o2_p->setValue( 0.0 );
            o2_t->setValue( 0.0 );
            air_t->setValue( ( p2->value() - p1->value() ) / NUtilities::NConstants::fillRateAir( imperial() ) );
            return;
        }
        o2_p->setValue( std::ceil( ( ( ( p2->value() * ( mix2->value() - NUtilities::NConstants::percentO2AtSurface() ) ) - ( p1->value() * ( mix1->value() - NUtilities::NConstants::percentO2AtSurface() ) ) ) / NUtilities::NConstants::percentN2AtSurface() ) + p1->value() ) );
    }

    if ( !mix1->has_value() && mix1->dependenciesSatisfied() )
    {
        mix1->setValue( ( p2->value() * ( mix2->value() - NUtilities::NConstants::percentO2AtSurface() ) ) - ( ( ( o2_p->value() + p1->value() ) * NUtilities::NConstants::percentN2AtSurface() ) / p1->value() ) + NUtilities::NConstants::percentO2AtSurface() );
    }

    if ( !p1->has_value() && p1->dependenciesSatisfied() )
    {
        p1->setValue( ( p2->value() * ( mix2->value() - NUtilities::NConstants::percentO2AtSurface() ) - NUtilities::NConstants::percentN2AtSurface() * o2_p->value() ) / ( ( mix1->value() - NUtilities::NConstants::percentO2AtSurface() ) + NUtilities::NConstants::percentN2AtSurface() ) );
        return;
    }

    if ( !mix2->has_value() && mix2->dependenciesSatisfied() )
    {
        mix2->setValue( ( ( ( ( o2_p->value() + p1->value() ) * NUtilities::NConstants::percentN2AtSurface() ) + ( p1->value() * ( mix1->value() - NUtilities::NConstants::percentO2AtSurface() ) ) ) / p2->value() ) + NUtilities::NConstants::percentO2AtSurface() );
    }

    if ( !p2->has_value() && p2->dependenciesSatisfied() )
    {
        p2->setValue( ( ( ( o2_p->value() + p1->value() ) * NUtilities::NConstants::percentN2AtSurface() ) + ( p1->value() * ( mix1->value() - NUtilities::NConstants::percentO2AtSurface() ) ) ) / ( mix2->value() - NUtilities::NConstants::percentO2AtSurface() ) );
    }

    if ( !o2_t->has_value() && o2_t->dependenciesSatisfied() )
    {
        o2_t->setValue( std::ceil( ( o2_p->value() - p1->value() ) / NUtilities::NConstants::fillRateO2( imperial() ) ) );
    }

    if ( !air_t->has_value() && air_t->dependenciesSatisfied() )
    {
        air_t->setValue( std::ceil( ( p2->value() - o2_p->value() ) / NUtilities::NConstants::fillRateAir( imperial() ) ) );
    }
}