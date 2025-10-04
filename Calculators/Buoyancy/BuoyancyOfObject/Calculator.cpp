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
    return "Buoyancy Characteristics of an Object";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Buoyancy Calculations" };
}

CSCUBACalculatorPage *CCalculator::constructPage( QWidget *parent ) const
{
    return new CPage( this, parent );
}

std::optional< TOptionalVariantVector > CCalculator::setupValues( bool updateFromRHS, std::size_t triggerPos, const TOptionalVariantVector &values ) const
{
    if ( values.size() != 3 )
        return {};

    auto retVal = values;

    auto &&buoyancy = retVal[ 0 ];
    auto &&weightOfObject = retVal[ 1 ];
    auto &&volumeDisplaced = retVal[ 2 ];

    if ( numEmpty( values ) == 0 )
    {
        if ( updateFromRHS )
            buoyancy.reset();
        else
        {
            if ( triggerPos == 2 )
                volumeDisplaced.reset();
            else
                buoyancy.reset();
        }
    }

    return retVal;
}

std::optional< TOptionalVariantVector > CCalculator::compute( const TOptionalVariantVector &values ) const
{
    if ( values.size() != 3 )
        return {};

    auto buoyancy = values[ 0 ];
    auto weightOfObject = values[ 1 ];
    auto volumeDisplaced = values[ 2 ];

    QString formula;
    bool aOK = valuesValid( values );
    if ( !aOK || !buoyancy.has_value() )
    {
        if ( aOK )
            buoyancy = std::get< double >( weightOfObject.value() ) - ( std::get< double >( volumeDisplaced.value() ) * weightOfWater( saltWater() ) );
        formula = QObject::tr( R"__(<buoyancy>=<weightOfObject> - [<volumeDisplaced> \times <weightOfWater>])__" );
    }
    else if ( !weightOfObject.has_value() )
    {
        weightOfObject = std::get< double >( buoyancy.value() ) + ( std::get< double >( volumeDisplaced.value() ) * weightOfWater( saltWater() ) );
        formula = QObject::tr( R"__(<weightOfObject>=<buoyancy> + <volumeDisplaced> \times <weightOfWater>)__" );
    }
    else if ( !volumeDisplaced.has_value() )
    {
        volumeDisplaced = ( std::get< double >( weightOfObject.value() ) - std::get< double >( buoyancy.value() ) ) / weightOfWater( saltWater() );
        formula = QObject::tr( R"__(<volumeDisplaced>=\frac{(<weightOfObject> - <buoyancy>)}{<weightOfWater>})__" );
    }

    updateFormula( formula, "<buoyancy>", buoyancy, QObject::tr( "Buoyancy" ), weightUnit( false, true ) );
    updateFormula( formula, "<weightOfObject>", weightOfObject, QObject::tr( "Weight of Object" ), weightUnit( false, true ) );
    updateFormula( formula, "<volumeDisplaced>", volumeDisplaced, QObject::tr( "Volume Displaced" ), volumeUnit( false, true ) );
    formula = formula.replace( "<weightOfWater>", weightOfWater( saltWater(), true ) );

    formula = formula.replace( " ", R"(\ )" );

    std::optional< TOptionalVariantVector > retVal;
    if ( aOK )
        retVal = { buoyancy, weightOfObject, volumeDisplaced };
    updateFormula( formula );

    return retVal;
}
