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
    return "Quick Estimates for Temperature and Pressure";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure, Temperature and Volume Calculations" };
}

CSCUBACalculatorPage *CCalculator::constructPage( QWidget *parent ) const
{
    return new CPage( this, parent );
}

std::optional< TOptionalVariantVector > CCalculator::compute( const TOptionalVariantVector &values ) const
{
    if ( !valuesValid( values ) )
        return {};

    auto t = values[ 0 ];
    auto p = values[ 1 ];

    // p*v = numMoles * R * t
    if ( !t.has_value() )
    {
        t = pressurePerTemp() * std::get< double >( p.value() );
    }
    else if ( !p.has_value() )
    {
        p = std::get< double >( t.value() ) / pressurePerTemp();
    }
    return TOptionalVariantVector( { t, p } );
}
