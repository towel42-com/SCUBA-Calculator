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
    return "Calculating Relative Change in Ambient Pressure";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure and Volume Conversions" };
}

CSCUBACalculatorPage *CCalculator::constructPage( QWidget *parent ) const
{
    return new CPage( this, parent );
}

std::optional< std::vector< std::optional< double > > > CCalculator::compute( const std::vector< std::optional< double > > &values ) const
{
    if ( !numEmptyOK( values ) )
        return {};

    auto relChange = values[ 0 ];
    auto p1 = values[ 1 ];
    auto p2 = values[ 2 ];

    if ( !relChange.has_value() )
    {
        relChange = p2.value() / p1.value();
    }
    else if ( !p2.has_value() )
    {
        p2 = relChange.value() * p1.value();
    }
    else if ( !p1.has_value() )
    {
        p1 = p2.value() / relChange.value();
    }
    return std::vector< std::optional< double > >( { relChange, p1, p2 } );
}
