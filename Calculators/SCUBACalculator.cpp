#include "SCUBACalculator.h"
#include "SCUBACalculator.h"
#include "SABUtils/WidgetChanged.h"
#include <QLineEdit>

CSCUBACalculator::CSCUBACalculator()
{
}

CSCUBACalculator::~CSCUBACalculator()
{
}

CSCUBACalculatorPage *CSCUBACalculator::getPage() const
{
    Q_ASSERT( fPage );
    return fPage;
}

CSCUBACalculatorPage *CSCUBACalculator::getPage( QWidget *parent )
{
    if ( !fPage )
    {
        fPage = constructPage( parent );
    }
    return fPage;
}

void CSCUBACalculator::init( bool imperial, bool saltWater )
{
    if ( fPage )
        fPage->init( imperial, saltWater );
}

void CSCUBACalculator::setImperial( bool imperial )
{
    if ( fPage )
        fPage->setImperial( imperial );
}

void CSCUBACalculator::setSaltWater( bool saltWater )
{
    if ( fPage )
        fPage->setSaltWater( saltWater );
}

void CSCUBACalculator::setUpdateFormulaFunc( const TUpdateFormulaFunc &func )
{
    fUpdateFormulaFunc = func;
}

bool CSCUBACalculator::imperial() const
{
    if ( fPage )
        return fPage->imperial();
    return false;
}

bool CSCUBACalculator::saltWater() const
{
    if ( fPage )
        return fPage->saltWater();
    return false;
}

double CSCUBACalculator::weightOfWater( bool saltWater ) const
{
    auto retVal = imperial() ? ( saltWater ? 62.4 : 64.0 ) : ( saltWater ? 1.0 : 1.03 );
    return retVal;
}

double CSCUBACalculator::depthToSingleAtmosphere( bool saltWater ) const
{
    auto retVal = imperial() ? ( saltWater ? 33.0 : 34.0 ) : ( saltWater ? 10.0 : 10.3 );
    return retVal;
}

double CSCUBACalculator::absZero() const
{
    return imperial() ? 460.0 : 273.0;
}

double CSCUBACalculator::pressureOffset() const
{
    return imperial() ? 14.7 : 1.0;
}

bool CSCUBACalculator::valuesValid( const TOptionalVariantVector &values, bool checkNumEmpty ) const
{
    if ( values.empty() )
        return false;

    if ( checkNumEmpty && numEmpty( values ) != 1 )
        return false;

    std::size_t start = 0;
    for ( auto &&ii : values )
    {
        if ( ii.has_value() && !std::holds_alternative< double >( ii.value() ) )
            return false;
    }
    return true;
}

void CSCUBACalculator::calculateDepthToFromPressure( bool saltWater, TOptionalVariant &pressure, TOptionalVariant &depth ) const
{
    if ( !pressure.has_value() && !depth.has_value() )
        return;

    auto depthOfATM = depthToSingleAtmosphere( saltWater );
    if ( !depth.has_value() && std::holds_alternative< double >( pressure.value() ) )
    {
        depth = ( std::get< double >( pressure.value() ) - 1 ) * depthOfATM;
    }
    else if ( !pressure.has_value() && std::holds_alternative< double >( depth.value() ) )
    {
        pressure = ( std::get< double >( depth.value() ) / depthOfATM ) + 1;
    }
}

void CSCUBACalculator::updateFormula( const QString &eq ) const
{
    if ( fUpdateFormulaFunc )
        fUpdateFormulaFunc( getPage(), eq );
}

void CSCUBACalculator::updateFormula( QString &formula, const QString &token, const TOptionalVariant &value, const QString &label, const QString &unit ) const
{
    QString newString;
    if ( value.has_value() )
        newString = QString( "%1%2" ).arg( doubleToString( value.value(), 2 ) );
    else
        newString = QString( "%1 (%2)" ).arg( label );
    newString = newString.arg( unit );
    formula = formula.replace( token, newString );
}

std::size_t CSCUBACalculator::numEmpty( const TOptionalVariantVector &values ) const
{
    std::size_t numEmpty = 0;

    for ( auto &&value : values )
    {
        numEmpty += value.has_value() ? 0 : 1;
    }
    return numEmpty;
}

double CSCUBACalculator::absZeroBasedTemp( double temp ) const
{
    return temp + absZero();
}

double CSCUBACalculator::fromAbsZeroBasedTemp( double temp ) const
{
    return temp - absZero();
}

double CSCUBACalculator::idealGasConstant() const
{
    if ( imperial() )
    {
        return 10.731577089016;
    }
    else
    {
        return 0.08206;
    }
}

double CSCUBACalculator::pressurePerTemp() const
{
    return imperial() ? 5 : 0.6;
}

