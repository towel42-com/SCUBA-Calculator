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
    fGroupName( groupName ),
    fJsonValue( QJsonValue() )
{
    auto jsonFile = QString( ":/calculator/%1.json" ).arg( projectName );
    setObjectName( fProjectName );

    loadJson( jsonFile );
}

CJsonCalculator::CJsonCalculator( const QString &objName, const QJsonObject &jsonObj, QObject *parent ) :
    CCalculatorBase( parent ),
    fJsonValue( jsonObj )
{
    setObjectName( objName );
    fGroupName = fProjectName = objName;
    loadJson();
}

CJsonCalculator::CJsonCalculator( const QString &objName, const QJsonValue &jsonValue, QObject *parent ) :
    CJsonCalculator( objName, jsonValue.toObject(), parent )
{
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

std::optional< std::list< CJsonCalculator * > > CJsonCalculator::create( const QString &fileName, QObject *parent, std::optional< QString > &errorMsg )
{
    auto doc = loadJsonDocument( fileName, errorMsg );
    if ( !doc.has_value() )
        return {};

    std::optional< std::list< CJsonCalculator * > > retVal;

    auto baseName = QFileInfo( fileName ).baseName();
    if ( doc.value().isObject() )
    {
        auto object = doc.value().object();
        auto curr = create( baseName, object, parent );
        if ( !curr || ( curr && curr->hasError() ) )
        {
            delete curr;
            return {};
        }
        retVal = std::list< CJsonCalculator * >( { curr } );
    }
    else if ( doc.value().isArray() )
    {
        retVal = std::list< CJsonCalculator * >();
        auto array = doc.value().array();
        int jj = 0;
        for ( auto &&ii : array )
        {
            if ( !ii.isObject() )
                continue;

            auto objName = baseName;
            if ( array.size() > 1 )
                objName += QString( "_%1" ).arg( jj++ );
            auto curr = create( objName, ii, parent );
            if ( !curr || ( curr && curr->hasError() ) )
            {
                errorMsg = curr->errorMsg();
                delete curr;
                continue;
            }
            retVal.value().push_back( curr );
        }
        if ( retVal.value().empty() )
            retVal = {};
    }

    return retVal;
}

CJsonCalculator *CJsonCalculator::create( const QString &objName, const QJsonValue &jsonObj, QObject *parent /*= nullptr */ )
{
    return new CJsonCalculator( objName, jsonObj, parent );
}

CJsonCalculator *CJsonCalculator::create( const QString &objName, const QJsonObject &jsonObj, QObject *parent /*= nullptr */ )
{
    return new CJsonCalculator( objName, jsonObj, parent );
}

CJsonCalculator::~CJsonCalculator()
{
}

QString CJsonCalculator::myCalculatorName() const
{
    Q_ASSERT_X( fName.has_value(), "myCalculatorName", "Need to have name field in the Json for this to not assert." ); 
    if ( fName.has_value() )
        return fName.value();
    return CCalculatorBase::myCalculatorName();
}

QString CJsonCalculator::myReversedCalculatorName() const
{
    Q_ASSERT_X( fReversedName.has_value(), "myCalculatorName", "Need to have name field in the Json for this to not assert." );
    if ( fReversedName.has_value() )
        return fReversedName.value();
    return CCalculatorBase::myReversedCalculatorName();
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

bool CJsonCalculator::loadJson( const QString &jsonFile )
{
    fErrorMsg.reset();

    auto doc = loadJsonDocument( jsonFile, fErrorMsg );
    if ( !doc.has_value() )
        return false;

    if ( !doc.value().isObject() )
    {
        fErrorMsg = tr( "Invalid JSON.  Expected root to be an object." );
        return false;
    }
    fJsonValue = doc.value().object();

    return loadJson();
}

bool CJsonCalculator::loadJson()
{
    if ( !fJsonValue.isObject() )
        return false;

    auto obj = fJsonValue.toObject();
    NSABUtils::fromJson( fIsReversible, obj, "reversible" );
    NSABUtils::fromJson( fFromToLabels, obj, "fromToLabels" );
    NSABUtils::fromJson( fName, obj, "name" );
    NSABUtils::fromJson( fReversedName, obj, "reversedName" );
    if ( !fFromToLabels.has_value() && !fName.has_value() )
    {
        fErrorMsg = tr( "Invalid JSON must have fromToLabels or name fields." );
        return false;
    }

    if ( !NSABUtils::fromJson( fPath, obj, "Path" ) )
    {
        fErrorMsg = tr( "Invalid JSON. Root object does not contain a Path field" );
        return false;
    }

    NSABUtils::fromJson( fReversePath, obj, "ReversePath" );
    NSABUtils::fromJson( fShowUnits, obj, "showUnits" );
    NSABUtils::fromJson( fShowWaterType, obj, "showWaterType" );

    NSABUtils::fromJson( fBaseFormula, obj, "baseFormula" );
    NSABUtils::fromJson( fReversedBaseFormula, obj, "reversedBaseFormula" );

    if ( !obj.contains( "variables" ) )
    {
        fErrorMsg = tr( "Invalid JSON: Root object contains no variables." );
        return false;
    }

    auto vars = obj[ "variables" ].toArray();
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

std::optional< QJsonDocument > CJsonCalculator::loadJsonDocument( const QString &jsonFile, std::optional< QString > &errorMsg )
{
    if ( !QFileInfo( jsonFile ).exists() )
    {
        errorMsg = tr( "JSON File '%1' does not exist." ).arg( jsonFile );
        return {};
    }

    auto fi = QFile( jsonFile );
    if ( !fi.open( QFile::ReadOnly | QFile::Text ) )
    {
        errorMsg = tr( "JSON File '%1' could not be opened." ).arg( jsonFile );
        return {};
    }

    auto data = fi.readAll();
    data.replace( R"(\)", R"(\\)" );

    QJsonParseError error;
    auto doc = QJsonDocument::fromJson( data, &error );
    if ( error.error != QJsonParseError::NoError )
    {
        errorMsg = error.errorString();
        return {};
    }

    return doc;
}
