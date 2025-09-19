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
    return "Buoyancy Characteristics of an Object";
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
    auto buoyancy = values[ 1 ];
    auto weightOfObject = values[ 2 ];
    auto volumeDisplaced = values[ 3 ];

    if ( !weightOfObject.has_value() )
    {
        weightOfObject = buoyancy.value() + ( volumeDisplaced.value() * weightOfWater( saltwater.value() != 0 ) );
    }
    else if ( !volumeDisplaced.has_value() )
    {
        volumeDisplaced = ( weightOfObject.value() - buoyancy.value() ) / weightOfWater( saltwater.value() != 0 );
    }
    else if ( !buoyancy.has_value() )
    {
        buoyancy = weightOfObject.value() - ( volumeDisplaced.value() * weightOfWater( saltwater.value() != 0 ) );
    }

    return std::vector< std::optional< double > >( { buoyancy, weightOfObject, volumeDisplaced } );
}

