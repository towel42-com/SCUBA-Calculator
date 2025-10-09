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
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Quick Estimates for Temperature and Pressure";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure, Temperature and Volume Calculations" };
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

QString CCalculator::computeAndGenerateFormula( bool & isBaseFormula ) const
{
    isBaseFormula = true;
    return {};
    //if ( !NUtilities::valuesValid( values ) )
    //    return {};

    //auto t = values[ 0 ];
    //auto p = values[ 1 ];

    //// p*v = numMoles * R * t
    //if ( !t.has_value() )
    //{
    //    t = NUtilities::NConstants::pressurePerTemp( imperial() ) * p.value();
    //}
    //else if ( !p.has_value() )
    //{
    //    p = t.value() / NUtilities::NConstants::pressurePerTemp( imperial() );
    //}
    //return TOptionalDoubleVector( { t, p } );
}

