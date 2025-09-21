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
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Depth Seawater to-from Freshwater";
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

    auto depthFreshWater = values[ 0 ];
    auto depthSaltWater = values[ 1 ];

    if ( !depthFreshWater.has_value() )
    {
        depthFreshWater = std::get< double >( depthSaltWater.value() ) * 1.03;
    }
    else if ( !depthSaltWater.has_value() )
    {
        depthSaltWater = std::get< double >( depthFreshWater.value() ) / 1.03;
    }
    return TOptionalVariantVector( { depthFreshWater, depthSaltWater } );
}