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
    virtual bool compute( std::vector< std::optional< double > > &values ) const override;
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

bool CCalculator::compute( std::vector< std::optional< double > > &values ) const
{
    if ( numEmptyOK( values ) != 1 )
        return false;

    auto &&saltwater = values[ 0 ];
    auto &&volumeDisplaced = values[ 3 ];
    auto &&negativeBuoyancy = values[ 1 ];

    if ( !volumeDisplaced.has_value() )
    {
        volumeDisplaced = negativeBuoyancy.value() / weightOfWater( saltwater.value() != 0 );
    }
    else if ( !negativeBuoyancy.has_value() )
    {
        negativeBuoyancy = volumeDisplaced.value() * weightOfWater( saltwater.value() != 0 );
    }
    return true;
}
