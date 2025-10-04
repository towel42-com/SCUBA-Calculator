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
    virtual bool usesSaltWater() const override { return true; }
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
    if ( values.size() != 4 )
        return {};

    auto saltWater = std::get< bool >( values[ 0 ].value() );
    auto buoyancy = values[ 1 ];
    auto weightOfObject = values[ 2 ];
    auto volumeDisplaced = values[ 3 ];

    QString equation;
    bool aOK = true;
    if ( !valuesValid( values ) )
    {
        aOK = false;
        equation = QObject::tr( R"__(<buoyancy>%1 = <weightOfObject>%1 - [<volumeDisplaced>%2 \times %3])__" );
    }
    else if ( !weightOfObject.has_value() )
    {
        weightOfObject = std::get< double >( buoyancy.value() ) + ( std::get< double >( volumeDisplaced.value() ) * weightOfWater( saltWater ) );
        equation = QObject::tr( R"__(<weightOfObject>%1=<buoyancy>%1 + <volumeDisplaced>%2 \times %3)__" );
    }
    else if ( !volumeDisplaced.has_value() )
    {
        volumeDisplaced = ( std::get< double >( weightOfObject.value() ) - std::get< double >( buoyancy.value() ) ) / weightOfWater( saltWater );
        equation = QObject::tr( R"__(<volumeDisplaced>%2=\frac{(<weightOfObject>%1 - <buoyancy>%1)}{%3})__" );
    }
    else if ( !buoyancy.has_value() )
    {
        buoyancy = std::get< double >( weightOfObject.value() ) - ( std::get< double >( volumeDisplaced.value() ) * weightOfWater( saltWater ) );
        equation = QObject::tr( R"__(<buoyancy>%1=(<weightOfObject>%1-(<volumeDisplaced>%2\times{%3}))__" );
    }

    equation = equation.arg( weightUnit( false, true ) ).arg( volumeUnit( false, true ) ).arg( weightOfWater( saltWater, true ) );

    if ( buoyancy.has_value() )
        equation = equation.replace( QObject::tr( "<buoyancy>" ), doubleToString( buoyancy.value(), 2 ) );
    else
        equation = equation.replace( QObject::tr( "<buoyancy>" ), QObject::tr( "Buoyancy" ) + " " );

    if ( weightOfObject.has_value() )
        equation = equation.replace( QObject::tr( "<weightOfObject>" ), doubleToString( weightOfObject.value(), 2 ) );
    else
        equation = equation.replace( QObject::tr( "<weightOfObject>" ), QObject::tr( "Weight of Object" ) + " " );

    if ( volumeDisplaced.has_value() )
        equation = equation.replace( QObject::tr( "<volumeDisplaced>" ), doubleToString( volumeDisplaced.value(), 2 ) );
    else
        equation = equation.replace( QObject::tr( "<volumeDisplaced>" ), QObject::tr( "Volume Displaced" ) + " " );

    std::optional< TOptionalVariantVector > retVal;
    if ( aOK )
        retVal = { buoyancy, weightOfObject, volumeDisplaced };
    updateEquation( equation );

    return retVal;
}
