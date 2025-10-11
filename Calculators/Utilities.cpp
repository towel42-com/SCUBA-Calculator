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

        QString pressurePerTemp( bool imperial, bool useAbbreviations, bool tex )
        {
            auto retVal = QString( tex ? R"__(%1\frac{%2}{%3})__" : "%1 (%2/%3)" );
            retVal = retVal.arg( NConstants::pressurePerTemp( imperial ) ).arg( NUnitStrings::pressureUnit( imperial, useAbbreviations, tex ) ).arg( NUnitStrings::tempUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString metUnit( bool imperial, bool useAbbreviations, bool tex )
        {
            auto retVal = QString( tex ? R"__(\frac{%1}{(%2 \times %3})__" : "%1/(%2*%3)" );
            retVal = retVal.arg( energyUnit( imperial, useAbbreviations, tex ) ).arg( NUnitStrings::weightUnit( imperial, useAbbreviations, tex ) ).arg( NUnitStrings::timeUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString scubaMET( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( NConstants::baseMETForScuba( imperial ) ).arg( NUnitStrings::metUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString weightOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex )
        {
            auto retVal = QString( tex ? R"__(%1 \frac{%2}{%3})__" : "%1 %2/%3" );
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

        QString metersToFeet( bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1\frac{%2}{%3})__" : "%1 (%2/%3)";
            retVal = retVal.arg( doubleToString( NConstants::metersToFeet(), 3 ) ).arg( lengthUnit( true, useAbbreviations, true ) ).arg( lengthUnit( false, useAbbreviations, true ) );
            return retVal;
        }

        QString freshWaterToSeaWater( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1\frac{%2}{%3})__" : "%1 (%2/%3)";
            retVal = retVal.arg( doubleToString( NConstants::freshWaterToSeaWater(), 2 ) ).arg( depthUnit( imperial, false, useAbbreviations, tex ) ).arg( depthUnit( imperial, true, useAbbreviations, tex ) );
            return retVal;
        }

        QString psiToBar( bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? R"__(%1\frac{%2}{%3})__" : "%1 (%2/%3)";
            retVal = retVal.arg( doubleToString( NConstants::barToPSI(), 1 ) ).arg( pressureUnit( true, useAbbreviations, true ) ).arg( pressureUnit( false, useAbbreviations, true ) );
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
    }

    namespace NConstants
    {
        double absZeroOffset( bool imperial )
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

        double metersToFeet()
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

        double barToPSI()
        {
            return 14.7;
        }

        double kjoulesPerKCal()
        {
            return 4.1840;
        }

        double lbsPerKG()
        {
            return 2.20462;
        }

        double baseMETForScuba( bool imperial )
        {
            auto retVal = 7.0;
            if ( imperial )
            {
                // 1 MET = 1 kcals
                //         ____________
                //         ( kgs * hours );
                // 1 MET = * ( jouls/kcals )  * ( lbs/kgs );
                retVal *= NConstants::kjoulesPerKCal() * NConstants::lbsPerKG();
            }
            return retVal;
        }

    }

    double toAbsZeroBasedTemp( bool imperial, double temp )
    {
        return temp + NConstants::absZeroOffset( imperial );
    }

    double fromAbsZeroBasedTemp( bool imperial, double temp )
    {
        return temp - NConstants::absZeroOffset( imperial );
    }

    QString barToPSIFormula( const QString &psiFieldName, const QString &barFieldName, const QString &psiToBarConstFieldName )
    {
        return QString( R"__(<%2>=<%1> \times <%3>)__" ).arg( barFieldName ).arg( psiFieldName ).arg( psiToBarConstFieldName );
    }

    double barToPSI( double bar )
    {
        auto barToPSI = NConstants::barToPSI();
        return bar * barToPSI;
    }

    QString psiToBarFormula( const QString &psiFieldName, const QString &barFieldName, const QString &barToSingleATMFieldName )
    {
        return QString( R"__(<%1>=\frac{<%2>}{<%3>})__" ).arg( barFieldName ).arg( psiFieldName ).arg( barToSingleATMFieldName );
    }

    double psiToBar( double psi )
    {
        auto barToPSI = NConstants::barToPSI();
        return psi / barToPSI;
    }

    QString depthToPressureFormula( const QString &ataFieldName, const QString &depthFieldName, const QString &depthToSingleATMFieldName )
    {
        return QString( R"__(<%1>=\frac{<%2>}{<%3>} + 1)__" ).arg( ataFieldName ).arg( depthFieldName ).arg( depthToSingleATMFieldName );
    }

    double depthToPressure( bool imperial, bool seaWater, double depth )
    {
        auto depthOfATM = NConstants::depthToSingleAtmosphere( imperial, seaWater );
        return ( depth / depthOfATM ) + 1;
    }

    QString pressureToDepthFormula( const QString &ataFieldName, const QString &depthFieldName, const QString &depthToSingleATMFieldName )
    {
        return QString( R"__(<%2>=(<%1>-1) \times <%3>)__" ).arg( ataFieldName ).arg( depthFieldName ).arg( depthToSingleATMFieldName );
    }

    double pressureToDepth( bool imperial, bool seaWater, double pressure )
    {
        auto depthOfATM = NConstants::depthToSingleAtmosphere( imperial, seaWater );
        return ( pressure - 1 ) * depthOfATM;
    }

    double depthFreshwaterToSeawater( double depthFW )
    {
        return depthFW / NConstants::freshWaterToSeaWater();
    }

    QString depthFreshwaterToSeawaterFormula( const QString &freshWaterFieldName, const QString &seaWaterFieldName, const QString &freshWaterToSeaWaterFieldName )
    {
        return QString( R"__(<%2>=\frac{<%1>}{<%3>})__" ).arg( freshWaterFieldName ).arg( seaWaterFieldName ).arg( freshWaterToSeaWaterFieldName );
    }

    double depthSeawaterToFreshwater( double depthSW )
    {
        return depthSW * NConstants::freshWaterToSeaWater();
    }

    QString depthSeawaterToFreshwaterFormula( const QString &freshWaterFieldName, const QString &seaWaterFieldName, const QString &freshWaterToSeaWaterFieldName )
    {
        return QString( R"__(<%1>=<%2> \times <%3>)__" ).arg( freshWaterFieldName ).arg( seaWaterFieldName ).arg( freshWaterToSeaWaterFieldName );
    }

    double feetToMeters( double feet )
    {
        return feet * NConstants::metersToFeet();
    }

    QString feetToMetersFormula( const QString &feetFieldName, const QString &metersFieldName, const QString &feetToMetersConstFieldName )
    {
        return QString( R"__(<%2>=\frac{<%1>}{<%3>})__" ).arg( feetFieldName ).arg( metersFieldName ).arg( feetToMetersConstFieldName );
    }

    double metersToFeet( double meters )
    {
        return meters / NConstants::metersToFeet();
    }

    QString metersToFeetFormula( const QString &feetFieldName, const QString &metersFieldName, const QString &feetToMetersConstFieldName )
    {
        return QString( R"__(<%1>=<%2> \times <%3>)__" ).arg( feetFieldName ).arg( metersFieldName ).arg( feetToMetersConstFieldName );
    }

    double quickDegreeChangeToPressure( bool imperial, double temperature )
    {
        return temperature / NConstants::pressurePerTemp( imperial );
    }

    double quickPressureChangeToDegree( bool imperial, double pressure )
    {
        return pressure * NConstants::pressurePerTemp( imperial );
    }

    QString quickDegreeChangeToPressureFormula( const QString &tempFieldName, const QString &pressureFieldName, const QString &pressurePerDegreeConstFieldName )
    {
        return QString( R"__(<%2> = \frac{<%1>}{<%3>})__" ).arg( tempFieldName ).arg( pressureFieldName ).arg( pressurePerDegreeConstFieldName );
    }

    QString quickPressureChangeToDegreeFormula( const QString &tempFieldName, const QString &pressureFieldName, const QString &pressurePerDegreeConstFieldName )
    {
        return QString( R"__(<%1> = <%2> \times <%3>)__" ).arg( tempFieldName ).arg( pressureFieldName ).arg( pressurePerDegreeConstFieldName );
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

    double lbsToKGs( double lbs )
    {
        return lbs / NConstants::lbsPerKG();
    }

    double kgsToLbs( double kgs )
    {
        return kgs * NConstants::lbsPerKG();
    }

    double kcalsToKJoules( double kcals )   // actually kcal
    {
        return kcals * NConstants::kjoulesPerKCal();
    }

    double kjoulesToKCals( double joules )
    {
        return joules / NConstants::kjoulesPerKCal();
    }

    double farenheightToCelsius( double temp )
    {
        return ( temp - 32 ) * 5.0 / 9.0;
    }

    double celsiusToFarenheight( double temp )
    {
        return ( temp * 9 / 5 ) + 32;
    }

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

    double computeCalories( bool imperial, double weight, double depth, double temperature, double activityLevelMultiplier, double duration )
    {
        if ( imperial )
        {
            weight = NUtilities::lbsToKGs( weight );
            depth = NUtilities::feetToMeters( depth );
            temperature = NUtilities::farenheightToCelsius( temperature );
        }

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

        metValue = NUtilities::NConstants::baseMETForScuba( false ) * metValue;

        // Calculate calories burned: MET * weight in kg * time in hours
        auto hours = duration / 60.0;
        auto calories = metValue * weight * hours;
        return calories;
    }


}
