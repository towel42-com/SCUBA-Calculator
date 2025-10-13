#include "SCUBACalculator.h"
#include "SCUBACalculatorPage.h"
#include "VariableInfo.h"
#include <QFrame>

#include <iterator>
#include <tuple>
#include <list>
#include <memory>
#include <unordered_set>

// https://scuba.garykessler.net/EANcalculator/EAN_psi.html
// mixing O2 + Air for proper Nitrox

//https://allthingsdiving.com/dive-calculators/
// SAC
// END
// tank gas volume

//https://www.divebuddy.com/calculator/
// altitude
// calories burned
// Dive weight calculator
// tank air volume
// SAC

//https://www.deepbluescubanm.com/pages/tools.aspx
// calorie burn estimator
//

//https://swimmingcalculators.com/scuba-diving-calculator/
// Dive weight
// buoyancy
// sac
// calories burned

CSCUBACalculator::CSCUBACalculator( QObject *parent ) :
    QObject( parent )
{
}

CSCUBACalculator::~CSCUBACalculator()
{
}

QString CSCUBACalculator::myReversedCalculatorName() const
{
    return {};
}

QString CSCUBACalculator::myReversedBaseFormula() const
{
    return {};
}

QString CSCUBACalculator::getBaseFormula() const
{
    auto retVal = ( isReversed() ) ? myReversedBaseFormula() : myBaseFormula();
    return retVal;
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
        std::tie( fPage, fNumVariables ) = CSCUBACalculatorPage::constructPage( this, parent );
    }
    return fPage;
}

void CSCUBACalculator::init( bool imperial, bool seaWater )
{
    setObjectName( calculatorName() );
    if ( fPage )
        fPage->init( imperial, seaWater );
}

