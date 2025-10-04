#include "Calculator.h"
#include "Page.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() {}
    virtual ~CCalculator() override {}

    QString calculatorName() const override;
    QStringList calculatorPath() const override;

    virtual CSCUBACalculatorPage *constructPage( QWidget *parent ) const override;
    virtual bool isWaterTypeBased() const override { return true; }
    virtual std::optional< TOptionalVariantVector > compute( const TOptionalVariantVector &values ) const override;
    virtual std::optional< TOptionalVariantVector > setupValues( bool updateFromRHS, std::size_t triggerPos, const TOptionalVariantVector &values ) const override;
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

CSCUBACalculatorPage *CCalculator::constructPage( QWidget *parent ) const
{
    return new CPage( this, parent );
}

std::optional< TOptionalVariantVector > CCalculator::setupValues( bool updateFromRHS, std::size_t triggerPos, const TOptionalVariantVector &values ) const
{
    (void)updateFromRHS;
    (void)triggerPos;
    (void)values;
    return {};
}

std::optional< TOptionalVariantVector > CCalculator::compute( const TOptionalVariantVector &values ) const
{
    bool depthForPressureOK = ( values[ 0 ].has_value() && ( values[ 1 ].has_value() || values[ 2 ].has_value() ) );
    bool po2OK = ( values[ 3 ].has_value() && values[ 4 ].has_value() );

    if ( !valuesValid( values ) && !po2OK && !depthForPressureOK )
        return {};

    auto saltWater = std::get< bool >( values[ 0 ].value() );
    auto pressureForDepth = values[ 1 ];
    auto depthForPressure = values[ 2 ];
    auto partialPressure = values[ 3 ];
    auto surfacePressure = values[ 4 ];

    if ( po2OK && ( !pressureForDepth.has_value() && !depthForPressure.has_value() ) )
    {
        pressureForDepth = std::get< double >( partialPressure.value() ) / std::get< double >( surfacePressure.value() );
        depthForPressureOK = true;
    }

    if ( depthForPressureOK && ( !pressureForDepth.has_value() || !depthForPressure.has_value() ) )
    {
        calculateDepthToFromPressure( saltWater, pressureForDepth, depthForPressure );
    }

    if ( valuesValid( { pressureForDepth, partialPressure, surfacePressure } ) )
    {
        if ( !partialPressure.has_value() )
        {
            partialPressure = std::get< double >( pressureForDepth.value() ) * std::get< double >( surfacePressure.value() );
        }
        else if ( !surfacePressure.has_value() )
        {
            surfacePressure = std::get< double >( partialPressure.value() ) / std::get< double >( partialPressure.value() );
        }
    }
    return TOptionalVariantVector( { pressureForDepth, depthForPressure, partialPressure, surfacePressure } );
}
