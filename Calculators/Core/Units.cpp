#include "Utilities.h"
#include <QString>
#include <QObject>

namespace NUtilities
{
    namespace NUnitStrings
    {
        QString volumeUnit( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal;
            if ( imperial )
            {
                if ( tex )
                    return QObject::tr( "ft^3", "volumeUnit" );
                else if ( useAbbreviations )
                    return QObject::tr( "cu ft", "volumeUnit" );
                else
                    return QObject::tr( "cubic feet", "volumeUnit" );
            }
            else
            {
                if ( useAbbreviations )
                    return QObject::tr( "l", "volumeUnit" );
                else
                    return QObject::tr( "liters", "volumeUnit" );
            }
        }

        QString weightUnit( bool imperial, bool useAbbreviations, bool /*tex*/ )
        {
            QString retVal;
            if ( imperial )
            {
                if ( useAbbreviations )
                    return QObject::tr( "lbs", "weightUnit" );
                else
                    return QObject::tr( "pounds", "weightUnit" );
            }
            else
            {
                if ( useAbbreviations )
                    return QObject::tr( "kg", "weightUnit" );
                else
                    return QObject::tr( "kilograms", "weightUnit" );
            }
        }

        QString depthUnit( bool imperial, bool seaWater, bool useAbbreviations, bool /*tex*/ )
        {
            if ( imperial )
            {
                if ( seaWater )
                {
                    if ( useAbbreviations )
                        return QObject::tr( "fsw", "depthUnit" );
                    else
                        return QObject::tr( "feet sea water", "depthUnit" );
                }
                else
                {
                    if ( useAbbreviations )
                        return QObject::tr( "ffw", "depthUnit" );
                    else
                        return QObject::tr( "feet fresh water", "depthUnit" );
                }
            }
            else
            {
                if ( seaWater )
                {
                    if ( useAbbreviations )
                        return QObject::tr( "msw", "depthUnit" );
                    else
                        return QObject::tr( "meters sea water", "depthUnit" );
                }
                else
                {
                    if ( useAbbreviations )
                        return QObject::tr( "mfw", "depthUnit" );
                    else
                        return QObject::tr( "meters fresh water", "depthUnit" );
                }
            }
        }

        QString lengthUnit( bool imperial, bool useAbbreviations, bool /*tex*/ )
        {
            if ( imperial )
            {
                if ( useAbbreviations )
                    return QObject::tr( "ft", "lengthUnit" );
                else
                    return QObject::tr( "feet", "lengthUnit" );
            }
            else
            {
                if ( useAbbreviations )
                    return QObject::tr( "m", "lengthUnit" );
                else
                    return QObject::tr( "meters", "lengthUnit" );
            }
        }

        QString pressureUnit( bool imperial, bool /*useAbbreviations*/, bool /*tex*/ )
        {
            return imperial ? QObject::tr( "PSI", "pressureUnit" ) : QObject::tr( "BAR", "pressureUnit" );
        }

        QString atmosphereUnit( bool imperial, bool /*useAbbreviations*/, bool /*tex*/ )
        {
            return imperial ? QObject::tr( "ATM", "pressureUnit" ) : QObject::tr( "BAR", "pressureUnit" );
        }

        QString tempUnit( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(^{\circ})__" : "\u00B0";
            if ( imperial )
            {
                if ( useAbbreviations )
                    retVal += QObject::tr( "F", "tempUnit" );
                else
                    retVal += QObject::tr( "Fahrenheit", "tempUnit" );
            }
            else
            {
                if ( useAbbreviations )
                    retVal += QObject::tr( "C", "tempUnit" );
                else
                    retVal += QObject::tr( "Celsius", "tempUnit" );
            }
            return retVal;
        }

        QString absZeroTempUnit( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal;

            retVal += tex ? R"__(^{\circ})__" : "\u00B0";
            if ( imperial )
            {
                if ( useAbbreviations )
                    retVal += QObject::tr( "R", "absZeroTempUnit" );
                else
                    retVal += QObject::tr( "Rankine", "absZeroTempUnit" );
            }
            else
            {
                if ( useAbbreviations )
                    retVal += QObject::tr( "K", "absZeroTempUnit" );
                else
                    retVal += QObject::tr( "Kelvin", "absZeroTempUnit" );
            }
            return retVal;
        }

        QString percentUnit( bool /*imperial*/, bool /*useAbbreviations*/, bool tex )
        {
            QString retVal = tex ? R"__(\%)__" : "%";
            return retVal;
        }

        QString timeUnit( bool /*imperial*/, bool /*useAbbreviations*/, bool /*tex*/ )
        {
            QString retVal = QObject::tr( "minutes" );
            return retVal;
        }

        // kcal/(kg*minute)
        QString metUnits( bool useAbbreviations, bool tex )
        {
            auto retVal = QString( tex ? R"__(\frac{%1}{%2 \times %3})__" : "%1/(%2*%3)" );
            retVal = retVal.arg( energyUnit( false, useAbbreviations, tex ) ).arg( NUnitStrings::weightUnit( false, useAbbreviations, tex ) ).arg( NUnitStrings::timeUnit( false, true, true ) );
            return retVal;
        }

        QString idealGasConstantUnit( bool imperial, bool useAbbreviations, bool tex )
        {
            QString format;
            // r = pv/nt
            if ( tex )
                format = QString( R"__(\frac{%2 \times %3}{%4 \times %5})__" );
            else
                format = QString( R"__((%2)x(%3)/(%4)x(%5))__" );
            return format   //
                .arg( pressureUnit( imperial, useAbbreviations, tex ) )   //
                .arg( volumeUnit( imperial, useAbbreviations, tex ) )   //
                .arg( molesUnit( imperial, useAbbreviations, tex ) )
                .arg( absZeroTempUnit( imperial, useAbbreviations, tex ) );
        }

        QString molesUnit( bool /*imperial*/, bool /*useAbbreviations*/, bool /*tex*/ )
        {
            return QObject::tr( "moles" );
        }

        QString energyUnit( bool /*imperial*/, bool useAbbreviations, bool /*tex*/ )
        {
            QString retVal;
            if ( useAbbreviations )
                retVal += QObject::tr( "kcal", "caloriesUnit" );
            else
                retVal += QObject::tr( "Calories", "caloriesUnit" );
            return retVal;
        }

        QString pressurePerMinuteUnit( bool imperial, bool useAbbreviations, bool tex )
        {
            return ratio( NUnitStrings::pressureUnit( imperial, useAbbreviations, tex ), NUnitStrings::timeUnit( imperial, useAbbreviations, tex ), tex );
        }

        QString volumePerMinuteUnit( bool imperial, bool useAbbreviations, bool tex )
        {
            return ratio( NUnitStrings::volumeUnit( imperial, useAbbreviations, tex ), NUnitStrings::timeUnit( imperial, useAbbreviations, tex ), tex );
        }
    }
}
