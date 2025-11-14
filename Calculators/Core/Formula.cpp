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
#include "CalculatorFwd.h"

#include "Formula.h"
#include "Utilities.h"
#include "VariableInfo.h"
#include "CalculatorBase.h"
#include "include/MathJaxQt6.h"

CFormula::CFormula( TVariableInfo variable, const QString &formula ) :
    fVariable( variable ),
    fFormula( formula )
{
    Q_ASSERT( fVariable );
}

CFormula::CFormula( TConstVariableInfo variable, const QString &formula ) :
    CFormula( std::const_pointer_cast< CVariableInfo >( variable ), formula )
{
}

CFormula::CFormula( const QString &formula ) :
    CFormula( TVariableInfo(), formula )
{
}

TFormula CFormula::applyVariables( bool imperial, bool seaWater, const TVariableInfoList &variables, EFormulaType formulaType )
{
    QString finalizedFormula = fFormula;
    for ( auto &&curr : variables )
    {
        finalizedFormula = curr->updateFormula( imperial, seaWater, finalizedFormula, formulaType );
    }
    NUtilities::NConstants::foreachConstantType(   //
        [ &, this ]( EVariableType currConst )   //
        {   //
            finalizedFormula = CVariableInfo::updateFormula( imperial, seaWater, finalizedFormula, currConst, ( formulaType == EFormulaType::eBaseFormula ) );
            return true;
        } );
    return std::make_shared< CFormula >( fVariable, finalizedFormula );
}

QString CFormula::equation( bool imperial, bool seaWater ) const
{
    if ( fVariable && !fFormula.isEmpty() )
        return QString( "%1 = %2" ).arg( fVariable->descriptiveName( imperial, seaWater ) ).arg( fFormula );
    if ( fVariable && fFormula.isEmpty() )
        return QString( "%1" ).arg( fVariable->descriptiveName( imperial, seaWater ) );
    if ( !fVariable && !fFormula.isEmpty() )
        return QString( "%1" ).arg( fFormula );
    return {};
}

void CFormula::cleanupFormula()
{
    fFormula = NTowel42::cleanupFormula( fFormula );
}

bool CFormula::isBaseFormula() const
{
    if ( !fVariable )
        return false;
    return fBaseFormula && !fVariable->isIntermediate();
}

bool CFormula::operator==( const TFormula &rhs ) const
{
    return operator==( rhs.get() );
}

bool CFormula::operator==( const CFormula *rhs ) const
{
    if ( !rhs )
        return false;

    return operator==( *rhs );
}

bool CFormula::operator==( const CFormula &rhs ) const
{
    return ( fVariable == rhs.fVariable )   //
           && ( fFormula == rhs.fFormula );   //
    //&& ( fBaseFormula == rhs.fBaseFormula );
}

TFormula CFormula::getFinalValueFormula( bool imperial, bool seaWater, CCalculatorBase *calculator )
{
    if ( !fVariable )
        return {};

    auto variables = NUtilities::getVariables( fFormula );

    bool hasUnsetVar = false;
    for ( auto &&ii : variables )
    {
        auto variable = calculator->getVariable( ii );
        if ( !variable && NUtilities::isConstantVariable( ii ) )
            continue;

        Q_ASSERT( variable || ( !variable && NUtilities::isConstantVariable( ii ) ) );
        if ( !variable )
            continue;

        if ( !variable->has_value() && !variable->dependenciesSatisfied() )
        {
            hasUnsetVar = true;
            break;
        }
    }
    if ( hasUnsetVar )
        return {};

    auto prevValue = fVariable->optValue();
    if ( prevValue.has_value() )
        fVariable->resetValue( imperial, seaWater, false, false );

    calculator->computeVariableValues();

    auto currValue = fVariable->optValue();
    Q_ASSERT( !prevValue.has_value() || ( prevValue == currValue ) || fVariable->isIntermediate() );

    auto valueString = fVariable->valueString( imperial, seaWater );

    TFormula retVal;

    if ( !valueString.isEmpty() )
    {
        retVal = std::make_shared< CFormula >( fVariable, R"__(\color{green}{)__" + valueString + R"__(})__" );
    }

    if ( prevValue.has_value() && ( prevValue != currValue ) && !fVariable->isIntermediate() )
        fVariable->setValue( prevValue );
    return retVal;
}
