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
    return "Calculating Partial Pressure for Given Depth";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Partial Pressure Calculations" };
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
    //bool depthForPressureOK = ( values[ 0 ].has_value() && ( values[ 1 ].has_value() || values[ 2 ].has_value() ) );
    //bool po2OK = ( values[ 3 ].has_value() && values[ 4 ].has_value() );

    //if ( !NUtilities::valuesValid( values ) && !po2OK && !depthForPressureOK )
    //    return {};

    //auto pressureForDepth = values[ 1 ];
    //auto depthForPressure = values[ 2 ];
    //auto partialPressure = values[ 3 ];
    //auto surfacePressure = values[ 4 ];

    //if ( po2OK && ( !pressureForDepth.has_value() && !depthForPressure.has_value() ) )
    //{
    //    pressureForDepth = partialPressure.value() / surfacePressure.value();
    //    depthForPressureOK = true;
    //}

    //if ( depthForPressureOK && ( !pressureForDepth.has_value() || !depthForPressure.has_value() ) )
    //{
    //    NUtilities::calculateDepthToFromPressure( imperial(), saltWater(), pressureForDepth, depthForPressure );
    //}

    //if ( NUtilities::valuesValid( { pressureForDepth, partialPressure, surfacePressure } ) )
    //{
    //    if ( !partialPressure.has_value() )
    //    {
    //        partialPressure = pressureForDepth.value() * surfacePressure.value();
    //    }
    //    else if ( !surfacePressure.has_value() )
    //    {
    //        surfacePressure = partialPressure.value() / partialPressure.value();
    //    }
    //}
    //return TOptionalDoubleVector( { pressureForDepth, depthForPressure, partialPressure, surfacePressure } );
}

void CCalculator::customDetermineVariableToUnset( ESide updateFromSide, QWidget *triggerWidget )
{
    (void)updateFromSide;
    (void)triggerWidget;
}
