#include "Formula.h"
#include "Utilities.h"
#include "T42-Qt6MathJax/include/Qt6MathJax.h"

#include <QString>

namespace NUtilities
{
    SFormula::SFormula( const QString &name, const QString &formula, bool imperial, bool seaWater, const TOptionalNameValuePair &nameValuePair /*= {} */ ) :
        fName( name ),
        fFormula( formula ),
        fImperial( imperial ),
        fSeaWater( seaWater ),
        fNameValuePair( nameValuePair )
    {
    }

    SFormula::SFormula( const SFormula &rhs, const TOptionalNameValuePair &nameValuePair ) :
        SFormula( rhs )
    {
        fNameValuePair = nameValuePair;
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

        if ( fNameValuePair.has_value() )
            retVal += "-" + fNameValuePair.value().first + "=" + NUtilities::doubleToString( fNameValuePair.value().second, 2 );

        return retVal;
    }

    QString SFormula::cleanedFormula() const
    {
        return NTowel42::cleanupFormula( formula() );
    }

}
