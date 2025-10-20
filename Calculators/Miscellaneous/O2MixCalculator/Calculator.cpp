#include "Calculator.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() {}
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList calculatorPath() const override;

    virtual TVariableInfoList getMyVariables() const override;

    virtual QString myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
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

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Miscellaneous" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "mix1", tr( "Starting Mix" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "p1", tr( "Starting Pressure" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "mix2", tr( "Final Mix" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "p2", tr( "Final Pressure" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "o2", tr( "100% O2 Fill" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
        };
}

/**
function check()
{
    // Define variables
    var error = [];
    var O2_press = O2_minutes = 0;
    var printed_answer = "";

    var strt_mix_field = document.getElementById( strt_mix_id );
    var strt_press_field = document.getElementById( strt_press_id );

    var end_mix_field = document.getElementById( end_mix_id );
    var end_press_field = document.getElementById( end_press_id );
    
    // Get user input
    var strt_mix = parseFloat( strt_mix_field.value );
    var strt_press = parseInt( strt_press_field.value );
    var end_mix = parseFloat( end_mix_field.value );
    var end_press = parseInt( end_press_field.value );

    // Make sure we are dealing with numbers; if not, force an error condition
    if ( isNaN( strt_mix ) )
        strt_mix = -99;
    if ( isNaN( strt_press ) )
        strt_press = -99;
    if ( isNaN( end_mix ) )
        end_mix = -99;
    if ( isNaN( end_press ) )
        end_press = -99;

    // Error checks we can do before calculating

    if ( strt_mix < 20.9 || strt_mix > 40 )
    {
        error.push( "Your starting gas blend must be a number between 20.9 and 40%" );
        strt_mix_field.focus();
    }

    if ( strt_press < 0 || strt_press > 4000 )
    {
        error.push( "Your starting pressure must be a number between 0 and 4000 psi" );
        strt_press_field.focus();
    }

    if ( end_mix < 20.9 || end_mix > 40 )
    {
        error.push( "Your desired gas blend must be a number between 20.9 and 40%" );
        end_mix_label.className = "error";
        end_mix_field.focus();
    }

    if ( end_press < 0 || end_press > 4000 )
    {
        error.push( "Your desired pressure must be a number between 0 and 4000 psi" );
        end_press_label.className = "error";
        end_press_field.focus();
    }

    if ( error.length )
    {
        output_errors( error );
        return false;
    }

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

QString CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return NUtilities::NConversions::pressureChangeForDegreeChangeFormula( "t1", "p1" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "p1" )
    {
        return NUtilities::NConversions::pressureChangeForDegreeChangeFormula( "t1", "p1" );
    }
    else if ( unsetVar->name() == "t1" )
    {
        return NUtilities::NConversions::degreeChangeForPressureChangeFormula( "t1", "p1" );
    }

    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto p1 = getVariable( "p1" );
    auto t1 = getVariable( "t1" );

    if ( unsetVar == p1 )
    {
        p1->setValue( NUtilities::NConversions::pressureChangeForDegreeChange( imperial(), t1->value() ) );
    }
    else if ( unsetVar == t1 )
    {
        t1->setValue( NUtilities::NConversions::degreeChangeForPressureChange( imperial(), p1->value() ) );
    }
}