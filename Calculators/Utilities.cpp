#include "Utilities.h"
#include <QString>
#include <QObject>

namespace NUtilities
{
    namespace NUnitStrings
    {
        QString getUnitLabel( bool imperial, bool seaWater, EUnit unit, bool useAbbreviations, bool tex )
        {
            switch ( unit )
            {
                case EUnit::eNone:
                    return {};
                case EUnit::eVolume:
                    return volumeUnit( imperial, useAbbreviations, tex );
                case EUnit::eWeight:
                    return weightUnit( imperial, useAbbreviations, tex );
                case EUnit::eLength:
                    return lengthUnit( imperial, useAbbreviations, tex );
                case EUnit::eDepth:
                    return depthUnit( imperial, seaWater, useAbbreviations, tex );
                case EUnit::ePressure:
                    return pressureUnit( imperial, useAbbreviations, tex );
                case EUnit::eTemperature:
                    return tempUnit( imperial, useAbbreviations, tex );
                case EUnit::eAbsZeroTemperature:
                    return absZeroTempUnit( imperial, useAbbreviations, tex );
                case EUnit::ePercent:
                    return percentUnit( tex );
                default:
                    return {};
            }
        };

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

        QString tempUnit( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"(^{\circ})" : "\u00B0";
            if ( imperial )
            {
                if ( useAbbreviations )
                    retVal += QObject::tr( "F", "tempUnit" );
                else
                    retVal += QObject::tr( "fahrenheit", "tempUnit" );
            }
            else
            {
                if ( useAbbreviations )
                    retVal += QObject::tr( "C", "tempUnit" );
                else
                    retVal += QObject::tr( "celsius", "tempUnit" );
            }
            return retVal;
        }

        QString absZeroTempUnit( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"(^{\circ})" : "\u00B0";
            if ( imperial )
            {
                if ( useAbbreviations )
                    retVal += QObject::tr( "R", "absZeroTempUnit" );
                else
                    retVal += QObject::tr( "ankine", "absZeroTempUnit" );
            }
            else
            {
                if ( useAbbreviations )
                    retVal += QObject::tr( "K", "absZeroTempUnit" );
                else
                    retVal += QObject::tr( "kelvin", "absZeroTempUnit" );
            }
            return retVal;
        }

        QString percentUnit( bool tex )
        {
            QString retVal = tex ? R"(\%)" : "%";
            return retVal;
        }

