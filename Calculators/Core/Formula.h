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

#include "CalculatorFwd.h"
#include <QString>
#include <optional>

class CFormula
{
public:
    CFormula( TVariableInfo variable, const QString &formula );
    CFormula( TConstVariableInfo variable, const QString &formula );
    CFormula( const QString &formula );

    static QString applyVariableValues( bool imperial, bool seaWater, const QString & formula, const TVariableInfoList &variables, EFormulaType formulaType );
    TFormula applyVariables( bool imperial, bool seaWater, const TVariableInfoList &variables, EFormulaType formulaType );
    QString equation( bool imperial, bool seaWater ) const;
    [[nodiscard]] TFormula getFinalValueFormula( bool imperial, bool seaWater, CCalculatorBase *calculator );
    void cleanupFormula();

    TVariableInfo variable() const { return fVariable; }
    QString formula() const { return fFormula; }

    bool isBaseFormula() const;
    void setBaseFormula( bool baseFormula ) { fBaseFormula = baseFormula; }

    bool isFormula( const TFormula &rhs ) const { return operator==( rhs ); }
    bool isFormula( const CFormula &rhs ) const { return operator==( rhs ); }
    bool isFormula( const CFormula *rhs ) const { return operator==( rhs ); }

    bool operator==( const CFormula &rhs ) const;
    bool operator==( const CFormula *rhs ) const;
    bool operator==( const TFormula &rhs ) const;

private:
    TVariableInfo fVariable;
    QString fFormula;
    bool fBaseFormula{ false };
};

inline bool operator==( const TFormula &lhs, const TFormula &rhs )
{
    return lhs->isFormula( rhs );
}

inline bool operator!=( const TFormula &lhs, const TFormula &rhs )
{
    return !operator==( lhs, rhs );
}

inline bool operator==( const TFormulaList &lsh, const TFormulaList &rhs )
{
    bool isSame = ( lsh.size() == rhs.size() );
    if ( isSame )
    {
        auto &&ii = lsh.begin();
        auto &&jj = rhs.begin();

        for ( ; isSame && ( ii != lsh.end() ) && ( jj != rhs.end() ); ++ii, ++jj )
        {
            isSame = ( *ii == *jj );
        }
    }
    return isSame;
}

inline bool operator!=( const TFormulaList &lhs, const TFormulaList &rhs )
{
    return !operator==( lhs, rhs );
}

#endif
