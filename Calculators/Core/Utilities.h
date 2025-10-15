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
        CALCULATORS_EXPORT QString pressureChangePerDegreeChange( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString percentUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString timeUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString idealGasConstantUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString molesUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString energyUnit( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString metUnit( bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString scubaMET( bool useAbbreviations, bool tex );

        CALCULATORS_EXPORT QString weightOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString idealGasConstant( bool imperial, bool useAbbreviations, bool tex );

        CALCULATORS_EXPORT QString percentN2AtSurface( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString percentO2AtSurface( bool imperial, bool useAbbreviations, bool tex );

        CALCULATORS_EXPORT QString depthToSingleAtmosphere( bool imperial, bool seaWater, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString feetToMeters( bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString metersToFeet( bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString lbsPerKgs( bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString freshWaterToSeaWater( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString psiToBar( bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString absZeroOffset( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString pressureOffset( bool imperial, bool useAbbreviations, bool tex );
    }

    namespace NConstants
    {
        extern const char *kPressurePerDegreeConstFieldName;
        extern const char *kWeightOfWaterConstFieldName;
        extern const char *kIdealGasConstantFieldName;
        extern const char *kFN2AtSurfaceFieldName;
        extern const char *kFO2AtSurfaceFieldName;
        extern const char *kDepthToSingleATMConstFieldName;
        extern const char *kFeetToMetersConstFieldName;
        extern const char *kMetersToFeetConstFieldName;
        extern const char *kLbsPerKgsConstFieldName;
        extern const char *kFreshWaterToSeaWaterConstFieldName;
        extern const char *kPSIToBarConstFieldName;
        extern const char *kAbsZeroOffsetConstFieldName;
        extern const char *kPressureOffsetConstFieldName;
        extern const char *kBaseMETofSCUBAConstFieldName;

        CALCULATORS_EXPORT double absZeroOffset( bool imperial );
        CALCULATORS_EXPORT double pressureChangePerDegreeChange( bool imperial );
        CALCULATORS_EXPORT double weightOfWater( bool imperial, bool seaWater );
        CALCULATORS_EXPORT double pressureOffset( bool imperial );
        CALCULATORS_EXPORT double depthToSingleAtmosphere( bool imperial, bool seaWater );
        CALCULATORS_EXPORT double idealGasConstant( bool imperial );

        CALCULATORS_EXPORT double metersPerFeet();
        CALCULATORS_EXPORT double feetPerMeters();
        CALCULATORS_EXPORT double freshWaterToSeaWater();

        CALCULATORS_EXPORT double percentN2AtSurface();
        CALCULATORS_EXPORT double percentO2AtSurface();
        CALCULATORS_EXPORT double barToPSI();

        CALCULATORS_EXPORT double lbsPerKGs();
        CALCULATORS_EXPORT double kgsPerLbs();

        CALCULATORS_EXPORT double baseMETForScuba();
    }

    namespace NConversions
    {
        CALCULATORS_EXPORT double toAbsZeroBasedTemp( bool imperial, double temp );
        CALCULATORS_EXPORT double fromAbsZeroBasedTemp( bool imperial, double temp );

        //CALCULATORS_EXPORT QString toAbsZeroBasedTempFormula( const QString &psiFieldName, const QString &barFieldName );
        //CALCULATORS_EXPORT QString fromAbsZeroBasedTempFormula( const QString &psiFieldName, const QString &barFieldName );

        CALCULATORS_EXPORT double psiToBar( double psi );
        CALCULATORS_EXPORT double barToPSI( double bar );

        CALCULATORS_EXPORT QString psiToBarFormula( const QString &psiFieldName, const QString &barFieldName );
        CALCULATORS_EXPORT QString barToPSIFormula( const QString &psiFieldName, const QString &barFieldName );

        CALCULATORS_EXPORT double depthToPressure( bool imperial, bool seaWater, double depth );
        CALCULATORS_EXPORT double pressureToDepth( bool imperial, bool seaWater, double pressure );

        CALCULATORS_EXPORT QString depthToPressureFormula( const QString &ataFieldName, const QString &depthFieldName );
        CALCULATORS_EXPORT QString pressureToDepthFormula( const QString &ataFieldName, const QString &depthFieldName );

        CALCULATORS_EXPORT double depthFreshwaterToSeawater( double depthFW );
        CALCULATORS_EXPORT double depthSeawaterToFreshwater( double depthSW );

        CALCULATORS_EXPORT QString depthFreshwaterToSeawaterFormula( const QString &freshWaterFieldName, const QString &seaWaterFieldName );
        CALCULATORS_EXPORT QString depthSeawaterToFreshwaterFormula( const QString &freshWaterFieldName, const QString &seaWaterFieldName );

        CALCULATORS_EXPORT double feetToMeters( double feet );
        CALCULATORS_EXPORT double metersToFeet( double meters );

        CALCULATORS_EXPORT QString feetToMetersFormula( const QString &feetFieldName, const QString &metersFieldName );
        CALCULATORS_EXPORT QString metersToFeetFormula( const QString &feetFieldName, const QString &metersFieldName );

        CALCULATORS_EXPORT double degreeChangeForPressureChange( bool imperial, double temperature );
        CALCULATORS_EXPORT double pressureChangeForDegreeChange( bool imperial, double pressure );

        CALCULATORS_EXPORT QString degreeChangeForPressureChangeFormula( const QString &tempFieldName, const QString &pressureFieldName );
        CALCULATORS_EXPORT QString pressureChangeForDegreeChangeFormula( const QString &tempFieldName, const QString &pressureFieldName );

        CALCULATORS_EXPORT double lbsToKGs( double lbs );
        CALCULATORS_EXPORT double kgsToLbs( double kgs );

        CALCULATORS_EXPORT QString lbsToKGsFormula( const QString &lbsFieldName, const QString &kgsFieldName );
        CALCULATORS_EXPORT QString kgsToLbsFormula( const QString &lbsFieldName, const QString &kgsFieldName );

        CALCULATORS_EXPORT double farenheightToCelsius( double temp );
        CALCULATORS_EXPORT double celsiusToFarenheight( double temp );

        CALCULATORS_EXPORT QString farenheightToCelsiusFormula( const std::optional< QString > &celsiusFieldName, const QString &farenheightFieldName );
        CALCULATORS_EXPORT QString celsiusToFarenheightFormula( const QString &celsiusFieldName, const std::optional< QString > &farenheightFieldName );

        namespace NCaloriesComputer
        {
            CALCULATORS_EXPORT double computeCalories( bool imperial, bool seaWater, double weight, double depth, double temperature, double activityLevelMultiplier, double duration );
            CALCULATORS_EXPORT double computeDuration( bool imperial, bool seaWater, double calories, double weight, double depth, double temperature, double activityLevelMultiplier );

            CALCULATORS_EXPORT QString computeCaloriesFormula( bool imperial, bool seaWater, const QString &caloriesFieldName, const QString &weightFieldName, const QString &depthFieldName, const std::pair< TOptionalDouble, QString > &tempFieldNameAndValue, const QString &activityLevelFieldName, const QString &durationFieldName );
            CALCULATORS_EXPORT QString computeDurationFormula( bool imperial, bool seaWater, const QString &caloriesFieldName, const QString &weightFieldName, const QString &depthFieldName, const std::pair< TOptionalDouble, QString > &tempFieldNameAndValue, const QString &activityLevelFieldName, const QString &durationFieldName );
        }
    }

    CALCULATORS_EXPORT QString joinFormulas( const QStringList &formulas );
    CALCULATORS_EXPORT QString descForType( EVariableType type );
    CALCULATORS_EXPORT QString fieldNameForType( EVariableType type );

    CALCULATORS_EXPORT std::size_t numEmpty( const TOptionalDoubleVector &values );
    CALCULATORS_EXPORT bool valuesValid( const TOptionalDoubleVector &values, bool checkNumEmpty = true );
    CALCULATORS_EXPORT QString doubleToString( const TOptionalDouble &value, int numDecimal );
};

#endif
