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
    virtual bool usesSaltwater() const override { return true; }
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Maximum Operating Depth (MOD)";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Partial Pressure Calculations" };
}

CSCUBACalculatorPage *CCalculator::constructPage( QWidget *parent ) const
{
    return new CPage( this, parent );
}

std::pair< std::optional< TOptionalVariantVector >, QString > CCalculator::compute( bool updateFromRHS, std::size_t triggerPos, const TOptionalVariantVector &values ) const
{
    if ( !valuesValid( values, false ) || ( numEmpty( values ) == 0 ) )
        return {};

    auto saltWater = std::get< bool >( values[ 0 ].value() );
    auto maxPO2 = std::get< double >( values[ 1 ].value() );
    auto ambientPressure = values[ 2 ];
    auto po2 = values[ 3 ];
    auto mod = values[ 4 ];

    bool pressurePO2OK = ambientPressure.has_value() || po2.has_value();
    if ( po2.has_value() && !ambientPressure.has_value() )
    {
        ambientPressure = maxPO2 / std::get< double >( po2.value() );
    }
    if ( numEmpty( { mod, ambientPressure } ) == 1 )
    {
        calculateDepthToFromPressure( saltWater, ambientPressure, mod );
    }

    if ( !po2.has_value() && ambientPressure.has_value() )
    {
        po2 = maxPO2 / std::get< double >( ambientPressure.value() );
    }

    if ( !mod.has_value() && ambientPressure.has_value() )
    {
        calculateDepthToFromPressure( saltWater, ambientPressure, mod );
    }
    return TOptionalVariantVector( { ambientPressure, po2, mod } );
}
