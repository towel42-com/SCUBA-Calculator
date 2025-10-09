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

    virtual std::list< std::shared_ptr< SVariableInfo > > getMyVariables() const override;
    virtual QString getDefaultFormula() const override;
    virtual QString computeAndGenerateFormula( bool & isBaseFormula ) const override;
    virtual TVariableInfo customDetermineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior );
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return tr( "Combined Gas Law" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Pressure, Temperature and Volume Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< SVariableInfo >( "p1", tr( "Pressure 1" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "v1", tr( "Volume 1" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "t1", tr( "Temperature 1" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "p2", tr( "Pressure 2" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "v2", tr( "Volume 2" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "t2", tr( "Temperature 2" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "absOffset", tr( "Absolute Temperature Offset" ), EVariableType::eAbsZeroOffsetConstant, EUnit::eNone, EVariableLoc::eRHS ),   //
        };
}

TVariableInfo CCalculator::customDetermineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior )
{
    TVariableInfo retVal;

    if ( updateFromSide == EVariableLoc::eLHS )
    {
        if ( getVariable( "p1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "p2" );
        else if ( getVariable( "t1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "t2" );
        else if ( getVariable( "v1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "v2" );
    }
    else if ( updateFromSide == EVariableLoc::eRHS )
    {
        if ( getVariable( "p2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "p1" );
        else if ( getVariable( "t2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "t1" );
        else if ( getVariable( "v2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "v1" );
    }
    return CSCUBACalculator::customDetermineVariableToUnset( updateFromSide, triggerWidget, preDefaultBehavior );
}

QString CCalculator::getDefaultFormula() const
{
    return R"__(\frac{<p1> \times <v1>}{<t1>} = \frac{<p2> \times <v2>}{<t2>})__";
}

QString CCalculator::computeAndGenerateFormula( bool &isBaseFormula ) const
{
    auto p1 = getVariable( "p1" );
    auto v1 = getVariable( "v1" );
    auto t1 = getVariable( "t1" );

    auto p2 = getVariable( "p2" );
    auto v2 = getVariable( "v2" );
    auto t2 = getVariable( "t2" );

    QString formula;
    bool aOK = numUnsetVariables() == 1;
    isBaseFormula = false;
    if ( !aOK )
    {
        formula = getDefaultFormula();
        isBaseFormula = true;
    }
    else if ( !p1->has_value() )
    {
        p1->setValue( p2->value() * ( v2->value() / v1->value() ) * ( NUtilities::toAbsZeroBasedTemp( imperial(), t1->value() ) / NUtilities::toAbsZeroBasedTemp( imperial(), t2->value() ) ) );
        formula = tr( R"__(<p1> = <p2> \times \frac{<v2>}{<v1>} \times \frac{<t1> + <absOffset>}{<t2> + <absOffset>})__" );
    }
    else if ( !p2->has_value() )
    {
        p2->setValue( p1->value() * ( v1->value() / v2->value() ) * ( NUtilities::toAbsZeroBasedTemp( imperial(), t2->value() ) / NUtilities::toAbsZeroBasedTemp( imperial(), t1->value() ) ) );
        formula = tr( R"__(<p2> = <p1> \times \frac{<v1>}{<v2>} \times \frac{<t2> + <absOffset>}{<t1> + <absOffset>})__" );
    }
    else if ( !v1->has_value() )
    {
        v1->setValue( v2->value() * ( NUtilities::toAbsZeroBasedTemp( imperial(), t1->value() ) / NUtilities::toAbsZeroBasedTemp( imperial(), t2->value() ) ) * ( p2->value() / p1->value() ) );
        formula = tr( R"__(<v1> = <v2> \times \frac{<t1> + <absOffset>}{<t2> + <absOffset>} \times \frac{<p2>}{<p1>})__" );
    }
    else if ( !v2->has_value() )
    {
        v2->setValue( v1->value() * ( NUtilities::toAbsZeroBasedTemp( imperial(), t2->value() ) / NUtilities::toAbsZeroBasedTemp( imperial(), t1->value() ) ) * ( p1->value() / p2->value() ) );
        formula = tr( R"__(<v2> = <v1> \times \frac{<t2> + <absOffset>}{<t1> + <absOffset>} \times \frac{<p1>}{<p2>})__" );
    }
    else if ( !t1->has_value() )
    {
        t1->setValue( t2->value() * ( p1->value() / p2->value() ) * ( v1->value() / v2->value() ) );
        formula = tr( R"__(<t1> = <t2> \times \frac{<p1>}{<p2>} \times \frac{<v1>}{<v2>})__" );
    }
    else if ( !t2->has_value() )
    {
        t2->setValue( t1->value() * ( p2->value() / p1->value() ) * ( v2->value() / v1->value() ) );
        formula = tr( R"__(<t2> = <t1> \times \frac{<p2>}{<p1>} \times \frac{<v2>}{<v1>})__" );
    }

    return formula;
}
