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

    fVariables = getMyVariables();
    for ( auto &&curr : fVariables )
    {
        if ( isReversed() )
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

void CSCUBACalculator::renderDefaultFormulas() const
{
    auto baseFormula = getBaseFormula();
    std::unordered_set< QString > baseFormulas;

    QString currBase;

    for ( auto imperial : { true, false } )
    {
        for ( auto seaWater : { true, false } )
        {
            auto formula = finalizeFormula( imperial, seaWater, baseFormula, EFormulaType::eBaseFormula );
            if ( ( this->imperial() == imperial ) && ( this->seaWater() == seaWater ) )
                currBase = formula;
            else
                baseFormulas.insert( formula );
        }
    }
    Q_ASSERT( baseFormulas.size() <= 4 );
    Q_ASSERT( !currBase.isEmpty() );
    if ( !currBase.isEmpty() )
        notifyOfNewFormula( currBase, EFormulaType::eBaseFormula );
    for ( auto &&formula : baseFormulas )
    {
        notifyOfNewFormula( formula, EFormulaType::eBaseFormula );
    }
}

TVariableInfo CSCUBACalculator::determineVariableToUnset( EVariableLoc /*updateFromSide*/, QWidget * /*triggerWidget*/, bool /*preDefaultBehavior*/ )
{
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
            varToReset = fLHSVariables.front();
        }
        else if ( fRHSVariables.size() == 2 )
        {
            Q_ASSERT( !triggerWidget || ( fRHSVariables.front()->isWidget( triggerWidget ) ) || ( fRHSVariables.back()->isWidget( triggerWidget ) ) );
            if ( fRHSVariables.front()->isWidget( triggerWidget ) )
                varToReset = fRHSVariables.back();
            else if ( !triggerWidget || fRHSVariables.back()->isWidget( triggerWidget ) )
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
            Q_ASSERT( !triggerWidget || ( fLHSVariables.front()->isWidget( triggerWidget ) ) || ( fLHSVariables.back()->isWidget( triggerWidget ) ) );
            if ( fLHSVariables.front()->isWidget( triggerWidget ) )
                varToReset = fLHSVariables.back();
            else if ( !triggerWidget || fLHSVariables.back()->isWidget( triggerWidget ) )
                varToReset = fLHSVariables.front();
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

        formula = finalizeFormula( currFormula.value(), EFormulaType::eCurrentValueFormula );
        notifyOfNewFormula( formula, EFormulaType::eCurrentValueFormula );
    }

    computeValues();
    updateFields( triggerWidget );

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
        if ( !curr->isWidget( triggerWidget ) || !curr->currFieldValue().has_value() )
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

