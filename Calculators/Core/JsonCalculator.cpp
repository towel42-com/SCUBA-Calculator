#include "JSONCalculator.h"
#include "VariableInfo.h"
#include "SABUtils/JsonUtils.h"

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QRegularExpression>

CJsonCalculator::CJsonCalculator( const QString &projectName, const QString &groupName, QObject *parent ) :
    CCalculatorBase( parent ),
    fProjectName( projectName ),
    fGroupName( groupName )
{
    fJSONFile = QString( ":/calculator/%1.json" ).arg( projectName );
    setObjectName( fProjectName );

    loadJSON();
}

CJsonCalculator::CJsonCalculator( const QString &fileName, QObject *parent ) :
    CCalculatorBase( parent ),
    fJSONFile( fileName )
{
    fGroupName = fProjectName = QFileInfo( fJSONFile ).baseName();
    setObjectName( fProjectName );

    loadJSON();
}

CJsonCalculator *CJsonCalculator::create( const QString &projectName, const QString &groupName, QObject *parent )
{
    auto retVal = new CJsonCalculator( projectName, groupName, parent );
    if ( retVal->hasError() )
    {
        delete retVal;
        retVal = nullptr;
    }
    return retVal;
}

CJsonCalculator *CJsonCalculator::create( const QString &fileName, QObject *parent )
{
    auto retVal = new CJsonCalculator( fileName, parent );
    if ( retVal->hasError() )
    {
        delete retVal;
        retVal = nullptr;
    }
    return retVal;
}

CJsonCalculator::~CJsonCalculator()
{
}

QString CJsonCalculator::calculatorProjectName() const
{
    return fProjectName;
}

QString CJsonCalculator::calculatorGroupName() const
{
    return fGroupName;
}

TVariableInfoList CJsonCalculator::getMyVariables( bool *preReversed ) const
{
    if ( preReversed )
        *preReversed = false;

    return fVariables;
}

TVariableInfo CJsonCalculator::findVariable( const QString &var ) const
{
    for ( auto &&ii : fVariables )
    {
        if ( ( ii->name() == var ) || ( ii->fieldName() == var ) )
        {
            return ii;
        }
    }
    return {};
}

TOptionalFormulaList CJsonCalculator::formulasForBaseFormula( bool imperial, bool seaWater, const std::optional< QString > &formula ) const
{
    if ( !formula.has_value() )
        return {};

    auto regExp = QRegularExpression( R"__(\<[A-Za-z]+\>)__" );
    if ( regExp.match( formula.value() ).hasMatch() )
    {
        auto var = findVariable( formula.value() );
        if ( var )
            return var->formulaList( imperial, seaWater );
    }
    return {};
}

TOptionalFormulaList CJsonCalculator::myBaseFormulas( bool imperial, bool seaWater ) const
{
    auto retVal = formulasForBaseFormula( imperial, seaWater, fBaseFormula );
    if ( retVal.has_value() )
        return retVal;

    if ( fVariables.empty() )
        return {};

    auto var = this->getFirstVariable( EVariableLoc::eLHS );
    if ( var )
        return var->formulaList( imperial, seaWater );
    return {};
}

TOptionalFormulaList CJsonCalculator::myReversedBaseFormulas( bool imperial, bool seaWater ) const
{
    auto retVal = formulasForBaseFormula( imperial, seaWater, fReversedBaseFormula );
    if ( retVal.has_value() )
        return retVal;

    if ( fVariables.empty() )
        return {};

    auto var = this->getFirstVariable( EVariableLoc::eLHS );
    if ( var )
        return var->formulaList( imperial, seaWater );
    return {};
}

TOptionalFormulaList CJsonCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    auto variable = findVariable( unsetVar );
    if ( !variable )
        return {};
    return variable->formulaList( imperial, seaWater );
}

void CJsonCalculator::computeVariableValues()
{
    for ( auto &&ii : fVariables )
    {
        if ( !ii->has_value() && ii->dependenciesSatisfied() )
        {
            ii->computeFromJS( imperial(), seaWater(), fVariables );
        }
    }
}

bool CJsonCalculator::jsonExists() const
{
    return QFileInfo( fJSONFile ).exists();
}

bool CJsonCalculator::loadJSON()
{
    fErrorMsg.reset();

    if ( !jsonExists() )
    {
        fErrorMsg = tr( "JSON File '%1' does not exist." ).arg( fJSONFile );
        return false;
    }

    auto fi = QFile( fJSONFile );
    if ( !fi.open( QFile::ReadOnly | QFile::Text ) )
    {
        fErrorMsg = tr( "JSON File '%1' could not be opened." ).arg( fJSONFile );
        return false;
    }

    auto data = fi.readAll();
    data.replace( R"(\)", R"(\\)" );

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson( data, &error );
    if ( error.error != QJsonParseError::NoError )
    {
        fErrorMsg = error.errorString();
        return false;
    }

    if ( !doc.isObject() )
    {
        fErrorMsg = tr( "Invalid JSON.  Expected root to be an object." );
        return false;
    }
    auto calcObj = doc.object();

    NSABUtils::fromJson( fIsReversible, calcObj, "reversible" );
    NSABUtils::fromJson( fFromToLabels, calcObj, "fromToLabels" );
    if ( !NSABUtils::fromJson( fPath, calcObj, "Path" ) )
    {
        fErrorMsg = tr( "Invalid JSON. Root object does not contain a Path field" );
        return false;
    }

    NSABUtils::fromJson( fReversePath, calcObj, "ReversePath" );
    NSABUtils::fromJson( fShowUnits, calcObj, "showUnits" );
    NSABUtils::fromJson( fShowWaterType, calcObj, "showWaterType" );

    NSABUtils::fromJson( fBaseFormula, calcObj, "baseFormula" );
    NSABUtils::fromJson( fReversedBaseFormula, calcObj, "reversedBaseFormula" );

    if ( !calcObj.contains( "variables" ) )
    {
        fErrorMsg = tr( "Invalid JSON: Root object contains no variables." );
        return false;
    }

    auto vars = calcObj[ "variables" ].toArray();
    for ( auto &&var : vars )
    {
        auto variable = CVariableInfo::fromJson( var.toObject(), fErrorMsg );
        if ( !variable || fErrorMsg.has_value() )
        {
            return false;
        }
        fVariables.push_back( variable );
    }
    return true;
}
