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
    return "Ideal Gas Law";
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

    auto p = values[ 0 ];
    auto v = values[ 1 ];
    auto numMoles = values[ 2 ];
    auto t = values[ 3 ];

    // p*v = numMoles * R * t
    if ( !p.has_value() )
    {
        p = std::get< double >( numMoles.value() ) * idealGasConstant() * absZeroBasedTemp( std::get< double >( t.value() ) ) / std::get< double >( v.value() );
    }
    else if ( !v.has_value() )
    {
        v = std::get< double >( numMoles.value() ) * idealGasConstant() * absZeroBasedTemp( std::get< double >( t.value() ) ) / std::get< double >( p.value() );
    }
    else if ( !numMoles.has_value() )
    {
        numMoles = ( std::get< double >( p.value() ) * std::get< double >( v.value() ) ) / ( idealGasConstant() * absZeroBasedTemp( std::get< double >( t.value() ) ) );
    }
    else if ( !t.has_value() )
    {
        t = fromAbsZeroBasedTemp( ( std::get< double >( p.value() ) * std::get< double >( v.value() ) ) / ( idealGasConstant() * std::get< double >( numMoles.value() ) ) );
    }
    return TOptionalVariantVector( { p, v, numMoles, t } );
}
