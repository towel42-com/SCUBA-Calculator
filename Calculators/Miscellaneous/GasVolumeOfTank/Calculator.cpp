#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/Formula.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "TankGasVolume" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual TOptionalFormulaList myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual TOptionalFormulaList getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
};

extern "C" CCalculatorBase *instantiateCalculator()
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
            CVariableInfo::create( "tankVolume", tr( "Tank Volume" ), EUnit::eVolume, EVariableLoc::eRHS ),   //
            CVariableInfo::create( "ratedTankPressure", tr( "Tank Pressure Rating" ), EUnit::ePressure, EVariableLoc::eRHS ),   //
            CVariableInfo::create( "currTankPressure", tr( "Current Tank Pressure" ), EUnit::ePressure, EVariableLoc::eRHS ),   //
            CVariableInfo::create( "gasVolume", tr( "Gas Volume" ), EUnit::eVolume, EVariableLoc::eLHS ),   //
        } );
    return retVal;
}

TOptionalFormulaList CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { std::make_shared< CFormula >( getVariable( "gasVolume" ), R"__(<tankVolume> \times \frac{<currTankPressure>}{<ratedTankPressure>})__" ) } );
}

TOptionalFormulaList CCalculator::getFormulasForVar( const QString &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar == "gasVolume" )
        return myBaseFormulas( false, false );
    else if ( unsetVar == "currTankPressure" )
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), R"__(<ratedTankPressure> \times \frac{<gasVolume>}{<tankVolume>})__" ) } );
    else if ( unsetVar == "ratedTankPressure" )
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), R"__(<currTankPressure> \times \frac{<tankVolume>}{<gasVolume>})__" ) } );
    else if ( unsetVar == "tankVolume" )
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), R"__(<gasVolume> \times \frac{<ratedTankPressure>}{<currTankPressure>})__" ) } );
    return {};
}

void CCalculator::computeVariableValues()
{
    auto gasVolume = getVariable( "gasVolume" );
    auto currTankPressure = getVariable( "currTankPressure" );
    auto ratedTankPressure = getVariable( "ratedTankPressure" );
    auto tankVolume = getVariable( "tankVolume" );

    if ( !gasVolume->has_value() && gasVolume->dependenciesSatisfied() )
    {
        gasVolume->setValue( tankVolume->value() * ( currTankPressure->value() / ratedTankPressure->value() ) );
    }
    
    if ( !currTankPressure->has_value() && currTankPressure->dependenciesSatisfied() )
    {
        currTankPressure->setValue( ratedTankPressure->value() * ( gasVolume->value() / tankVolume->value() ) );
    }
    
    if ( !ratedTankPressure->has_value() && ratedTankPressure->dependenciesSatisfied() )
    {
        ratedTankPressure->setValue( currTankPressure->value() * ( tankVolume->value() / gasVolume->value() ) );
    }
    
    if ( !tankVolume->has_value() && tankVolume->dependenciesSatisfied() )
    {
        tankVolume->setValue( gasVolume->value() * ( ratedTankPressure->value() / currTankPressure->value() ) );
    }
}