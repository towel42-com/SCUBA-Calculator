// The MIT License( MIT )
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
#include <optional>
#include <functional>
#include <unordered_map>

class QLineEdit;
class QDoubleSpinBox;
class QFormLayout;
class QComboBox;
class QDoubleSpinBox;

struct SRange
{
    SRange();
    SRange( double min, double max, std::optional< double > defaultValue, double step );
    SRange( QDoubleSpinBox *spinBox );

    double fMin{ 0.0 };
    double fMax{ 0.0 };
    std::optional< double > fDefaultValue;
    double fStep{ 0.0 };

    bool operator==( const SRange &rhs );
    bool operator!=( const SRange &rhs ) { return !operator==( rhs ); }
};

template< typename T >
struct SBaseInfo
{
    SBaseInfo( const std::optional< bool > &imperial, const std::optional< bool > &seaWater, const T &range ) :
        fImperial( imperial ),
        fSeaWater( seaWater ),
        fValue( range )
    {
    }
    std::optional< bool > fImperial;
    std::optional< bool > fSeaWater;
    T fValue;
};

template<>
struct std::hash< std::pair< bool, bool > >
{
    std::size_t operator()( const std::pair< bool, bool > &k ) const
    {
        using std::hash;
        using std::size_t;

        // Compute individual hash values for first,
        // second and third and combine them using XOR
        // and bit shifting:

        return ( ( std::hash< bool >()( k.first ) ^ ( std::hash< bool >()( k.second ) << 1 ) ) >> 1 );
    }
};

template< typename T >
struct SVariableValue
{
    std::optional< T > fDefaultValue;
    std::unordered_map< std::pair< bool, bool >, T > fValues;

    void setDefault( const T &value ) { fDefaultValue = value; }
    void addValue( const SBaseInfo< T > &value ) { addValue( value.fImperial, value.fSeaWater, value.fValue ); }
    void addValue( std::optional< bool > imperial, std::optional< bool > seaWater, const T &value )
    {
        if ( !imperial.has_value() && !seaWater.has_value() )
            setDefault( value );
        else if ( imperial.has_value() && !seaWater.has_value() )
        {
            fValues[ { imperial.value(), false } ] = value;
            fValues[ { imperial.value(), true } ] = value;
        }
        else if ( !imperial.has_value() && seaWater.has_value() )
        {
            fValues[ { false, seaWater.value() } ] = value;
            fValues[ { true, seaWater.value() } ] = value;
        }
        else if ( imperial.has_value() && seaWater.has_value() )
            fValues[ { imperial.value(), seaWater.value() } ] = value;

        if ( !fDefaultValue.has_value() )
            setDefault( value );
    }

    bool has_value() const
    {
        if ( fDefaultValue.has_value() )
            return true;
        return !fValues.empty();
    }

    std::optional< T > getValue( bool imperial, bool seaWater ) const
    {
        auto pos = fValues.find( { imperial, seaWater } );
        if ( pos != fValues.end() )
            return ( *pos ).second;
        return fDefaultValue;
    }

    std::size_t numValues() const
    {
        std::size_t retVal = 0;
        if ( fDefaultValue.has_value() )
            retVal++;
        retVal += fValues.size();
        return retVal;
    }
};

class CALCULATORS_EXPORT CVariableInfo
{
public:
    CVariableInfo( const QString &name, const QString &desc, EVariableType type, EUnit unitType, EVariableLoc variableLocation );
    CVariableInfo( const QString &name, const QString &desc, EUnit unitType, EVariableLoc variableLocation, const SBaseInfo< SRange > &range );
    CVariableInfo( const QString &name, const QString &desc, EUnit unitType, EVariableLoc variableLocation, const SBaseInfo< TNamedValueItemList > &values );
    CVariableInfo( const QString &name, const QString &desc, EVariableType type, EVariableLoc variableLocation, EUnit unitLabel, bool imperial );

    [[nodiscard]] std::shared_ptr< CVariableInfo > clone( const QString &suffix = {} ) const;

    ~CVariableInfo() {}

    [[nodiscard]] QString name() const { return fName; }
    [[nodiscard]] QString fieldName() const;   // includes < >
    [[nodiscard]] QString valueString( bool imperial, bool seaWater ) const;
    [[nodiscard]] QString descriptiveName( bool imperial, bool seaWater ) const;

    [[nodiscard]] EVariableLoc variableLoc() const { return fVariableLocation; }

    bool createWidgets( CSCUBACalculatorPage *page, QFormLayout *formLayout );

