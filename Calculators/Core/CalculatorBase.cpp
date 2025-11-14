#include "CalculatorBase.h"
#include "CalculatorPage.h"
#include "VariableInfo.h"
#include "Utilities.h"
#include "Formula.h"

#include <memory>
#include <QRegularExpression>

// https://scuba.garykessler.net/EANcalculator/EAN_psi.html
// https://allthingsdiving.com/dive-calculators/
// https://www.divebuddy.com/calculator/
// https://swimmingcalculators.com/scuba-diving-calculator/
// https://swimmingcalculators.com/scuba-diving-weight-calculator/

Q_LOGGING_CATEGORY( Calculator, "Towel42.Calculator", QtMsgType::QtInfoMsg )

CCalculatorBase::CCalculatorBase( QObject *parent ) :
    QObject( parent )
{
}

CCalculatorBase::~CCalculatorBase()
{
}

QString CCalculatorBase::myCalculatorName() const
{
    Q_ASSERT_X( isReversible(), "myCalculatorName", "Should be overridden for non reversible calculators." );
    return {};
}

QString CCalculatorBase::myReversedCalculatorName() const
{
    Q_ASSERT_X( isReversible(), "myCalculatorName", "Should be overridden for non reversible calculators." );
    return {};
}

std::optional< TFormulaList > CCalculatorBase::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return {};
}

TFormulaList CCalculatorBase::getBaseFormulas() const
{
    auto retVal = ( isReversed() ) ? myReversedBaseFormulas( imperial(), seaWater() ) : myBaseFormulas( imperial(), seaWater() );
    if ( retVal.has_value() )
        return retVal.value();
    return {};
}

QWidget *CCalculatorBase::getPage() const
{
    Q_ASSERT( fPage );
    return fPage;
}

QWidget *CCalculatorBase::getPage( QWidget *parent )
{
    if ( !fPage )
    {
        std::tie( fPage, fNumVariables ) = CCalculatorPage::constructPage( this, parent );
    }
    return fPage;
}

void CCalculatorBase::init( bool imperial, bool seaWater )
{
    setObjectName( calculatorName() );
    if ( fPage )
        fPage->init( imperial, seaWater );
}

void CCalculatorBase::setIsReversed( CCalculatorBase *nonReversedCalc, bool isReversed )
{
    fReversed = { nonReversedCalc, isReversed };
    if ( fReversed.second )
    {
        auto currName = objectName();
        auto pos = currName.indexOf( "To" );
        if ( pos != -1 )
        {
            auto lhs = currName.left( pos );
            auto rhs = currName.mid( pos + 2 );
            setObjectName( rhs + "To" + lhs );
        }
    }
}

QStringList CCalculatorBase::calculatorPath() const
{
    QStringList retVal;
    if ( isReversed() )
    {
        retVal = myReversedCalculatorPath();
        if ( retVal.isEmpty() )
            retVal = myCalculatorPath();
    }
    else
        retVal = myCalculatorPath();
    Q_ASSERT_X( !retVal.isEmpty(), "calculatorPath", "myCalculatorPath and/or myReversedCalculatorPath is not overridden" );
    return retVal;
}

QString CCalculatorBase::calculatorName() const
{
    auto labels = fromToLabels();
    if ( isReversible() && labels.has_value() )
    {
        if ( isReversed() )
            return tr( "%1 to %2" ).arg( labels.value().second ).arg( labels.value().first );
        else
            return tr( "%2 to %1" ).arg( labels.value().second ).arg( labels.value().first );
    }

    return ( isReversed() ) ? myReversedCalculatorName() : myCalculatorName();
}

void CCalculatorBase::setImperial( bool imperial )
{
    if ( fPage )
        fPage->setImperial( imperial );
}

void CCalculatorBase::setSeaWater( bool seaWater )
{
    if ( fPage )
        fPage->setSeaWater( seaWater );
}

void CCalculatorBase::setUpdateFormulaFunc( const TUpdateFormulaFunc &func )
{
    fUpdateFormulaFunc = func;
}

bool CCalculatorBase::imperial() const
{
    if ( fPage )
        return fPage->imperial();
    return false;
}

bool CCalculatorBase::seaWater() const
{
    if ( fPage )
        return fPage->seaWater();
    return false;
}

