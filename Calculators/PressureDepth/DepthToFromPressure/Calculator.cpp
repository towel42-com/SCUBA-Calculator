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
    return "Depth to-from Pressure";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure and Depth Conversions" };
}

CSCUBACalculatorPage *CCalculator::constructPage( QWidget *parent ) const
{
    return new CPage( this, parent );
}

std::optional< std::vector< std::optional< double > > > CCalculator::compute( const std::vector< std::optional< double > > &values ) const
{
    if ( !numEmptyOK( values ) )
        return {};

    auto saltWater = values[ 0 ];
    if ( !saltWater.has_value() )
        return {};
    auto pressure = values[ 1 ];
    auto depth = values[ 2 ];

    auto lengthOfATM = lengthToSingleAtmosphere( saltWater.value() );
    if ( !depth.has_value() )
    {
        depth = ( pressure.value() - 1 ) * lengthOfATM;
    }
    else if ( !pressure.has_value() )
    {
        pressure = ( depth.value() / lengthOfATM ) + 1;
    }
    return std::vector< std::optional< double > >( { pressure, depth } );
}
