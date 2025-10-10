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

    virtual QString getBaseFormula() const override;   // for descriptive purposes
    virtual std::optional< QString > getCurrentFormula() const override;   // returns the current formula in use

    virtual void computeValues() const override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return tr( "Calculating Relative Change in Ambient Pressure" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Pressure and Volume Conversions" ) };
}


TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< SVariableInfo >( "relChange", tr( "Relative Change in Ambient Pressure" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "p2", tr( "Pressure 2" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "p1", tr( "Pressure 1" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
        };
}

QString CCalculator::getBaseFormula() const
{
    return R"__(<relChange> = \frac{<p2>}{<p1>})__";
}

QString CCalculator::computeAndGenerateFormula( bool &isBaseFormula ) const
{
    auto relChange = getVariable( "relChange" );
    auto p1 = getVariable( "p1" );
    auto p2 = getVariable( "p2" );

    QString formula;
    bool aOK = numUnsetVariables() == 1;
    isBaseFormula = false;
    if ( !aOK )
    {
        formula = getBaseFormula();
        isBaseFormula = true;
    }
    else if ( !relChange->has_value() )
    {
        relChange->setValue( p2->value() / p1->value() );
        formula = tr( R"__(<relChange> = \frac{<p2>}{<p1>})__" );
    }
    else if ( !p1->has_value() )
    {
        p1->setValue( p2->value() / relChange->value() );
        formula = tr( R"__(<p1> = \frac{<p2>}{<relChange>})__" );
    }
    else if ( !p2->has_value() )
    {
        p2->setValue( relChange->value() * p1->value() );
        formula = tr( R"__(<p2> = <p2> \times <relChange>)__" );
    }
    return formula;
}

