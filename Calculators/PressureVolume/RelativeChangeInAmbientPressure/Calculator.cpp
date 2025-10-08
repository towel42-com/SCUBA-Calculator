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
    virtual QString computeAndGenerateFormula() const override;
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Calculating Relative Change in Ambient Pressure";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure and Volume Conversions" };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            //std::make_shared< SVariableInfo >( "volumeDisplaced", tr( "Volume Displaced" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eLHS ),   //
        };
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

    //auto relChange = values[ 0 ];
    //auto p1 = values[ 1 ];
    //auto p2 = values[ 2 ];

    //if ( !relChange.has_value() )
    //{
    //    relChange = p2.value() / p1.value();
    //}
    //else if ( !p2.has_value() )
    //{
    //    p2 = relChange.value() * p1.value();
    //}
    //else if ( !p1.has_value() )
    //{
    //    p1 = p2.value() / relChange.value();
    //}
    //return TOptionalDoubleVector( { relChange, p1, p2 } );
}

