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
#include <memory>
#include <QString>

class CSCUBACalculatorPage;
class CSCUBACalculator;
class QWidget;
class QLabel;
class QLineEdit;
class QString;

enum class EFormulaType;

using TOptionalDouble = std::optional< double >;
using TOptionalDoubleVector = std::vector< TOptionalDouble >;

using TUpdateFormulaFunc = std::function< void( CSCUBACalculatorPage *, const QString &, EFormulaType ) >;
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
    ePercent,
    eTime,
    eCalories
};

enum class EVariableType
{
    eIntermediate,
    eVariable,
    ePressurePerDegreeConst,
    eWeightOfWaterConst,
    eIdealGasConst,
    eFN2AtSurfaceConst,
    eFO2AtSurfaceConst,
    eDepthToSingleATMConst,
    eFeetToMetersConst,
    eMetersToFeetConst,
    eLbsPerKgsConst,
    eFreshWaterToSeaWaterConst,
    ePSIToBarConst,
    eAbsZeroOffsetConst,
    ePressureOffsetConst,
    eBaseMETofSCUBAConst
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
using TVariableInfo = std::shared_ptr< CVariableInfo >;
using TVariableInfoList = std::list< TVariableInfo >;

using TNamedValueItem = std::pair< QString, TOptionalDouble >;
using TNamedValueItemList = std::list< TNamedValueItem >;
using TOptionalNamedValueItemList = std::optional< TNamedValueItemList >;

namespace NUtilities
{
    struct SFormula;
}
using TNamedFormulaList = std::list< std::shared_ptr< NUtilities::SFormula > >;
using TStringList = std::list< QString >;
using TValuesForVariablePair = std::pair< TVariableInfo, TOptionalDoubleVector >;
using TValesForVariablePairVector = std::vector< TValuesForVariablePair >;

using TNameValuePair = std::pair< QString, TOptionalDouble >;
using TOptionalNameValuePair = std::optional< TNameValuePair >;

#endif
