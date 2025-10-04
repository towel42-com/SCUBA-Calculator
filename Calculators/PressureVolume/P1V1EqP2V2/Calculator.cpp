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
    virtual std::optional< TOptionalVariantVector > setupValues( bool updateFromRHS, std::size_t triggerPos, const TOptionalVariantVector &values ) const override;
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Calculating Volume/Pressure Following a Change in Pressure/Volume (P1xV1=P2xV2)";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure and Volume Conversions" };
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
    if ( !valuesValid( values ) )
        return {};

    auto p1 = values[ 0 ];
    auto v1 = values[ 1 ];
    auto p2 = values[ 2 ];
    auto v2 = values[ 3 ];

    if ( !p1.has_value() )
    {
        p1 = ( std::get< double >( p2.value() ) * std::get< double >( v2.value() ) ) / std::get< double >( v1.value() );
    }
    else if ( !v1.has_value() )
    {
        v1 = ( std::get< double >( p2.value() ) * std::get< double >( v2.value() ) ) / std::get< double >( p1.value() );
    }
    else if ( !p2.has_value() )
    {
        p2 = ( std::get< double >( p1.value() ) * std::get< double >( v1.value() ) ) / std::get< double >( v2.value() );
    }
    else if ( !v2.has_value() )
    {
        v2 = ( std::get< double >( p1.value() ) * std::get< double >( v1.value() ) ) / std::get< double >( p2.value() );
    }
    return TOptionalVariantVector( { p1, v1, p2, v2 } );
}
