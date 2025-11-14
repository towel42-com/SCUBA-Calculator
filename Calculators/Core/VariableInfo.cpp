#include "VariableInfo.h"
#include "CalculatorPage.h"
#include "CalculatorBase.h"

#include "Utilities.h"
#include "SABUtils/DelayLineEdit.h"

#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QComboBox>

#include <optional>
#include <algorithm>

CVariableInfo::CVariableInfo( const QString &name, const QString &desc, EUnit unitType, EVariableLoc variableLocation ) :
    fName( name ),
    fDescription( desc ),
    fUnit( unitType ),
    fVariableLocation( variableLocation )
{
}

CVariableInfo::CVariableInfo( const QString &name, const QString &desc, EUnit unitType, EVariableLoc variableLocation, bool imperial ) :
    CVariableInfo( name, desc, EUnit::eNone, variableLocation )
{
    setUnitOverride( unitType, imperial );
}

CVariableInfo::CVariableInfo( const QString &name, const QString &desc, EUnit unitType, EVariableLoc variableLocation, const SBaseInfo< SRange > &rangeInfo ) :
    CVariableInfo( name, desc, unitType, variableLocation )
{
    addRange( rangeInfo );
}

CVariableInfo::CVariableInfo( const QString &name, const QString &desc, EUnit unitType, EVariableLoc variableLocation, const SBaseInfo< TNamedValueItemList > &valuesInfo ) :
    CVariableInfo( name, desc, unitType, variableLocation )
{
    addValues( valuesInfo );
}

bool CVariableInfo::createWidgets( CCalculatorPage *page, QFormLayout *formLayout )
{
    if ( !isVariable() )
        return false;

    fLabel = new QLabel( page );
    fLabel->setText( QString( "%1:" ).arg( fDescription ) );

    QHBoxLayout *fieldLayout = nullptr;
    if ( hasRange() )
    {
        auto spinBox = new QDoubleSpinBox( page );
        fField = spinBox;
        spinBox->setObjectName( fName.data() );
        spinBox->setDecimals( 2 );
    }
    else if ( hasValues() )
    {
        auto comboBox = new QComboBox( page );
        comboBox->setEditable( false );
        comboBox->setObjectName( fName.data() );
        fField = comboBox;

        auto le = new NSABUtils::CDelayLineEdit( page );
        le->setObjectName( ( fName + "-auxData" ).data() );
        fExtraInputWidgets.push_back( le );

        fieldLayout = new QHBoxLayout();
        fieldLayout->addWidget( comboBox );
        fieldLayout->addWidget( le );
    }
    else
    {
        fField = new NSABUtils::CDelayLineEdit( page );
        fField->setObjectName( fName.data() );
    }

    fUnitLabel = new QLabel( page );

    auto hLayout = new QHBoxLayout();
    if ( fieldLayout )
        hLayout->addLayout( fieldLayout );
    else
        hLayout->addWidget( fField );
    hLayout->addWidget( fUnitLabel );

    formLayout->addRow( fLabel, hLayout );
    page->addWidget( fVariableLocation, fField );
    for ( auto &&ii : fExtraInputWidgets )
        page->addWidget( fVariableLocation, ii );

    updateLabels( page->imperial(), page->seaWater() );
    return true;
}

void CVariableInfo::updateLabels( bool imperial, bool seaWater )
{
    if ( !isVariable() )
        return;

    Q_ASSERT( fField && fUnitLabel );

    auto unitText = this->unitText( imperial, seaWater, false, EFormulaType::eCurrentFormula );

    if ( lineEdit() )
    {
        auto placeHolder = fDescription;
        if ( !unitText.isEmpty() )
            placeHolder += QString( "(%1)" ).arg( unitText );

        lineEdit()->setPlaceholderText( placeHolder );
    }

    if ( fUnitLabel )
        fUnitLabel->setText( unitText );
}

