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

#ifndef FORMULA_H
#define FORMULA_H

#include "SCUBACalculatorFwd.h"

namespace NUtilities
{
    struct CALCULATORS_EXPORT SFormula
    {
        SFormula();
        SFormula( const SFormula &rhs, const TOptionalNameValuePair &nameValuePair );
        SFormula( const QString &name, const QString &formula, bool imperial, bool seaWater, const TOptionalNameValuePair &nameValuePair = {} );
        virtual ~SFormula() = default;

        virtual bool operator<( const SFormula &rhs ) const;
        virtual QString name() const;

        virtual bool imperial() const { return fImperial; }
        virtual bool seaWater() const { return fSeaWater; }
        virtual QString formula() const { return fFormula; }
        virtual QString cleanedFormula() const;

        virtual void setFormula( const QString &formula ) { fFormula = formula; }
        virtual bool operator==( const SFormula &rhs ) const;
        virtual bool operator!=( const SFormula &rhs ) const { return !operator==( rhs ); }

    private:
        QString fName;
        QString fFormula;
        bool fImperial{ false };
        bool fSeaWater{ false };
        TOptionalNameValuePair fNameValuePair;
    };
};

#endif
