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

std::optional< std::vector< std::optional< double > > > CCalculator::compute( const std::vector< std::optional< double > > &values ) const
{
    if ( !values[ 0 ].has_value() || !values[ 1 ].has_value() )
        return {};

    if ( numEmpty( values ) == 0 )
        return {};

    auto saltWater = values[ 0 ].value() != 0.0;
    auto maxPO2 = values[ 1 ].value();
    auto ambientPressure = values[ 2 ];
    auto po2 = values[ 3 ];
    auto mod = values[ 4 ];

    bool pressurePO2OK = ambientPressure.has_value() || po2.has_value();
    if ( po2.has_value() && !ambientPressure.has_value() )
    {
        ambientPressure = maxPO2 / po2.value();
    }
    if ( numEmpty( { mod, ambientPressure } ) == 1 )
    {
        calculateDepthToFromPressure( saltWater, ambientPressure, mod );
    }

    if ( !po2.has_value() && ambientPressure.has_value() )
    {
        po2 = maxPO2 / ambientPressure.value();
    }

    if ( !mod.has_value() && ambientPressure.has_value() )
    {
        calculateDepthToFromPressure( saltWater, ambientPressure, mod );
    }
    return std::vector< std::optional< double > >( { ambientPressure, po2, mod } );
}
