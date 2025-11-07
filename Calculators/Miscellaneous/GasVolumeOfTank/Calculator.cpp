#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/FormulaString.h"

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

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual std::optional< TFormulaStringList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaStringList > getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
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

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
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

std::optional< TFormulaStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( "gasVolume" ), R"__(<tankVolume> \times \frac{<currTankPressure>}{<ratedTankPressure>})__" ) } );
}

std::optional< TFormulaStringList > CCalculator::getFormulasForVar( const QString &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar == "gasVolume" )
        return myBaseFormulas( false, false );
    else if ( unsetVar == "currTankPressure" )
        return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( unsetVar ), R"__(<ratedTankPressure> \times \frac{<gasVolume>}{<tankVolume>})__" ) } );
    else if ( unsetVar == "ratedTankPressure" )
        return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( unsetVar ), R"__(<currTankPressure> \times \frac{<tankVolume>}{<gasVolume>})__" ) } );
    else if ( unsetVar == "tankVolume" )
        return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( unsetVar ), R"__(<gasVolume> \times \frac{<ratedTankPressure>}{<currTankPressure>})__" ) } );
    return {};
}

void CCalculator::computeVariableValues()
{
    auto gasVolume = getVariable( "gasVolume" );
    auto currTankPressure = getVariable( "currTankPressure" );
    auto ratedTankPressure = getVariable( "ratedTankPressure" );
    auto tankVolume = getVariable( "tankVolume" );

    if ( !gasVolume->has_value() && tankVolume->has_value() && currTankPressure->has_value() && ratedTankPressure->has_value() )
    {
        gasVolume->setValue( tankVolume->value() * ( currTankPressure->value() / ratedTankPressure->value() ) );
    }
    
    if ( gasVolume->has_value() && tankVolume->has_value() && !currTankPressure->has_value() && ratedTankPressure->has_value() )
    {
        currTankPressure->setValue( ratedTankPressure->value() * ( gasVolume->value() / tankVolume->value() ) );
    }
    
    if ( gasVolume->has_value() && tankVolume->has_value() && currTankPressure->has_value() && !ratedTankPressure->has_value() )
    {
        ratedTankPressure->setValue( currTankPressure->value() * ( tankVolume->value() / gasVolume->value() ) );
    }
    
    if ( gasVolume->has_value() && !tankVolume->has_value() && currTankPressure->has_value() && ratedTankPressure->has_value() )
    {
        tankVolume->setValue( gasVolume->value() * ( ratedTankPressure->value() / currTankPressure->value() ) );
    }
}