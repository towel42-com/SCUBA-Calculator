#include "Utilities.h"
#include <QString>
#include <QObject>

namespace NUtilities
{
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
            return ratioFormula( barFieldName, psiFieldName, fieldNameForType( EVariableType::ePSIToBarConst ) );
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
            return depthFW * NConstants::freshWaterToSeaWater();
        }

        QString depthFreshwaterToSeawaterFormula( const QString &freshWaterFieldName, const QString &seaWaterFieldName )
        {
            return QString( R"__(<%1>=<%2> \times <%3>)__" ).arg( seaWaterFieldName ).arg( freshWaterFieldName ).arg( fieldNameForType( EVariableType::eFreshWaterToSeaWaterConst ) );
        }

        double depthSeawaterToFreshwater( double depthSW )
        {
            return depthSW * NConstants::seaWaterToFreshWater();
        }

        QString depthSeawaterToFreshwaterFormula( const QString &freshWaterFieldName, const QString &seaWaterFieldName )
        {
            return QString( R"__(<%1>=<%2> \times <%3>)__" ).arg( freshWaterFieldName ).arg( seaWaterFieldName ).arg( fieldNameForType( EVariableType::eSeaWaterToFreshWaterConst ) );
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
            return ratioFormula( tempFieldName, pressureFieldName, fieldNameForType( EVariableType::ePressurePerDegreeConst ) );
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

        QString sacToRMVFormula( const QString &sacFieldName, const QString &rmvFieldName, const QString &tankVolumeFieldName, const QString &tankPressureFieldName )
        {
            return QString( R"__(<%1>=<%2> \times \frac{<%3>}{<%4>})__" ).arg( rmvFieldName ).arg( sacFieldName ).arg( tankVolumeFieldName ).arg( tankPressureFieldName );
        }

        QString rmvToSACFormula( const QString &sacFieldName, const QString &rmvFieldName, const QString &tankVolumeFieldName, const QString &tankPressureFieldName )
        {
            return QString( R"__(<%2>=<%1> \times \frac{<%4>}{<%3>})__" ).arg( rmvFieldName ).arg( sacFieldName ).arg( tankVolumeFieldName ).arg( tankPressureFieldName );
        }

        QString cubicFeetToLitersFormula( const QString &cubicFeetFieldName, const QString &litersFieldName )
        {
            return QString( R"__(<%1> = <%2> \times <%3>)__" ).arg( litersFieldName ).arg( cubicFeetFieldName ).arg( NConstants::kLitersToCubicFeetFieldName );
        }

        QString litersToCubicFeetFormula( const QString &cubicFeetFieldName, const QString &litersFieldName )
        {
            return QString( R"__(<%1> = <%2> \times <%3>)__" ).arg( cubicFeetFieldName ).arg( litersFieldName ).arg( NConstants::kCubicFeetToLitersFieldName );
        }

        double surfacePressureAtAltitude( bool imperial, double altitude )
        {
            return NConstants::pressureAtSurface( imperial ) - ( altitude * NConstants::pressureLossPerAltitude( imperial ) );
        }

        QString surfacePressureAtAltitudeFormula( bool imperial, const QString &surfacePressureFieldName, const QString &altitudeFieldName )
        {
            auto retVal = QString( R"__(<%1> = %2 - ( <%3> \times %4 ))__" )   //
                              .arg( surfacePressureFieldName )
                              .arg( NConstants::pressureAtSurface( imperial, true, true ) )
                              .arg( altitudeFieldName )
                              .arg( NConstants::pressureLossPerAltitude( imperial, true, true ) );
            return retVal;
        }

        double altitudeForSurfacePressure( bool imperial, double surfacePressure )
        {
            return ( surfacePressure - NConstants::pressureAtSurface( imperial ) ) / NConstants::pressureLossPerAltitude( imperial );
        }

        QString altitudeForSurfacePressureFormula( bool imperial, const QString &surfacePressureFieldName, const QString &altitudeFieldName )
        {
            auto retVal = QString( R"__(<%3> = \frac{<%1> - %2}{%4})__" )   //
                              .arg( surfacePressureFieldName )
                              .arg( NConstants::pressureAtSurface( imperial, true, true ) )
                              .arg( altitudeFieldName )
                              .arg( NConstants::pressureLossPerAltitude( imperial, true, true ) );
            return retVal;
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
                }
            */

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
                    formulas.push_back( QString( R"__(<%1> = <%2> \times %3)__" ).arg( weightFieldName + "M" ).arg( weightFieldName ).arg( NConstants::kgsPerLbs( true, true ) ) );
                    if ( seaWater )
                        formulas.push_back( QString( R"__(<%1> = <%2> \times %3)__" ).arg( depthFieldName + "MS" ).arg( depthFieldName ).arg( NConstants::feetToMeters( true, true ) ) );
                    else
                        formulas.push_back( QString( R"__(<%1> = \frac{<%2> \times %3}{%4} )__" ).arg( depthFieldName + "MS" ).arg( depthFieldName ).arg( NConstants::feetToMeters( true, true ) ).arg( NConstants::freshWaterToSeaWater( false, true, true ) ) );

                    formulas.push_back( farenheightToCelsiusFormula( tempFieldNameAndValue.second + "M", tempFieldNameAndValue.second ) );
                    actualWeightFieldName += "M";
                    actualDepthFieldName += "MS";
                    actualTempFieldName += "M";
                }
                else if ( !seaWater )
                {
                    formulas.push_back( depthFreshwaterToSeawaterFormula( depthFieldName, depthFieldName + "MS" ) );
                    actualDepthFieldName += "MS";
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
                return joinFormulas( formulas ).value();
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
                    formulas.push_back( QString( R"__(<%1> = <%2> \times %3)__" ).arg( weightFieldName + "C" ).arg( weightFieldName ).arg( NConstants::kgsPerLbs( true, true ) ) );
                    if ( seaWater )
                        formulas.push_back( QString( R"__(<%1> = <%2> \times %3)__" ).arg( depthFieldName + "C" ).arg( depthFieldName ).arg( NConstants::feetToMeters( true, true ) ) );
                    else
                        formulas.push_back( QString( R"__(<%1> = \frac{<%2> \times %3}{%4} )__" ).arg( depthFieldName + "C" ).arg( depthFieldName ).arg( NConstants::feetToMeters( true, true ) ).arg( NConstants::freshWaterToSeaWater( false, true, true ) ) );

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
                return joinFormulas( formulas ).value();
            }
        }
    }
}
