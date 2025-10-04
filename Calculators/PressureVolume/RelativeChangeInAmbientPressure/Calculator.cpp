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
    virtual std::optional< TOptionalVariantVector > compute( const TOptionalVariantVector &values ) const override;
    virtual std::optional< TOptionalVariantVector > setupValues( bool updateFromRHS, std::size_t triggerPos, const TOptionalVariantVector &values ) const override;
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

    auto relChange = values[ 0 ];
    auto p1 = values[ 1 ];
    auto p2 = values[ 2 ];

    if ( !relChange.has_value() )
    {
        relChange = std::get< double >( p2.value() ) / std::get< double >( p1.value() );
    }
    else if ( !p2.has_value() )
    {
        p2 = std::get< double >( relChange.value() ) * std::get< double >( p1.value() );
    }
    else if ( !p1.has_value() )
    {
        p1 = std::get< double >( p2.value() ) / std::get< double >( relChange.value() );
    }
    return TOptionalVariantVector( { relChange, p1, p2 } );
}