    void updateLabels( bool imperial, bool seaWater );
    void updateValuesAndRanges( bool imperial, bool seaWater );   // if the values or range need updating due to conditions
    [[nodiscard]] QString unitText( bool imperial, bool seaWater, bool tex, EFormulaType formulaType ) const;

    void resetValue( bool imperial, bool seaWater, bool updateUI, bool notifyUI );   // if updateUI set, fField is updated, if notifyUpdate is true signals are emitted of the change

    void updateFieldFromValue( bool imperial, bool seaWater, bool notifyUI = false );   // updates fField from fValue
    void updateValueFromField();   // updates fValue from fField
    [[nodiscard]] QString updateFormula( bool imperial, bool seaWater, const QString &formula, EFormulaType formulaType ) const;
    [[nodiscard]] static QString updateFormula( bool imperial, bool seaWater, const QString &formula, EVariableType constantType, bool descriptionNotValue );

    [[nodiscard]] int numDecimals() const { return ( ( fUnit == EUnit::ePercent ) || ( fUnit == EUnit::eLargePercent ) ) ? 0 : 2; }
    [[nodiscard]] double formulaValue() const;   // user responsible for calling has_value first
    [[nodiscard]] double value() const;   // user responsible for calling has_value first
    [[nodiscard]] TOptionalDouble optValue() const;

    [[nodiscard]] bool has_value() const { return fValue.has_value(); }
    [[nodiscard]] void setValue( TOptionalDouble value ) { fValue = value; }

    [[nodiscard]] bool isVariable() const { return fType == EVariableType::eVariable; }
    [[nodiscard]] bool isIntermediate() const { return fType == EVariableType::eIntermediate; }
    [[nodiscard]] bool isConstant() const;

    [[nodiscard]] bool isWidget( QWidget *widget ) const;

    void setDefaultRange( const SRange &range );
    void addRange( std::optional< bool > imperial, std::optional< bool > seaWater, const SRange &range );
    void addRange( const SBaseInfo< SRange > &range );
    void setDefaultValues( const TNamedValueItemList &values );
    void addValues( std::optional< bool > imperial, std::optional< bool > seaWater, const TNamedValueItemList &values );
    void addValues( const SBaseInfo< TNamedValueItemList > &valueInfo );
    void setUnitOverride( EUnit unit, bool imperial ) { fUnitOverride = { unit, imperial }; }   // overrides default behavior and always uses this string for the label

    [[nodiscard]] QLineEdit *lineEdit() const;
    [[nodiscard]] QDoubleSpinBox *doubleSpinBox() const;
    [[nodiscard]] QComboBox *comboBox() const;

    void reverseVariableLoc();
    void setVariableLoc( EVariableLoc loc );

    [[nodiscard]] bool needsFieldUpdate( QWidget *triggerWidget );
    [[nodiscard]] bool hasValues() const;
    [[nodiscard]] bool hasRange() const;
    [[nodiscard]] std::optional< TOptionalDoubleVector > validValues( bool imperial, bool seaWater ) const;

    [[nodiscard]] bool hasCustomValue() const;

    void setDependencies( const TVariableInfoList &dependencies );
    bool dependenciesSatisfied() const;

private:
    TOptionalNamedValueItemList getValues( bool imperial, bool seaWater ) const;
    TOptionalDouble valueForString( const QString &text ) const;
    void updateFieldFromValue( QDoubleSpinBox *spinBox, bool notifyUI );
    void updateFieldFromValue( QComboBox *comboBox, bool notifyUI );
    void updateFieldFromValue( QLineEdit *lineEdit, bool notifyUI );

    void clearField( bool imperial, bool seaWater, bool notifyUI );
    void setupValues( bool imperial, bool seaWater );
    void setupRange( bool imperial, bool seaWater );

    QString fName;
    QString fDescription;   // used as place holder text as well
    EVariableType fType{ EVariableType::eVariable };
    EUnit fUnit{ EUnit::eNone };
    EVariableLoc fVariableLocation{ EVariableLoc::eRHS };

    QLabel *fLabel{ nullptr };
    QWidget *fField{ nullptr };
    QLabel *fUnitLabel{ nullptr };

    std::list< QWidget * > fExtraInputWidgets;

    SVariableValue< SRange > fRanges;
    SVariableValue< TNamedValueItemList > fValues;
    std::optional< std::pair< EUnit, bool > > fUnitOverride;
    TOptionalDouble fValue;
    TWeakVariableInfoList fDependencies;
};
#endif