        QString pressurePerTemp( bool imperial, bool useAbbreviations, bool tex )
        {
            auto retVal = tex ? QObject::tr( "%1 (\frac{%2}{%3}", "pressurePerTemp" ) : QObject::tr( "%1 (%2/%3)", "pressurePerTemp" );
            retVal = retVal.arg( NConstants::pressurePerTemp( imperial ) ).arg( NUnitStrings::pressureUnit( imperial, useAbbreviations, tex ) ).arg( NUnitStrings::tempUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString weightOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex )
        {
            auto retVal = tex ? QObject::tr( R"(%1 \frac{%2}{%3})", "weightOfWater" ) : QObject::tr( "%1 %2/%3 of water", "weightOfWater" );
            auto weightOfWater = NUtilities::doubleToString( NConstants::weightOfWater( imperial, seaWater ), 2 );
            retVal = retVal.arg( weightOfWater ).arg( weightUnit( imperial, useAbbreviations, tex ) ).arg( volumeUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString idealGasConstant( bool imperial, bool useAbbreviations, bool tex )
        {
            if ( tex )
                return QObject::tr( R"__(p(%1) \times V(%2)=n(moles)\times%3\timesT(%4))__", "idealGasConstant" ).arg( pressureUnit( imperial, useAbbreviations, tex ) ).arg( volumeUnit( imperial, useAbbreviations, tex ) ).arg( NConstants::idealGasConstant( imperial ) ).arg( tempUnit( imperial, useAbbreviations, tex ), "" );
            else
                return QObject::tr( "%1 (%2)x(%3)/(n moles)x(%4)", "idealGasConstant" ).arg( NConstants::idealGasConstant( imperial ) ).arg( volumeUnit( imperial, useAbbreviations, tex ) ).arg( pressureUnit( imperial, useAbbreviations, tex ) ).arg( tempUnit( imperial, useAbbreviations, tex ) );
        }

        QString percentN2AtSurface( bool tex )
        {
            return QObject::tr( R"__(%1%2)__", "percentN2AtSurface" ).arg( NConstants::percentN2AtSurface() ).arg( percentUnit( tex ) );
        }

        QString percentO2AtSurface( bool tex )
        {
            return QObject::tr( R"__(%1%2)__", "percentO2AtSurface" ).arg( NConstants::percentO2AtSurface() ).arg( percentUnit( tex ) );
        }

        QString depthToSingleAtmosphere( bool imperial, bool seaWater, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( doubleToString( NConstants::depthToSingleAtmosphere( imperial, seaWater ), 2 ) ).arg( lengthUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString feetToMeters( bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1\frac{%2}{%3})__" : "%1 (%2/%3)";
            retVal = retVal.arg( doubleToString( NConstants::feetToMeters(), 3 ) ).arg( lengthUnit( false, useAbbreviations, true ) ).arg( lengthUnit( true, useAbbreviations, true ) );
            return retVal;
        }

        QString freshWaterToSeaWater( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1\frac{%2}{%3})__" : "%1 (%2/%3)";
            retVal = retVal.arg( doubleToString( NConstants::freshWaterToSeaWater(), 2 ) ).arg( depthUnit( imperial, false, useAbbreviations, tex ) ).arg( depthUnit( imperial, true, useAbbreviations, tex ) );
            return retVal;
        }
    }

    namespace NConstants
    {
        double absZero( bool imperial )
        {
            return imperial ? 460.0 : 273.0;
        }

        double pressurePerTemp( bool imperial )
        {
            return imperial ? 5 : 0.6;
        }

        double weightOfWater( bool imperial, bool seaWater )
        {
            auto retVal = imperial ? ( seaWater ? 62.4 : 64.0 ) : ( seaWater ? 1.0 : 1.03 );
            return retVal;
        }

        double pressureOffset( bool imperial )
        {
            return imperial ? 14.7 : 1.0;
        }

        double depthToSingleAtmosphere( bool imperial, bool seaWater )
        {
            auto retVal = imperial ? ( seaWater ? 33.0 : 34.0 ) : ( seaWater ? 10.0 : 10.3 );
            return retVal;
        }

        double idealGasConstant( bool imperial )
        {
            if ( imperial )
            {
                return 10.731577089016;
            }
            else
            {
                return 0.08206;
            }
        }

        double freshWaterToSeaWater()
        {
            return 1.03;
        }

        double feetToMeters()
        {
            return 0.3048;
        }

        double percentN2AtSurface()
        {
            return 0.79;
        }

        double percentO2AtSurface()
        {
            return 0.21;
        }
    }

    double toAbsZeroBasedTemp( bool imperial, double temp )
    {
        return temp + NConstants::absZero( imperial );
    }

    double fromAbsZeroBasedTemp( bool imperial, double temp )
    {
        return temp - NConstants::absZero( imperial );
    }

    QString pressureFromDepthFormula( const QString ataFieldName, const QString depthToSingleATMFieldName, const QString &depthFieldName )
    {
        return QString( R"__(<%1>=\frac{<%2>}{<%3>} + 1)__" ).arg( ataFieldName ).arg( depthFieldName ).arg( depthToSingleATMFieldName );
    }

    double pressureFromDepth( bool imperial, bool seaWater, double depth )
    {
        auto depthOfATM = NConstants::depthToSingleAtmosphere( imperial, seaWater );
        return ( depth / depthOfATM ) + 1;
    }

    QString depthFromPressureFormula( const QString ataFieldName, const QString depthToSingleATMFieldName, const QString &depthFieldName )
    {
        return QString( R"__(<%2>=(<%1>-1)*<%3>)__" ).arg( ataFieldName ).arg( depthFieldName ).arg( depthToSingleATMFieldName );
    }

    double depthFromPressure( bool imperial, bool seaWater, double pressure )
    {
        auto depthOfATM = NConstants::depthToSingleAtmosphere( imperial, seaWater );
        return ( pressure - 1 ) * depthOfATM;
    }

    void calculateDepthToFromPressure( bool imperial, bool seaWater, TOptionalDouble &pressure, TOptionalDouble &depth )
    {
        if ( !pressure.has_value() && !depth.has_value() )
            return;

        if ( pressure.has_value() && depth.has_value() )
            return;

        if ( !depth.has_value() )
        {
            depth = depthFromPressure( imperial, seaWater, pressure.value() );
        }
        else if ( !pressure.has_value() )
        {
            pressure = pressureFromDepth( imperial, seaWater, depth.value() );
        }
    }

    std::size_t numEmpty( const TOptionalDoubleVector &values )
    {
        std::size_t numEmpty = 0;

        for ( auto &&value : values )
        {
            numEmpty += value.has_value() ? 0 : 1;
        }
        return numEmpty;
    }

    bool valuesValid( const TOptionalDoubleVector &values, bool checkNumEmpty )
    {
        if ( values.empty() )
            return false;
        if ( checkNumEmpty && numEmpty( values ) != 1 )
            return false;
        return true;
    }

    QString doubleToString( const TOptionalDouble &value, int numDecimal )
    {
        QString retVal;
        if ( value.has_value() )
            retVal = QString( "%1" ).arg( value.value(), 0, 'f', numDecimal );
        return retVal;
    }
}
