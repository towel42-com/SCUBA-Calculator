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

std::optional< std::vector< std::optional< double > > > CCalculator::compute( const std::vector< std::optional< double > > &values ) const
{
    if ( !numEmptyOK( values ) )
        return {};

    auto p = values[ 0 ];
    auto v = values[ 1 ];
    auto numMoles = values[ 2 ];
    auto t = values[ 3 ];

    // p*v = numMoles * R * t
    if ( !p.has_value() )
    {
        p = numMoles.value() * idealGasConstant() * absZeroBasedTemp( t.value() ) / v.value();
    }
    else if ( !v.has_value() )
    {
        v = numMoles.value() * idealGasConstant() * absZeroBasedTemp( t.value() ) / p.value();
    }
    else if ( !numMoles.has_value() )
    {
        numMoles = ( p.value() * v.value() ) / ( idealGasConstant() * absZeroBasedTemp( t.value() ) );
    }
    else if ( !t.has_value() )
    {
        t = fromAbsZeroBasedTemp( ( p.value() * v.value() ) / ( idealGasConstant() * numMoles.value() ) );
    }
    return std::vector< std::optional< double > >( { p, v, numMoles, t } );
}
