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
    virtual bool isWaterTypeBased() const override { return true; }
    virtual std::optional< TOptionalVariantVector > compute( const TOptionalVariantVector &values ) const override;
    virtual std::optional< TOptionalVariantVector > setupValues( bool updateFromRHS, std::size_t triggerPos, const TOptionalVariantVector &values ) const override;
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

std::optional< TOptionalVariantVector > CCalculator::setupValues( bool updateFromRHS, std::size_t triggerPos, const TOptionalVariantVector &values ) const
{
    (void)updateFromRHS;
    (void)triggerPos;
    (void)values;
    return {};
}

std::optional< TOptionalVariantVector > CCalculator::compute( const TOptionalVariantVector &values ) const
{
    if ( !valuesValid( values ) )
        return {};

    auto saltWater = std::get< bool >( values[ 0 ].value() );
    auto ead = values[ 1 ];
    auto fn2 = values[ 2 ];
    auto depth = values[ 3 ];

    if ( !ead.has_value() )
    {
        ead = ( ( std::get< double >( fn2.value() ) / percentN2AtSurface() ) * ( std::get< double >( depth.value() ) + depthToSingleAtmosphere( saltWater ) ) ) - depthToSingleAtmosphere( saltWater );
    }
    else if ( !fn2.has_value() )
    {
        fn2 = ( percentN2AtSurface() * ( std::get< double >( ead.value() ) + depthToSingleAtmosphere( saltWater ) ) ) / ( std::get< double >( depth.value() ) + depthToSingleAtmosphere( saltWater ) );
    }
    else if ( !depth.has_value() )
    {
        depth = ( ( std::get< double >( ead.value() ) + depthToSingleAtmosphere( saltWater ) ) / ( std::get< double >( fn2.value() ) / percentN2AtSurface() ) ) - depthToSingleAtmosphere( saltWater );
    }
    return TOptionalVariantVector( { ead, fn2, depth } );
}