QString CVariableInfo::unitText( bool imperial, bool seaWater, bool tex, EFormulaType formulaType ) const
{
    auto unitType = fUnit;

    if ( fUnitOverride.has_value() )
    {
        unitType = fUnitOverride.value().first;
        imperial = fUnitOverride.value().second;
    }

    switch ( unitType )
    {
        case EUnit::eNone:
            return {};
        case EUnit::eVolume:
            return NUtilities::NUnitStrings::volumeUnit( imperial, true, tex );
        case EUnit::eCalories:
            return NUtilities::NUnitStrings::energyUnit( imperial, true, tex );
        case EUnit::eVolumePerMinute:
            return NUtilities::NUnitStrings::volumePerMinuteUnit( imperial, true, tex );
        case EUnit::ePressurePerMinute:
            return NUtilities::NUnitStrings::pressurePerMinuteUnit( imperial, true, tex );
        case EUnit::eWeight:
            return NUtilities::NUnitStrings::weightUnit( imperial, true, tex );
        case EUnit::eLength:
            return NUtilities::NUnitStrings::lengthUnit( imperial, true, tex );
        case EUnit::eDepth:
            return NUtilities::NUnitStrings::depthUnit( imperial, seaWater, true, tex );
        case EUnit::ePressure:
            return NUtilities::NUnitStrings::pressureUnit( imperial, true, tex );
        case EUnit::eAtmospheres:
            return NUtilities::NUnitStrings::atmosphereUnit( imperial, true, tex );
        case EUnit::eTemperature:
            return NUtilities::NUnitStrings::tempUnit( imperial, true, tex );
        case EUnit::eAbsZeroTemperature:
            {
                if ( tex && ( !has_value() || ( formulaType != EFormulaType::eCurrentValueFormula ) ) )
                    return NUtilities::NUnitStrings::absZeroTempUnit( imperial, true, true );
                else
                    return NUtilities::NUnitStrings::tempUnit( imperial, true, false );
            }
        case EUnit::ePercent:
        case EUnit::eLargePercent:
            return NUtilities::NUnitStrings::percentUnit( imperial, true, tex );
        case EUnit::eTime:
            return NUtilities::NUnitStrings::timeUnit( imperial, true, tex );
        default:
            return {};
    }
}

void CVariableInfo::resetValue( bool imperial, bool seaWater, bool updateUI, bool notifyUI )
{
    fValue.reset();
    if ( !updateUI )
        return;
    updateFieldFromValue( imperial, seaWater, notifyUI );
}

TOptionalDouble CVariableInfo::optValue() const
{
    TOptionalDouble retVal;
    if ( isVariable() )
    {
        if ( lineEdit() && !lineEdit()->text().isEmpty() )
            retVal = valueForString( lineEdit()->text() );
        else if ( doubleSpinBox() )
            retVal = doubleSpinBox()->value();
        else if ( comboBox() )
        {
            auto le = fExtraInputWidgets.empty() ? nullptr : ( dynamic_cast< QLineEdit * >( fExtraInputWidgets.front() ) );
            if ( !comboBox()->currentData().isNull() )
                retVal = comboBox()->currentData().toDouble();
            else if ( le && !le->text().isEmpty() )
                retVal = valueForString( le->text() );
        }
    }
    else if ( isIntermediate() )
    {
        retVal = fValue;
    }
    return retVal;
}

QLineEdit *CVariableInfo::lineEdit() const
{
    return ( dynamic_cast< QLineEdit * >( fField ) );
}

QDoubleSpinBox *CVariableInfo::doubleSpinBox() const
{
    return ( dynamic_cast< QDoubleSpinBox * >( fField ) );
}

QComboBox *CVariableInfo::comboBox() const
{
    return ( dynamic_cast< QComboBox * >( fField ) );
}

void CVariableInfo::reverseVariableLoc()
{
    if ( fVariableLocation == EVariableLoc::eLHS )
        setVariableLoc( EVariableLoc::eRHS );
    else if ( fVariableLocation == EVariableLoc::eRHS )
        setVariableLoc( EVariableLoc::eLHS );
}

void CVariableInfo::setVariableLoc( EVariableLoc loc )
{
    if ( !isVariable() )
        return;
    fVariableLocation = loc;
}

bool CVariableInfo::needsFieldUpdate( QWidget *triggerWidget )
{
    if ( !isWidget( triggerWidget ) )
        return true;

    auto currValue = optValue();
    if ( !comboBox() && !currValue.has_value() )
        return true;

    if ( comboBox() )
    {
        auto le = fExtraInputWidgets.empty() ? nullptr : ( dynamic_cast< QLineEdit * >( fExtraInputWidgets.front() ) );
        if ( le )
        {
            auto leValue = valueForString( le->text() );
            if ( currValue != leValue )
                return true;
        }
    }
    return false;
}

bool CVariableInfo::hasValues() const
{
    return fValues.has_value();
}

bool CVariableInfo::hasRange() const
{
    return fRanges.has_value();
}

