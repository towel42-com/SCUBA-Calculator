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
    virtual std::optional< TOptionalVariantVector > compute( const TOptionalVariantVector &values ) const override;
    virtual bool usesSaltwater() const override { return true; }
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

CSCUBACalculatorPage *CCalculator::constructPage( QWidget *parent ) const
{
    return new CPage( this, parent );
}

std::optional< TOptionalVariantVector > CCalculator::compute( const TOptionalVariantVector &values ) const
{
    if ( !valuesValid( values ) )
        return {};

    auto saltWater = std::get< bool >( values[ 0 ].value() );
    auto pressure = values[ 1 ];
    auto depth = values[ 2 ];

    calculateDepthToFromPressure( saltWater, pressure, depth );
    return TOptionalVariantVector( { pressure, depth } );
}
