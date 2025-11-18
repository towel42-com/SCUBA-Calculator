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

#include "CalculatorFwd.h"

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

        CALCULATORS_EXPORT QString pressureChangePerDegreeChange( bool imperial, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString lbsPerKGs( bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString kgsPerLbs( bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString scubaMET( bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString fillRateO2( bool imperial, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString fillRateAir( bool imperial, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString weightPerVolumeOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString volumePerWeightOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString idealGasConstant( bool imperial, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString percentN2AtSurface( bool imperial, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString percentO2AtSurface( bool imperial, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString singleATMPerDepth( bool imperial, bool seaWater, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString metersPerFoot( bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString feetPerMeter( bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString freshWaterToSeaWater( bool imperial, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString seaWaterToFreshWater( bool imperial, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString barPerPSI( bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString psiPerBAR( bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString pressureAtSurface( bool imperial, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString pressureLossPerAltitudeGain( bool imperial, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString safetyStopDepth( bool imperial, bool seaWater, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString waterWeightAdjustment( bool imperial, bool seaWater, bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString litersPerCubicFoot( bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString cubicFeetPerLiter( bool useAbbreviations, bool tex, EFormulaType formulaType );
        CALCULATORS_EXPORT QString absZeroOffset( bool imperial, bool useAbbreviations, bool tex, EFormulaType formulaType );

        void foreachConstantType( const std::function< bool( EConstantType ) > &onConstType );   // function returns true if the loop should continue
        QString constantString( bool imperial, bool seaWater, EConstantType constantType, EFormulaType formulaType );
        TOptionalDouble constantValue( bool imperial, bool seaWater, EConstantType constantType );
    }

    namespace NConversions
    {
        CALCULATORS_EXPORT double toAbsZeroBasedTemp( bool imperial, double temp );
        CALCULATORS_EXPORT double fromAbsZeroBasedTemp( bool imperial, double temp );

        CALCULATORS_EXPORT double psiToBar( double psi );
        CALCULATORS_EXPORT double barToPSI( double bar );

        CALCULATORS_EXPORT TFormula psiToBarFormula( const TConstVariableInfo &psi, const TConstVariableInfo &bar );
        CALCULATORS_EXPORT TFormula barToPSIFormula( const TConstVariableInfo &psi, const TConstVariableInfo &bar );

        CALCULATORS_EXPORT double depthToATA( bool imperial, bool seaWater, double depth );
        CALCULATORS_EXPORT double ataToDepth( bool imperial, bool seaWater, double pressure );

        CALCULATORS_EXPORT TFormula depthToATAFormula( const TConstVariableInfo &ata, const TConstVariableInfo &depth );
        CALCULATORS_EXPORT TFormula ataToDepthFormula( const TConstVariableInfo &ata, const TConstVariableInfo &depth );

        CALCULATORS_EXPORT double depthFreshwaterToSeawater( double depthFW );
        CALCULATORS_EXPORT double depthSeawaterToFreshwater( double depthSW );

        CALCULATORS_EXPORT TFormula depthFreshwaterToSeawaterFormula( const TConstVariableInfo &freshWater, const TConstVariableInfo &seaWater );
        CALCULATORS_EXPORT TFormula depthSeawaterToFreshwaterFormula( const TConstVariableInfo &freshWater, const TConstVariableInfo &seaWater );

        CALCULATORS_EXPORT double feetToMeters( double feet );
        CALCULATORS_EXPORT double metersToFeet( double meters );

        CALCULATORS_EXPORT TFormula feetToMetersFormula( const TConstVariableInfo &feet, const TConstVariableInfo &meters );
        CALCULATORS_EXPORT TFormula metersToFeetFormula( const TConstVariableInfo &feet, const TConstVariableInfo &meters );

        CALCULATORS_EXPORT double degreeChangeForPressureChange( bool imperial, double temperature );
        CALCULATORS_EXPORT double pressureChangeForDegreeChange( bool imperial, double pressure );

        CALCULATORS_EXPORT TFormula degreeChangeForPressureChangeFormula( const TConstVariableInfo &temperature, const TConstVariableInfo &pressure );
        CALCULATORS_EXPORT TFormula pressureChangeForDegreeChangeFormula( const TConstVariableInfo &temperature, const TConstVariableInfo &pressure );

        CALCULATORS_EXPORT double lbsToKGs( double lbs );
        CALCULATORS_EXPORT double kgsToLbs( double kgs );

        CALCULATORS_EXPORT TFormula lbsToKGsFormula( const TConstVariableInfo &lbs, const TConstVariableInfo &kgs );
        CALCULATORS_EXPORT TFormula kgsToLbsFormula( const TConstVariableInfo &lbs, const TConstVariableInfo &kgs );

        CALCULATORS_EXPORT double farenheightToCelsius( double temp );
        CALCULATORS_EXPORT double celsiusToFarenheight( double temp );

        CALCULATORS_EXPORT TFormula farenheightToCelsiusFormula( const TConstVariableInfo &celsius, const TConstVariableInfo &farenheight );
        CALCULATORS_EXPORT TFormula celsiusToFarenheightFormula( const TConstVariableInfo &celsius, const TConstVariableInfo &farenheight );

        CALCULATORS_EXPORT double cubicFeetToLiters( double volume );
        CALCULATORS_EXPORT double litersToCubicFeet( double volume );

        CALCULATORS_EXPORT TFormula cubicFeetToLitersFormula( const TConstVariableInfo &cubicFeet, const TConstVariableInfo &liters );
        CALCULATORS_EXPORT TFormula litersToCubicFeetFormula( const TConstVariableInfo &cubicFeet, const TConstVariableInfo &liters );

        CALCULATORS_EXPORT double sacToRMV( double sac, double volume, double pressure );
        CALCULATORS_EXPORT double rmvToSAC( double rmv, double volume, double pressure );
        CALCULATORS_EXPORT TFormula sacToRMVFormula( const TConstVariableInfo &sac, const TConstVariableInfo &rmv, const TConstVariableInfo &tankVolume, const TConstVariableInfo &tankPressure );
        CALCULATORS_EXPORT TFormula rmvToSACFormula( const TConstVariableInfo &sac, const TConstVariableInfo &rmv, const TConstVariableInfo &tankVolume, const TConstVariableInfo &tankPressure );

        CALCULATORS_EXPORT double surfacePressureAtAltitude( bool imperial, double altitude );
        CALCULATORS_EXPORT TFormula surfacePressureAtAltitudeFormula( bool imperial, const TConstVariableInfo &surfacePressure, const TConstVariableInfo &altitude );

        CALCULATORS_EXPORT double altitudeForSurfacePressure( bool imperial, double surfacePressure );
        CALCULATORS_EXPORT TFormula altitudeForSurfacePressureFormula( bool imperial, const TConstVariableInfo &surfacePressure, const TConstVariableInfo &altitude );

        namespace NCaloriesComputer
        {
            CALCULATORS_EXPORT double computeCalories( bool imperial, bool seaWater, double weight, double depth, double temperature, double activityLevelMultiplier, double duration );
            CALCULATORS_EXPORT double computeDuration( bool imperial, bool seaWater, double calories, double weight, double depth, double temperature, double activityLevelMultiplier );

            CALCULATORS_EXPORT TFormulaList computeCaloriesFormula( bool imperial, bool seaWater, const TConstVariableInfo &calories, const TConstVariableInfo &weight, const TConstVariableInfo &depth, const TConstVariableInfo &temperature, const TConstVariableInfo &activityLevel, const TConstVariableInfo &duration );
            CALCULATORS_EXPORT TFormulaList computeDurationFormula( bool imperial, bool seaWater, const TConstVariableInfo &calories, const TConstVariableInfo &weight, const TConstVariableInfo &depth, const TConstVariableInfo &temperature, const TConstVariableInfo &activityLevel, const TConstVariableInfo &duration );
        }
    }

    CALCULATORS_EXPORT std::optional< QString > joinFormulas( bool imperial, bool seaWater, const TFormulaList &formulas );
    CALCULATORS_EXPORT QString equation( bool imperial, bool seaWater, const TFormula &formula );
    CALCULATORS_EXPORT QString descForType( EConstantType type );
    CALCULATORS_EXPORT QString fieldNameForType( EConstantType type );
    CALCULATORS_EXPORT std::optional< EConstantType > typeForFieldName( const QString &fieldName );
    CALCULATORS_EXPORT bool isConstantVariable( const QString &fieldName );

    CALCULATORS_EXPORT std::size_t numEmpty( const TOptionalDoubleVector &values );
    CALCULATORS_EXPORT QString doubleToString( const TOptionalDouble &value, int numDecimal );

    CALCULATORS_EXPORT TFormula ratioFormula( const TConstVariableInfo &returnVariable, const TConstVariableInfo &numerator, const TConstVariableInfo &denominator );
    CALCULATORS_EXPORT TFormula ratioFormula( const TConstVariableInfo &returnVariable, const TConstVariableInfo &numerator, const QString &denominator );
    CALCULATORS_EXPORT TFormula ratioFormula( const TConstVariableInfo &returnVariable, const QString &numerator, const TConstVariableInfo &denominator );
    CALCULATORS_EXPORT TFormula ratioFormula( const TConstVariableInfo &returnVariable, const TConstVariableInfo &numerator, EConstantType denominator );
    CALCULATORS_EXPORT TFormula ratioFormula( const TConstVariableInfo &returnVariable, EConstantType numerator, const TConstVariableInfo &denominator );
    CALCULATORS_EXPORT TFormula ratioFormula( const TConstVariableInfo &returnVariable, const TConstVariableInfo &numerator, const TConstVariableInfo &denominator );

    CALCULATORS_EXPORT QString ratio( const QString &numerator, const QString &denominator, bool tex );
    CALCULATORS_EXPORT QString ratio( const QString &description, const QString &numerator, const QString &denominator, bool tex );
    CALCULATORS_EXPORT QString ratio( double value, const QString &numerator, const QString &denominator, bool tex, std::optional< int > numDecimal = {} );

    CALCULATORS_EXPORT QStringList getVariables( const QString &formula );
    CALCULATORS_EXPORT bool hasVariable( const QString &formula );

    CALCULATORS_EXPORT [[nodiscard]] TFormulaList sortAndUniquifyFormulas( const TFormulaList &formulas );

    CALCULATORS_EXPORT bool fromString( std::optional< EUnit > &value, const QString &str );
    CALCULATORS_EXPORT QString toString( EUnit value );
    CALCULATORS_EXPORT bool fromString( std::optional< EVariableLoc > &value, const QString &str );
    CALCULATORS_EXPORT QString toString( EVariableLoc value );
};

#endif