void CCalculatorBase::notifyOfNewFormula( const QString &formula, bool finished ) const
{
    if ( fUpdateFormulaFunc )
        fUpdateFormulaFunc( dynamic_cast< CCalculatorPage * >( getPage() ), formula, finished );
}

TVariableInfoList &CCalculatorBase::getVariables()
{
    if ( fVariables.empty() )
    {
        initVariables();
    }
    return fVariables;
}

const TVariableInfoList &CCalculatorBase::getVariables() const
{
    return fVariables;
}

TVariableInfoList &CCalculatorBase::getRHSVariables()
{
    if ( fVariables.empty() )
    {
        initVariables();
    }
    return fRHSVariables;
}

const TVariableInfoList &CCalculatorBase::getRHSVariables() const
{
    return fRHSVariables;
}

TVariableInfoList &CCalculatorBase::getLHSVariables()
{
    if ( fVariables.empty() )
    {
        initVariables();
    }
    return fLHSVariables;
}

const TVariableInfoList &CCalculatorBase::getLHSVariables() const
{
    return fLHSVariables;
}

void CCalculatorBase::initVariables()
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
    setupDependencies();
    setupCustomDependencies();

    Q_ASSERT( ( fLHSVariables.empty() && fRHSVariables.empty() ) || ( !fLHSVariables.empty() && !fRHSVariables.empty() ) );
}

void CCalculatorBase::setupDependencies()
{
    for ( auto &&ii = fVariables.begin(); ii != fVariables.end(); ++ii )
    {
        //if ( ( *ii )->isConstant() )
        //    continue;

        TVariableInfoList dependencies = fVariables;
        dependencies.remove_if(   //
            [ ii ]( const TVariableInfo &curr )   //
            {
                //if ( curr->isConstant() )
                //    return true;
                if ( curr == *ii )
                    return true;
                return false;
            } );
        ( *ii )->setDependencies( dependencies );
    }
}

void CCalculatorBase::setDependencies( const QString &varName, const QString &dep )
{
    setDependencies( varName, QStringList() << dep );
}

void CCalculatorBase::setDependencies( const QString &varName, const QStringList &deps )
{
    auto var = getVariable( varName );
    Q_ASSERT( var );
    if ( !var )
        return;
    TVariableInfoList depVars;
    for ( auto &&ii : deps )
    {
        auto dep = getVariable( ii );
        Q_ASSERT( dep );
        if ( !dep )
            continue;
        depVars.push_back( dep );
    }
    var->setDependencies( depVars );
}

void CCalculatorBase::setupCustomDependencies()
{
}

void CCalculatorBase::resetVariables()
{
    for ( auto &&ii = fVariables.begin(); ii != fVariables.end(); ++ii )
    {
        ( *ii )->resetValue( imperial(), seaWater(), true, false );
    }

    auto formula = finalizeFormulas( imperial(), seaWater(), getBaseFormulas() );
    updateFields( nullptr );
    notifyOfNewFormula( formula, true );
}

TVariableInfoList CCalculatorBase::unsetVariables() const
{
    TVariableInfoList retVal;
    for ( auto &&ii : fVariables )
    {
        if ( !ii->isVariable() )
            continue;
        if ( !ii->has_value() )
            retVal.push_back( ii );
    }
    return retVal;
}

std::size_t CCalculatorBase::numUnsetVariables( QWidget *triggerWidget ) const
{
    std::size_t retVal = 0;
    for ( auto &&ii : fVariables )
    {
        if ( !ii->isVariable() )
            continue;
        if ( triggerWidget && ii->isWidget( triggerWidget ) )
            continue;
        if ( !ii->has_value() )
            retVal++;
    }
    return retVal;
}

bool CCalculatorBase::allVariablesUnset() const
{
    for ( auto &&ii : fVariables )
    {
        if ( !ii->isVariable() )
            continue;
        if ( ii->hasValues() && ii->has_value() )
            return false;
        if ( ii->has_value() )
            return false;
    }
    return true;
}

TConstVariableInfo CCalculatorBase::getVariable( const QString &varName ) const
{
    auto pos = fVariableMap.find( varName );
    if ( pos == fVariableMap.end() )
        return {};
    return ( *pos ).second;
}