double CSCUBACalculator::percentN2AtSurface() const
{
    return 0.79;
}

double CSCUBACalculator::percentO2AtSurface() const
{
    return 0.21;
}

CSCUBACalculatorPage::CSCUBACalculatorPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    QWidget( parent ),
    fCalculator( calculator )
{
    connect( this, &CSCUBACalculatorPage::sigUpdateValues, [ = ]() { updateValues( nullptr ); } );
}

CSCUBACalculatorPage::~CSCUBACalculatorPage()
{
}

void CSCUBACalculatorPage::init( bool imperial, bool saltWater )
{
    fImperial = imperial;
    fSaltWater = saltWater;
    updateValues( nullptr );
}

void CSCUBACalculatorPage::setImperial( bool imperial )
{
    fImperial = imperial;
    updateValues( nullptr );
}

void CSCUBACalculatorPage::setSaltWater( bool saltWater )
{
    fSaltWater = saltWater;
    emit sigUpdateValues();
}

void CSCUBACalculatorPage::addWidgets( bool rhs, const std::list< QWidget * > &widgets )
{
    for ( auto &&ii : widgets )
    {
        addWidget( rhs, ii );
    }
}

void CSCUBACalculatorPage::updateValues( QWidget *widget )
{
    fNeedsInit = false;
    updateValuesInternal( widget );
}

void CSCUBACalculatorPage::addWidget( bool rhs, QWidget *widget )
{
    fWidgets[ widget ] = rhs;

    NSABUtils::setupWidgetChanged(
        widget,
        [ = ]( QObject * )
        {
            auto pos = fWidgets.find( widget );
            if ( pos == fWidgets.end() )
                return;
            setUpdateFromRHS( ( *pos ).second );
            updateValues( widget );
        } );
}

TOptionalVariant CSCUBACalculatorPage::getValue( const QString &text ) const
{
    if ( text.trimmed().isEmpty() )
        return {};
    bool aOK = false;
    auto retVal = text.trimmed().toDouble( &aOK );
    if ( !aOK )
        return {};
    return retVal;
}

QString doubleToString( const TOptionalVariant &value, int numDecimal )
{
    QString retVal;
    if ( value.has_value() && std::holds_alternative< double >( value.value() ) )
        retVal = QString( "%1" ).arg( std::get< double >( value.value() ), 0, 'f', numDecimal );
    return retVal;
}

void CSCUBACalculatorPage::setValue( QLineEdit *le, const TOptionalVariant &origValue, const TOptionalVariant &newValue, int numDecimal /*= 1 */ )
{
    if ( !le || !newValue.has_value() || !std::holds_alternative< double >( newValue.value() ) )
        return;

    auto newValueString = doubleToString( newValue, numDecimal );
    if ( doubleToString( origValue, numDecimal ) == newValueString )
        return;

    le->blockSignals( true );
    le->setText( newValueString );
    le->blockSignals( false );
}

QString CSCUBACalculator::lengthUnit( bool singular, bool /*tex*/ ) const
{
    QString retVal;
    if ( imperial() )
    {
        if ( singular )
            return QObject::tr( "ft" );
        else
            return QObject::tr( "feet" );
    }
    else
    {
        if ( singular )
            return QObject::tr( "meter" );
        else
            return QObject::tr( "meters" );
    }
}

QString CSCUBACalculatorPage::lengthUnit( bool singular, bool tex ) const
{
    return calculator()->lengthUnit( singular, tex );
}

QString CSCUBACalculator::volumeUnit( bool singular, bool tex ) const
{
    QString retVal;
    if ( imperial() )
    {
        return tex ? QObject::tr( "ft^3" ) : QObject::tr( "cu ft" );
    }
    else
    {
        if ( singular )
            return QObject::tr( "l" );
        else
            return QObject::tr( "l" );
    }
}

QString CSCUBACalculatorPage::volumeUnit( bool singular, bool tex ) const
{
    return calculator()->volumeUnit( singular, tex );
}

QString CSCUBACalculator::weightUnit( bool singular, bool /*tex*/ ) const
{
    QString retVal;
    if ( imperial() )
    {
        if ( singular )
            return QObject::tr( "lb" );
        else
            return QObject::tr( "lbs" );
    }
    else
    {
        if ( singular )
            return QObject::tr( "kg" );
        else
            return QObject::tr( "kgs" );
    }
}

QString CSCUBACalculatorPage::weightUnit( bool singular, bool tex ) const
{
    return calculator()->weightUnit( singular, tex );
}

QString CSCUBACalculator::pressureUnit( bool /*tex*/ ) const
{
    return imperial() ? QObject::tr( "ATM" ) : QObject::tr( "BAR" );
}

