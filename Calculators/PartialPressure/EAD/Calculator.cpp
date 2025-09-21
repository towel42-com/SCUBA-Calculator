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
    return "Equivalent Air Depth (EAD)";
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
    if ( !numEmptyOK( values ) )
        return {};

    if ( !values[ 0 ].has_value() )
        return {};
    auto saltWater = values[ 0 ] != 0;
    auto ead = values[ 1 ];
    auto fn2 = values[ 2 ];
    auto depth = values[ 3 ];

    if ( !ead.has_value() )
    {
        ead = ( ( fn2.value() / percentN2AtSurface() ) * ( depth.value() + depthToSingleAtmosphere( saltWater ) ) ) - depthToSingleAtmosphere( saltWater );
    }
    else if ( !fn2.has_value() )
    {
        fn2 = ( percentN2AtSurface() * ( ead.value() + depthToSingleAtmosphere( saltWater ) ) ) / ( depth.value() + depthToSingleAtmosphere( saltWater ) );
    }
    else if ( !depth.has_value() )
    {
        depth = ( ( ead.value() + depthToSingleAtmosphere( saltWater ) ) / ( fn2.value() / percentN2AtSurface() ) ) - depthToSingleAtmosphere( saltWater );
    }
    return std::vector< std::optional< double > >( { ead, fn2, depth } );
}