std::optional< TOptionalDoubleVector > CVariableInfo::validValues( bool imperial, bool seaWater ) const
{
    if ( !hasValues() )
        return {};

    auto values = getValues( imperial, seaWater );
    if ( !values.has_value() )
        return {};

    TOptionalDoubleVector retVal;
    for ( auto &&ii : values.value() )
    {
        if ( ii.second.has_value() )
            retVal.push_back( ii.second );
    }
    std::sort( retVal.begin(), retVal.end() );
    if ( retVal.empty() )
        return {};
    return retVal;
}

TOptionalNamedValueItemList CVariableInfo::getValues( bool imperial, bool seaWater ) const
{
    return fValues.getValue( imperial, seaWater );
}

bool CVariableInfo::hasCustomValue() const
{
    if ( !fValues.has_value() )
        return false;
    if ( fValues.fDefaultValue.has_value() )
    {
        for ( auto &&ii : fValues.fDefaultValue.value() )
        {
            if ( ii.second.has_value() )
                continue;
            return true;
        }
    }
    else
    {
        for ( auto &&ii : fValues.fValues )
        {
            for ( auto &&jj : ii.second )
            {
                if ( jj.second.has_value() )
                    continue;
                return true;
            }
        }
    }
    return false;
}

void CVariableInfo::setDependencies( const TVariableInfoList &dependencies )
{
    fDependencies.clear();
    for ( auto &&ii : dependencies )
    {
        Q_ASSERT( ii.get() != this );
        fDependencies.emplace_back( ii );
    }
}

bool CVariableInfo::dependenciesSatisfied() const
{
    for ( auto &&ii : fDependencies )
    {
        if ( !ii.lock()->has_value() )
            return false;
    }
    return true;
}

void CVariableInfo::updateValueFromField()
{
    if ( !isVariable() )
        return;

    fValue.reset();
    Q_ASSERT( fField );
    if ( !fField )
        return;

    fValue = optValue();

    if ( comboBox() )
    {
        auto le = fExtraInputWidgets.empty() ? nullptr : ( dynamic_cast< QLineEdit * >( fExtraInputWidgets.front() ) );
        if ( le )
            le->setEnabled( comboBox()->currentData().isNull() );
    }
}

TOptionalDouble CVariableInfo::valueForString( const QString &text ) const
{
    if ( text.trimmed().isEmpty() )
        return {};
    bool aOK = false;
    auto retVal = text.trimmed().toDouble( &aOK );
    if ( !aOK )
        return {};
    return retVal;
}

QString CVariableInfo::updateFormula( bool imperial, bool seaWater, const QString &formula, EFormulaType formulaType ) const
{
    QString value;
    QString format;
    if ( ( formulaType == EFormulaType::eCurrentValueFormula ) && has_value() )
    {
        value = NUtilities::doubleToString( formulaValue(), numDecimals() );
        format = QString( "%1%2" );
    }
    else
    {
        value = fDescription;
        format = QString( "%1 (%2)" );
    }

    auto newString = QString( format ).arg( value );
    auto unit = unitText( imperial, seaWater, true, formulaType );

    if ( ( fUnit == EUnit::eAbsZeroTemperature ) && has_value() && ( formulaType == EFormulaType::eCurrentValueFormula ) )
    {
        newString = QString( R"__((%1%2 + %3))__" ).arg( value ).arg( unit ).arg( NUtilities::NConstants::absZeroOffset( imperial, true, true, false ) );
    }
    else
    {
        newString = newString.arg( unit );
        newString.replace( " ()", "" );
    }

    QString retVal = formula;

    auto token = fieldName();
    if ( isIntermediate() )
    {
        if ( formulaType != EFormulaType::eCurrentValueFormula )
        {
            auto labelString = QString( "%1 (%2)" ).arg( fDescription );
            labelString = labelString.arg( unitText( imperial, seaWater, true, formulaType ) );
            labelString.replace( " ()", "" );
            retVal = retVal.replace( token, labelString );
        }
        else
            retVal = retVal.replace( token, newString );
        token = QString( "<%1_value>" ).arg( name() );
    }

    retVal.replace( token, newString );
    return retVal;
}

QString CVariableInfo::updateFormula( bool imperial, bool seaWater, const QString &formula, EConstantType varType, bool descriptionNotValue )
{
    auto token = NUtilities::fieldNameForType( varType );
    auto pos = formula.indexOf( token );
    if ( pos == -1 )
        return formula;

    QString constantString = NUtilities::NConstants::constantString( imperial, seaWater, varType, descriptionNotValue );
    QString retVal = formula;
    retVal.replace( token, constantString );
    return retVal;
}

QString CVariableInfo::descriptiveName( bool imperial, bool seaWater ) const
{
    auto retVal = updateFormula( imperial, seaWater, fieldName(), EFormulaType::eBaseFormula );
    return retVal;
}

