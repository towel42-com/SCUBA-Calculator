#include "Calculator.h"
#include "VariableInfo.h"
#include "Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() {}
    virtual ~CCalculator() override {}

    virtual QString calculatorName() const override;
    virtual QStringList calculatorPath() const override;

    virtual void resetVariables() override { CSCUBACalculator::resetVariables(); }
    virtual QFrame *svgFrame() const override { return CSCUBACalculator::svgFrame(); }
    virtual QSvgWidget *svgWidget() const override { return CSCUBACalculator::svgWidget(); }

    virtual TVariableInfoList getMyVariables() const override;
    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior );

    virtual QString getBaseFormula() const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo & unsetVar ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo & unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return tr( "Calculating Volume Following a Change in Temperature" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Pressure, Temperature and Volume Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "v1", tr( "Volume 1" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "t1", tr( "Temperature 1" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "v2", tr( "Volume 2" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "t2", tr( "Temperature 2" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "absOffset", tr( "Absolute Temperature Offset" ), EVariableType::eAbsZeroOffsetConstant, EUnit::eNone, EVariableLoc::eRHS ),   //
        };
}

TVariableInfo CCalculator::determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior )
{
    TVariableInfo retVal;

    if ( updateFromSide == EVariableLoc::eLHS )
    {
        if ( getVariable( "t1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "t2" );
        else if ( getVariable( "v1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "v2" );
    }
    else if ( updateFromSide == EVariableLoc::eRHS )
    {
        if ( getVariable( "t2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "t1" );
        else if ( getVariable( "v2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "v1" );
    }
    else
        retVal = CSCUBACalculator::determineVariableToUnset( updateFromSide, triggerWidget, preDefaultBehavior );
    return retVal;
}

QString CCalculator::getBaseFormula() const
{
    return R"__(\frac{<v1>}{<t1>} = \frac{<v2>}{<t2>})__";
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo & unsetVar ) const
{
    if ( unsetVar->name() == "v1" )
    {
        // V1 = V2 * ( t1/t2 );
        return R"__(<v1> = <v2> \times \frac{<t1> + <absOffset>}{<t2> + <absOffset>})__";
    }
    else if ( unsetVar->name() == "v2" )
    {
        // V2 = V1 * ( t2/t1 );
        return R"__(<v2> = <v1> \times \frac{<t2> + <absOffset>}{<t1> + <absOffset>})__";
    }
    else if ( unsetVar->name() == "t1" )
    {
        // T1 = t2*(V1/v2)
        return R"__(<t1> = [(<t2> + <absOffset>) \times \frac{<v1>}{<v2>}] - <absOffset>)__";
    }
    else if ( unsetVar->name() == "t2" )
    {
        // T2 = t1*(V2/v1)
        return R"__(<t2> = [(<t1> + <absOffset>) \times \frac{<v2>}{<v1>}] - <absOffset>)__";
    }

    return {};
}

void CCalculator::computeValueForVar( TVariableInfo & unsetVar )
{
    auto v1 = getVariable( "v1" );
    auto t1 = getVariable( "t1" );

    auto v2 = getVariable( "v2" );
    auto t2 = getVariable( "t2" );

    if ( unsetVar == v1 )
    {
        // V1 = V2 * ( t1/t2 );
        v1->setValue( NUtilities::toAbsZeroBasedTemp( imperial(), t1->value() ) * ( v2->value() / NUtilities::toAbsZeroBasedTemp( imperial(), t2->value() ) ) );
    }
    else if ( unsetVar == v2 )
    {
        // V2 = V1 * ( t2/t1 );
        v2->setValue( NUtilities::toAbsZeroBasedTemp( imperial(), t2->value() ) * ( v1->value() / NUtilities::toAbsZeroBasedTemp( imperial(), t1->value() ) ) );
    }
    else if ( unsetVar == t1 )
    {
        // T1 = t2*(V1/v2)
        t1->setValue( NUtilities::fromAbsZeroBasedTemp( imperial(), NUtilities::toAbsZeroBasedTemp( imperial(), t2->value() ) * ( v1->value() / v2->value() ) ) );
    }
    else if ( unsetVar == t2 )
    {
        // T2 = t1*(V2/v1)
        t2->setValue( NUtilities::fromAbsZeroBasedTemp( imperial(), NUtilities::toAbsZeroBasedTemp( imperial(), t1->value() ) * ( v2->value() / v1->value() ) ) );
    }
}