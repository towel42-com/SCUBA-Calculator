#include "Calculator.h"
#include "VariableInfo.h"
#include "Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() {}
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList calculatorPath() const override;

    virtual TVariableInfoList getMyVariables() const override;

    virtual QString myBaseFormula() const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo & unsetVar ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo & unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Ideal Gas Law" );
}

QStringList CCalculator::calculatorPath() const
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
            std::make_shared< CVariableInfo >( "idealGasConstant", tr( "Ideal Gas Constant" ), EVariableType::eIdealGasConstant, EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "absOffset", tr( "Absolute Temperature Offset" ), EVariableType::eAbsZeroOffsetConstant, EUnit::eNone, EVariableLoc::eRHS ),   //
        };
}

QString CCalculator::myBaseFormula() const
{
    return R"__(<p> \times <v> = <numMoles> \times <idealGasConstant> \times <t>)__";
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo & unsetVar ) const
{
    if ( unsetVar->name() == "p" )
    {
        // p = nrt/v
        return R"__(<p> = \frac{<numMoles> \times <idealGasConstant> \times (<t> + <absOffset>)}{<v>})__";
    }
    else if ( unsetVar->name() == "v" )
    {
        // v = nrt/p
        return R"__(<v> = \frac{<numMoles> \times <idealGasConstant> \times (<t> + <absOffset>)}{<p>})__";
    }
    else if ( unsetVar->name() == "numMoles" )
    {
        // n = pv/rt
        return R"__(<numMoles> = \frac{<p> \times <v>}{<idealGasConstant> \times (<t> + <absOffset>)})__";
    }
    else if ( unsetVar->name() == "t" )
    {
        // t = pv/nr
        return R"__(<t> = (\frac{<p> \times <v>}{<idealGasConstant> \times <numMoles>}) - <absOffset>)__";
    }

    return {};
}

void CCalculator::computeValueForVar( TVariableInfo & unsetVar )
{
    auto p = getVariable( "p" );
    auto v = getVariable( "v" );
    auto numMoles = getVariable( "numMoles" );
    auto t = getVariable( "t" );

    if ( unsetVar == p )
    {
        // p = nrt/v
        p->setValue( numMoles->value() * NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::toAbsZeroBasedTemp( imperial(), t->value() ) / v->value() );
    }
    else if ( unsetVar == v )
    {
        // v = nrt/p
        v->setValue( numMoles->value() * NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::toAbsZeroBasedTemp( imperial(), t->value() ) / p->value() );
    }
    else if ( unsetVar == numMoles )
    {
        // n = pv/rt
        numMoles->setValue( ( p->value() * v->value() ) / ( NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::toAbsZeroBasedTemp( imperial(), t->value() ) ) );
    }
    else if ( unsetVar == t )
    {
        // t = pv/nr
        t->setValue( NUtilities::fromAbsZeroBasedTemp( imperial(), ( p->value() * v->value() ) / ( NUtilities::NConstants::idealGasConstant( imperial() ) * numMoles->value() ) ) );
    }
}