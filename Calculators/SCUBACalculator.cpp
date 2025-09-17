#include "SCUBACalculator.h"
#include "SABUtils/WidgetChanged.h"

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

CSCUBACalculatorPage::CSCUBACalculatorPage( QWidget *parent ) :
    QWidget( parent )
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
