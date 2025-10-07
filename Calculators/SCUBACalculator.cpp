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

void CSCUBACalculator::notifyOfNewFormula( const QString &eq, bool baseFormula ) const
{
    if ( fUpdateFormulaFunc )
        fUpdateFormulaFunc( getPage(), eq, baseFormula );
}

std::list< std::shared_ptr< SVariableInfo > > &CSCUBACalculator::getVariables()
{
    if ( fVariables.empty() )
    {
        fVariableMap.clear();
        fVariables = getMyVariables();
        for ( auto &&curr : fVariables )
        {
            fVariableMap[ curr->fName ] = curr;
        }
    }
    return fVariables;
}

const std::list< std::shared_ptr< SVariableInfo > > &CSCUBACalculator::getVariables() const
{
    return fVariables;
}

void CSCUBACalculator::resetVariables()
{
    for ( auto &&ii = fVariables.begin(); ii != fVariables.end(); ++ii )
    {
        ( *ii )->resetValue( true, false );
    }

    auto formula = finalizeFormula( imperial(), saltWater(), getDefaultFormula(), true );
    notifyOfNewFormula( formula, false );
}

std::size_t CSCUBACalculator::numUnsetVariables() const
{
    std::size_t retVal = 0;
    for ( auto &&ii : fVariables )
    {
        if ( ii->fType != EVariableType::eVariable )
            continue;
        if ( !ii->fValue.has_value() )
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
        for ( auto saltWater : { true, false } )
        {
            auto formula = finalizeFormula( imperial, saltWater, baseFormula, false );
            defaultFormulas.insert( formula );
        }
    }
    Q_ASSERT( defaultFormulas.size() <= 4 );
    for ( auto &&formula : defaultFormulas )
    {
        notifyOfNewFormula( formula, true );
    }
}

void CSCUBACalculator::customDetermineVariableToUnset( ESide /*updateFromSide*/, QWidget * /*triggerWidget*/ )
{
}

void CSCUBACalculator::determineVariableToUnset( ESide updateFromSide, QWidget *triggerWidget )
{
    if ( numUnsetVariables() != 0 )
        return;

    auto &&[ lhsVars, rhsVars, globals ] = getVariableSides();
    getVariableSides();

    TVariableInfo varToReset;

    if ( lhsVars.empty() || rhsVars.empty() )
        return;

    if ( updateFromSide == ESide::eGlobal )
    {
        varToReset = {};
    }
    else if ( updateFromSide == ESide::eRHS )
    {
        if ( ( lhsVars.size() == 1 ) || ( rhsVars.size() == 1 ) )
        {
            varToReset = lhsVars.front();
        }
        else if ( rhsVars.size() == 2 )
        {
            Q_ASSERT( ( rhsVars.front()->fField == triggerWidget ) || ( rhsVars.back()->fField == triggerWidget ) );
            if ( rhsVars.front()->fField == triggerWidget )
                varToReset = rhsVars.back();
            else if ( rhsVars.back()->fField == triggerWidget )
                varToReset = rhsVars.front();
        }
    }
    else if ( updateFromSide == ESide::eLHS )
    {
        if ( ( rhsVars.size() == 1 ) || ( lhsVars.size() == 1 ) )
        {
            varToReset = rhsVars.front();
        }
        else if ( lhsVars.size() == 2 )
        {
            Q_ASSERT( ( lhsVars.front()->fField == triggerWidget ) || ( lhsVars.back()->fField == triggerWidget ) );
            if ( lhsVars.front()->fField == triggerWidget )
                varToReset = lhsVars.back();
            else if ( lhsVars.back()->fField == triggerWidget )
                varToReset = lhsVars.front();
        }
    }

    if ( varToReset )
    {
        varToReset->resetValue( false, false );
        return;
    }
    customDetermineVariableToUnset( updateFromSide, triggerWidget );
}

std::tuple< TVariableInfoList, TVariableInfoList, TVariableInfoList > CSCUBACalculator::getVariableSides() const
{
    auto &&variables = getVariables();
    std::list< TVariableInfo > lhs;
    std::list< TVariableInfo > rhs;
    std::list< TVariableInfo > global;

    TVariableInfo lhsVar{};
    std::pair< TVariableInfo, TVariableInfo > rhsVars;
    for ( auto &&curr : variables )
    {
        if ( !curr->isVariable() )
            continue;

        if ( curr->fVariableLocation == ESide::eRHS )
            rhs.push_back( curr );
        else if ( curr->fVariableLocation == ESide::eLHS )
            lhs.push_back( curr );
        else
            global.push_back( curr );
    }
    return { lhs, rhs, global };
}

void CSCUBACalculator::compute( ESide updateFromSide, QWidget *triggerWidget )
{
    auto &&variables = getVariables();
    for ( auto &&curr : variables )
    {
        curr->updateLabels( imperial(), saltWater() );
        curr->updateValueFromField();
    }

    determineVariableToUnset( updateFromSide, triggerWidget );

    auto formula = computeAndGenerateFormula();

    formula = finalizeFormula( imperial(), saltWater(), formula, false );
    notifyOfNewFormula( formula, false );

    formula = finalizeFormula( imperial(), saltWater(), getDefaultFormula(), true );
    notifyOfNewFormula( formula, true );
}

QString CSCUBACalculator::finalizeFormula( bool imperial, bool saltWater, const QString &formula, bool defaultFormula ) const
{
    auto &&variables = getVariables();

    QString retVal = formula;
    for ( auto &&curr : variables )
    {
        curr->updateFormula( imperial, saltWater, retVal, defaultFormula );
        if ( !defaultFormula )
            curr->updateFieldFromValue();
    }

    retVal = retVal.replace( " ", R"(\ )" );
    return retVal;
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