TVariableInfo CCalculatorBase::getVariable( const QString &varName )
{
    auto retVal = const_cast< const CCalculatorBase * >( this )->getVariable( varName );
    if ( !retVal )
        return {};
    return std::const_pointer_cast< CVariableInfo >( retVal );
}

TVariableInfoList CCalculatorBase::getMyVariables() const
{
    bool preReversed = false;
    return getMyVariables( &preReversed );
}

TVariableInfo CCalculatorBase::determineVariableToUnset( EVariableLoc /*updateFromSide*/, QWidget * /*triggerWidget*/, bool /*preDefaultBehavior*/ )
{
    return {};
}

void CCalculatorBase::initResources() const
{
}

TVariableInfo CCalculatorBase::getLastVariable( EVariableLoc side ) const
{
    auto &&variables = ( side == EVariableLoc::eLHS ) ? fLHSVariables : fRHSVariables;

    for ( auto &&ii = variables.rbegin(); ii != variables.rend(); ++ii )
    {
        if ( ( *ii )->isVariable() )
            return *ii;
    }
    return {};
}

TVariableInfo CCalculatorBase::getFirstVariable( EVariableLoc side ) const
{
    auto &&variables = ( side == EVariableLoc::eLHS ) ? fLHSVariables : fRHSVariables;
    for ( auto &&ii : variables )
    {
        if ( ii->isVariable() )
            return ii;
    }
    return {};
}

std::size_t CCalculatorBase::numVariables( EVariableLoc side ) const
{
    auto &&variables = ( side == EVariableLoc::eLHS ) ? fLHSVariables : fRHSVariables;
    std::size_t retVal = 0;
    for ( auto &&ii : variables )
    {
        if ( ii->isVariable() )
            retVal++;
    }
    return retVal;
}

void CCalculatorBase::determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget )
{
    if ( numUnsetVariables( triggerWidget ) != 0 )
        return;

    if ( fLHSVariables.empty() || fRHSVariables.empty() )
        return;

    auto varToReset = determineVariableToUnset( updateFromSide, triggerWidget, true );
    if ( !varToReset && ( updateFromSide == EVariableLoc::eRHS ) )
    {
        if ( ( numVariables( EVariableLoc::eLHS ) == 1 ) || ( numVariables( EVariableLoc::eRHS ) == 1 ) )
        {
            varToReset = getFirstVariable( EVariableLoc::eLHS );
        }
        else if ( numVariables( EVariableLoc::eRHS ) == 2 )
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
        if ( ( numVariables( EVariableLoc::eRHS ) == 1 ) || ( numVariables( EVariableLoc::eLHS ) == 1 ) )
        {
            varToReset = getFirstVariable( EVariableLoc::eRHS );
        }
        else if ( numVariables( EVariableLoc::eLHS ) == 2 )
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
        varToReset->resetValue( imperial(), seaWater(), false, false );
    }
}

void CCalculatorBase::compute( EVariableLoc updateFromSide, QWidget *triggerWidget )
{
    auto &&variables = getVariables();
    bool imperial = this->imperial();
    bool seaWater = this->seaWater();
    for ( auto &&curr : variables )
    {
        curr->updateValuesAndRanges( imperial, seaWater );
        curr->updateLabels( imperial, seaWater );
        curr->updateValueFromField();
    }

    determineVariableToUnset( updateFromSide, triggerWidget );

    auto currFormulas = getCurrentFormulas();
    if ( currFormulas.has_value() )
    {
        auto formula = finalizeFormulas( imperial, seaWater, currFormulas.value() );
        notifyOfNewFormula( formula, true );
    }

    computeValues();
    updateFields( triggerWidget );
}

