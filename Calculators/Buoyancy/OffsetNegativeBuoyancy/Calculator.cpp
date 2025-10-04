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
    virtual bool isWaterTypeBased() const { return true; }
    virtual std::optional< TOptionalVariantVector > compute( const TOptionalVariantVector &values ) const override;
    virtual std::optional< TOptionalVariantVector > setupValues( bool updateFromRHS, std::size_t triggerPos, const TOptionalVariantVector &values ) const override;
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

std::optional< TOptionalVariantVector > CCalculator::setupValues( bool updateFromRHS, std::size_t /*triggerPos*/, const TOptionalVariantVector &values ) const
{
    if ( values.size() != 2 )
        return {};

    auto retVal = values;

    auto &&volumeDisplaced = retVal[ 0 ];
    auto &&negativeBuoyancy = retVal[ 1 ];

    if ( numEmpty( values ) == 0 )
    {
        if ( updateFromRHS )
        {
            volumeDisplaced.reset();
        }
        else
        {
            negativeBuoyancy.reset();
        }
    }
    return retVal;
}

std::optional< TOptionalVariantVector > CCalculator::compute( const TOptionalVariantVector &values ) const
{
    if ( values.size() != 2 )
        return {};

    auto volumeDisplaced = values[ 0 ];
    auto negativeBuoyancy = values[ 1 ];

    QString formula;
    bool aOK = valuesValid( values );
    if ( !aOK || !volumeDisplaced.has_value() )
    {
        if ( aOK )
            volumeDisplaced = std::get< double >( negativeBuoyancy.value() ) / weightOfWater( saltWater() );
        formula = QObject::tr( R"__(<volumeDisplaced>=\frac{<negativeBuoyancy>}{<weightOfWater>})__" );
    }
    else if ( !negativeBuoyancy.has_value() )
    {
        formula = QObject::tr( R"__(<negativeBuoyancy>=<volumeDisplaced>\times<weightOfWater>)__" );
        negativeBuoyancy = std::get< double >( volumeDisplaced.value() ) * weightOfWater( saltWater() );
    }

    updateFormula( formula, "<volumeDisplaced>", volumeDisplaced, QObject::tr( "Volume Displaced" ), volumeUnit( false, true ) ); 
    updateFormula( formula, "<negativeBuoyancy>", negativeBuoyancy, QObject::tr( "Negative Buoyancy" ), weightUnit( false, true ) ); 
    formula = formula.replace( "<weightOfWater>", weightOfWater( saltWater(), true ) );
    formula = formula.replace( " ", R"(\ )" );

    std::optional< TOptionalVariantVector > retVal;
    if ( aOK )
        retVal = { volumeDisplaced, negativeBuoyancy };
    updateFormula( formula );

    return retVal;
}
