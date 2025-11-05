#include "GeneratedFormulaData.h"
#include "Formula.h"
#include "Utilities.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

CGeneratedFormulaData::CGeneratedFormulaData( const TFormulaList &formulas, const std::function< bool( const QString &formula ) > &beenCreated )
{
    for ( auto &&jj : formulas )
    {
        addFormula( jj, beenCreated );
    }
    sortByName();
}

void CGeneratedFormulaData::newArray()
{
    fJsonArrays.push_back( {} );
    fJSONSize = { 0, 0 };
}

QJsonArray &CGeneratedFormulaData::jsonArray()
{
    if ( fJsonArrays.empty() )
    {
        newArray();
    }
    return fJsonArrays.back();
}

bool CGeneratedFormulaData::operator==( const CGeneratedFormulaData &rhs ) const
{
    if ( fNumErrors != rhs.fNumErrors )
        return false;
    if ( fNumToRender != rhs.fNumToRender )
        return false;
    if ( fNumTotal != rhs.fNumTotal )
        return false;

    if ( fAllFormulas != rhs.fAllFormulas )
        return false;

    if ( fByNameList.size() != rhs.fByNameList.size() )
        return false;

    auto ii = fByNameList.begin();
    auto jj = rhs.fByNameList.begin();
    for ( ; ( ii != fByNameList.end() ) && ( jj != rhs.fByNameList.end() ); ++ii, ++jj )
    {
        if ( **ii != **jj )
            return false;
    }
    return true;
}

void CGeneratedFormulaData::addFormula( const TFormula &formula, const std::function< bool( const QString &formula ) > &beenCreated )
{
    auto tex = formula->formulas();
    auto formulaString = NUtilities::joinFormulas( tex );
    if ( !formulaString.has_value() )
        return;

    auto pos = fAllFormulas.find( formulaString.value() );
    if ( pos != fAllFormulas.end() )
        return;

    fAllFormulas.insert( formulaString.value() );
    fByNameList.emplace_back( formula );
    fNumTotal++;
    if ( !beenCreated( formulaString.value() ) )
    {
        fNumToRender++;
    }
}

void CGeneratedFormulaData::addSVG( QJsonObject &obj, const std::optional< QByteArray > &svg, const std::optional< QDateTime > &renderedDate )
{
    if ( !svg.has_value() || svg.value().isEmpty() )
        return;

    auto base64 = svg.value().toBase64();
    obj.insert( "svg", QJsonValue::fromVariant( base64 ) );
    auto dt = renderedDate.has_value() ? renderedDate.value() : QDateTime::currentDateTime();
    obj.insert( "renderDate", QJsonValue::fromVariant( dt ) );

    if ( obj.contains( "error" ) )
        fNumErrors++;
    jsonArray().append( obj );

    if ( ( fJSONSize.first % 9 ) == 0 )
    {
        QJsonDocument doc;
        doc.setArray( jsonArray() );
        fJSONSize.second = doc.toJson( QJsonDocument::Indented ).length();   // will be slightly off, but close enough

        if ( fJSONSize.second >= ( 45ULL * 1024ULL * 1024ULL ) )
        {
            newArray();
        }
    }
    fJSONSize.first++;
}

void CGeneratedFormulaData::sortByName()
{
    fByNameList.sort(   //
        []( const TFormula &lhs, const TFormula &rhs )   //
        {   //
            return lhs->name() < rhs->name();
        } );
}

