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
    virtual std::pair< std::optional< TOptionalVariantVector >, QString > compute( bool updateFromRHS, std::size_t triggerPos, const TOptionalVariantVector &values ) const override;
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Combined Gas Law";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure, Temperature and Volume Calculations" };
}

CSCUBACalculatorPage *CCalculator::constructPage( QWidget *parent ) const
{
    return new CPage( this, parent );
}

std::pair< std::optional< TOptionalVariantVector >, QString > CCalculator::compute( bool updateFromRHS, std::size_t triggerPos, const TOptionalVariantVector &values ) const
{
    if ( !valuesValid( values ) )
        return {};

    auto t1 = values[ 0 ];
    auto t2 = values[ 1 ];
    auto v1 = values[ 2 ];
    auto v2 = values[ 3 ];
    auto p1 = values[ 4 ];
    auto p2 = values[ 5 ];

    // p1*v1/t1 = p2*v2/t2
    if ( !t1.has_value() )
    {
        t1 = fromAbsZeroBasedTemp( absZeroBasedTemp( std::get< double >( t2.value() ) ) * ( std::get< double >( p1.value() ) / std::get< double >( p2.value() ) ) * ( std::get< double >( v1.value() ) / std::get< double >( v2.value() ) ) );
    }
    else if ( !t2.has_value() )
    {
        t2 = fromAbsZeroBasedTemp( absZeroBasedTemp( std::get< double >( t1.value() ) ) * ( std::get< double >( p2.value() ) / std::get< double >( p1.value() ) ) * ( std::get< double >( v2.value() ) / std::get< double >( v1.value() ) ) );
    }
    else if ( !v1.has_value() )
    {
        v1 = std::get< double >( v2.value() ) * ( absZeroBasedTemp( std::get< double >( t1.value() ) ) / absZeroBasedTemp( std::get< double >( t2.value() ) ) ) * ( std::get< double >( p2.value() ) / std::get< double >( p1.value() ) );
    }
    else if ( !v2.has_value() )
    {
        v2 = std::get< double >( v1.value() ) * ( absZeroBasedTemp( std::get< double >( t2.value() ) ) / absZeroBasedTemp( std::get< double >( t1.value() ) ) ) * ( std::get< double >( p1.value() ) / std::get< double >( p2.value() ) );
    }
    else if ( !p1.has_value() )
    {
        p1 = std::get< double >( p2.value() ) * ( std::get< double >( v2.value() ) / std::get< double >( v1.value() ) ) * ( absZeroBasedTemp( std::get< double >( t1.value() ) ) / absZeroBasedTemp( std::get< double >( t2.value() ) ) );
    }
    else if ( !p2.has_value() )
    {
        p2 = std::get< double >( p1.value() ) * ( std::get< double >( v1.value() ) / std::get< double >( v2.value() ) ) * ( absZeroBasedTemp( std::get< double >( t2.value() ) ) / absZeroBasedTemp( std::get< double >( t1.value() ) ) );
    }
    return TOptionalVariantVector( { t1, t2, v1, v2, p1, p2 } );
}
