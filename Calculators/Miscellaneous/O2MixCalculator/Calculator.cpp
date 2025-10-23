#include "Calculator.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <cmath>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "O2MixCalculator" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual TVariableInfoList getMyVariables() const override;

    virtual std::optional< QString > myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
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

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal =   //
        TVariableInfoList( {
            std::make_shared< CVariableInfo >( "mix1", tr( "Starting Mix" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS, SRange( { NUtilities::NConstants::percentO2AtSurface(), 0.40, {}, 0.01 } ) ),   //
            std::make_shared< CVariableInfo >( "p1", tr( "Starting Pressure" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS, SRange( { 0.0, 4000, {}, 10 } ) ),   //
            std::make_shared< CVariableInfo >( "mix2", tr( "Final Mix" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS, SRange( { NUtilities::NConstants::percentO2AtSurface(), 0.40, {}, 0.01 } ) ),   //
            std::make_shared< CVariableInfo >( "p2", tr( "Final Pressure" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS, SRange( { 0.0, 4000, {}, 10 } ) ),   //
            std::make_shared< CVariableInfo >( "o2_p", tr( "Fill with 100% O2 to Pressure" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "o2_t", tr( "Approximate Time to fill with 100% O2" ), EVariableType::eIntermediate, EUnit::eTime, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "air_t", tr( "Fill Time with Air (%1%)" ).arg( NUtilities::doubleToString( NUtilities::NConstants::percentO2AtSurface(), 3 ) ), EVariableType::eIntermediate, EUnit::eTime, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::eFO2AtSurfaceConst ),   //
            std::make_shared< CVariableInfo >( EVariableType::eFN2AtSurfaceConst ),   //
            std::make_shared< CVariableInfo >( EVariableType::eFillRateO2Const ),   //
            std::make_shared< CVariableInfo >( EVariableType::eFillRateAirConst ),   //
        } );
    auto pos = std::next( retVal.begin() );
    ( *pos )->setRange( false, {}, SRange( { 0.0, NUtilities::NConversions::psiToBar( 4000 ), {}, 1 } ) );
    pos++;
    pos++;
    ( *pos )->setRange( false, {}, SRange( { NUtilities::NConversions::psiToBar( 1000 ), NUtilities::NConversions::psiToBar( 4000 ), {}, 1 } ) );
    return retVal;
}

std::optional< QString > CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    QStringList formulas;

    formulas << QString( R"__(<o2_p> = \frac{( <p2> \times (<mix2> - <%1>) ) - ( <p1> \times ( <mix1> - <%1> ) )}{<%2>} - <p1>)__" ).arg( NUtilities::fieldNameForType( EVariableType::eFO2AtSurfaceConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eFN2AtSurfaceConst ) );
    formulas << QString( R"__(<o2_t> = \frac{<o2_p> - <p1>}{<%1>})__" ).arg( NUtilities::fieldNameForType( EVariableType::eFillRateO2Const ) );
    formulas << QString( R"__(<air_t> = \frac{<p2>-<o2_p>}{<%1>})__" ).arg( NUtilities::fieldNameForType( EVariableType::eFillRateAirConst ) );

    return NUtilities::joinFormulas( formulas );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    QStringList formulas;
    if ( unsetVar->name() == "o2_p" )
    {
        return getBaseFormula();
    }
    else if ( unsetVar->name() == "mix1" )
    {
        formulas << QString( R"__(<mix1> = \frac{( <p2> \times (<mix2> - <%1>) ) - ((<o2_p> + <p1>) \times <%2>)}{<p1>} + <%1>)__" ).arg( NUtilities::fieldNameForType( EVariableType::eFO2AtSurfaceConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eFN2AtSurfaceConst ) );
    }
    else if ( unsetVar->name() == "p1" )
    {
        formulas << QString( R"__(<p1> = \frac{<p2>(<mix2> - <%1>) - <%2> \times <o2_p>}{(<mix1> - <%1>) + <%2>})__" ).arg( NUtilities::fieldNameForType( EVariableType::eFO2AtSurfaceConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eFN2AtSurfaceConst ) );
    }
    else if ( unsetVar->name() == "mix2" )
    {
        formulas << QString( R"__(<mix2> = \frac{((<o2_p> + <p1>) \times <%2>) + ( <p1> \times ( <mix1> - <%1> ) )}{<p2>} + <%1>)__" ).arg( NUtilities::fieldNameForType( EVariableType::eFO2AtSurfaceConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eFN2AtSurfaceConst ) );
    }
    else if ( unsetVar->name() == "p2" )
    {
        formulas << QString( R"__(<p2> = \frac{((<o2_p> + <p1>) \times <%2>) + ( <p1> \times ( <mix1> - <%1> ) )}{<mix2> - <%1>})__" ).arg( NUtilities::fieldNameForType( EVariableType::eFO2AtSurfaceConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eFN2AtSurfaceConst ) );
    }
    if ( !formulas.isEmpty() )
    {
        formulas << QString( R"__(<o2_t> = \frac{<o2_p> - <p1>}{<%1>})__" ).arg( NUtilities::fieldNameForType( EVariableType::eFillRateO2Const ) );
        formulas << QString( R"__(<air_t> = \frac{<p2>-<o2_p>}{<%1>})__" ).arg( NUtilities::fieldNameForType( EVariableType::eFillRateAirConst ) );
    }
    return NUtilities::joinFormulas( formulas );
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

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto mix1 = getVariable( "mix1" );
    auto p1 = getVariable( "p1" );
    auto mix2 = getVariable( "mix2" );
    auto p2 = getVariable( "p2" );

    auto o2_p = getVariable( "o2_p" );
    auto o2_t = getVariable( "o2_t" );
    auto air_t = getVariable( "air_t" );

    if ( unsetVar == o2_p )
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

    else if ( unsetVar == mix1 )
    {
        mix1->setValue( ( p2->value() * ( mix2->value() - NUtilities::NConstants::percentO2AtSurface() ) ) - ( ( ( o2_p->value() + p1->value() ) * NUtilities::NConstants::percentN2AtSurface() ) / p1->value() ) + NUtilities::NConstants::percentO2AtSurface() );
    }
    else if ( unsetVar->name() == "p1" )
    {
        p1->setValue( ( p2->value() * ( mix2->value() - NUtilities::NConstants::percentO2AtSurface() ) - NUtilities::NConstants::percentN2AtSurface() * o2_p->value() ) / ( ( mix1->value() - NUtilities::NConstants::percentO2AtSurface() ) + NUtilities::NConstants::percentN2AtSurface() ) );
        return;
    }
    else if ( unsetVar->name() == "mix2" )
    {
        mix2->setValue( ( ( ( ( o2_p->value() + p1->value() ) * NUtilities::NConstants::percentN2AtSurface() ) + ( p1->value() * ( mix1->value() - NUtilities::NConstants::percentO2AtSurface() ) ) ) / p2->value() ) + NUtilities::NConstants::percentO2AtSurface() );
    }
    else if ( unsetVar->name() == "p2" )
    {
        p2->setValue( ( ( ( o2_p->value() + p1->value() ) * NUtilities::NConstants::percentN2AtSurface() ) + ( p1->value() * ( mix1->value() - NUtilities::NConstants::percentO2AtSurface() ) ) ) / ( mix2->value() - NUtilities::NConstants::percentO2AtSurface() ) );
    }

    o2_t->setValue( std::ceil( ( o2_p->value() - p1->value() ) / NUtilities::NConstants::fillRateO2( imperial() ) ) );
    air_t->setValue( std::ceil( ( p2->value() - o2_p->value() ) / NUtilities::NConstants::fillRateAir( imperial() ) ) );
}