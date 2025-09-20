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

CSCUBACalculatorPage *CSCUBACalculator::getPage( QWidget *parent ) const
{
    if ( !fPage )
        fPage = constructPage( parent );
    return fPage;
}

void CSCUBACalculator::setImperial( bool imperial )
{
    if ( fPage )
        fPage->setImperial( imperial );
}

void CSCUBACalculator::setMetric( bool metric )
{
    if ( fPage )
        fPage->setMetric( metric );
}

bool CSCUBACalculator::imperial() const
{
    if ( fPage )
        return fPage->imperial();
    return false;
}

bool CSCUBACalculator::metric() const
{
    if ( fPage )
        return fPage->metric();
    return false;
}

double CSCUBACalculator::weightOfWater( bool saltWater ) const
{
    auto retVal = imperial() ? ( saltWater ? 64.0 : 62.4 ) : ( saltWater ? 1.0 : 1.03 );
    return retVal;
}

double CSCUBACalculator::lengthToSingleAtmosphere( bool saltWater ) const
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

bool CSCUBACalculator::numEmptyOK( const std::vector< std::optional< double > > &values ) const
{
    return numEmpty( values ) == 1;
}

std::size_t CSCUBACalculator::numEmpty( const std::vector< std::optional< double > > &values ) const
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

CSCUBACalculatorPage::CSCUBACalculatorPage( const CSCUBACalculator *calculator, QWidget *parent ) :
    QWidget( parent ),
    fCalculator( calculator )
{
    connect( this, &CSCUBACalculatorPage::sigUnitsChanged, [ = ]() { updateValues( nullptr ); } );
}

CSCUBACalculatorPage::~CSCUBACalculatorPage()
{
}

void CSCUBACalculatorPage::setImperial( bool imperial )
{
    fImperial = imperial;
    emit sigUnitsChanged();
}

void CSCUBACalculatorPage::setMetric( bool metric )
{
    fImperial = !metric;
    emit sigUnitsChanged();
}

void CSCUBACalculatorPage::addWidgets( bool rhs, const std::list< QWidget * > &widgets )
{
    for ( auto &&ii : widgets )
    {
        addWidget( rhs, ii );
    }
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

std::optional< double > CSCUBACalculatorPage::getValue( const QString &text ) const
{
    if ( text.isEmpty() )
        return {};
    bool aOK = false;
    auto retVal = text.toDouble( &aOK );
    if ( !aOK )
        return {};
    return retVal;
}

QString CSCUBACalculatorPage::doubleToString( const std::optional< double > &value, int numDecimal ) const
{
    QString retVal;
    if ( value.has_value() )
        retVal = QString( "%1" ).arg( value.value(), 0, 'f', numDecimal );
    return retVal;
}

void CSCUBACalculatorPage::setValue( QLineEdit *le, const std::optional< double > &value, int numDecimal )
{
    if ( !le || !value.has_value() )
        return;
    le->blockSignals( true );
    le->setText( doubleToString( value, numDecimal ) );
    le->blockSignals( false );
}

QString CSCUBACalculatorPage::lengthUnit( bool singular ) const
{
    QString retVal;
    if ( imperial() )
    {
        if ( singular )
            return tr( "ft" );
        else
            return tr( "feet" );
    }
    else
    {
        if ( singular )
            return tr( "meter" );
        else
            return tr( "meters" );
    }
}

QString CSCUBACalculatorPage::volumeUnit( bool singular ) const
{
    QString retVal;
    if ( imperial() )
    {
        return tr( "cu ft" );
    }
    else
    {
        if ( singular )
            return tr( "liter" );
        else
            return tr( "liters" );
    }
}

QString CSCUBACalculatorPage::weightUnit( bool singular ) const
{
    QString retVal;
    if ( imperial() )
    {
        if ( singular )
            return tr( "lb" );
        else
            return tr( "lbs" );
    }
    else
    {
        if ( singular )
            return tr( "kg" );
        else
            return tr( "kgs" );
    }
}

QString CSCUBACalculatorPage::pressureUnit() const
{
    return imperial() ? tr( "ATM" ) : tr( "BAR" );
}

QString CSCUBACalculatorPage::pressurePerTemp() const
{
    return tr( "%1 (%2/%3)" ).arg( calculator()->pressurePerTemp() ).arg( pressureUnit() ).arg( tempUnit( false ) );
}

QString CSCUBACalculatorPage::tempUnit( bool absZero ) const
{
    QString retVal = "\u00B0";
    if ( imperial() )
    {
        if ( absZero )
            retVal += tr( "R" );
        else
            retVal += tr( "F" );
    }
    else
    {
        if ( absZero )
            retVal += tr( "K" );
        else
            retVal += tr( "C" );
    }
    return retVal;
}

QString CSCUBACalculatorPage::weightOfWater( bool saltWater ) const
{
    auto retVal = tr( "(%1 %2/%3 of water)" ).arg( doubleToString( calculator()->weightOfWater( saltWater ), 1 ) ).arg( weightUnit( false ) ).arg( volumeUnit( true ) );
    return retVal;
}

QString CSCUBACalculatorPage::idealGasConstant() const
{
    return QString( "%1 (%2)x(%3)/(mol)x(%4)" ).arg( calculator()->idealGasConstant() ).arg( volumeUnit( true ) ).arg( pressureUnit() ).arg( tempUnit( true ) );
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

extern "C" CSCUBACalculatorPage *getPage( CSCUBACalculator *calculator, QWidget *parentWidget )
{
    if ( !calculator )
        return nullptr;
    return calculator->getPage( parentWidget );
}

extern "C" void setImperial( CSCUBACalculator *calculator, bool imperial )
{
    if ( !calculator )
        return;
    calculator->setImperial( imperial );
}

extern "C" void setMetric( CSCUBACalculator *calculator, bool metric )
{
    if ( !calculator )
        return;
    calculator->setMetric( metric );
}
