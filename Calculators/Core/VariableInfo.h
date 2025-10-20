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
#include <QStringList>
#include <list>

class QLineEdit;
class QDoubleSpinBox;
class QFormLayout;
class QComboBox;

struct SRange
{
    double fMin{ 0.0 };
    double fMax{ 0.0 };
    double fDefaultValue{ 0.0 };
    double fStep{ 0.0 };
};

class CALCULATORS_EXPORT CVariableInfo
{
public:
    CVariableInfo( const QString &name, const QString &desc, EVariableType type, EUnit unitType, EVariableLoc variableLocation );
    CVariableInfo( const QString &name, const QString &desc, EVariableType type, EUnit unitType, EVariableLoc variableLocation, const SRange &range );
    CVariableInfo( const QString &name, const QString &desc, EVariableType type, EUnit unitType, EVariableLoc variableLocation, const TNamedValueItemList &values );
    CVariableInfo( const QString &name, const QString &desc, EVariableType type, EUnit unitType, EVariableLoc variableLocation, const QString &unitLabel );
    CVariableInfo( EVariableType type );   // for use with constants
    ~CVariableInfo() {}

    QString name() const { return fName; }
    EVariableLoc variableLoc() const { return fVariableLocation; }

    bool createWidgets( CSCUBACalculatorPage *page, QFormLayout *formLayout );

    void updateLabels( bool imperial, bool seaWater );

public:
    QString unitText( bool imperial, bool seaWater, bool tex, EFormulaType formulaType ) const;

    void resetValue( bool updateUI, bool notifyUI );   // if updateUI set, fField is updated, if notifyUpdate is true signals are emitted of the change

    void updateFieldFromValue( bool notifyUI = false );   // updates fField from fValue
    void updateValueFromField();   // updates fValue from fField
    void updateFormula( bool imperial, bool seaWater, QString &newFormula, EFormulaType formulaType ) const;

    int numDecimals() const { return ( fUnit == EUnit::ePercent ) ? 0 : 2; }
    double formulaValue() const;   // user responsible for calling has_value first
    double value() const;   // user responsible for calling has_value first
    TOptionalDouble optValue() const;

    bool has_value() const { return fValue.has_value(); }
    void setValue( TOptionalDouble value ) { fValue = value; }

    bool isVariable() const { return fType == EVariableType::eVariable; }
    bool isWidget( QWidget *widget ) const;

    void setRange( const SRange &range ) { fRange = range; }
    void setValues( const TNamedValueItemList &values ) { fValues = values; }
    void setUnitLabel( const QString &string ) { fUnitText = string; }   // overrides default behavior and always uses this string for the label

    QLineEdit *lineEdit() const;
    QDoubleSpinBox *doubleSpinBox() const;
    QComboBox *comboBox() const;

    void reverseVariableLoc();

    bool needsFieldUpdate( QWidget *triggerWidget );
    bool hasValues() const;
    std::optional< TOptionalDoubleVector > validValues() const;
    bool hasCustomValue() const;

private:
    TOptionalDouble valueForString( const QString &text ) const;
    void updateFieldFromValue( QDoubleSpinBox *spinBox, bool notifyUI );
    void updateFieldFromValue( QComboBox *comboBox, bool notifyUI );
    void updateFieldFromValue( QLineEdit *lineEdit, bool notifyUI );

private:
    void clearField( bool notifyUI );

    QString fName;
    QString fDescription;   // used as place holder text as well
    EVariableType fType{ EVariableType::eVariable };
    EUnit fUnit{ EUnit::eNone };

    EVariableLoc fVariableLocation{ EVariableLoc::eRHS };

    QLabel *fLabel{ nullptr };
    QWidget *fField{ nullptr };
    QLabel *fUnitLabel{ nullptr };

    std::list< QWidget * > fExtraInputWidgets;

    std::optional< SRange > fRange;
    TOptionalNamedValueItemList fValues;
    std::optional< QString > fUnitText;
    TOptionalDouble fValue;
};
#endif
