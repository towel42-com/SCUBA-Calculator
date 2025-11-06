// The MIT License( MIT )
//
// Copyright( c ) 2025 Scott Aron Bloom
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sub-license, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef UTILITIES_H
#define UTILITIES_H

#include "SCUBACalculatorFwd.h"

namespace NUtilities
{
    namespace NUnitStrings
    {
        CALCULATORS_EXPORT QString volumeUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString weightUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString lengthUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString depthUnit( bool imperial, bool seaWater, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString pressureUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString atmosphereUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString tempUnit( bool imperial, bool useAbbreviations, bool tex );   // C/F
        CALCULATORS_EXPORT QString absZeroTempUnit( bool imperial, bool useAbbreviations, bool tex );   //K/R
        CALCULATORS_EXPORT QString percentUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString timeUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString idealGasConstantUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString molesUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString energyUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString metUnits( bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString pressurePerMinuteUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString volumePerMinuteUnit( bool imperial, bool useAbbreviations, bool tex );
    }

    namespace NConstants
    {
        extern const char *kPressurePerDegreeConstFieldName;
        extern const char *kWeightPerVolumeOfWaterConstFieldName;
        extern const char *kVolumePerWeightOfWaterConstFieldName;
        extern const char *kIdealGasConstantFieldName;
        extern const char *kFN2AtSurfaceFieldName;
        extern const char *kFO2AtSurfaceFieldName;
        extern const char *kDepthToSingleATMConstFieldName;
        extern const char *kFeetToMetersConstFieldName;
        extern const char *kMetersToFeetConstFieldName;
        extern const char *kLbsPerKgsConstFieldName;
        extern const char *kKgsPerLbsConstFieldName;
        extern const char *kFreshWaterToSeaWaterConstFieldName;
        extern const char *kSeaWaterToFreshWaterConstFieldName;
        extern const char *kPSIToBarConstFieldName;
        extern const char *kBarToPSIConstFieldName;
        extern const char *kAbsZeroOffsetConstFieldName;
        extern const char *kPressureAtSurfaceConstFieldName;
        extern const char *kBaseMETofSCUBAConstFieldName;
        extern const char *kFillRateAirConstFieldName;
        extern const char *kFillRateO2ConstFieldName;
        extern const char *kCubicFeetToLitersFieldName;
        extern const char *kLitersToCubicFeetFieldName;
        extern const char *kSafetyStopDepthConstFieldName;
        extern const char *kWaterWeightAdjustmentFieldName;

        CALCULATORS_EXPORT double absZeroOffset( bool imperial );
        CALCULATORS_EXPORT double pressureChangePerDegreeChange( bool imperial );
        CALCULATORS_EXPORT double weightPerVolumeOfWater( bool imperial, bool seaWater );
        CALCULATORS_EXPORT double volumePerWeightOfWater( bool imperial, bool seaWater );
        CALCULATORS_EXPORT double pressureAtSurface( bool imperial );
        CALCULATORS_EXPORT double depthToSingleAtmosphere( bool imperial, bool seaWater );
        CALCULATORS_EXPORT double idealGasConstant( bool imperial );

        CALCULATORS_EXPORT double metersPerFeet();
        CALCULATORS_EXPORT double feetPerMeters();

        CALCULATORS_EXPORT double freshWaterToSeaWater();
        CALCULATORS_EXPORT double seaWaterToFreshWater();

        CALCULATORS_EXPORT double percentN2AtSurface();
        CALCULATORS_EXPORT double percentO2AtSurface();
        CALCULATORS_EXPORT double barToPSI();
        CALCULATORS_EXPORT double psiToBar();

        CALCULATORS_EXPORT double lbsPerKGs();
        CALCULATORS_EXPORT double kgsPerLbs();

        CALCULATORS_EXPORT double baseMETForScuba();
        CALCULATORS_EXPORT double fillRateAir( bool imperial );
        CALCULATORS_EXPORT double fillRateO2( bool imperial );

        CALCULATORS_EXPORT double cubicFeetPerLiter();
        CALCULATORS_EXPORT double litersPerCubicFeet();

        CALCULATORS_EXPORT double pressureLossPerAltitude( bool imperial );
        CALCULATORS_EXPORT double safetyStopDepth( bool imperial, bool seaWater );
        CALCULATORS_EXPORT double waterWeightAdjustment( bool imperial, bool seaWater );

        CALCULATORS_EXPORT QString pressureChangePerDegreeChange( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString lbsPerKgs( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString kgsPerLbs( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString scubaMET( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString fillRateO2( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString fillRateAir( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString weightPerVolumeOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString volumePerWeightOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString idealGasConstant( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString percentN2AtSurface( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString percentO2AtSurface( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString depthToSingleAtmosphere( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString feetToMeters( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString metersToFeet( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString freshWaterToSeaWater( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString seaWaterToFreshWater( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString psiToBar( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString barToPSI( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString pressureAtSurface( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString pressureLossPerAltitude( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString safetyStop( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString waterWeightAdjustment( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString cubicFeetToLiters( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString litersToCubicFeet( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString absZeroOffset( bool imperial, bool useAbbreviations, bool tex, bool description );

        void foreachConstantType( const std::function< void( EVariableType ) > &onConstType );
        QString constantString( bool imperial, bool seaWater, EVariableType constantType, bool description );
    }

    namespace NConversions
    {
        CALCULATORS_EXPORT double toAbsZeroBasedTemp( bool imperial, double temp );
        CALCULATORS_EXPORT double fromAbsZeroBasedTemp( bool imperial, double temp );

        CALCULATORS_EXPORT double psiToBar( double psi );
        CALCULATORS_EXPORT double barToPSI( double bar );

        CALCULATORS_EXPORT TFormulaString psiToBarFormula( const TConstVariableInfo &psi, const TConstVariableInfo &bar );
        CALCULATORS_EXPORT TFormulaString barToPSIFormula( const TConstVariableInfo &psi, const TConstVariableInfo &bar );

        CALCULATORS_EXPORT double depthToATA( bool imperial, bool seaWater, double depth );
        CALCULATORS_EXPORT double ataToDepth( bool imperial, bool seaWater, double pressure );

        CALCULATORS_EXPORT TFormulaString depthToATAFormula( const TConstVariableInfo &ata, const TConstVariableInfo &depth );
        CALCULATORS_EXPORT TFormulaString ataToDepthFormula( const TConstVariableInfo &ata, const TConstVariableInfo &depth );

        CALCULATORS_EXPORT double depthFreshwaterToSeawater( double depthFW );
        CALCULATORS_EXPORT double depthSeawaterToFreshwater( double depthSW );

        CALCULATORS_EXPORT TFormulaString depthFreshwaterToSeawaterFormula( const TConstVariableInfo &freshWater, const TConstVariableInfo &seaWater );
        CALCULATORS_EXPORT TFormulaString depthSeawaterToFreshwaterFormula( const TConstVariableInfo &freshWater, const TConstVariableInfo &seaWater );

        CALCULATORS_EXPORT double feetToMeters( double feet );
        CALCULATORS_EXPORT double metersToFeet( double meters );

        CALCULATORS_EXPORT TFormulaString feetToMetersFormula( const TConstVariableInfo &feet, const TConstVariableInfo &meters );
        CALCULATORS_EXPORT TFormulaString metersToFeetFormula( const TConstVariableInfo &feet, const TConstVariableInfo &meters );

        CALCULATORS_EXPORT double degreeChangeForPressureChange( bool imperial, double temperature );
        CALCULATORS_EXPORT double pressureChangeForDegreeChange( bool imperial, double pressure );

        CALCULATORS_EXPORT TFormulaString degreeChangeForPressureChangeFormula( const TConstVariableInfo &temperature, const TConstVariableInfo &pressure );
        CALCULATORS_EXPORT TFormulaString pressureChangeForDegreeChangeFormula( const TConstVariableInfo &temperature, const TConstVariableInfo &pressure );

        CALCULATORS_EXPORT double lbsToKGs( double lbs );
        CALCULATORS_EXPORT double kgsToLbs( double kgs );

        CALCULATORS_EXPORT TFormulaString lbsToKGsFormula( const TConstVariableInfo &lbs, const TConstVariableInfo &kgs );
        CALCULATORS_EXPORT TFormulaString kgsToLbsFormula( const TConstVariableInfo &lbs, const TConstVariableInfo &kgs );

        CALCULATORS_EXPORT double farenheightToCelsius( double temp );
        CALCULATORS_EXPORT double celsiusToFarenheight( double temp );

        CALCULATORS_EXPORT TFormulaString farenheightToCelsiusFormula( const TConstVariableInfo &celsius, const TConstVariableInfo &farenheight );
        CALCULATORS_EXPORT TFormulaString celsiusToFarenheightFormula( const TConstVariableInfo &celsius, const TConstVariableInfo &farenheight );

        CALCULATORS_EXPORT double cubicFeetToLiters( double volume );
        CALCULATORS_EXPORT double litersToCubicFeet( double volume );

        CALCULATORS_EXPORT TFormulaString cubicFeetToLitersFormula( const TConstVariableInfo &cubicFeet, const TConstVariableInfo &liters );
        CALCULATORS_EXPORT TFormulaString litersToCubicFeetFormula( const TConstVariableInfo &cubicFeet, const TConstVariableInfo &liters );

        CALCULATORS_EXPORT double sacToRMV( double sac, double volume, double pressure );
        CALCULATORS_EXPORT double rmvToSAC( double rmv, double volume, double pressure );
        CALCULATORS_EXPORT TFormulaString sacToRMVFormula( const TConstVariableInfo &sac, const TConstVariableInfo &rmv, const TConstVariableInfo &tankVolume, const TConstVariableInfo &tankPressure );
        CALCULATORS_EXPORT TFormulaString rmvToSACFormula( const TConstVariableInfo &sac, const TConstVariableInfo &rmv, const TConstVariableInfo &tankVolume, const TConstVariableInfo &tankPressure );

        CALCULATORS_EXPORT double surfacePressureAtAltitude( bool imperial, double altitude );
        CALCULATORS_EXPORT TFormulaString surfacePressureAtAltitudeFormula( bool imperial, const TConstVariableInfo &surfacePressure, const TConstVariableInfo &altitude );

        CALCULATORS_EXPORT double altitudeForSurfacePressure( bool imperial, double surfacePressure );
        CALCULATORS_EXPORT TFormulaString altitudeForSurfacePressureFormula( bool imperial, const TConstVariableInfo &surfacePressure, const TConstVariableInfo &altitude );

        namespace NCaloriesComputer
        {
            CALCULATORS_EXPORT double computeCalories( bool imperial, bool seaWater, double weight, double depth, double temperature, double activityLevelMultiplier, double duration );
            CALCULATORS_EXPORT double computeDuration( bool imperial, bool seaWater, double calories, double weight, double depth, double temperature, double activityLevelMultiplier );

            CALCULATORS_EXPORT TFormulaStringList computeCaloriesFormula( bool imperial, bool seaWater, const TConstVariableInfo &calories, const TConstVariableInfo &weight, const TConstVariableInfo &depth, const TConstVariableInfo &temperature, const TConstVariableInfo &activityLevel, const TConstVariableInfo &duration );
            CALCULATORS_EXPORT TFormulaStringList computeDurationFormula( bool imperial, bool seaWater, const TConstVariableInfo &calories, const TConstVariableInfo &weight, const TConstVariableInfo &depth, const TConstVariableInfo &temperature, const TConstVariableInfo &activityLevel, const TConstVariableInfo &duration );
        }
    }

    CALCULATORS_EXPORT std::optional< QString > joinFormulas( bool imperial, bool seaWater, const TFormulaStringList &formulas );
    CALCULATORS_EXPORT QString createEquation( bool imperial, bool seaWater, const TFormulaString &formula );
    CALCULATORS_EXPORT QString descForType( EVariableType type );
    CALCULATORS_EXPORT QString fieldNameForType( EVariableType type );
    CALCULATORS_EXPORT bool isConstantVariable( EVariableType type );

    CALCULATORS_EXPORT std::size_t numEmpty( const TOptionalDoubleVector &values );
    CALCULATORS_EXPORT QString doubleToString( const TOptionalDouble &value, int numDecimal );

    CALCULATORS_EXPORT TFormulaString ratioFormula( const TConstVariableInfo &returnVariable, const TConstVariableInfo &numerator, const TConstVariableInfo &denominator );
    CALCULATORS_EXPORT TFormulaString ratioFormula( const TConstVariableInfo &returnVariable, const TConstVariableInfo &numerator, const QString &denominator );
    CALCULATORS_EXPORT TFormulaString ratioFormula( const TConstVariableInfo &returnVariable, const QString &numerator, const TConstVariableInfo &denominator );
    CALCULATORS_EXPORT TFormulaString ratioFormula( const TConstVariableInfo &returnVariable, const TConstVariableInfo &numerator, EVariableType denominator );
    CALCULATORS_EXPORT TFormulaString ratioFormula( const TConstVariableInfo &returnVariable, EVariableType numerator, const TConstVariableInfo &denominator );
    CALCULATORS_EXPORT TFormulaString ratioFormula( const TConstVariableInfo &returnVariable, const TConstVariableInfo &numerator, const TConstVariableInfo &denominator );

    CALCULATORS_EXPORT QString ratio( const QString &numerator, const QString &denominator, bool tex );
    CALCULATORS_EXPORT QString ratio( const QString &description, const QString &numerator, const QString &denominator, bool tex );
    CALCULATORS_EXPORT QString ratio( double value, const QString &numerator, const QString &denominator, bool tex, std::optional< int > numDecimal = {} );

    CALCULATORS_EXPORT QStringList getVariables( const QString &formula );
    CALCULATORS_EXPORT bool hasVariable( const QString &formula );
};

#endif