QString CVariableInfo::valueString( bool imperial, bool seaWater ) const
{
    if ( !has_value() )
        return {};

    auto retVal = updateFormula( imperial, seaWater, fieldName(), EFormulaType::eCurrentValueFormula );
    return retVal;
}

double CVariableInfo::value() const
{
    if ( ( fUnit == EUnit::ePercent ) && ( fValue.value() >= 5 ) )
    {
        return fValue.value() / 100;
    }
    return fValue.value();
}

bool CVariableInfo::isWidget( QWidget *widget ) const
{
    if ( widget == fField )
        return true;
    for ( auto &&ii : fExtraInputWidgets )
    {
        if ( ii == widget )
            return true;
    }
    return false;
}

void CVariableInfo::setDefaultRange( const SRange &range )
{
    fRanges.setDefault( range );
}

void CVariableInfo::addRange( std::optional< bool > imperial, std::optional< bool > seaWater, const SRange &range )
{
    Q_ASSERT( isVariable() );
    fRanges.addValue( imperial, seaWater, range );
}

void CVariableInfo::addRange( const SBaseInfo< SRange > &rangeInfo )
{
    Q_ASSERT( isVariable() );
    fRanges.addValue( rangeInfo );
}

void CVariableInfo::setDefaultValues( const TNamedValueItemList &values )
{
    fValues.setDefault( values );
}

void CVariableInfo::addValues( std::optional< bool > imperial, std::optional< bool > seaWater, const TNamedValueItemList &values )
{
    Q_ASSERT( isVariable() );
    fValues.addValue( imperial, seaWater, values );
}

void CVariableInfo::addValues( const SBaseInfo< TNamedValueItemList > &valueInfo )
{
    Q_ASSERT( isVariable() );
    fValues.addValue( valueInfo );
}

double CVariableInfo::formulaValue() const
{
    if ( !has_value() )
        return 0.0;

    if ( fUnit == EUnit::ePercent )
    {
        return value() * 100;
    }
    return value();
}

void CVariableInfo::clearField( bool imperial, bool seaWater, bool notifyUI )
{
    if ( !notifyUI )
        fField->blockSignals( true );

    if ( lineEdit() )
        lineEdit()->clear();
    else if ( doubleSpinBox() )
    {
        auto value = fRanges.getValue( imperial, seaWater );
        if ( value.has_value() && value.value().fDefaultValue.has_value() )
            doubleSpinBox()->setValue( value.value().fDefaultValue.value() );
        else
            doubleSpinBox()->setValue( doubleSpinBox()->minimum() );
    }
    else if ( comboBox() )
        comboBox()->setCurrentIndex( 0 );

    fValue = optValue();

    if ( !notifyUI )
        fField->blockSignals( false );
}

void CVariableInfo::updateFieldFromValue( bool imperial, bool seaWater, bool notifyUI )
{
    if ( !isVariable() )
        return;

    Q_ASSERT( fField );
    if ( !fField )
        return;

    if ( !has_value() )
        clearField( imperial, seaWater, notifyUI );

    if ( !has_value() )
        return;

    if ( lineEdit() )
        updateFieldFromValue( lineEdit(), notifyUI );
    else if ( doubleSpinBox() )
        updateFieldFromValue( doubleSpinBox(), notifyUI );
    else if ( comboBox() )
        updateFieldFromValue( comboBox(), notifyUI );
}

void CVariableInfo::updateFieldFromValue( QDoubleSpinBox *spinBox, bool notifyUI )
{
    Q_ASSERT( has_value() );
    if ( !has_value() )
        return;

    if ( !spinBox )
        return;

    if ( !notifyUI )
        spinBox->blockSignals( true );

    spinBox->setValue( value() );

    if ( !notifyUI )
        spinBox->blockSignals( false );
}

void CVariableInfo::updateFieldFromValue( QLineEdit *lineEdit, bool notifyUI )
{
    Q_ASSERT( has_value() );
    if ( !has_value() )
        return;

    if ( !lineEdit )
        return;

    if ( !notifyUI )
        lineEdit->blockSignals( true );

    auto newValueString = NUtilities::doubleToString( value(), 2 );
    lineEdit->setText( newValueString );

    if ( !notifyUI )
        lineEdit->blockSignals( false );
}

