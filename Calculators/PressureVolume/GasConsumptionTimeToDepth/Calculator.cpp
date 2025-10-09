#include "Calculator.h"
#include "VariableInfo.h"

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
    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior );
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return tr( "Calculating Depth at for given Gas Consumption Time" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Pressure and Volume Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< SVariableInfo >( "p1", tr( "Gas Consumption 1" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "m1", tr( "Time 1" ), EVariableType::eVariable, EUnit::eTime, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "p2", tr( "Gas Consumption 2" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "m2", tr( "Time 2" ), EVariableType::eVariable, EUnit::eTime, EVariableLoc::eRHS ),   //
        };
}

TVariableInfo CCalculator::determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior )
{
    TVariableInfo retVal;

    if ( updateFromSide == EVariableLoc::eLHS )
    {
        if ( getVariable( "p1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "p2" );
        else if ( getVariable( "m1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "m2" );
    }
    else if ( updateFromSide == EVariableLoc::eRHS )
    {
        if ( getVariable( "p2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "p1" );
        else if ( getVariable( "m2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "m1" );
    }
    else
        retVal = CSCUBACalculator::determineVariableToUnset( updateFromSide, triggerWidget, preDefaultBehavior );
    return retVal;
}

QString CCalculator::getDefaultFormula() const
{
    return R"__(<p1> \times <m1> = <p2> \times <m2>)__";
}

QString CCalculator::computeAndGenerateFormula( bool &isBaseFormula ) const
{
    auto p1 = getVariable( "p1" );
    auto m1 = getVariable( "m1" );

    auto p2 = getVariable( "p2" );
    auto m2 = getVariable( "m2" );

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
        p1->setValue( p2->value() * ( m2->value() / m1->value() ) );
        formula = tr( R"__(<p1> = <p2> \times \frac{<m2>}{<m1>})__" );
    }
    else if ( !p2->has_value() )
    {
        p2->setValue( p1->value() * ( m1->value() / m2->value() ) );
        formula = tr( R"__(<p2> = <p1> \times \frac{<m1>}{<m2>})__" );
    }
    else if ( !m1->has_value() )
    {
        m1->setValue( m2->value() * ( p2->value() / p1->value() ) );
        formula = tr( R"__(<m1> = <m2> \times \frac{<p2>}{<p1>})__" );
    }
    else if ( !m2->has_value() )
    {
        m2->setValue( m1->value() * ( p1->value() / p2->value() ) );
        formula = tr( R"__(<m2> = <m1> \times \frac{<p1>}{<p2>})__" );
    }
    return formula;
}
