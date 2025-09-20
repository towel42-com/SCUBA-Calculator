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
    virtual std::optional< std::vector< std::optional< double > > > compute( const std::vector< std::optional< double > > &values ) const override;
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Calculating Volume Following a Change in Temperature";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure, Temperature and Volume Calculations" };
}

CSCUBACalculatorPage *CCalculator::constructPage( QWidget *parent ) const
{
    return new CPage( this, parent );
}

std::optional< std::vector< std::optional< double > > > CCalculator::compute( const std::vector< std::optional< double > > &values ) const
{
    if ( !numEmptyOK( values ) )
        return {};

    auto t1 = values[ 0 ];
    auto t2 = values[ 1 ];
    auto v1 = values[ 2 ];
    auto v2 = values[ 3 ];

    if ( !v2.has_value() )
    {
        v2 = absZeroBasedTemp( t2.value() ) * ( v1.value()/absZeroBasedTemp( t1.value() ) );
    }
    else if ( !v1.has_value() )
    {
        v1 = absZeroBasedTemp( t1.value() ) * ( v2.value() / absZeroBasedTemp( t2.value() ) );
    }
    else if ( !t1.has_value() )
    {
        t1 = ( absZeroBasedTemp( t2.value() ) * ( v1.value() / v2.value() ) ) - absZero();
    }
    else if ( !t2.has_value() )
    {
        t2 = ( absZeroBasedTemp( t1.value() ) * ( v2.value() / v1.value() ) ) - absZero();
    }
    return std::vector< std::optional< double > >( { t1, t2, v1, v2 } );
}