std::optional< TFormulaList > CCalculatorBase::getCurrentFormulas() const
{
    auto baseFormulas = getBaseFormulas();

    auto unsetVar = getFirstUnsetVariable();
    auto formulasForVar = getFormulasForVar( unsetVar ? unsetVar->name() : QString(), imperial(), seaWater() );
    if ( !formulasForVar.has_value() )
        return baseFormulas;

    bool baseIsSame = ( formulasForVar.value().size() == baseFormulas.size() );
    if ( baseIsSame )
    {
        auto &&ii = formulasForVar.value().begin();
        auto &&jj = baseFormulas.begin();

        for ( ; baseIsSame && ( ii != formulasForVar.value().end() ) && ( jj != baseFormulas.end() ); ++ii, ++jj )
        {
            baseIsSame = ( *ii == *jj );
        }
    }

    auto retVal = baseFormulas;
    if ( !baseIsSame )
    {
        for ( auto &&ii : retVal )
        {
            ii->setBaseFormula( true );
        }
        retVal.insert( retVal.end(), formulasForVar.value().begin(), formulasForVar.value().end() );
    }

    retVal = NUtilities::sortAndUniquifyFormulas( retVal );

    return retVal;
}

void CCalculatorBase::computeValues()
{
    computeVariableValues();
}

bool CCalculatorBase::valuesSetProperly() const
{
    auto numUnset = numUnsetVariables();
    return ( numUnset == numAllowedUnset() );
}

TVariableInfoList CCalculatorBase::getUnsetVariables() const
{
    TVariableInfoList retVal;
    for ( auto &&ii : fVariables )
    {
        if ( !ii->isVariable() )
            continue;
        if ( !ii->has_value() )
        {
            retVal.push_back( ii );
        }
    }
    return retVal;
}

TVariableInfo CCalculatorBase::getFirstUnsetVariable() const
{
    auto unset = getUnsetVariables();
    if ( unset.empty() )
        return {};
    return unset.front();
}

void CCalculatorBase::updateFields( QWidget *triggerWidget ) const
{
    auto &&variables = getVariables();

    for ( auto &&curr : variables )
    {
        if ( curr->needsFieldUpdate( triggerWidget ) )
            curr->updateFieldFromValue( imperial(), seaWater() );
    }
}

TFormulaList CCalculatorBase::finalizeFormula( bool imperial, bool seaWater, const TFormula &formula )
{
    if ( !formula )
        return { formula };

    TFormulaList retVal;

    auto baseFormula = applyVariables( imperial, seaWater, formula, EFormulaType::eBaseFormula );
    retVal.emplace_back( baseFormula );

    if ( !formula->isBaseFormula() )
    {
        auto currFormula = applyVariables( imperial, seaWater, formula, EFormulaType::eCurrentFormula );
        if ( currFormula != baseFormula )
            retVal.emplace_back( currFormula );

        auto currValueFormula = applyVariables( imperial, seaWater, formula, EFormulaType::eCurrentValueFormula );
        if ( ( currValueFormula != baseFormula ) && ( currValueFormula != currFormula ) )
            retVal.emplace_back( currValueFormula );

        auto finalValue = formula->getFinalValueFormula( imperial, seaWater, this );
        if ( finalValue )
            retVal.push_back( finalValue );
    }
    return retVal;
}

QString CCalculatorBase::finalizeFormulas( bool imperial, bool seaWater, const TFormulaList &formulas )
{
    QString retVal;

    TFormulaList allFormulas;
    for ( auto &&ii : formulas )
    {
        auto currFormulas = finalizeFormula( imperial, seaWater, ii );
        allFormulas.insert( allFormulas.end(), currFormulas.begin(), currFormulas.end() );
    }

    auto joined = NUtilities::joinFormulas( imperial, seaWater, allFormulas );
    if ( !joined.has_value() )
        return {};

    retVal = postProcessFormula( joined.value() );

    return retVal;
}

QString CCalculatorBase::postProcessFormula( const QString &formula ) const
{
    auto retVal = formula;

    retVal = retVal.replace( " ", R"__(\ )__" );
    auto regEx = QRegularExpression( R"__(([^\\])%)__" );
    auto matchII = regEx.globalMatch( retVal );
    int offset = 0;
    while ( matchII.hasNext() )
    {
        auto match = matchII.next();
        auto replacementText = match.captured( 1 ) + R"__(\%)__";
        auto start = match.capturedStart( 1 );
        retVal.replace( start + offset, 2, replacementText );
        offset++;
    }
    return retVal;
}

TFormula CCalculatorBase::applyVariables( bool imperial, bool seaWater, const TFormula &formula, EFormulaType formulaType ) const
{
    return formula->applyVariables( imperial, seaWater, getVariables(), formulaType );
}
