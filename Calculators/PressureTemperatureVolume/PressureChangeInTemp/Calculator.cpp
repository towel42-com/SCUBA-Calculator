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
    return "Calculating Pressure Following a Change in Temperature";
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
    auto p1 = values[ 2 ];
    auto p2 = values[ 3 ];

    if ( !p2.has_value() )
    {
        p2 = ( absZeroBasedTemp( std::get< double >( t2.value() ) ) * ( ( std::get< double >( p1.value() ) + pressureOffset() ) / absZeroBasedTemp( std::get< double >( t1.value() ) ) ) ) - pressureOffset();
    }
    else if ( !p1.has_value() )
    {
        p1 = ( absZeroBasedTemp( std::get< double >( t1.value() ) ) * ( ( std::get< double >( p2.value() ) + pressureOffset() ) / absZeroBasedTemp( std::get< double >( t2.value() ) ) ) ) - pressureOffset();
    }
    else if ( !t1.has_value() )
    {
        t1 = fromAbsZeroBasedTemp( ( ( std::get< double >( p1.value() ) + pressureOffset() ) * absZeroBasedTemp( std::get< double >( t2.value() ) ) ) / ( std::get< double >( p2.value() ) + pressureOffset() ) );
    }
    else if ( !t2.has_value() )
    {
        t2 = fromAbsZeroBasedTemp( ( ( std::get< double >( p2.value() ) + pressureOffset() ) * absZeroBasedTemp( std::get< double >( t1.value() ) ) ) / ( std::get< double >( p1.value() ) + pressureOffset() ) );
    }
    return TOptionalVariantVector( { t1, t2, p1, p2 } );
}
