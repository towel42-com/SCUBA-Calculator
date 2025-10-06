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

#ifndef VARIABLEINFO_H
#define VARIABLEINFO_H

#include "SCUBACalculatorFwd.h"
#include <QString>

struct SVariableInfo
{
    SVariableInfo( const QString &name, const QString &desc, EVariableType type, EUnit unitType, bool onRHSByDefault );
    void updateLabels( bool imperial, bool saltWater );

    void resetValue( bool updateUI, bool notifyUI ); // if updateUI set, fField is updated, if notifyUpdate is true signals are emitted of the change
    void updateFieldFromValue( bool notifyUI = false );   // updates fField from fValue
    void updateValueFromField();   // updates fValue from fField

    void updateFormula( bool imperial, bool saltWater, QString &newFormula, bool defaultFormula ) const;

    double value() const { return fValue.value(); }   // user responsible for calling has_value first
    bool has_value() const { return fValue.has_value(); }
    void setValue( double value ) { fValue = value; }

    bool isVariable() const { return fType == EVariableType::eVariable; }
    QString fName;
    QString fDescription;   // used as place holder text as well

    EVariableType fType{ EVariableType::eVariable };
    EUnit fUnit{ EUnit::eNone };

    bool fRHSVariable{ true };

    TOptionalDouble fValue;
    QLabel *fLabel{ nullptr };
    QWidget *fField{ nullptr };
    QLabel *fUnitLabel{ nullptr };
};
#endif
