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

        QString pressureChangePerDegreeChange( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1 \frac{%2}{%3})__" : "%1 (%2/%3)";
            retVal = retVal.arg( NConstants::pressureChangePerDegreeChange( imperial ) ).arg( NUnitStrings::pressureUnit( imperial, useAbbreviations, tex ) ).arg( NUnitStrings::tempUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString lbsPerKgs( bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1 \frac{%2}{%3})__" : "%1 (%2/%3)";
            retVal = retVal.arg( NConstants::lbsPerKGs() ).arg( NUnitStrings::weightUnit( true, useAbbreviations, tex ) ).arg( NUnitStrings::weightUnit( false, useAbbreviations, tex ) );
            return retVal;
        }

        QString kgsPerLbs( bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1 \frac{%2}{%3})__" : "%1 (%2/%3)";
            retVal = retVal.arg( NConstants::kgsPerLbs() ).arg( NUnitStrings::weightUnit( false, useAbbreviations, tex ) ).arg( NUnitStrings::weightUnit( true, useAbbreviations, tex ) );
            return retVal;
        }

        // kcal/(kg*minute)
        QString metUnit( bool useAbbreviations, bool tex )
        {
            auto retVal = QString( tex ? R"__(\frac{%1}{%2 \times %3})__" : "%1/(%2*%3)" );
            retVal = retVal.arg( energyUnit( false, useAbbreviations, tex ) ).arg( NUnitStrings::weightUnit( false, useAbbreviations, tex ) ).arg( NUnitStrings::timeUnit( false, true, true ) );
            return retVal;
        }

        QString scubaMET( bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( NConstants::baseMETForScuba() ).arg( NUnitStrings::metUnit( useAbbreviations, tex ) );
            return retVal;
        }

        QString flowRateUnit( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(\frac{%2}{%3})__" : "(%2/%3)";
            retVal = retVal.arg( pressureUnit( imperial, useAbbreviations, tex ) ).arg( NUnitStrings::timeUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString volumePerMinuteUnit( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(\frac{%2}{%3})__" : "(%2/%3)";
            retVal = retVal.arg( volumeUnit( imperial, useAbbreviations, tex ) ).arg( NUnitStrings::timeUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString fillRateO2( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( NConstants::fillRateO2( imperial ) ).arg( NUnitStrings::flowRateUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString fillRateAir( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( NConstants::fillRateAir( imperial ) ).arg( NUnitStrings::flowRateUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString weightOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1 \frac{%2}{%3})__" : "%1 (%2/%3)";
            auto weightOfWater = doubleToString( NConstants::weightOfWater( imperial, seaWater ), 2 );
            retVal = retVal.arg( weightOfWater ).arg( weightUnit( imperial, useAbbreviations, tex ) ).arg( volumeUnit( imperial, useAbbreviations, tex ) );
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
            //if ( imperial )
            //{
            //    if ( useAbbreviations )
            //        retVal += QObject::tr( "kJ", "caloriesUnit" );
            //    else
            //        retVal += QObject::tr( "KiloJoules", "caloriesUnit" );
            //}
            //else
            //{
            if ( useAbbreviations )
                retVal += QObject::tr( "kcal", "caloriesUnit" );
            else
                retVal += QObject::tr( "Calories", "caloriesUnit" );
            //}
            return retVal;
        }

        QString idealGasConstant( bool imperial, bool useAbbreviations, bool tex )
        {
            // pv = nrt
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( doubleToString( NConstants::idealGasConstant( imperial ), 5 ) ).arg( idealGasConstantUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString percentN2AtSurface( bool imperial, bool useAbbreviations, bool tex )
        {
            return QObject::tr( R"__(%1%2)__", "percentN2AtSurface" ).arg( NConstants::percentN2AtSurface() ).arg( percentUnit( imperial, useAbbreviations, tex ) );
        }

        QString percentO2AtSurface( bool imperial, bool useAbbreviations, bool tex )
        {
            return QObject::tr( R"__(%1%2)__", "percentO2AtSurface" ).arg( NConstants::percentO2AtSurface() ).arg( percentUnit( imperial, useAbbreviations, tex ) );
        }

        QString depthToSingleAtmosphere( bool imperial, bool seaWater, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( doubleToString( NConstants::depthToSingleAtmosphere( imperial, seaWater ), 2 ) ).arg( lengthUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString feetToMeters( bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1 \frac{%2}{%3})__" : "%1 (%2/%3)";
            retVal = retVal.arg( doubleToString( NConstants::feetPerMeters(), 3 ) ).arg( lengthUnit( false, useAbbreviations, tex ) ).arg( lengthUnit( true, useAbbreviations, tex ) );
            return retVal;
        }

        QString metersToFeet( bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1 \frac{%2}{%3})__" : "%1 (%2/%3))";
            retVal = retVal.arg( doubleToString( NConstants::metersPerFeet(), 3 ) ).arg( lengthUnit( true, useAbbreviations, tex ) ).arg( lengthUnit( false, useAbbreviations, tex ) );
            return retVal;
        }

        QString freshWaterToSeaWater( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1 \frac{%2}{%3})__" : "%1 (%2/%3)";
            retVal = retVal.arg( doubleToString( NConstants::freshWaterToSeaWater(), 2 ) ).arg( depthUnit( imperial, false, useAbbreviations, tex ) ).arg( depthUnit( imperial, true, useAbbreviations, tex ) );
            return retVal;
        }

        QString psiToBar( bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1\frac{%2}{%3})__" : "%1 (%2/%3)";
            retVal = retVal.arg( doubleToString( NConstants::psiToBar(), 3 ) ).arg( pressureUnit( true, useAbbreviations, tex ) ).arg( pressureUnit( false, useAbbreviations, tex ) );
            return retVal;
        }

        QString barToPSI( bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1\frac{%2}{%3})__" : "%1 (%2/%3)";
            retVal = retVal.arg( doubleToString( NConstants::barToPSI(), 1 ) ).arg( pressureUnit( false, useAbbreviations, tex ) ).arg( pressureUnit( true, useAbbreviations, tex ) );
            return retVal;
        }

        QString absZeroOffset( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( doubleToString( NConstants::absZeroOffset( imperial ), 0 ) ).arg( tempUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString pressureOffset( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( doubleToString( NConstants::pressureOffset( imperial ), 1 ) ).arg( pressureUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString cubicFeetToLiters( bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1\frac{%2}{%3})__" : "%1 (%2/%3)";
            retVal = retVal.arg( doubleToString( NConstants::cubicFeetPerLiter(), 4 ) ).arg( volumeUnit( true, useAbbreviations, tex ) ).arg( volumeUnit( false, useAbbreviations, tex ) );
            return retVal;
        }

        QString litersToCubicFeet( bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1\frac{%2}{%3})__" : "%1 (%2/%3)";
            retVal = retVal.arg( doubleToString( NConstants::litersPerCubicFeet(), 4 ) ).arg( volumeUnit( false, useAbbreviations, tex ) ).arg( volumeUnit( true, useAbbreviations, tex ) );
            return retVal;
        }
    }

    namespace NConstants
    {
        const char *kPressurePerDegreeConstFieldName = "pressurePerDegree";
        const char *kWeightOfWaterConstFieldName = "weightOfWater";
        const char *kIdealGasConstantFieldName = "idealGasConstant";
        const char *kFN2AtSurfaceFieldName = "FN2AtSurface";
        const char *kFO2AtSurfaceFieldName = "FO2AtSurface";
        const char *kDepthToSingleATMConstFieldName = "depthToSingleAtmosphere";
        const char *kFeetToMetersConstFieldName = "feetToMeters";
        const char *kMetersToFeetConstFieldName = "metersToFeet";
        const char *kLbsPerKgsConstFieldName = "lbsToKgs";
        const char *kKgsPerLbsConstFieldName = "kgsToLbs";
        const char *kFreshWaterToSeaWaterConstFieldName = "freshWaterToSeaWater";
        const char *kPSIToBarConstFieldName = "psiToBar";
        const char *kBarToPSIConstFieldName = "barToPSI";
        const char *kAbsZeroOffsetConstFieldName = "absZeroOffset";
        const char *kPressureOffsetConstFieldName = "pressureOffset";
        const char *kBaseMETofSCUBAConstFieldName = "baseMETOfScuba";
        const char *kFillRateAirConstFieldName = "fillRateAir";
        const char *kFillRateO2ConstFieldName = "fillRateO2";
        const char *kCubicFeetToLitersFieldName = "cubicFeetToLiters";
        const char *kLitersToCubicFeetFieldName = "litersToCubicFeet";

        double absZeroOffset( bool imperial )
        {
            return imperial ? 460.0 : 273.0;
        }

        double pressureChangePerDegreeChange( bool imperial )
        {
            return imperial ? 5 : 0.6;
        }

        double weightOfWater( bool imperial, bool seaWater )
        {
            auto retVal = imperial ? 62.4 : 1.0;
            retVal *= seaWater ? 1.0 : freshWaterToSeaWater();
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

        double metersPerFeet()
        {
            return 0.3048;
        }

        double feetPerMeters()
        {
            return 1 / metersPerFeet();
        }

        double percentN2AtSurface()
        {
            return 1.0 - percentO2AtSurface();
        }

        double percentO2AtSurface()
        {
            return 0.209;
        }

        double barToPSI()
        {
            return 14.5037738;
        }

        double psiToBar()
        {
            return 1 / barToPSI();
        }

        double lbsPerKGs()
        {
            return 2.20462;
        }

        double kgsPerLbs()
        {
            return 1 / lbsPerKGs();
        }

        double cubicFeetPerLiter()
        {
            return 1 / litersPerCubicFeet();
        }

        double litersPerCubicFeet()
        {
            return 28.3168;
        }

        // the base unit of 7.0 is
        // kcal/(kg*hour)
        // we return kcal/(kg*min) by dividing by 60
        double baseMETForScuba()
        {
            return 7.0 / 60.0;
        }

        double fillRateAir( bool imperial )
        {
            double retVal = 600;   // psi/min
            if ( !imperial )
            {
                retVal = NUtilities::NConversions::psiToBar( retVal );
            }
            return retVal;
        }

        double fillRateO2( bool imperial )
        {
            double retVal = 60;   // psi/min
            if ( !imperial )
            {
                retVal = NUtilities::NConversions::psiToBar( retVal );
            }
            return retVal;
        }
    }

    QString descForType( EVariableType type )
    {
        switch ( type )
        {
            case EVariableType::eIntermediate:
            case EVariableType::eVariable:
                return {};
            case EVariableType::ePressurePerDegreeConst:
                return QObject::tr( "Pressure Change per Degree Change", "descForType" );
            case EVariableType::eWeightOfWaterConst:
                return QObject::tr( "Weight of Water", "descForType" );
            case EVariableType::eIdealGasConst:
                return QObject::tr( "Ideal Gas Constant", "descForType" );
            case EVariableType::eFN2AtSurfaceConst:
                return QObject::tr( "Partial Pressure of Nitrogen at Surface", "descForType" );
            case EVariableType::eFO2AtSurfaceConst:
                return QObject::tr( "Partial Pressure of Oxygen at Surface", "descForType" );
            case EVariableType::eDepthToSingleATMConst:
                return QObject::tr( "Depth of a Single Atmosphere", "descForType" );
            case EVariableType::eFeetToMetersConst:
                return QObject::tr( "How many Feet to a Meter", "descForType" );
            case EVariableType::eMetersToFeetConst:
                return QObject::tr( "How many Meters to a Foot", "descForType" );
            case EVariableType::eLbsPerKgsConst:
                return QObject::tr( "How many Pounds per Kilogram", "descForType" );
            case EVariableType::eKgsPerLbsConst:
                return QObject::tr( "How many Kilograms per Pound", "descForType" );
            case EVariableType::eFreshWaterToSeaWaterConst:
                return QObject::tr( "How much depth of Freshwater to Seawater", "descForType" );
            case EVariableType::ePSIToBarConst:
                return QObject::tr( "PSI to Bar", "descForType" );
            case EVariableType::eBarToPSIConst:
                return QObject::tr( "Bar to PSI", "descForType" );
            case EVariableType::eAbsZeroOffsetConst:
                return QObject::tr( "Absolute Zero Offset", "descForType" );
            case EVariableType::ePressureOffsetConst:
                return QObject::tr( "Pressure Offset", "descForType" );
            case EVariableType::eBaseMETofSCUBAConst:
                return QObject::tr( "Base MET Value for SCUBA", "descForType" );
            case EVariableType::eFillRateAirConst:
                return QObject::tr( "Fill Rate for Air", "descForType" );
            case EVariableType::eFillRateO2Const:
                return QObject::tr( "Fill Rate for Pure O2", "descForType" );
            case EVariableType::eCubicFeetToLitersConst:
                return QObject::tr( "Cubic Feet to Liters", "descForType" );
            case EVariableType::eLitersToCubicFeetConst:
                return QObject::tr( "Liters to Cubic Feet", "descForType" );
        };
        return {};
    }

    QString fieldNameForType( EVariableType type )
    {
        switch ( type )
        {
            case EVariableType::eIntermediate:
            case EVariableType::eVariable:
                return {};
            case EVariableType::ePressurePerDegreeConst:
                return NConstants::kPressurePerDegreeConstFieldName;
            case EVariableType::eWeightOfWaterConst:
                return NConstants::kWeightOfWaterConstFieldName;
            case EVariableType::eIdealGasConst:
                return NConstants::kIdealGasConstantFieldName;
            case EVariableType::eFN2AtSurfaceConst:
                return NConstants::kFN2AtSurfaceFieldName;
            case EVariableType::eFO2AtSurfaceConst:
                return NConstants::kFO2AtSurfaceFieldName;
            case EVariableType::eDepthToSingleATMConst:
                return NConstants::kDepthToSingleATMConstFieldName;
            case EVariableType::eFeetToMetersConst:
                return NConstants::kFeetToMetersConstFieldName;
            case EVariableType::eMetersToFeetConst:
                return NConstants::kMetersToFeetConstFieldName;
            case EVariableType::eLbsPerKgsConst:
                return NConstants::kLbsPerKgsConstFieldName;
            case EVariableType::eKgsPerLbsConst:
                return NConstants::kKgsPerLbsConstFieldName;
            case EVariableType::eFreshWaterToSeaWaterConst:
                return NConstants::kFreshWaterToSeaWaterConstFieldName;
            case EVariableType::ePSIToBarConst:
                return NConstants::kPSIToBarConstFieldName;
            case EVariableType::eBarToPSIConst:
                return NConstants::kBarToPSIConstFieldName;
            case EVariableType::eAbsZeroOffsetConst:
                return NConstants::kAbsZeroOffsetConstFieldName;
            case EVariableType::ePressureOffsetConst:
                return NConstants::kPressureOffsetConstFieldName;
            case EVariableType::eBaseMETofSCUBAConst:
                return NConstants::kBaseMETofSCUBAConstFieldName;
            case EVariableType::eFillRateAirConst:
                return NConstants::kFillRateAirConstFieldName;
            case EVariableType::eFillRateO2Const:
                return NConstants::kFillRateO2ConstFieldName;
            case EVariableType::eCubicFeetToLitersConst:
                return NConstants::kCubicFeetToLitersFieldName;
            case EVariableType::eLitersToCubicFeetConst:
                return NConstants::kLitersToCubicFeetFieldName;
        };
        return {};
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

    QString doubleToString( const TOptionalDouble &value, int numDecimal )
    {
        QString retVal;
        if ( value.has_value() )
            retVal = QString( "%1" ).arg( value.value(), 0, 'f', numDecimal );
        return retVal;
    }

    namespace NConversions
    {
        double toAbsZeroBasedTemp( bool imperial, double temp )
        {
            return temp + NConstants::absZeroOffset( imperial );
        }

        double fromAbsZeroBasedTemp( bool imperial, double temp )
        {
            return temp - NConstants::absZeroOffset( imperial );
        }

        QString barToPSIFormula( const QString &psiFieldName, const QString &barFieldName )
        {
            return QString( R"__(<%2>=<%1> \times <%3>)__" ).arg( barFieldName ).arg( psiFieldName ).arg( NConstants::kPSIToBarConstFieldName );
        }

        double barToPSI( double bar )
        {
            return bar * NConstants::barToPSI();
        }

        QString psiToBarFormula( const QString &psiFieldName, const QString &barFieldName )
        {
            return QString( R"__(<%1>=\frac{<%2>}{<%3>})__" ).arg( barFieldName ).arg( psiFieldName ).arg( NConstants::kPSIToBarConstFieldName );
        }

        double psiToBar( double psi )
        {
            return psi / NConstants::barToPSI();
        }

        QString depthToATAFormula( const QString &ataFieldName, const QString &depthFieldName )
        {
            return QString( R"__(<%1>=\frac{<%2>}{<%3>} + 1)__" ).arg( ataFieldName ).arg( depthFieldName ).arg( NConstants::kDepthToSingleATMConstFieldName );
        }

        double depthToATA( bool imperial, bool seaWater, double depth )
        {
            auto depthOfATM = NConstants::depthToSingleAtmosphere( imperial, seaWater );
            return ( depth / depthOfATM ) + 1;
        }

        QString ataToDepthFormula( const QString &ataFieldName, const QString &depthFieldName )
        {
            return QString( R"__(<%2>=(<%1>-1) \times <%3>)__" ).arg( ataFieldName ).arg( depthFieldName ).arg( NConstants::kDepthToSingleATMConstFieldName );
        }

        double ataToDepth( bool imperial, bool seaWater, double pressure )
        {
            auto depthOfATM = NConstants::depthToSingleAtmosphere( imperial, seaWater );
            return ( pressure - 1 ) * depthOfATM;
        }

        double depthFreshwaterToSeawater( double depthFW )
        {
            return depthFW / NConstants::freshWaterToSeaWater();
        }

        QString depthFreshwaterToSeawaterFormula( const QString &freshWaterFieldName, const QString &seaWaterFieldName )
        {
            return QString( R"__(<%2>=\frac{<%1>}{<%3>})__" ).arg( freshWaterFieldName ).arg( seaWaterFieldName ).arg( NConstants::kFreshWaterToSeaWaterConstFieldName );
        }

        double depthSeawaterToFreshwater( double depthSW )
        {
            return depthSW * NConstants::freshWaterToSeaWater();
        }

        QString depthSeawaterToFreshwaterFormula( const QString &freshWaterFieldName, const QString &seaWaterFieldName )
        {
            return QString( R"__(<%1>=<%2> \times <%3>)__" ).arg( freshWaterFieldName ).arg( seaWaterFieldName ).arg( NConstants::kFreshWaterToSeaWaterConstFieldName );
        }

        double feetToMeters( double feet )
        {
            return feet * NConstants::metersPerFeet();
        }

        QString feetToMetersFormula( const QString &feetFieldName, const QString &metersFieldName )
        {
            return QString( R"__(<%2>=<%1> \times <%3>)__" ).arg( feetFieldName ).arg( metersFieldName ).arg( NConstants::kFeetToMetersConstFieldName );
        }

        double metersToFeet( double meters )
        {
            return meters * NConstants::feetPerMeters();
        }

        QString metersToFeetFormula( const QString &feetFieldName, const QString &metersFieldName )
        {
            return QString( R"__(<%1>=<%2> \times <%3>)__" ).arg( feetFieldName ).arg( metersFieldName ).arg( NConstants::kMetersToFeetConstFieldName );
        }

        double pressureChangeForDegreeChange( bool imperial, double temperature )
        {
            return temperature * NConstants::pressureChangePerDegreeChange( imperial );
        }

        double degreeChangeForPressureChange( bool imperial, double pressure )
        {
            return pressure / NConstants::pressureChangePerDegreeChange( imperial );
        }

        QString pressureChangeForDegreeChangeFormula( const QString &tempFieldName, const QString &pressureFieldName )
        {
            return QString( R"__(<%2> = <%1> \times <%3>)__" ).arg( tempFieldName ).arg( pressureFieldName ).arg( NConstants::kPressurePerDegreeConstFieldName );
        }

        QString degreeChangeForPressureChangeFormula( const QString &tempFieldName, const QString &pressureFieldName )
        {
            return QString( R"__(<%1> = \frac{<%2>}{<%3>})__" ).arg( tempFieldName ).arg( pressureFieldName ).arg( NConstants::kPressurePerDegreeConstFieldName );
        }

        double lbsToKGs( double lbs )
        {
            return lbs * NConstants::kgsPerLbs();
        }

        double kgsToLbs( double kgs )
        {
            return kgs * NConstants::lbsPerKGs();
        }

        QString lbsToKGsFormula( const QString &lbsFieldName, const QString &kgsFieldName )
        {
            return QString( R"__(<%1> = <%2> \times <%3>)__" ).arg( kgsFieldName ).arg( lbsFieldName ).arg( NConstants::kKgsPerLbsConstFieldName );
        }

        QString kgsToLbsFormula( const QString &lbsFieldName, const QString &kgsFieldName )
        {
            return QString( R"__(<%1> = <%2> \times <%3>)__" ).arg( lbsFieldName ).arg( kgsFieldName ).arg( NConstants::kLbsPerKgsConstFieldName );
        }

        double farenheightToCelsius( double temp )
        {
            return ( temp - 32 ) * 5.0 / 9.0;
        }

        QString farenheightToCelsiusFormula( const std::optional< QString > &celsiusFieldName, const QString &farenheightFieldName )
        {
            auto retVal = QString( R"__((<%1> - 32 ) \times \frac{5%2}{9%3})__" ).arg( farenheightFieldName ).arg( NUnitStrings::tempUnit( false, true, true ) ).arg( NUnitStrings::tempUnit( true, true, true ) );
            if ( celsiusFieldName.has_value() )
            {
                retVal = QString( "<%1> = %2" ).arg( celsiusFieldName.value() ).arg( retVal );
            }
            return retVal;
        }

        double celsiusToFarenheight( double temp )
        {
            return ( temp * 9 / 5 ) + 32;
        }

        QString celsiusToFarenheightFormula( const QString &celsiusFieldName, const std::optional< QString > &farenheightFieldName )
        {
            auto retVal = QString( R"__((<%1> \times \frac{9%2}{5%3}) + 32)__" ).arg( celsiusFieldName ).arg( NUnitStrings::tempUnit( true, true, true ) ).arg( NUnitStrings::tempUnit( false, true, true ) );
            if ( farenheightFieldName.has_value() )
            {
                retVal = QString( "<%1> = %2" ).arg( farenheightFieldName.value() ).arg( retVal );
            }
            return retVal;
        }

        double cubicFeetToLiters( double cuft )
        {
            return cuft * NConstants::litersPerCubicFeet();
        }

        double litersToCubicFeet( double liters )
        {
            return liters * NConstants::cubicFeetPerLiter();
        }

        double sacToRMV( double sac, double volume, double pressure )
        {
            auto tankBaseline = volume / pressure;
            auto rmv = sac * tankBaseline;
            return rmv;
        }

        double rmvToSAC( double rmv, double volume, double pressure )
        {
            auto tankBaseline = volume / pressure;
            auto sac = rmv / tankBaseline;
            return sac;
        }

        QString cubicFeetToLitersFormula( const QString &cubicFeetFieldName, const QString &litersFieldName )
        {
            return QString( R"__(<%1> = <%2> \times <%3>)__" ).arg( litersFieldName ).arg( cubicFeetFieldName ).arg( NConstants::kLitersToCubicFeetFieldName );
        }

        QString litersToCubicFeetFormula( const QString &cubicFeetFieldName, const QString &litersFieldName )
        {
            return QString( R"__(<%1> = <%2> \times <%3>)__" ).arg( cubicFeetFieldName ).arg( litersFieldName ).arg( NConstants::kCubicFeetToLitersFieldName );
        }

        namespace NCaloriesComputer
        {
            /*
// Calculate calories function
    function calculateCalories() {
        // Get input values
        let weight = parseFloat(document.getElementById('weight').value);
        const duration = parseFloat(document.getElementById('duration').value);
        let depth = parseFloat(document.getElementById('depth').value);
        let temperature = parseFloat(document.getElementById('temperature').value);
        const activityFactor = parseFloat(document.getElementById('activity').value);
            
        // Convert units if necessary
        if (weightUnit === 'lbs') {
            weight = weight * 0.453592; // Convert lbs to kg
        }
            
        if (depthUnit === 'ft') {
            depth = depth * 0.3048; // Convert feet to meters
        }
            
        if (tempUnit === 'f') {
            temperature = (temperature - 32) * 5/9; // Convert F to C
        }
            
        // Base MET value for scuba diving (Metabolic Equivalent of Task)
        let metValue = 7.0;
            
        // Adjust for depth - approximately 2% increase per 10 meters
        metValue *= (1 + (depth * 0.002));
            
        // Adjust for temperature - approximately 1.5% increase per degree below 25°C
        if (temperature < 25) {
            metValue *= (1 + ((25 - temperature) * 0.015));
        }
            
        // Apply activity factor
        metValue *= activityFactor;
            
        // Calculate calories burned: MET * weight in kg * time in hours
        const hours = duration / 60;
        const calories = metValue * weight * hours;
            
        return Math.round(calories);
    }*/

            double computeCalories( bool imperial, bool seaWater, double weight, double depth, double temperature, double activityLevelMultiplier, double duration )
            {
                if ( imperial )
                {
                    weight = lbsToKGs( weight );
                    depth = feetToMeters( depth );
                    temperature = farenheightToCelsius( temperature );
                }

                if ( !seaWater )
                    depth = depthFreshwaterToSeawater( depth );

                auto tempThreshold = 25.0;
                auto percentPerTemp = 0.015;
                auto percentPerDepth = 0.02 / 10.0;

                auto metValue = 1.0;

                // Adjust for depth - approximately 2% increase per 10 meters
                metValue *= ( 1 + ( depth * percentPerDepth ) );

                // Adjust for temperature - approximately 1.5% increase per degree below 25°C
                if ( temperature < tempThreshold )
                {
                    metValue *= ( 1 + ( ( tempThreshold - temperature ) * percentPerTemp ) );
                }

                metValue *= activityLevelMultiplier;

                metValue = NConstants::baseMETForScuba() * metValue;

                // Calculate calories burned: MET * weight in kg * time in minutes
                auto calories = metValue * weight * duration;
                return calories;
            }

            QString computeCaloriesFormula( bool imperial, bool seaWater, const QString &caloriesFieldName, const QString &weightFieldName, const QString &depthFieldName, const std::pair< TOptionalDouble, QString > &tempFieldNameAndValue, const QString &activityLevelFieldName, const QString &durationFieldName )
            {
                auto actualWeightFieldName = weightFieldName;
                auto actualDepthFieldName = depthFieldName;
                auto actualTempFieldName = tempFieldNameAndValue.second;
                QStringList formulas;
                if ( imperial )
                {
                    formulas.push_back( QString( R"__(<%1> = <%2> \times %3)__" ).arg( weightFieldName + "C" ).arg( weightFieldName ).arg( NUnitStrings::kgsPerLbs( true, true ) ) );
                    if ( seaWater )
                        formulas.push_back( QString( R"__(<%1> = <%2> \times %3)__" ).arg( depthFieldName + "C" ).arg( depthFieldName ).arg( NUnitStrings::feetToMeters( true, true ) ) );
                    else
                        formulas.push_back( QString( R"__(<%1> = \frac{<%2> \times %3}{%4} )__" ).arg( depthFieldName + "C" ).arg( depthFieldName ).arg( NUnitStrings::feetToMeters( true, true ) ).arg( NUnitStrings::freshWaterToSeaWater( false, true, true ) ) );

                    formulas.push_back( farenheightToCelsiusFormula( tempFieldNameAndValue.second + "C", tempFieldNameAndValue.second ) );
                    actualWeightFieldName += "C";
                    actualDepthFieldName += "C";
                    actualTempFieldName += "C";
                }
                else if ( !seaWater )
                {
                    formulas.push_back( depthFreshwaterToSeawaterFormula( depthFieldName, depthFieldName + "C" ) );
                    actualDepthFieldName += "C";
                }

                QString retVal = QString( "<%1> = <%2>" ).arg( caloriesFieldName ).arg( NConstants::kBaseMETofSCUBAConstFieldName );
                retVal += QString( R"__( \times ( 1 + [ <%1> \times \frac{2\%}{10%2} ] ) )__" ).arg( actualDepthFieldName ).arg( NUnitStrings::depthUnit( false, true, true, true ) );

                if ( !tempFieldNameAndValue.first.has_value() || ( tempFieldNameAndValue.first.value() < 25.0 ) )
                {
                    retVal += QString( R"__( \times ( 1 + [ 25.0%2 - <%1> \times \frac{1.5\%}{%2} ] ) )__" ).arg( actualTempFieldName ).arg( NUnitStrings::tempUnit( false, true, true ) );
                }

                retVal += QString( R"__( \times <%1> \times \frac{<%2>}{100\%} \times <%3>)__" )   //
                              .arg( actualWeightFieldName )
                              .arg( activityLevelFieldName )
                              .arg( durationFieldName );

                formulas.push_back( retVal );
                return joinFormulas( formulas );
            }

            double computeDuration( bool imperial, bool seaWater, double calories, double weight, double depth, double temperature, double activityLevelMultiplier )
            {
                if ( imperial )
                {
                    weight = lbsToKGs( weight );
                    depth = feetToMeters( depth );
                    temperature = farenheightToCelsius( temperature );
                }

                if ( !seaWater )
                    depth = depthFreshwaterToSeawater( depth );

                auto tempThreshold = 25.0;
                auto percentPerTemp = 0.015;
                auto percentPerDepth = 0.02 / 10.0;

                auto metValue = 1.0;

                // Adjust for depth - approximately 2% increase per 10 meters
                metValue *= ( 1 + ( depth * percentPerDepth ) );

                // Adjust for temperature - approximately 1.5% increase per degree below 25°C
                if ( temperature < tempThreshold )
                {
                    metValue *= ( 1 + ( ( tempThreshold - temperature ) * percentPerTemp ) );
                }

                metValue *= activityLevelMultiplier;

                metValue = NConstants::baseMETForScuba() * metValue;

                // Calculate calories burned: MET * weight in kg * time in minutes
                auto duration = calories / ( metValue * weight );
                return duration;
            }

            QString computeDurationFormula( bool imperial, bool seaWater, const QString &caloriesFieldName, const QString &weightFieldName, const QString &depthFieldName, const std::pair< TOptionalDouble, QString > &tempFieldNameAndValue, const QString &activityLevelFieldName, const QString &durationFieldName )
            {
                //auto weightFormula = QString( "<%1>" ).arg( weightFieldName );
                //auto depthFormula = QString( "<%1>" ).arg( depthFieldName );
                //auto temperatureFormula = QString( "<%1>" ).arg( tempFieldNameAndValue.second );
                auto actualDepthFieldName = depthFieldName;
                auto actualTempFieldName = tempFieldNameAndValue.second;
                auto actualWeightFieldName = weightFieldName;
                QStringList formulas;
                if ( imperial )
                {
                    formulas.push_back( QString( R"__(<%1> = <%2> \times %3)__" ).arg( weightFieldName + "C" ).arg( weightFieldName ).arg( NUnitStrings::kgsPerLbs( true, true ) ) );
                    if ( seaWater )
                        formulas.push_back( QString( R"__(<%1> = <%2> \times %3)__" ).arg( depthFieldName + "C" ).arg( depthFieldName ).arg( NUnitStrings::feetToMeters( true, true ) ) );
                    else
                        formulas.push_back( QString( R"__(<%1> = \frac{<%2> \times %3}{%4} )__" ).arg( depthFieldName + "C" ).arg( depthFieldName ).arg( NUnitStrings::feetToMeters( true, true ) ).arg( NUnitStrings::freshWaterToSeaWater( false, true, true ) ) );

                    formulas.push_back( farenheightToCelsiusFormula( tempFieldNameAndValue.second + "C", tempFieldNameAndValue.second ) );
                    actualDepthFieldName += "C";
                    actualTempFieldName += "C";
                }
                else if ( !seaWater )
                {
                    formulas.push_back( depthFreshwaterToSeawaterFormula( depthFieldName, depthFieldName + "C" ) );
                    actualDepthFieldName += "C";
                }

                QString retVal = QString( R"__(<%1> = \frac{<%2>}{<%3>)__" ).arg( durationFieldName ).arg( caloriesFieldName ).arg( NConstants::kBaseMETofSCUBAConstFieldName );
                retVal += QString( R"__( \times ( 1 + [ <%1> \times \frac{2\%}{10%2} ] ) )__" ).arg( actualDepthFieldName ).arg( NUnitStrings::depthUnit( false, true, true, true ) );

                if ( !tempFieldNameAndValue.first.has_value() || ( tempFieldNameAndValue.first.value() < 25.0 ) )
                {
                    retVal += QString( R"__( \times ( 1 + [ 25.0%2 - <%1> \times \frac{1.5\%}{%2} ] ) )__" ).arg( actualTempFieldName ).arg( NUnitStrings::tempUnit( false, true, true ) );
                }

                retVal += QString( R"__( \times \frac{<%1>}{100\%} \times <%2> })__" )   //
                              .arg( activityLevelFieldName )
                              .arg( actualWeightFieldName );

                formulas.push_back( retVal );
                return joinFormulas( formulas );
            }
        }
    }

    QString joinFormulas( const QStringList &formulas )
    {
        auto retVal = formulas.join( R"( \newline )" );
        if ( formulas.size() > 1 )
        {
            retVal = QString( R"__(\begin{align})__" ) + retVal + QString( R"__(\end{align})__" );
            retVal.replace( "=", "& =" );
        }
        return retVal;
    }
}
