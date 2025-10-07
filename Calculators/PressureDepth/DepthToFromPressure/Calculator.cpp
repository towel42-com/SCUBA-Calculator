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

    virtual bool isWaterTypeBased() const override { return true; }

    virtual std::list< std::shared_ptr< SVariableInfo > > getMyVariables() const override;
    virtual QString getDefaultFormula() const override;
    virtual QString computeAndGenerateFormula() const override;
    virtual void customDetermineVariableToUnset( ESide updateFromSide, QWidget *triggerWidget ) override;
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Depth to-from Pressure";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure and Depth Conversions" };
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

    //auto pressure = values[ 0 ];
    //auto depth = values[ 1 ];

    //NUtilities::calculateDepthToFromPressure( imperial(), saltWater(), pressure, depth );
    //return TOptionalDoubleVector( { pressure, depth } );
}


void CCalculator::customDetermineVariableToUnset( ESide updateFromSide, QWidget *triggerWidget )
{
    (void)updateFromSide;
    (void)triggerWidget;
}

