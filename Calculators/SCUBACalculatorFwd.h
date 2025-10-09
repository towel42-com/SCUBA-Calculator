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

class CSCUBACalculatorPage;
class CSCUBACalculator;
class QWidget;
class QLabel;
class QLineEdit;
class QString;

using TOptionalDouble = std::optional< double >;
using TOptionalDoubleVector = std::vector< TOptionalDouble >;

using TUpdateFormulaFunc = std::function< void( CSCUBACalculatorPage *, const QString &, bool ) >;
using TInstantiateCalcFunc = CSCUBACalculator *(*)();
using TGetPageFunc = CSCUBACalculatorPage *(*)( CSCUBACalculator *calculator, QWidget *, bool *needsInit );
using TSetBoolFunc = void ( * )( CSCUBACalculator *calculator, bool );
using TInitFunc = void ( * )( CSCUBACalculator *calculator, bool imperial, bool seaWater );
using TSetUpdateFormulaFunc = void ( * )( CSCUBACalculator *calculator, const TUpdateFormulaFunc & );

constexpr char kInstantiateCalcFuncName[] = "instantiateCalculator";
constexpr char kGetPageFuncName[] = "getPage";
constexpr char kSetImperialFuncName[] = "setImperial";
constexpr char kSetSeaWaterFuncName[] = "setSeaWater";
constexpr char kSetUpdateFormulaFuncName[] = "setUpdateFormulaFunc";
constexpr char kInitFuncName[] = "initCalculatorPage";

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
    ePercent
};

enum class EVariableType
{
    eHidden,
    eVariable,
    ePressurePerTempConst,
    eWeightOfWaterConst,
    eIdealGasConst,
    eFN2AtSurfaceConst,
    eFO2AtSurfaceConst,
    eDepthToSingleAtmosphereConst,
    eMetersToFeetConst,
    eSeaWaterToFreshWaterConst,
    ePSIToBarConst,
    eAbsZeroOffsetConst
};

enum class EVariableLoc
{
    eLHS,
    eRHS,
    eGlobal
};

struct SVariableInfo;
using TVariableInfo = std::shared_ptr< SVariableInfo >;
using TVariableInfoList = std::list< TVariableInfo >;

using TNamedValueItem = std::pair< QString, TOptionalDouble >;
using TNamedValueItemList = std::list< TNamedValueItem >;
using TOptionalNamedValueItemList = std::optional< TNamedValueItemList >;

#endif
