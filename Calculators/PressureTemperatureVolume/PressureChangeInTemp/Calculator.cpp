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

std::optional< std::vector< std::optional< double > > > CCalculator::compute( const std::vector< std::optional< double > > &values ) const
{
    if ( !numEmptyOK( values ) )
        return {};

    auto t1 = values[ 0 ];
    auto t2 = values[ 1 ];
    auto p1 = values[ 2 ];
    auto p2 = values[ 3 ];

    if ( !p2.has_value() )
    {
        p2 = ( absZeroBasedTemp( t2.value() ) * ( ( p1.value() + pressureOffset() ) / absZeroBasedTemp( t1.value() ) ) ) - pressureOffset();
    }
    else if ( !p1.has_value() )
    {
        p1 = ( absZeroBasedTemp( t1.value() ) * ( ( p2.value() + pressureOffset() ) / absZeroBasedTemp( t2.value() ) ) ) - pressureOffset();
    }
    else if ( !t1.has_value() )
    {
        t1 = fromAbsZeroBasedTemp( ( ( p1.value() + pressureOffset() ) * absZeroBasedTemp( t2.value() ) ) / ( p2.value() + pressureOffset() ) );
    }
    else if ( !t2.has_value() )
    {
        t2 = fromAbsZeroBasedTemp( ( ( p2.value() + pressureOffset() ) * absZeroBasedTemp( t1.value() ) ) / ( p1.value() + pressureOffset() ) );
    }
    return std::vector< std::optional< double > >( { t1, t2, p1, p2 } );
}
