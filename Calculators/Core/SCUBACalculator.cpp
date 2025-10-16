#include "SCUBACalculator.h"
#include "SCUBACalculatorPage.h"
#include "VariableInfo.h"
#include "Utilities.h"
#include "Formula.h"

#include <QFrame>

#include <iterator>
#include <tuple>
#include <list>
#include <memory>
#include <unordered_set>
#include <QJsonArray>

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

Q_LOGGING_CATEGORY( ScubaCalculator, "Towel42.ScubaCalculator", QtMsgType::QtInfoMsg )

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
    return myReversedBaseFormula( imperial(), seaWater() );
}

QString CSCUBACalculator::myReversedBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return {};
}

QString CSCUBACalculator::myBaseFormula() const
{
    return myBaseFormula( imperial(), seaWater() );
}

QString CSCUBACalculator::getBaseFormula() const
{
    auto retVal = ( isReversed() ) ? myReversedBaseFormula() : myBaseFormula();
    return retVal;
}

QWidget *CSCUBACalculator::getPage() const
{
    Q_ASSERT( fPage );
    return fPage;
}

QWidget *CSCUBACalculator::getPage( QWidget *parent )
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

void CSCUBACalculator::notifyOfNewFormula( const QString &formula, EFormulaType formulaType, bool finished ) const
{
    if ( fUpdateFormulaFunc )
        fUpdateFormulaFunc( dynamic_cast< CSCUBACalculatorPage * >( getPage() ), formula, formulaType, finished );
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
    notifyOfNewFormula( formula, EFormulaType::eBaseFormula, true );
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

std::pair< TFormulaList, TValuesForVariablePairVector > CSCUBACalculator::getFormulaListAndValues() const
{
    TFormulaList namedFormulas;

    for ( auto imperial : { true, false } )
    {
        for ( auto seaWater : { true, false } )
        {
            auto baseFormula = myBaseFormula( imperial, seaWater );
            namedFormulas.push_back( std::make_shared< NUtilities::SFormula >( this->calculatorName() + "-baseFormula", baseFormula, imperial, seaWater ) );

            auto reverseBase = myReversedBaseFormula( imperial, seaWater );
            if ( !reverseBase.isEmpty() && ( baseFormula != reverseBase ) )
            {
                namedFormulas.push_back( std::make_shared< NUtilities::SFormula >( this->calculatorName() + "-reverseBaseFormula", reverseBase, imperial, seaWater ) );
            }
        }
    }
    TValuesForVariablePairVector variablesWithValues;
    for ( auto &&ii : fVariables )
    {
        if ( !ii->isVariable() )
            continue;

        auto values = ii->validValues();
        if ( !values.has_value() )
            continue;

        variablesWithValues.emplace_back( ii, values.value() );
    }
    std::sort(
        variablesWithValues.begin(), variablesWithValues.end(),   //
        []( const TValuesForVariablePair &lhs, const TValuesForVariablePair &rhs )   //
        {   //
            return lhs.first->name() < rhs.first->name();
        } );

    // first get all the formulas without variables that have "values" without a custom value
    // custom means we need the formula blank for that variable
    for ( auto imperial : { true, false } )
    {
        for ( auto seaWater : { true, false } )
        {
            for ( auto &&ii : fVariables )
            {
                if ( !ii->isVariable() )
                    continue;

                if ( ii->hasValues() && !ii->hasCustomValue() )
                    continue;

                auto currFormula = getFormulaForVar( ii, imperial, seaWater );
                if ( !currFormula.has_value() || currFormula.value().isEmpty() )
                    continue;

                namedFormulas.push_back( std::make_shared< NUtilities::SFormula >( calculatorName() + "-" + ii->name(), currFormula.value(), imperial, seaWater ) );
            }
        }
    }

    return { namedFormulas, variablesWithValues };
}

TFormulaList CSCUBACalculator::getFormulaList() const
{
    auto &&[ allFormulas, valuesForVariables ] = getFormulaListAndValues();

    Q_ASSERT( valuesForVariables.size() <= 1 );

    TFormulaList retVal;
    std::unordered_set< QString > existingFormulas;

    std::unordered_map< TVariableInfo, TOptionalDouble > currValues;
    for ( auto &&jj : fVariables )
    {
        currValues[ jj ] = jj->optValue();
        jj->resetValue( false, false );
    }

    for ( auto &&currFormula : allFormulas )
    {
        auto formula = finalizeFormula( currFormula->imperial(), currFormula->seaWater(), currFormula->formula(), EFormulaType::eBaseFormula );
        auto pos = existingFormulas.find( formula );
        if ( pos == existingFormulas.end() )
        {
            existingFormulas.insert( formula );

            auto newFormula = std::make_shared< NUtilities::SFormula >( *currFormula );
            newFormula->setFormula( formula );
            retVal.push_back( newFormula );
        }

        for ( auto &&ii : valuesForVariables )
        {
            auto var = ii.first;
            Q_ASSERT( var );
            if ( !var )
                continue;

            for ( auto &&jj : ii.second )
            {
                if ( !jj.has_value() )
                    continue;

                ii.first->setValue( jj );

                formula = finalizeFormula( currFormula->imperial(), currFormula->seaWater(), currFormula->formula(), EFormulaType::eCurrentValueFormula );
                auto pos = existingFormulas.find( formula );
                if ( pos == existingFormulas.end() )
                {
                    auto newFormula = std::make_shared< NUtilities::SFormula >( *currFormula, TNameValuePair( ii.first->name(), jj ) );
                    newFormula->setFormula( formula );
                    existingFormulas.insert( formula );
                    retVal.emplace_back( newFormula );
                }
            }
        }
    }

    for ( auto &&jj : currValues )
    {
        jj.first->setValue( jj.second );
    }

    return retVal;
}

std::shared_ptr< SGeneratedFormulaData > CSCUBACalculator::getAllFormulas() const
{
    auto retVal = std::make_shared< SGeneratedFormulaData >();

    qCDebug( ScubaCalculator ).noquote().nospace() << "Getting Formulas from: " << calculatorName();
    auto formulas = getFormulaList();

    for ( auto &&jj : formulas )
    {
        auto tex = jj->formula();

        auto pos = retVal->fAllFormulas.find( tex );
        if ( pos != retVal->fAllFormulas.end() )
            continue;

        retVal->fAllFormulas.insert( tex );
        retVal->fByNameList.emplace_back( jj );
    }
    return retVal;
}

void CSCUBACalculator::initResources() const
{
    Q_INIT_RESOURCE( calculator );
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
        notifyOfNewFormula( formula, EFormulaType::eCurrentFormula, false );
    }

    computeValues();
    updateFields( triggerWidget );

    if ( currFormula.has_value() )
    {
        auto formula = finalizeFormula( currFormula.value(), EFormulaType::eCurrentValueFormula );
        notifyOfNewFormula( formula, EFormulaType::eCurrentValueFormula, false );
    }

    auto formula = finalizeFormula( getBaseFormula(), EFormulaType::eBaseFormula );
    notifyOfNewFormula( formula, EFormulaType::eBaseFormula, true );
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

std::optional< QString > CSCUBACalculator::getFormulaForVar( const TConstVariableInfo &unsetVar ) const
{
    return getFormulaForVar( unsetVar, imperial(), seaWater() );
}

void CSCUBACalculator::updateFields( QWidget *triggerWidget ) const
{
    auto &&variables = getVariables();

    for ( auto &&curr : variables )
    {
        if ( curr->needsFieldUpdate( triggerWidget ) )
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

void SGeneratedFormulaData::sortByName()
{
    fByNameList.sort(   //
        []( const TFormula &lhs, const TFormula &rhs )   //
        {   //
            return lhs->name() < rhs->name();
        } );
}

std::pair< int, int > SGeneratedFormulaData::formulaCounts( const std::function< bool( const QString & formula ) > & beenCreated ) const
{
    int total = 0;
    int needsRendering = 0;

    for ( auto &&curr : fByNameList )
    {
        total++;
        if ( !beenCreated( curr->formula() ) )
            needsRendering++;
    }
    return { total, needsRendering };
};
