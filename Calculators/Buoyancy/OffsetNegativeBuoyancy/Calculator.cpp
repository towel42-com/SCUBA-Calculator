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

std::optional< TOptionalVariantVector > CCalculator::compute( const TOptionalVariantVector &values ) const
{
    if ( !valuesValid( values ) )
        return {};

    auto saltWater = std::get< bool >( values[ 0 ].value() );
    auto volumeDisplaced = values[ 1 ];
    auto negativeBuoyancy = values[ 2 ];

    if ( !volumeDisplaced.has_value() )
    {
        volumeDisplaced = std::get< double >( negativeBuoyancy.value() ) / weightOfWater( saltWater );
    }
    else if ( !negativeBuoyancy.has_value() )
    {
        negativeBuoyancy = std::get< double >( volumeDisplaced.value() ) * weightOfWater( saltWater );
    }
    return TOptionalVariantVector( { volumeDisplaced, negativeBuoyancy } );
}
