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

#ifndef GENERATEDFORMULADATA_H
#define GENERATEDFORMULADATA_H

#include "SCUBACalculatorFwd.h"

#include <unordered_set>
#include <list>
#include <utility>
#include <cstdint>

class CALCULATORS_EXPORT CGeneratedFormulaData
{
public:
    CGeneratedFormulaData( const TFormulaList &formulas, const std::function< bool( const QString &formula ) > &beenCreated );

    virtual const TFormulaList &formulaList() const { return fByNameList; }

    virtual void addSVG( QJsonObject &obj, const std::optional< QByteArray > &svg, const std::optional< QDateTime > &renderedDate );
    virtual const std::list< QJsonArray > &jsonArrays() const { return fJsonArrays; }
    virtual bool updated() const { return fNumToRender != 0; }
    virtual bool hasError() const { return fNumErrors != 0; }

    virtual std::size_t numTotal() const { return fNumTotal; }
    virtual std::size_t numToRender() const { return fNumToRender; }
    virtual std::size_t numErrors() const { return fNumErrors; }

    virtual bool operator==( const CGeneratedFormulaData &rhs ) const;

private:
    virtual QJsonArray &jsonArray();
    virtual void newArray();
    std::unordered_set< QString > fAllFormulas;

    TFormulaList fByNameList;
    std::list< QJsonArray > fJsonArrays;
    std::pair< std::size_t, std ::size_t > fJSONSize{ 0, 0 };   // num, size

    void sortByName();
    void addFormula( const TFormula &formula, const std::function< bool( const QString &formula ) > &beenCreated );
    std::size_t fNumErrors{ 0 };
    std::size_t fNumToRender{ 0 };
    std::size_t fNumTotal{ 0 };
};

#endif
