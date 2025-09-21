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
    virtual bool usesSaltwater() const override { return true; }
    virtual std::optional< TOptionalVariantVector > compute( const TOptionalVariantVector &values ) const override;
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

std::optional< TOptionalVariantVector > CCalculator::compute( const TOptionalVariantVector &values ) const
{
    if ( !valuesValid( values ) )
        return {};

    auto saltwater = std::get< bool >( values[ 0 ].value() );
    auto buoyancy = values[ 1 ];
    auto weightOfObject = values[ 2 ];
    auto volumeDisplaced = values[ 3 ];

    if ( !weightOfObject.has_value() )
    {
        weightOfObject = std::get< double >( buoyancy.value() ) + ( std::get< double >( volumeDisplaced.value() ) * weightOfWater( saltwater ) );
    }
    else if ( !volumeDisplaced.has_value() )
    {
        volumeDisplaced = ( std::get< double >( weightOfObject.value() ) - std::get< double >( buoyancy.value() ) ) / weightOfWater( saltwater );
    }
    else if ( !buoyancy.has_value() )
    {
        buoyancy = std::get< double >( weightOfObject.value() ) - ( std::get< double >( volumeDisplaced.value() ) * weightOfWater( saltwater ) );
    }

    return TOptionalVariantVector( { buoyancy, weightOfObject, volumeDisplaced } );
}