void CVariableInfo::updateFieldFromValue( QComboBox *comboBox, bool notifyUI )
{
    Q_ASSERT( has_value() );
    if ( !has_value() )
        return;

    if ( !comboBox )
        return;

    bool customSelection = false;
    auto pos = comboBox->findData( value() );
    if ( pos == -1 )
    {
        pos = comboBox->findData( QVariant() );
        customSelection = pos != -1;
    }
    Q_ASSERT( pos != -1 );
    if ( pos == -1 )
        return;

    if ( !notifyUI )
        comboBox->blockSignals( true );
    comboBox->setCurrentIndex( pos );
    Q_ASSERT( ( fExtraInputWidgets.size() == 1 ) && dynamic_cast< QLineEdit * >( fExtraInputWidgets.front() ) );
    QLineEdit *le = nullptr;
    if ( ( fExtraInputWidgets.size() == 1 ) && dynamic_cast< QLineEdit * >( fExtraInputWidgets.front() ) )
    {
        le = dynamic_cast< QLineEdit * >( fExtraInputWidgets.front() );
        if ( !notifyUI )
            le->blockSignals( true );

        le->setEnabled( customSelection );

        if ( !customSelection )
        {
            auto newValueString = NUtilities::doubleToString( value(), 2 );
            le->setText( newValueString );
        }

        if ( !notifyUI )
            le->blockSignals( false );
    }

    if ( !notifyUI )
        comboBox->blockSignals( false );
}

void CVariableInfo::updateValuesAndRanges( bool imperial, bool seaWater )
{
    setupValues( imperial, seaWater );
    setupRange( imperial, seaWater );
}

std::shared_ptr< CVariableInfo > CVariableInfo::clone( const QString &suffix /*={}*/ ) const
{
    auto retVal = std::make_shared< CVariableInfo >( name() + suffix, fDescription, fUnit, fVariableLocation );

    retVal->fRanges = fRanges;
    retVal->fValues = fValues;
    retVal->fUnitOverride = fUnitOverride;
    retVal->fValue = fValue;
    retVal->fIntermediate = fIntermediate;

    return retVal;
}

QString CVariableInfo::fieldName() const
{
    return QString( "<%1>" ).arg( name() );
}

void CVariableInfo::setupRange( bool imperial, bool seaWater )
{
    if ( !doubleSpinBox() )
        return;

    auto range = fRanges.getValue( imperial, seaWater );
    doubleSpinBox()->setEnabled( range.has_value() );
    if ( !range.has_value() )
        return;

    auto currRange = SRange( doubleSpinBox() );
    if ( range.value() == currRange )
        return;

    auto prev = doubleSpinBox()->blockSignals( true );
    doubleSpinBox()->setMinimum( range.value().fMin );
    doubleSpinBox()->setMaximum( range.value().fMax );
    doubleSpinBox()->setSingleStep( range.value().fStep );
    if ( range.value().fDefaultValue.has_value() )
        doubleSpinBox()->setValue( range.value().fDefaultValue.value() );
    doubleSpinBox()->blockSignals( prev );
}

void CVariableInfo::setupValues( bool imperial, bool seaWater )
{
    if ( !comboBox() )
        return;

    auto values = fValues.getValue( imperial, seaWater );
    comboBox()->setEnabled( values.has_value() );
    if ( !values.has_value() )
    {
        return;
    }

    TNamedValueItemList currValues;
    for ( auto ii = 0; ii < comboBox()->count(); ++ii )
    {
        auto currText = comboBox()->itemText( ii );
        auto currValue = comboBox()->itemData( ii );
        TOptionalDouble value;
        if ( !currValue.isNull() )
            value = currValue.toDouble();
        currValues.emplace_back( currText, value );
    }

    if ( values == currValues )
        return;

    auto prev = comboBox()->blockSignals( true );
    comboBox()->clear();
    for ( auto &&ii : values.value() )
    {
        comboBox()->addItem( ii.first, ii.second.has_value() ? ii.second.value() : QVariant() );
    }
    comboBox()->blockSignals( prev );
}

SRange::SRange()
{
}

SRange::SRange( double min, double max, std::optional< double > defaultValue, double step ) :
    fMin( min ),
    fMax( max ),
    fDefaultValue( defaultValue ),
    fStep( step )
{
}

SRange::SRange( QDoubleSpinBox *spinBox )
{
    if ( !spinBox )
        return;
    fMin = spinBox->minimum();
    fMax = spinBox->maximum();
    fDefaultValue = spinBox->value();
    fStep = spinBox->singleStep();
}

bool SRange::operator==( const SRange &rhs )
{
    return ( fMin == rhs.fMin )   //
           && ( fMax == rhs.fMax )   //
           && ( fDefaultValue == rhs.fDefaultValue )   //
           && ( fStep == rhs.fStep );
}
