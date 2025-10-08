#include "Calculator.h"
#include "Page.h"

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
    virtual QString computeAndGenerateFormula() const override;
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Calculating Volume/Pressure Following a Change in Pressure/Volume (P1xV1=P2xV2)";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure and Volume Conversions" };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return {};
}

QString CCalculator::getDefaultFormula() const
{
    return {};
}

QString CCalculator::computeAndGenerateFormula() const
{
    return {};
    //if ( !NUtilities::valuesValid( values ) )
    //    return {};

    //auto p1 = values[ 0 ];
    //auto v1 = values[ 1 ];
    //auto p2 = values[ 2 ];
    //auto v2 = values[ 3 ];

    //if ( !p1.has_value() )
    //{
    //    p1 = ( p2.value() * v2.value() ) / v1.value();
    //}
    //else if ( !v1.has_value() )
    //{
    //    v1 = ( p2.value() * v2.value() ) / p1.value();
    //}
    //else if ( !p2.has_value() )
    //{
    //    p2 = ( p1.value() * v1.value() ) / v2.value();
    //}
    //else if ( !v2.has_value() )
    //{
    //    v2 = ( p1.value() * v1.value() ) / p2.value();
    //}
    //return TOptionalDoubleVector( { p1, v1, p2, v2 } );
}

