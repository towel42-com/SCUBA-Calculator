#include "Formula.h"
#include "Utilities.h"
#include "VariableInfo.h"
#include "FormulaString.h"

#include "T42-MathJaxQt6/include/MathJaxQt6.h"

#include <QString>

namespace NUtilities
{
    SFormula::SFormula( const QString &name, const TFormulaString &formula, bool imperial, bool seaWater, const TOptionalVariableValuePairVector &nameValuePair /*= {} */ ) :
        fName( name ),
        fFormulas( { formula } ),
        fImperial( imperial ),
        fSeaWater( seaWater ),
        fNameValuePairs( nameValuePair )
    {
    }

    SFormula::SFormula( const QString &name, const TFormulaStringList &formulas, bool imperial, bool seaWater, const TOptionalVariableValuePairVector &nameValuePair /*= {} */ ) :
        fName( name ),
        fFormulas( formulas ),
        fImperial( imperial ),
        fSeaWater( seaWater ),
        fNameValuePairs( nameValuePair )
    {
    }

    SFormula::SFormula( const SFormula &rhs, const TOptionalVariableValuePairVector &nameValuePair ) :
        SFormula( rhs )
    {
        fNameValuePairs = nameValuePair;
    }

    SFormula::SFormula()
    {
    }

    bool SFormula::operator<( const SFormula &rhs ) const
    {
        return name() < rhs.name();
    }

    QString SFormula::name() const
    {
        auto retVal = fName;
        if ( fImperial )
            retVal += "-unit=imperial";
        else
            retVal += "-unit=metric";
        if ( fSeaWater )
            retVal += "-water=sea";
        else
            retVal += "-water=fresh";

        if ( fNameValuePairs.has_value() )
        {
            for ( auto &&ii : fNameValuePairs.value() )
            {
                retVal += "-" + ii.first->name() + "=" + NUtilities::doubleToString( ii.second, 2 );
            }
        }
        return retVal;
    }

    QString SFormula::formula() const
    {
        auto retVal = NUtilities::joinFormulas( imperial(), seaWater(), cleanedFormulas() );
        if ( !retVal.has_value() )
            return {};
        return retVal.value();
    }

    TFormulaStringList SFormula::cleanedFormulas() const
    {
        auto retVal = this->formulas();
        for ( auto &&ii : retVal )
        {
            ii->cleanupFormula();
        }
        return retVal;
    }

    void SFormula::setFormula( const QString &formula )
    {
        fFormulas.clear();
        fFormulas.emplace_back( std::make_shared< CFormulaString >( formula ) );
    }

    bool SFormula::operator==( const SFormula &rhs ) const
    {
        //if ( fName != rhs.fName )
        //    return false;
        if ( fFormulas != rhs.fFormulas )
            return false;
        if ( fImperial != rhs.fImperial )
            return false;
        if ( fSeaWater != rhs.fSeaWater )
            return false;

        if ( fNameValuePairs.has_value() != rhs.fNameValuePairs.has_value() )
            return false;

        if ( fNameValuePairs.has_value() /* && rhs.fNameValuePair.has_value() */ )
        {
            if ( fNameValuePairs.value().size() != rhs.fNameValuePairs.value().size() )
                return false;

            auto ii = fNameValuePairs.value().begin();
            auto jj = rhs.fNameValuePairs.value().begin();
            for ( ; ii != fNameValuePairs.value().end() && jj != rhs.fNameValuePairs.value().end(); ++ii, ++jj )
            {
                if ( ( *ii ).first != ( *jj ).first )
                    return false;

                if ( ( *ii ).second.has_value() != ( *jj ).second.has_value() )
                    return false;

                if ( ( *ii ).second != ( *jj ).second )
                {
                    return false;
                }
            }
        }
        return true;
    }

}
