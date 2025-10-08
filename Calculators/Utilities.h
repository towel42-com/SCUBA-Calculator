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
        CALCULATORS_EXPORT QString tempUnit( bool imperial, bool useAbbreviations, bool tex );   // C/F
        CALCULATORS_EXPORT QString absZeroTempUnit( bool imperial, bool useAbbreviations, bool tex );   //K/R
        CALCULATORS_EXPORT QString pressurePerTemp( bool imperial, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString percentUnit( bool tex );

        CALCULATORS_EXPORT QString weightOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString idealGasConstant( bool imperial, bool useAbbreviations, bool tex );

        CALCULATORS_EXPORT QString percentN2AtSurface( bool tex );
        CALCULATORS_EXPORT QString percentO2AtSurface( bool tex );

        CALCULATORS_EXPORT QString depthToSingleAtmosphere( bool imperial, bool seaWater, bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString feetToMeters( bool useAbbreviations, bool tex );
        CALCULATORS_EXPORT QString freshWaterToSeaWater( bool imperial, bool useAbbreviations, bool tex );

        CALCULATORS_EXPORT QString getUnitLabel( bool imperial, bool seaWater, EUnit unit, bool useAbbreviations, bool tex );
    }

    namespace NConstants
    {
        CALCULATORS_EXPORT double absZero( bool imperial );
        CALCULATORS_EXPORT double pressurePerTemp( bool imperial );
        CALCULATORS_EXPORT double weightOfWater( bool imperial, bool seaWater );
        CALCULATORS_EXPORT double pressureOffset( bool imperial );
        CALCULATORS_EXPORT double depthToSingleAtmosphere( bool imperial, bool seaWater );
        CALCULATORS_EXPORT double idealGasConstant( bool imperial );

        CALCULATORS_EXPORT double feetToMeters();
        CALCULATORS_EXPORT double freshWaterToSeaWater();

        CALCULATORS_EXPORT double percentN2AtSurface();
        CALCULATORS_EXPORT double percentO2AtSurface();
    }

    CALCULATORS_EXPORT double toAbsZeroBasedTemp( bool imperial, double temp );
    CALCULATORS_EXPORT double fromAbsZeroBasedTemp( bool imperial, double temp );

    CALCULATORS_EXPORT double pressureFromDepth( bool imperial, bool seaWater, double depth );
    CALCULATORS_EXPORT double depthFromPressure( bool imperial, bool seaWater, double pressure );
    CALCULATORS_EXPORT void calculateDepthToFromPressure( bool imperial, bool seaWater, TOptionalDouble &pressure, TOptionalDouble &depth );

    CALCULATORS_EXPORT QString pressureFromDepthFormula( const QString ataFieldName, const QString depthToSingleATMFieldName, const QString &depthFieldName );
    CALCULATORS_EXPORT QString depthFromPressureFormula( const QString ataFieldName, const QString depthToSingleATMFieldName, const QString &depthFieldName );

    CALCULATORS_EXPORT std::size_t numEmpty( const TOptionalDoubleVector &values );

    CALCULATORS_EXPORT bool valuesValid( const TOptionalDoubleVector &values, bool checkNumEmpty = true );
    CALCULATORS_EXPORT QString doubleToString( const TOptionalDouble &value, int numDecimal );
};

#endif
