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
    return "PSI to-from BAR";
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

    auto psi = values[ 0 ];
    auto bar = values[ 1 ];

    if ( !psi.has_value() )
    {
        psi = std::get< double >( bar.value() ) * 14.7;
    }
    else if ( !bar.has_value() )
    {
        bar = std::get< double >( psi.value() ) / 14.7;
    }
    return TOptionalVariantVector( { psi, bar } );
}
