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
    return "Calculating Partial Pressure for Given Depth";
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
    if ( !values[ 0 ].has_value() )
        return {};

    bool depthForPressureOK = ( values[ 0 ].has_value() && ( values[ 1 ].has_value() || values[ 2 ].has_value() ) );
    bool po2OK = ( values[ 3 ].has_value() && values[ 4 ].has_value() );

    if ( !numEmptyOK( values ) && !po2OK && !depthForPressureOK )
        return {};

    auto saltWater = values[ 0 ].value() != 0.0;
    auto pressureForDepth = values[ 1 ];
    auto depthForPressure = values[ 2 ];
    auto partialPressure = values[ 3 ];
    auto surfacePressure = values[ 4 ];

    if ( po2OK && ( !pressureForDepth.has_value() && !depthForPressure.has_value() ) )
    {
        pressureForDepth = partialPressure.value() / surfacePressure.value();
        depthForPressureOK = true;
    }

    if ( depthForPressureOK && ( !pressureForDepth.has_value() || !depthForPressure.has_value() ) )
    {
        calculateDepthToFromPressure( saltWater, pressureForDepth, depthForPressure );
    }

    if ( numEmptyOK( { pressureForDepth, partialPressure, surfacePressure } ) )
    {
        if ( !partialPressure.has_value() )
        {
            partialPressure = pressureForDepth.value() * surfacePressure.value();
        }
        else if ( !surfacePressure.has_value() )
        {
            surfacePressure = partialPressure.value() / partialPressure.value();
        }
    }
    return std::vector< std::optional< double > >( { pressureForDepth, depthForPressure, partialPressure, surfacePressure } );
}
