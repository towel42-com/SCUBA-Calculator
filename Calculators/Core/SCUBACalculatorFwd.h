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

#ifndef SCUBACALCULATOR_FWD_H
#define SCUBACALCULATOR_FWD_H

#if defined( WINDOWS ) || defined( WIN32 ) || defined( Q_OS_WINDOWS )
    #ifdef CALCULATORS_INTERNAL
        #define CALCULATORS_EXPORT __declspec( dllexport )
    #else
        #define CALCULATORS_EXPORT __declspec( dllimport )
    #endif
#else
    #define CALCULATORS_EXPORT
#endif

#include <optional>
#include <functional>
#include <vector>
#include <list>
#include <memory>
#include <unordered_set>
#include <tuple>
#include <optional>
#include <unordered_map>

#include <QString>
#include <QLoggingCategory>

class CSCUBACalculatorPage;
class CSCUBACalculator;
class QWidget;
class QLabel;
class QLineEdit;
class QString;
class QJsonArray;

enum class EFormulaType;

using TOptionalDouble = std::optional< double >;
using TOptionalDoubleVector = std::vector< TOptionalDouble >;

using TUpdateFormulaFunc = std::function< void( CSCUBACalculatorPage *, const QString &, bool finished ) >;
using TInstantiateCalcFunc = CSCUBACalculator *(*)();

constexpr char kInstantiateCalcFuncName[] = "instantiateCalculator";

enum class EUnit
{
    eNone,
    eVolume,
    eWeight,
    eLength,
    eDepth,
    ePressure,
    eAtmospheres,
    eTemperature,
    eAbsZeroTemperature,
    ePercent,   // values expected to be below 100%
    eLargePercent,   // values expected to be above 100%
    eTime,
    eCalories,
    eVolumePerMinute,
    ePressurePerMinute
};

enum class EVariableType
{
    eIntermediate,
    eVariable,
    ePressurePerDegreeConst,
    eWeightPerVolumeOfWaterConst,
    eVolumePerWeightOfWaterConst,
    eIdealGasConst,
    eFN2AtSurfaceConst,
    eFO2AtSurfaceConst,
    eDepthToSingleATMConst,
    eFeetToMetersConst,
    eMetersToFeetConst,
    eLbsPerKgsConst,
    eKgsPerLbsConst,
    eFreshWaterToSeaWaterConst,
    eSeaWaterToFreshWaterConst,
    ePSIToBarConst,
    eBarToPSIConst,
    eAbsZeroOffsetConst,
    ePressureAtSurfaceConst,
    ePressureLossPerAltitudeGainConst,
    eBaseMETofSCUBAConst,
    eFillRateO2Const,
    eFillRateAirConst,
    eCubicFeetToLitersConst,
    eLitersToCubicFeetConst,
    eSafetyStopDepthConst,
    eWaterWeightAdjustmentConst
};

enum class EVariableLoc
{
    eLHS,
    eRHS
};

enum class EFormulaType
{
    eBaseFormula,   // the formula used as part of the description up top
    eCurrentFormula,   // the current formula in use (may be the same as base formula) without values
    eCurrentValueFormula   // the current formula with values filled in
};

class CVariableInfo;
using TConstVariableInfo = std::shared_ptr< const CVariableInfo >;
using TConstVariableInfoList = std::list< TConstVariableInfo >;

using TVariableInfo = std::shared_ptr< CVariableInfo >;
using TVariableInfoList = std::list< TVariableInfo >;

using TNamedValueItem = std::pair< QString, TOptionalDouble >;
using TNamedValueItemList = std::list< TNamedValueItem >;
using TOptionalNamedValueItemList = std::optional< TNamedValueItemList >;

namespace NUtilities
{
    struct SFormula;
}
using TFormula = std::shared_ptr< NUtilities::SFormula >;
using TFormulaList = std::list< TFormula >;
using TStringList = std::list< QString >;
using TValuesForVariablePair = std::pair< TVariableInfo, TOptionalDoubleVector >;
using TValuesForVariablePairVector = std::vector< TValuesForVariablePair >;
using TVariableValuePair = std::pair< TVariableInfo, TOptionalDouble >;
using TVariableValuePairVector = std::vector< TVariableValuePair >;
using TOptionalVariableValuePairVector = std::optional< TVariableValuePairVector >;
using TVariableValuePairVectorVector = std::vector< TVariableValuePairVector >;

using TNameValuePair = std::pair< QString, TOptionalDouble >;
using TOptionalNameValuePair = std::optional< TNameValuePair >;

class CGeneratedFormulaData;
using TGeneratedFormulaData = std::shared_ptr< CGeneratedFormulaData >;
using TFormulaMap = std::unordered_map< CSCUBACalculator *, TGeneratedFormulaData >;

class CFormulaString;
using TFormulaString = std::shared_ptr< CFormulaString >;
using TFormulaStringList = std::list< TFormulaString >;

CALCULATORS_EXPORT Q_DECLARE_LOGGING_CATEGORY( ScubaCalculator );

#endif
