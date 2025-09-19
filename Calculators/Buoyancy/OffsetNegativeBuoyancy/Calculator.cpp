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
    return "Offsetting Negative Buoyancy";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Buoyancy Calculations" };
}

CSCUBACalculatorPage *CCalculator::constructPage( QWidget *parent ) const
{
    return new CPage( this, parent );
}

std::optional< std::vector< std::optional< double > > > CCalculator::compute( const std::vector< std::optional< double > > &values ) const
{
    if ( !numEmptyOK( values ) )
        return {};

    auto saltwater = values[ 0 ];
    auto volumeDisplaced = values[ 1 ];
    auto negativeBuoyancy = values[ 2 ];

    if ( !volumeDisplaced.has_value() )
    {
        volumeDisplaced = negativeBuoyancy.value() / weightOfWater( saltwater.value() != 0 );
    }
    else if ( !negativeBuoyancy.has_value() )
    {
        negativeBuoyancy = volumeDisplaced.value() * weightOfWater( saltwater.value() != 0 );
    }
    return std::vector< std::optional< double > >( { volumeDisplaced, negativeBuoyancy } );
}
