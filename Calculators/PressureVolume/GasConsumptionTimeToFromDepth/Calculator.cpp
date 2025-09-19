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
    return "Calculating Gas Consumption Time to-from Specific Depth";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure and Volume Conversions" };
}

CSCUBACalculatorPage *CCalculator::constructPage( QWidget *parent ) const
{
    return new CPage( this, parent );
}

std::optional< std::vector< std::optional< double > > > CCalculator::compute( const std::vector< std::optional< double > > &values ) const
{
    if ( !numEmptyOK( values ) )
        return {};
    auto p1 = values[ 0 ];
    auto t1 = values[ 1 ];
    auto p2 = values[ 2 ];
    auto t2 = values[ 3 ];

    if ( !p1.has_value() )
    {
        p1 = ( p2.value() * t2.value() ) / t1.value();
    }
    else if ( !t1.has_value() )
    {
        t1 = ( p2.value() * t2.value() ) / p1.value();
    }
    else if ( !p2.has_value() )
    {
        p2 = ( p1.value() * t1.value() ) / t2.value();
    }
    else if ( !t2.has_value() )
    {
        t2 = ( p1.value() * t1.value() ) / p2.value();
    }
    return std::vector< std::optional< double > >( { p1, t1, p2, t2 } );
}
