#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "TankGasVolume" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

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
    return tr( "Volume of Gas in Tank" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Miscellaneous" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal =   //
        TVariableInfoList( {
            std::make_shared< CVariableInfo >( "tankVolume", tr( "Tank Volume" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "ratedTankPressure", tr( "Tank Pressure Rating" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "currTankPressure", tr( "Current Tank Pressure" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "gasVolume", tr( "Gas Volume" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eLHS ),   //
        } );
    return retVal;
}

std::optional< QString > CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return R"__(<gasVolume> = <tankVolume> \times \frac{<currTankPressure>}{<ratedTankPressure>})__";
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "gasVolume" )
        return myBaseFormula( false, false );
    else if ( unsetVar->name() == "currTankPressure" )
        return R"__(<currTankPressure> = <ratedTankPressure> \times \frac{<gasVolume>}{<tankVolume>})__";
    else if ( unsetVar->name() == "ratedTankPressure" )
        return R"__(<ratedTankPressure> = <currTankPressure> \times \frac{<tankVolume>}{<gasVolume>})__";
    else if ( unsetVar->name() == "tankVolume" )
        return R"__(<tankVolume> = <gasVolume> \times \frac{<ratedTankPressure>}{<currTankPressure>})__";
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto gasVolume = getVariable( "gasVolume" );
    auto currTankPressure = getVariable( "currTankPressure" );
    auto ratedTankPressure = getVariable( "ratedTankPressure" );
    auto tankVolume = getVariable( "tankVolume" );

    if ( unsetVar == gasVolume )
    {
        gasVolume->setValue( tankVolume->value() * ( currTankPressure->value() / ratedTankPressure->value() ) );
    }
    else if ( unsetVar == currTankPressure )
    {
        currTankPressure->setValue( ratedTankPressure->value() * ( gasVolume->value() / tankVolume->value() ) );
    }
    else if ( unsetVar == ratedTankPressure )
    {
        ratedTankPressure->setValue( currTankPressure->value() * ( tankVolume->value() / gasVolume->value() ) );
    }
    else if ( unsetVar == tankVolume )
    {
        tankVolume->setValue( gasVolume->value() * ( ratedTankPressure->value() / currTankPressure->value() ) );
    }
}