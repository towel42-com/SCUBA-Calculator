#include "Utilities.h"
#include "VariableInfo.h"
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

        TFormulaString barToPSIFormula( const TConstVariableInfo &psi, const TConstVariableInfo &bar )
        {
            return { psi, QString( R"__(%1 \times <%3>)__" ).arg( bar->fieldName() ).arg( fieldNameForType( EVariableType::ePSIToBarConst ) ) };
        }

        double barToPSI( double bar )
        {
            return bar * NConstants::barToPSI();
        }

        TFormulaString psiToBarFormula( const TConstVariableInfo &psi, const TConstVariableInfo &bar )
        {
            return ratioFormula( psi, bar, fieldNameForType( EVariableType::ePSIToBarConst ) );
        }

        double psiToBar( double psi )
        {
            return psi / NConstants::barToPSI();
        }

        TFormulaString depthToATAFormula( const TConstVariableInfo &ata, const TConstVariableInfo &depth )
        {
            return { ata, QString( R"__(\frac{%2}{<%3>} + 1)__" ).arg( depth->fieldName() ).arg( NConstants::kDepthToSingleATMConstFieldName ) };
        }

        double depthToATA( bool imperial, bool seaWater, double depth )
        {
            auto depthOfATM = NConstants::depthToSingleAtmosphere( imperial, seaWater );
            return ( depth / depthOfATM ) + 1;
        }

        TFormulaString ataToDepthFormula( const TConstVariableInfo &ata, const TConstVariableInfo &depth )
        {
            return { depth, QString( R"__((%1-1) \times <%3>)__" ).arg( ata->fieldName() ).arg( NConstants::kDepthToSingleATMConstFieldName ) };
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

        TFormulaString depthFreshwaterToSeawaterFormula( const TConstVariableInfo &freshWater, const TConstVariableInfo &seaWater )
        {
            return { seaWater, QString( R"__(%2 \times <%3>)__" ).arg( freshWater->fieldName() ).arg( fieldNameForType( EVariableType::eFreshWaterToSeaWaterConst ) ) };
        }

        double depthSeawaterToFreshwater( double depthSW )
        {
            return depthSW * NConstants::seaWaterToFreshWater();
        }

        TFormulaString depthSeawaterToFreshwaterFormula( const TConstVariableInfo &freshWater, const TConstVariableInfo &seaWater )
        {
            return { freshWater, QString( R"__(%2 \times <%3>)__" ).arg( seaWater->fieldName() ).arg( fieldNameForType( EVariableType::eSeaWaterToFreshWaterConst ) ) };
        }

        double feetToMeters( double feet )
        {
            return feet * NConstants::metersPerFeet();
        }

        TFormulaString feetToMetersFormula( const TConstVariableInfo &feet, const TConstVariableInfo &meters )
        {
            return { meters, QString( R"__(%1 \times <%3>)__" ).arg( feet->fieldName() ).arg( NConstants::kFeetToMetersConstFieldName ) };
        }

        double metersToFeet( double meters )
        {
            return meters * NConstants::feetPerMeters();
        }

        TFormulaString metersToFeetFormula( const TConstVariableInfo &feet, const TConstVariableInfo &meters )
        {
            return { feet, QString( R"__(%2 \times <%3>)__" ).arg( meters->fieldName() ).arg( NConstants::kMetersToFeetConstFieldName ) };
        }

        double pressureChangeForDegreeChange( bool imperial, double temperature )
        {
            return temperature * NConstants::pressureChangePerDegreeChange( imperial );
        }

        double degreeChangeForPressureChange( bool imperial, double pressure )
        {
            return pressure / NConstants::pressureChangePerDegreeChange( imperial );
        }

        TFormulaString pressureChangeForDegreeChangeFormula( const TConstVariableInfo &temp, const TConstVariableInfo &pressure )
        {
            return { pressure, QString( R"__(%1 \times <%3>)__" ).arg( temp->fieldName() ).arg( NConstants::kPressurePerDegreeConstFieldName ) };
        }

        TFormulaString degreeChangeForPressureChangeFormula( const TConstVariableInfo &temp, const TConstVariableInfo &pressure )
        {
            return ratioFormula( temp, pressure, fieldNameForType( EVariableType::ePressurePerDegreeConst ) );
        }

        double lbsToKGs( double lbs )
        {
            return lbs * NConstants::kgsPerLbs();
        }

        double kgsToLbs( double kgs )
        {
            return kgs * NConstants::lbsPerKGs();
        }

        TFormulaString lbsToKGsFormula( const TConstVariableInfo &lbs, const TConstVariableInfo &kgs )
        {
            return { kgs, QString( R"__(%2 \times <%3>)__" ).arg( lbs->fieldName() ).arg( NConstants::kKgsPerLbsConstFieldName ) };
        }

        TFormulaString kgsToLbsFormula( const TConstVariableInfo &lbs, const TConstVariableInfo &kgs )
        {
            return { lbs, QString( R"__(%2 \times <%3>)__" ).arg( kgs->fieldName() ).arg( NConstants::kLbsPerKgsConstFieldName ) };
        }

        double farenheightToCelsius( double temp )
        {
            return ( temp - 32 ) * 5.0 / 9.0;
        }

        TFormulaString farenheightToCelsiusFormula( const TConstVariableInfo &celsius, const TConstVariableInfo &farenheight )
        {
            auto formula = QString( R"__((%1 - 32 ) \times \frac{5%2}{9%3})__" ).arg( farenheight->fieldName() ).arg( NUnitStrings::tempUnit( false, true, true ) ).arg( NUnitStrings::tempUnit( true, true, true ) );
            return { celsius, formula };
        }

        double celsiusToFarenheight( double temp )
        {
            return ( temp * 9 / 5 ) + 32;
        }

        TFormulaString celsiusToFarenheightFormula( const TConstVariableInfo &celsius, const TConstVariableInfo &farenheight )
        {
            auto formula = QString( R"__((%1 \times \frac{9%2}{5%3}) + 32)__" ).arg( celsius->fieldName() ).arg( NUnitStrings::tempUnit( true, true, true ) ).arg( NUnitStrings::tempUnit( false, true, true ) );
            return { farenheight, formula };
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

        TFormulaString sacToRMVFormula( const TConstVariableInfo &sac, const TConstVariableInfo &rmv, const TConstVariableInfo &tankVolume, const TConstVariableInfo &tankPressure )
        {
            return { rmv, QString( R"__(%2 \times \frac{%3}{%4})__" ).arg( sac->fieldName() ).arg( tankVolume->fieldName() ).arg( tankPressure->fieldName() ) };
        }

        TFormulaString rmvToSACFormula( const TConstVariableInfo &sac, const TConstVariableInfo &rmv, const TConstVariableInfo &tankVolume, const TConstVariableInfo &tankPressure )
        {
            return { sac, QString( R"__(%1 \times \frac{%4}{%3})__" ).arg( rmv->fieldName() ).arg( tankVolume->fieldName() ).arg( tankPressure->fieldName() ) };
        }

        TFormulaString cubicFeetToLitersFormula( const TConstVariableInfo &cubicFeet, const TConstVariableInfo &liters )
        {
            return { liters, QString( R"__(%2 \times <%3>)__" ).arg( cubicFeet->fieldName() ).arg( NConstants::kLitersToCubicFeetFieldName ) };
        }

        TFormulaString litersToCubicFeetFormula( const TConstVariableInfo &cubicFeet, const TConstVariableInfo &liters )
        {
            return { cubicFeet, QString( R"__(%2 \times <%3>)__" ).arg( liters->fieldName() ).arg( NConstants::kCubicFeetToLitersFieldName ) };
        }

        double surfacePressureAtAltitude( bool imperial, double altitude )
        {
            return NConstants::pressureAtSurface( imperial ) - ( altitude * NConstants::pressureLossPerAltitude( imperial ) );
        }

        TFormulaString surfacePressureAtAltitudeFormula( bool imperial, const TConstVariableInfo &surfacePressure, const TConstVariableInfo &altitude )
        {
            auto retVal = QString( R"__(%2 - ( %3 \times %4 ))__" )   //
                              .arg( NConstants::pressureAtSurface( imperial, true, true, false ) )
                              .arg( altitude->fieldName() )
                              .arg( NConstants::pressureLossPerAltitude( imperial, true, true, false ) );
            return { surfacePressure, retVal };
        }

        double altitudeForSurfacePressure( bool imperial, double surfacePressure )
        {
            return ( surfacePressure - NConstants::pressureAtSurface( imperial ) ) / NConstants::pressureLossPerAltitude( imperial );
        }

        TFormulaString altitudeForSurfacePressureFormula( bool imperial, const TConstVariableInfo &surfacePressure, const TConstVariableInfo &altitude )
        {
            auto formula = QString( R"__(\frac{%1 - %2}{%4})__" )   //
                               .arg( surfacePressure->fieldName() )
                               .arg( NConstants::pressureAtSurface( imperial, true, true, false ) )
                               .arg( NConstants::pressureLossPerAltitude( imperial, true, true, false ) );
            return { altitude, formula };
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

            TFormulaStringList computeCaloriesFormula( bool imperial, bool seaWater, const TConstVariableInfo &calories, const TConstVariableInfo &weight, const TConstVariableInfo &depth, const TConstVariableInfo &temperature, const TConstVariableInfo &activityLevel, const TConstVariableInfo &duration )
            {
                auto actualWeight = weight;
                auto actualDepth = depth;
                auto actualTemp = temperature;
                TFormulaStringList formulas;
                if ( imperial )
                {
                    formulas.emplace_back( actualWeight = weight->clone( "M" ), QString( R"__(%2 \times %3)__" ).arg( weight->fieldName() ).arg( NConstants::kgsPerLbs( true, true, false ) ) );
                    if ( seaWater )
                        formulas.emplace_back( actualDepth = depth->clone( "MS" ), QString( R"__(%2 \times %3)__" ).arg( depth->fieldName() ).arg( NConstants::feetToMeters( true, true, false ) ) );
                    else
                        formulas.emplace_back( actualDepth = depth->clone( "MS" ), QString( R"__(\frac{%2 \times %3}{%4} )__" ).arg( depth->fieldName() ).arg( NConstants::feetToMeters( true, true, false ) ).arg( NConstants::freshWaterToSeaWater( false, true, true, false ) ) );

                    formulas.emplace_back( farenheightToCelsiusFormula( actualTemp = temperature->clone( "M" ), temperature ) );
                }
                else if ( !seaWater )
                {
                    formulas.emplace_back( depthFreshwaterToSeawaterFormula( depth, actualDepth = depth->clone( "MS" ) ) );
                }

                auto rhs = QString( "<%2 > " ).arg( NConstants::kBaseMETofSCUBAConstFieldName );
                rhs += QString( R"__( \times ( 1 + [ %1 \times \frac{2\%}{10%2} ] ) )__" ).arg( actualDepth->fieldName() ).arg( NUnitStrings::depthUnit( false, true, true, true ) );

                if ( !temperature->optValue().has_value() || ( temperature->optValue().value() < 25.0 ) )
                {
                    rhs += QString( R"__( \times ( 1 + [ 25.0%2 - %1 \times \frac{1.5\%}{%2} ] ) )__" ).arg( actualTemp->fieldName() ).arg( NUnitStrings::tempUnit( false, true, true ) );
                }

                rhs += QString( R"__( \times %1 \times \frac{%2}{100\%} \times %3)__" )   //
                           .arg( actualWeight->fieldName() )
                           .arg( activityLevel->fieldName() )
                           .arg( duration->fieldName() );

                formulas.emplace_back( calories, rhs );
                return formulas;
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

            TFormulaStringList computeDurationFormula( bool imperial, bool seaWater, const TConstVariableInfo &calories, const TConstVariableInfo &weight, const TConstVariableInfo &depth, const TConstVariableInfo &temperature, const TConstVariableInfo &activityLevel, const TConstVariableInfo &duration )
            {
                auto actualDepth = depth;
                auto actualTemp = temperature;
                auto actualWeight = weight;
                TFormulaStringList formulas;
                if ( imperial )
                {
                    formulas.emplace_back( actualWeight = weight->clone( "M" ), QString( R"__(%2 \times %3)__" ).arg( weight->fieldName() ).arg( NConstants::kgsPerLbs( true, true, false ) ) );
                    if ( seaWater )
                        formulas.emplace_back( actualDepth = depth->clone( "MS" ), QString( R"__(%2 \times %3)__" ).arg( depth->fieldName() ).arg( NConstants::feetToMeters( true, true, false ) ) );
                    else
                        formulas.emplace_back( actualDepth = depth->clone( "MS" ), QString( R"__(\frac{%2 \times %3}{%4} )__" ).arg( depth->fieldName() ).arg( NConstants::feetToMeters( true, true, false ) ).arg( NConstants::freshWaterToSeaWater( false, true, true, false ) ) );

                    formulas.emplace_back( farenheightToCelsiusFormula( actualTemp = temperature->clone( "M" ), temperature ) );
                }
                else if ( !seaWater )
                {
                    formulas.emplace_back( depthFreshwaterToSeawaterFormula( depth, actualDepth = depth->clone( "MS" ) ) );
                }

                auto rhs = QString( R"__(\frac{%2}{<%3>)__" ).arg( calories->fieldName() ).arg( NConstants::kBaseMETofSCUBAConstFieldName );
                rhs += QString( R"__( \times ( 1 + [ %1 \times \frac{2\%}{10%2} ] ) )__" ).arg( actualDepth->fieldName() ).arg( NUnitStrings::depthUnit( false, true, true, true ) );

                if ( !temperature->optValue().has_value() || ( temperature->optValue().value() < 25.0 ) )
                {
                    rhs += QString( R"__( \times ( 1 + [ 25.0%2 - %1 \times \frac{1.5\%}{%2} ] ) )__" ).arg( actualTemp->fieldName() ).arg( NUnitStrings::tempUnit( false, true, true ) );
                }

                rhs += QString( R"__( \times \frac{%1}{100\%} \times %2 })__" )   //
                           .arg( activityLevel->fieldName() )
                           .arg( actualWeight->fieldName() );

                formulas.emplace_back( duration, rhs );
                return formulas;
            }
        }
    }
}
