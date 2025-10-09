#include "SCUBACalculator.h"
#include "SCUBACalculatorPage.h"
#include "VariableInfo.h"
#include <QFrame>
#include <QSvgWidget>

#include <iterator>
#include <tuple>
#include <list>
#include <memory>
#include <unordered_set>

CSCUBACalculator::CSCUBACalculator( QObject *parent ) :
    QObject( parent )
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
        std::tie( fPage, fSvgFrame, fSvgWidget, fNumVariables ) = CSCUBACalculatorPage::constructPage( this, parent );
    }
    return fPage;
}

void CSCUBACalculator::init( bool imperial, bool seaWater )
{
    setObjectName( calculatorName() );
    if ( fPage )
        fPage->init( imperial, seaWater );
}

void CSCUBACalculator::setImperial( bool imperial )
{
    if ( fPage )
        fPage->setImperial( imperial );
}

void CSCUBACalculator::setSeaWater( bool seaWater )
{
    if ( fPage )
        fPage->setSeaWater( seaWater );
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

bool CSCUBACalculator::seaWater() const
{
    if ( fPage )
        return fPage->seaWater();
    return false;
}

void CSCUBACalculator::notifyOfNewFormula( const QString &eq, bool baseFormula ) const
{
    if ( fUpdateFormulaFunc )
        fUpdateFormulaFunc( getPage(), eq, baseFormula );
}

std::list< std::shared_ptr< SVariableInfo > > &CSCUBACalculator::getVariables()
{
    if ( fVariables.empty() )
    {
        initVariables();
    }
    return fVariables;
}

const std::list< std::shared_ptr< SVariableInfo > > &CSCUBACalculator::getVariables() const
{
    return fVariables;
}

std::list< std::shared_ptr< SVariableInfo > > &CSCUBACalculator::getGlobalVariables()
{
    if ( fVariables.empty() )
    {
        initVariables();
    }
    return fGlobalVariables;
}

const std::list< std::shared_ptr< SVariableInfo > > &CSCUBACalculator::getGlobalVariables() const
{
    return fGlobalVariables;
}

std::list< std::shared_ptr< SVariableInfo > > &CSCUBACalculator::getRHSVariables()
{
    if ( fVariables.empty() )
    {
        initVariables();
    }
    return fRHSVariables;
}

const std::list< std::shared_ptr< SVariableInfo > > &CSCUBACalculator::getRHSVariables() const
{
    return fRHSVariables;
}

std::list< std::shared_ptr< SVariableInfo > > &CSCUBACalculator::getLHSVariables()
{
    if ( fVariables.empty() )
    {
        initVariables();
    }
    return fLHSVariables;
}

const std::list< std::shared_ptr< SVariableInfo > > &CSCUBACalculator::getLHSVariables() const
{
    return fLHSVariables;
}

void CSCUBACalculator::initVariables()
{
    fLHSVariables.clear();
    fRHSVariables.clear();
    fGlobalVariables.clear();
    fVariableMap.clear();

    fVariables = getMyVariables();
    for ( auto &&curr : fVariables )
    {
        fVariableMap[ curr->name() ] = curr;
        if ( curr->variableLoc() == EVariableLoc::eLHS )
            fLHSVariables.push_back( curr );
        else if ( curr->variableLoc() == EVariableLoc::eRHS )
            fRHSVariables.push_back( curr );
        else if ( curr->variableLoc() == EVariableLoc::eGlobal )
            fGlobalVariables.push_back( curr );
    }

    Q_ASSERT( ( fLHSVariables.empty() && fRHSVariables.empty() ) || ( !fLHSVariables.empty() && !fRHSVariables.empty() ) );
}

void CSCUBACalculator::resetVariables()
{
    for ( auto &&ii = fVariables.begin(); ii != fVariables.end(); ++ii )
    {
        ( *ii )->resetValue( true, false );
    }

    auto formula = finalizeFormula( getDefaultFormula(), true );
    updateFields( nullptr );
    notifyOfNewFormula( formula, false );
}

std::size_t CSCUBACalculator::numUnsetVariables() const
{
    std::size_t retVal = 0;
    for ( auto &&ii : fVariables )
    {
        if ( !ii->isVariable() )
            continue;
        if ( !ii->has_value() )
            retVal++;
    }
    return retVal;
}

std::shared_ptr< SVariableInfo > CSCUBACalculator::getVariable( const QString &varName ) const
{
    auto pos = fVariableMap.find( varName );
    if ( pos == fVariableMap.end() )
        return {};
    return ( *pos ).second;
}

void CSCUBACalculator::renderDefaultFormulas() const
{
    auto baseFormula = getDefaultFormula();
    std::unordered_set< QString > defaultFormulas;

    for ( auto imperial : { true, false } )
    {
        for ( auto seaWater : { true, false } )
        {
            auto formula = finalizeFormula( imperial, seaWater, baseFormula, true );
            defaultFormulas.insert( formula );
        }
    }
    Q_ASSERT( defaultFormulas.size() <= 4 );
    for ( auto &&formula : defaultFormulas )
    {
        notifyOfNewFormula( formula, true );
    }
}

TVariableInfo CSCUBACalculator::customDetermineVariableToUnset( EVariableLoc /*updateFromSide*/, QWidget * /*triggerWidget*/, bool /*preDefaultBehavior*/ )
{
    return {};
}

void CSCUBACalculator::determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget )
{
    if ( numUnsetVariables() != 0 )
        return;

    if ( fLHSVariables.empty() || fRHSVariables.empty() )
        return;

    auto varToReset = customDetermineVariableToUnset( updateFromSide, triggerWidget, true );

    if ( !varToReset && ( updateFromSide == EVariableLoc::eGlobal ) )
    {
        varToReset = {};
    }
    else if ( !varToReset && ( updateFromSide == EVariableLoc::eRHS ) )
    {
        if ( ( fLHSVariables.size() == 1 ) || ( fRHSVariables.size() == 1 ) )
        {
            varToReset = fLHSVariables.front();
        }
        else if ( fRHSVariables.size() == 2 )
        {
            Q_ASSERT( ( fRHSVariables.front()->isWidget( triggerWidget ) ) || ( fRHSVariables.back()->isWidget( triggerWidget ) ) );
            if ( fRHSVariables.front()->isWidget( triggerWidget ) )
                varToReset = fRHSVariables.back();
            else if ( fRHSVariables.back()->isWidget( triggerWidget ) )
                varToReset = fRHSVariables.front();
        }
    }
    else if ( !varToReset && ( updateFromSide == EVariableLoc::eLHS ) )
    {
        if ( ( fRHSVariables.size() == 1 ) || ( fLHSVariables.size() == 1 ) )
        {
            varToReset = fRHSVariables.front();
        }
        else if ( fLHSVariables.size() == 2 )
        {
            Q_ASSERT( ( fLHSVariables.front()->isWidget( triggerWidget ) ) || ( fLHSVariables.back()->isWidget( triggerWidget ) ) );
            if ( fLHSVariables.front()->isWidget( triggerWidget ) )
                varToReset = fLHSVariables.back();
            else if ( fLHSVariables.back()->isWidget( triggerWidget ) )
                varToReset = fLHSVariables.front();
        }
    }

    if ( !varToReset )
        varToReset = customDetermineVariableToUnset( updateFromSide, triggerWidget, false );

    if ( varToReset )
    {
        varToReset->resetValue( false, false );
        return;
    }
}

