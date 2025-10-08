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
    return "Calculating Pressure Following a Change in Temperature";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure, Temperature and Volume Calculations" };
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

    //auto t1 = values[ 0 ];
    //auto t2 = values[ 1 ];
    //auto p1 = values[ 2 ];
    //auto p2 = values[ 3 ];

    //if ( !p2.has_value() )
    //{
    //    p2 = ( NUtilities::toAbsZeroBasedTemp( imperial(), t2.value() ) * ( ( p1.value() + NUtilities::NConstants::pressureOffset( imperial() ) ) / NUtilities::toAbsZeroBasedTemp( imperial(), t1.value() ) ) ) - NUtilities::NConstants::pressureOffset( imperial() );
    //}
    //else if ( !p1.has_value() )
    //{
    //    p1 = ( NUtilities::toAbsZeroBasedTemp( imperial(), t1.value() ) * ( ( p2.value() + NUtilities::NConstants::pressureOffset( imperial() ) ) / NUtilities::toAbsZeroBasedTemp( imperial(), t2.value() ) ) ) - NUtilities::NConstants::pressureOffset( imperial() );
    //}
    //else if ( !t1.has_value() )
    //{
    //    t1 = NUtilities::fromAbsZeroBasedTemp( imperial(), ( ( p1.value() + NUtilities::NConstants::pressureOffset( imperial() ) ) * NUtilities::toAbsZeroBasedTemp( imperial(), t2.value() ) ) / ( p2.value() + NUtilities::NConstants::pressureOffset( imperial() ) ) );
    //}
    //else if ( !t2.has_value() )
    //{
    //    t2 = NUtilities::fromAbsZeroBasedTemp( imperial(), ( ( p2.value() + NUtilities::NConstants::pressureOffset( imperial() ) ) * NUtilities::toAbsZeroBasedTemp( imperial(), t1.value() ) ) / ( p1.value() + NUtilities::NConstants::pressureOffset( imperial() ) ) );
    //}
    //return TOptionalDoubleVector( { t1, t2, p1, p2 } );
}


