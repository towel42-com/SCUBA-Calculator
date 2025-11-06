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
        CALCULATORS_EXPORT double absZeroOffset( bool imperial );
        CALCULATORS_EXPORT double pressureChangePerDegreeChange( bool imperial );
        CALCULATORS_EXPORT double weightPerVolumeOfWater( bool imperial, bool seaWater );
        CALCULATORS_EXPORT double volumePerWeightOfWater( bool imperial, bool seaWater );
        CALCULATORS_EXPORT double pressureAtSurface( bool imperial );
        CALCULATORS_EXPORT double singleATMPerDepth( bool imperial, bool seaWater );
        CALCULATORS_EXPORT double idealGasConstant( bool imperial );

        CALCULATORS_EXPORT double metersPerFoot();
        CALCULATORS_EXPORT double feetPerMeters();

        CALCULATORS_EXPORT double freshWaterToSeaWater();
        CALCULATORS_EXPORT double seaWaterToFreshWater();

        CALCULATORS_EXPORT double percentN2AtSurface();
        CALCULATORS_EXPORT double percentO2AtSurface();
        CALCULATORS_EXPORT double psiPerBAR();
        CALCULATORS_EXPORT double barPerPSI();

        CALCULATORS_EXPORT double lbsPerKGs();
        CALCULATORS_EXPORT double kgsPerLbs();

        CALCULATORS_EXPORT double scubaMET();
        CALCULATORS_EXPORT double fillRateAir( bool imperial );
        CALCULATORS_EXPORT double fillRateO2( bool imperial );

        CALCULATORS_EXPORT double cubicFeetPerLiter();
        CALCULATORS_EXPORT double litersPerCubicFoot();

        CALCULATORS_EXPORT double pressureLossPerAltitudeGain( bool imperial );
        CALCULATORS_EXPORT double safetyStopDepth( bool imperial, bool seaWater );
        CALCULATORS_EXPORT double waterWeightAdjustment( bool imperial, bool seaWater );

        CALCULATORS_EXPORT QString pressureChangePerDegreeChange( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString lbsPerKGs( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString kgsPerLbs( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString scubaMET( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString fillRateO2( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString fillRateAir( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString weightPerVolumeOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString volumePerWeightOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString idealGasConstant( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString percentN2AtSurface( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString percentO2AtSurface( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString singleATMPerDepth( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString metersPerFoot( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString feetPerMeter( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString freshWaterToSeaWater( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString seaWaterToFreshWater( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString barPerPSI( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString psiPerBAR( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString pressureAtSurface( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString pressureLossPerAltitudeGain( bool imperial, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString safetyStopDepth( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString waterWeightAdjustment( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString litersPerCubicFoot( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString cubicFeetPerLiter( bool useAbbreviations, bool tex, bool description );
        CALCULATORS_EXPORT QString absZeroOffset( bool imperial, bool useAbbreviations, bool tex, bool description );

        void foreachConstantType( const std::function< void( EVariableType ) > &onConstType );
        QString constantString( bool imperial, bool seaWater, EVariableType constantType, bool description );
        TOptionalDouble constantValue( bool imperial, bool seaWater, EVariableType constantType );
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
    CALCULATORS_EXPORT QString equation( bool imperial, bool seaWater, const TFormulaString &formula );
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