QString CSCUBACalculator::calculatorName() const
{
    auto retVal = ( isReversed() ) ? myReversedCalculatorName() : myCalculatorName();
    return retVal;
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

void CSCUBACalculator::notifyOfNewFormula( const QString &formula, EFormulaType formulaType ) const
{
    if ( fUpdateFormulaFunc )
        fUpdateFormulaFunc( getPage(), formula, formulaType );
}

TVariableInfoList &CSCUBACalculator::getVariables()
{
    if ( fVariables.empty() )
    {
        initVariables();
    }
    return fVariables;
}

const TVariableInfoList &CSCUBACalculator::getVariables() const
{
    return fVariables;
}

TVariableInfoList &CSCUBACalculator::getRHSVariables()
{
    if ( fVariables.empty() )
    {
        initVariables();
    }
    return fRHSVariables;
}

const TVariableInfoList &CSCUBACalculator::getRHSVariables() const
{
    return fRHSVariables;
}

TVariableInfoList &CSCUBACalculator::getLHSVariables()
{
    if ( fVariables.empty() )
    {
        initVariables();
    }
    return fLHSVariables;
}

const TVariableInfoList &CSCUBACalculator::getLHSVariables() const
{
    return fLHSVariables;
}

void CSCUBACalculator::initVariables()
{
    fLHSVariables.clear();
    fRHSVariables.clear();
    fVariableMap.clear();

    bool preReversed = false;
    fVariables = getMyVariables( &preReversed );
    for ( auto &&curr : fVariables )
    {
        if ( !preReversed && isReversed() )
            curr->reverseVariableLoc();
        fVariableMap[ curr->name() ] = curr;
        if ( curr->variableLoc() == EVariableLoc::eLHS )
            fLHSVariables.push_back( curr );
        else if ( curr->variableLoc() == EVariableLoc::eRHS )
            fRHSVariables.push_back( curr );
    }

    Q_ASSERT( ( fLHSVariables.empty() && fRHSVariables.empty() ) || ( !fLHSVariables.empty() && !fRHSVariables.empty() ) );
}

void CSCUBACalculator::resetVariables()
{
    for ( auto &&ii = fVariables.begin(); ii != fVariables.end(); ++ii )
    {
        ( *ii )->resetValue( true, false );
    }

    auto formula = finalizeFormula( getBaseFormula(), EFormulaType::eBaseFormula );
    updateFields( nullptr );
    notifyOfNewFormula( formula, EFormulaType::eBaseFormula );
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

TConstVariableInfo CSCUBACalculator::getVariable( const QString &varName ) const
{
    auto pos = fVariableMap.find( varName );
    if ( pos == fVariableMap.end() )
        return {};
    return ( *pos ).second;
}

TVariableInfo CSCUBACalculator::getVariable( const QString &varName )
{
    auto pos = fVariableMap.find( varName );
    if ( pos == fVariableMap.end() )
        return {};
    return ( *pos ).second;
}

TVariableInfoList CSCUBACalculator::getMyVariables( bool *preReversed ) const
{
    *preReversed = false;
    return getMyVariables();
}

TVariableInfo CSCUBACalculator::determineVariableToUnset( EVariableLoc /*updateFromSide*/, QWidget * /*triggerWidget*/, bool /*preDefaultBehavior*/ )
{
    return {};
}

TNamedFormulaList CSCUBACalculator::myGetAllFormulas() const
{
    TNamedFormulaList retVal;
    
    std::unordered_set< QString > allFormulas;
    auto baseFormula = myBaseFormula();
    allFormulas.insert( baseFormula );
    retVal.emplace_back( this->calculatorName() + "-baseFormula", baseFormula );

    auto reverseBase = myReversedBaseFormula();
    if ( !reverseBase.isEmpty() && ( baseFormula != reverseBase ) )
    {
        allFormulas.insert( reverseBase );
        retVal.emplace_back( this->calculatorName() + "-reverseBaseFormula", reverseBase );
    }

    for ( auto &&ii : fVariables )
    {
        if ( !ii->isVariable() )
            continue;

        auto currFormula = getFormulaForVar( ii );
        if ( !currFormula.has_value() || currFormula.value().isEmpty() )
            continue;

        auto pos = allFormulas.find( currFormula.value() );
        if ( pos != allFormulas.end() )
            continue;
        allFormulas.insert( currFormula.value() );
        retVal.emplace_back( calculatorName() + "-" + ii->name(), currFormula.value() );
    }

    return retVal;
}

TNamedFormulaList CSCUBACalculator::getAllFormulas() const
{
    auto allFormulas = myGetAllFormulas();

    TNamedFormulaList retVal;
    std::unordered_set< QString > existingFormulas;

    for ( auto &&currFormula : allFormulas )
    {
        for ( auto imperial : { true, false } )
        {
            for ( auto seaWater : { true, false } )
            {
                auto formula = finalizeFormula( imperial, seaWater, currFormula.second, EFormulaType::eBaseFormula );
                auto pos = existingFormulas.find( formula );
                if ( pos != existingFormulas.end() )
                    continue;

                auto formulaName = currFormula.first;
                formulaName += "_";
                if ( imperial )
                    formulaName += "i";
                else
                    formulaName += "m";
                if ( seaWater )
                    formulaName += "s";
                else
                    formulaName += "f";

                existingFormulas.insert( formula );
                retVal.emplace_back( formulaName, formula );
            }
        }
    }
    return retVal;
}

TVariableInfo CSCUBACalculator::getLastVariable( EVariableLoc side ) const
{
    auto &&variables = ( side == EVariableLoc::eLHS ) ? fLHSVariables : fRHSVariables;

    for ( auto &&ii = variables.rbegin(); ii != variables.rend(); ++ii )
    {
        if ( ( *ii )->isVariable() )
            return *ii;
    }
    return {};
}

TVariableInfo CSCUBACalculator::getFirstVariable( EVariableLoc side ) const
{
    auto &&variables = ( side == EVariableLoc::eLHS ) ? fLHSVariables : fRHSVariables;
    for ( auto &&ii : variables )
    {
        if ( ii->isVariable() )
            return ii;
    }
    return {};
}

void CSCUBACalculator::determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget )
{
    if ( numUnsetVariables() != 0 )
        return;

    if ( fLHSVariables.empty() || fRHSVariables.empty() )
        return;

    auto varToReset = determineVariableToUnset( updateFromSide, triggerWidget, true );
    if ( !varToReset && ( updateFromSide == EVariableLoc::eRHS ) )
    {
        if ( ( fLHSVariables.size() == 1 ) || ( fRHSVariables.size() == 1 ) )
        {
            varToReset = getFirstVariable( EVariableLoc::eLHS );
        }
        else if ( fRHSVariables.size() == 2 )
        {
            Q_ASSERT( !triggerWidget || ( fRHSVariables.front()->isWidget( triggerWidget ) ) || ( fRHSVariables.back()->isWidget( triggerWidget ) ) );
            if ( fRHSVariables.front()->isWidget( triggerWidget ) )
                varToReset = getLastVariable( EVariableLoc::eRHS );
            else if ( !triggerWidget || fRHSVariables.back()->isWidget( triggerWidget ) )
                varToReset = getFirstVariable( EVariableLoc::eRHS );
        }
    }
    else if ( !varToReset && ( updateFromSide == EVariableLoc::eLHS ) )
    {
        if ( ( fRHSVariables.size() == 1 ) || ( fLHSVariables.size() == 1 ) )
        {
            varToReset = getFirstVariable( EVariableLoc::eRHS );
        }
        else if ( fLHSVariables.size() == 2 )
        {
            Q_ASSERT( !triggerWidget || ( fLHSVariables.front()->isWidget( triggerWidget ) ) || ( fLHSVariables.back()->isWidget( triggerWidget ) ) );
            if ( fLHSVariables.front()->isWidget( triggerWidget ) )
                varToReset = getLastVariable( EVariableLoc::eLHS );
            else if ( !triggerWidget || fLHSVariables.back()->isWidget( triggerWidget ) )
                varToReset = getFirstVariable( EVariableLoc::eLHS );
        }
    }

    if ( !varToReset )
        varToReset = determineVariableToUnset( updateFromSide, triggerWidget, false );

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

    auto currFormula = getCurrentFormula();
    if ( currFormula.has_value() )
    {
        auto formula = finalizeFormula( currFormula.value(), EFormulaType::eCurrentFormula );
        notifyOfNewFormula( formula, EFormulaType::eCurrentFormula );
    }

    computeValues();
    updateFields( triggerWidget );

    if ( currFormula.has_value() )
    {
        auto formula = finalizeFormula( currFormula.value(), EFormulaType::eCurrentValueFormula );
        notifyOfNewFormula( formula, EFormulaType::eCurrentValueFormula );
    }

    auto formula = finalizeFormula( getBaseFormula(), EFormulaType::eBaseFormula );
    notifyOfNewFormula( formula, EFormulaType::eBaseFormula );
}

std::optional< QString > CSCUBACalculator::getCurrentFormula() const
{
    auto numUnset = numUnsetVariables();
    if ( numUnset != 1 )
        return getBaseFormula();

    auto unsetVar = getFirstUnsetVariable();
    if ( !unsetVar )
        return {};

    return getFormulaForVar( unsetVar );
}

void CSCUBACalculator::computeValues()
{
    auto numUnset = numUnsetVariables();
    if ( numUnset != 1 )
        return;

    auto unsetVar = getFirstUnsetVariable();
    if ( !unsetVar )
        return;

    computeValueForVar( unsetVar );
}

TVariableInfo CSCUBACalculator::getFirstUnsetVariable() const
{
    for ( auto &&ii : fVariables )
    {
        if ( !ii->isVariable() )
            continue;
        if ( !ii->has_value() )
        {
            return ii;
            break;
        }
    }
    return {};
}

void CSCUBACalculator::updateFields( QWidget *triggerWidget ) const
{
    auto &&variables = getVariables();

    for ( auto &&curr : variables )
    {
        if ( !curr->isWidget( triggerWidget ) || !curr->optValue().has_value() )
            curr->updateFieldFromValue();
    }
}

QString CSCUBACalculator::finalizeFormula( bool imperial, bool seaWater, const QString &formula, EFormulaType formulaType ) const
{
    auto &&variables = getVariables();

    QString retVal = formula;
    for ( auto &&curr : variables )
    {
        curr->updateFormula( imperial, seaWater, retVal, formulaType );
    }

    retVal = retVal.replace( " ", R"__(\ )__" );
    return retVal;
}

QString CSCUBACalculator::finalizeFormula( const QString &formula, EFormulaType formulaType ) const
{
    return finalizeFormula( imperial(), seaWater(), formula, formulaType );
}