QString CSCUBACalculatorPage::pressureUnit( bool tex ) const
{
    return calculator()->pressureUnit( tex );
}

QString CSCUBACalculator::pressurePerTemp( bool tex ) const
{
    auto retVal = tex ? QObject::tr( "%1 (\frac{%2}{%3}" ) : QObject::tr( "%1 (%2/%3)" );
    retVal = retVal.arg( pressurePerTemp() ).arg( pressureUnit( tex ) ).arg( tempUnit( false, tex ) );
    return retVal;
}

QString CSCUBACalculatorPage::pressurePerTemp( bool tex ) const
{
    return calculator()->pressurePerTemp( tex );
}

QString CSCUBACalculator::tempUnit( bool absZero, bool tex ) const
{
    QString retVal = tex ? QObject::tr( R"(^{\circ})" ) : QObject::tr( "\u00B0" );
    if ( imperial() )
    {
        if ( absZero )
            retVal += QObject::tr( "R" );
        else
            retVal += QObject::tr( "F" );
    }
    else
    {
        if ( absZero )
            retVal += QObject::tr( "K" );
        else
            retVal += QObject::tr( "C" );
    }
    return retVal;
}

QString CSCUBACalculatorPage::tempUnit( bool absZero, bool tex ) const
{
    return calculator()->tempUnit( absZero, tex );
}

QString CSCUBACalculator::weightOfWater( bool saltWater, bool tex ) const
{
    auto retVal = tex ? QObject::tr( R"(%1 \frac{%2}{%3})" ) : QObject::tr( "%1 %2/%3 of water" );
    auto weightOfWater = doubleToString( this->weightOfWater( saltWater ), 2 );
    retVal = retVal.arg( weightOfWater ).arg( weightUnit( false, tex ) ).arg( volumeUnit( true, tex ) );
    return retVal;
}

QString CSCUBACalculatorPage::weightOfWater( bool saltWater, bool tex ) const
{
    return calculator()->weightOfWater( saltWater, tex );
}

QString CSCUBACalculator::idealGasConstant( bool tex ) const
{
    if ( tex )
        return QObject::tr( R"(__p(%1)\timesV(%2)=n(moles)\times%3\timesT(%4)__)" ).arg( pressureUnit( tex ) ).arg( volumeUnit( true, tex ) ).arg( idealGasConstant() ).arg( tempUnit( true, tex ) );
    else
        return QObject::tr( "%1 (%2)x(%3)/(n moles)x(%4)" ).arg( idealGasConstant() ).arg( volumeUnit( true, tex ) ).arg( pressureUnit( tex ) ).arg( tempUnit( true, tex ) );
}

QString CSCUBACalculatorPage::idealGasConstant( bool tex ) const
{
    return calculator()->idealGasConstant( tex );
}

bool CSCUBACalculatorPage::isWaterTypeBased() const
{
    return fCalculator->isWaterTypeBased();
}

bool CSCUBACalculatorPage::needsInit() const
{
    return fNeedsInit;
}

void CSCUBACalculatorPage::setUpdateFromRHS( bool updateFromRHS )
{
    fUpdateFromRHS = updateFromRHS;
}

void CSCUBACalculatorPage::slotWidgetChanged( QWidget *widget )
{
    auto pos = fWidgets.find( widget );
    if ( pos == fWidgets.end() )
        return;
    setUpdateFromRHS( ( *pos ).second );
    updateValues( widget );
}

extern "C" CSCUBACalculatorPage *getPage( CSCUBACalculator *calculator, QWidget *parentWidget, bool *needsInit )
{
    if ( needsInit )
        *needsInit = false;
    if ( !calculator )
        return nullptr;
    auto retVal = calculator->getPage( parentWidget );
    if ( needsInit )
        *needsInit = retVal->needsInit();
    return retVal;
}

extern "C" void setImperial( CSCUBACalculator *calculator, bool imperial )
{
    if ( !calculator )
        return;
    calculator->setImperial( imperial );
}

extern "C" void setSaltWater( CSCUBACalculator *calculator, bool saltWater )
{
    if ( !calculator )
        return;
    calculator->setSaltWater( saltWater );
}

extern "C" CALCULATORS_EXPORT void setUpdateFormulaFunc( CSCUBACalculator *calculator, const TUpdateFormulaFunc &func )
{
    if ( !calculator )
        return;
    calculator->setUpdateFormulaFunc( func );
}

extern "C" CALCULATORS_EXPORT void initCalculatorPage( CSCUBACalculator *calculator, bool imperial, bool saltWater )
{
    if ( !calculator )
        return;
    calculator->init( imperial, saltWater );
}