void CSCUBACalculator::compute( EVariableLoc updateFromSide, QWidget *triggerWidget )
{
    auto &&variables = getVariables();
    for ( auto &&curr : variables )
    {
        curr->updateLabels( imperial(), seaWater() );
        curr->updateValueFromField();
    }

    determineVariableToUnset( updateFromSide, triggerWidget );

    bool isBaseFormula = false;
    auto formula = computeAndGenerateFormula( isBaseFormula );

    formula = finalizeFormula( formula, isBaseFormula );
    updateFields( triggerWidget );
    notifyOfNewFormula( formula, isBaseFormula );

    formula = finalizeFormula( getDefaultFormula(), true );
    notifyOfNewFormula( formula, true );
}

void CSCUBACalculator::updateFields( QWidget *triggerWidget ) const
{
    auto &&variables = getVariables();

    for ( auto &&curr : variables )
    {
        if ( !curr->isWidget( triggerWidget ) || !curr->currFieldValue().has_value() )
            curr->updateFieldFromValue();
    }
}

QString CSCUBACalculator::finalizeFormula( bool imperial, bool seaWater, const QString &formula, bool isBaseFormula ) const
{
    auto &&variables = getVariables();

    QString retVal = formula;
    for ( auto &&curr : variables )
    {
        curr->updateFormula( imperial, seaWater, retVal, isBaseFormula );
    }

    retVal = retVal.replace( " ", R"(\ )" );
    return retVal;
}

QString CSCUBACalculator::finalizeFormula( const QString &formula, bool isBaseFormula ) const
{
    return finalizeFormula( imperial(), seaWater(), formula, isBaseFormula );
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

extern "C" void setSeaWater( CSCUBACalculator *calculator, bool seaWater )
{
    if ( !calculator )
        return;
    calculator->setSeaWater( seaWater );
}

extern "C" CALCULATORS_EXPORT void setUpdateFormulaFunc( CSCUBACalculator *calculator, const TUpdateFormulaFunc &func )
{
    if ( !calculator )
        return;
    calculator->setUpdateFormulaFunc( func );
}

extern "C" CALCULATORS_EXPORT void initCalculatorPage( CSCUBACalculator *calculator, bool imperial, bool seaWater )
{
    if ( !calculator )
        return;
    calculator->init( imperial, seaWater );
}
