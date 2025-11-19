#include "VariableInfo.h"
#include "CalculatorPage.h"
#include "CalculatorBase.h"
#include "Formula.h"

#include "Utilities.h"
#include "SABUtils/DelayLineEdit.h"
#include "SABUtils/JsonUtils.h"

#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QJsonObject>
#include <QJSEngine>
#include <QJSValue>
#include <optional>
#include <algorithm>
#include <memory>

CVariableInfo::CVariableInfo( const QString &name, const QString &desc, EUnit unitType, EVariableLoc variableLocation, const SForceShared & ) :
    fName( name ),
    fDescription( desc ),
    fUnit( unitType ),
    fVariableLocation( variableLocation )
{
}

TVariableInfo CVariableInfo::create( const QString &name, const QString &desc, EUnit unitType, EVariableLoc variableLocation )
{
    return std::make_shared< CVariableInfo >( name, desc, unitType, variableLocation, SForceShared() );
}

CVariableInfo::CVariableInfo( const QString &name, const QString &desc, EUnit unitType, EVariableLoc variableLocation, bool imperial, const SForceShared &forceShared ) :
    CVariableInfo( name, desc, EUnit::eNone, variableLocation, forceShared )
{
    setUnitOverride( unitType, imperial );
}

TVariableInfo CVariableInfo::create( const QString &name, const QString &desc, EUnit unitType, EVariableLoc variableLocation, bool imperial )
{
    return std::make_shared< CVariableInfo >( name, desc, unitType, variableLocation, imperial, SForceShared() );
}

CVariableInfo::CVariableInfo( const QString &name, const QString &desc, EUnit unitType, EVariableLoc variableLocation, const SBaseInfo< SRange > &rangeInfo, const SForceShared &forceShared ) :
    CVariableInfo( name, desc, unitType, variableLocation, forceShared )
{
    addRange( rangeInfo );
}

TVariableInfo CVariableInfo::create( const QString &name, const QString &desc, EUnit unitType, EVariableLoc variableLocation, const SBaseInfo< SRange > &rangeInfo )
{
    return std::make_shared< CVariableInfo >( name, desc, unitType, variableLocation, rangeInfo, SForceShared() );
}

CVariableInfo::CVariableInfo( const QString &name, const QString &desc, EUnit unitType, EVariableLoc variableLocation, const SBaseInfo< TNamedValueItemList > &valuesInfo, const SForceShared &forceShared ) :
    CVariableInfo( name, desc, unitType, variableLocation, forceShared )
{
    addValues( valuesInfo );
}

TVariableInfo CVariableInfo::create( const QString &name, const QString &desc, EUnit unitType, EVariableLoc variableLocation, const SBaseInfo< TNamedValueItemList > &valuesInfo )
{
    return std::make_shared< CVariableInfo >( name, desc, unitType, variableLocation, valuesInfo, SForceShared() );
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

void CVariableInfo::setValue( TOptionalDouble value )
{
    if ( value.has_value() && ( fUnit == EUnit::eLargePercent ) )
    {
        value.value() *= 100;
    }
    fValue = value;
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

TOptionalFormulaList CVariableInfo::formulaList( bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( !fTexFormulas.has_value() || fTexFormulas.value().empty() )
        return {};

    TFormulaList retVal;
    for ( auto ii : fTexFormulas.value() )
    {
        auto curr = std::make_shared< CFormula >( shared_from_this(), ii );
        retVal.push_back( curr );
    }
    return retVal;
}

void CVariableInfo::computeFromJS( bool imperial, bool seaWater, const TVariableInfoList &variables )
{
    if ( !fJSFormulas.has_value() || fJSFormulas.value().empty() )
        return;

    setValue( {} );

    QJSEngine engine;
    for ( auto &&ii : fJSFormulas.value() )
    {
        auto finalizedFormula = CFormula::applyVariableValues( imperial, seaWater, ii, variables, EFormulaType::eJSFormula );

        auto value = engine.evaluate( finalizedFormula );
        if ( value.isError() || !value.isNumber() )
            return;

        auto currVal = value.toNumber();
        setValue( currVal );
    }
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
    bool hasUnit = true;
    if ( ( ( formulaType == EFormulaType::eCurrentValueFormula ) || ( formulaType == EFormulaType::eJSFormula ) ) && has_value() )
    {
        value = NUtilities::doubleToString( formulaValue( formulaType ), numDecimals( formulaType ) );
        if ( formulaType == EFormulaType::eCurrentValueFormula )
            format = QString( "%1%2" );
        else
        {
            format = "%1";
            hasUnit = false;
        }
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
        newString = QString( R"__((%1%2 + %3))__" ).arg( value ).arg( unit ).arg( NUtilities::NConstants::absZeroOffset( imperial, true, true, formulaType ) );
    }
    else if ( ( fUnit == EUnit::eAbsZeroTemperature ) && has_value() && ( formulaType == EFormulaType::eJSFormula ) )
    {
        newString = QString( R"__((%1 + %2))__" ).arg( value ).arg( NUtilities::doubleToString( NUtilities::NConstants::absZeroOffset( imperial ), 4 ) );
    }
    else
    {
        if ( hasUnit )
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

QString CVariableInfo::updateFormula( bool imperial, bool seaWater, const QString &formula, EConstantType varType, EFormulaType formulaType )
{
    auto token = NUtilities::fieldNameForType( varType );
    auto pos = formula.indexOf( token );
    if ( pos == -1 )
        return formula;

    QString constantString = NUtilities::NConstants::constantString( imperial, seaWater, varType, formulaType );
    QString retVal = formula;
    retVal.replace( token, constantString );
    return retVal;
}

int CVariableInfo::numDecimals( EFormulaType formulaType ) const
{
    if ( formulaType == EFormulaType::eJSFormula )
        return -1;
    return ( ( fUnit == EUnit::ePercent ) || ( fUnit == EUnit::eLargePercent ) ) ? 0 : 2;
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

void CVariableInfo::setUnitOverride( EUnit unit, bool imperial )
{
    fUnit = EUnit::eNone;
    fUnitOverride = { unit, imperial };
}

double CVariableInfo::value() const
{
    auto retVal = fValue.value();
    if ( ( fUnit == EUnit::eLargePercent ) || ( ( fUnit == EUnit::ePercent ) && ( fValue.value() >= 5 ) ) )
    {
        retVal = retVal / 100;
    }
    return retVal;
}

double CVariableInfo::formulaValue( EFormulaType formulaType ) const
{
    if ( !has_value() )
        return 0.0;

    auto retVal = value();
    if ( ( formulaType != EFormulaType::eJSFormula ) && ( fUnit == EUnit::eLargePercent ) )
    {
        retVal = retVal * 100.0;
    }
    return retVal;
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
    auto retVal = std::make_shared< CVariableInfo >( name() + suffix, fDescription, fUnit, fVariableLocation, SForceShared() );

    retVal->fRanges = fRanges;
    retVal->fValues = fValues;
    retVal->fUnitOverride = fUnitOverride;
    retVal->fValue = fValue;
    retVal->fIntermediate = fIntermediate;

    return retVal;
}

TVariableInfo CVariableInfo::fromJson( const QJsonObject &obj, std::optional< QString > &errorMsg )
{
    errorMsg = {};
    if ( obj.isEmpty() )
    {
        errorMsg = QObject::tr( "Invalid JSON object, its empty.", "CVariableInfo::fromJson" );
        return {};
    }

    QString name;
    if ( !NSABUtils::fromJson( name, obj, "name" ) || name.isEmpty() )
    {
        errorMsg = QObject::tr( "Invalid JSON object, missing name field.", "CVariableInfo::fromJson" );
        return {};
    }

    QString description;
    if ( !NSABUtils::fromJson( description, obj, "description" ) || description.isEmpty() )
    {
        errorMsg = QObject::tr( "Invalid JSON object, missing description field.", "CVariableInfo::fromJson" );
        return {};
    }

    bool intermediate{ false };
    NSABUtils::fromJson( intermediate, obj, "intermediate" );

    QString locationStr;
    if ( !NSABUtils::fromJson( locationStr, obj, "location" ) || locationStr.isEmpty() )
    {
        errorMsg = QObject::tr( "Invalid JSON object, missing location field.", "CVariableInfo::fromJson" );
        return {};
    }

    std::optional< EVariableLoc > location;
    if ( !NUtilities::fromString( location, locationStr ) || !location.has_value() )
    {
        errorMsg = QObject::tr( "Invalid JSON object, invalid location value '%1'.", "CVariableInfo::fromJson" ).arg( locationStr );
        return {};
    }

    QString unitStr;
    if ( !NSABUtils::fromJson( unitStr, obj, "unit" ) || unitStr.isEmpty() )
    {
        errorMsg = QObject::tr( "Invalid JSON object, missing unit field.", "CVariableInfo::fromJson" );
        return {};
    }

    std::optional< EUnit > unitType;
    if ( !NUtilities::fromString( unitType, unitStr ) || !unitType.has_value() )
    {
        errorMsg = QObject::tr( "Invalid JSON object, missing unit value '%1'.", "CVariableInfo::fromJson" ).arg( unitStr );
        return {};
    }

    std::optional< bool > imperial;
    NSABUtils::fromJson( imperial, obj, "imperial" );

    if ( obj.contains( "texFormula" ) && obj.contains( "texFormulas" ) )
    {
        errorMsg = QObject::tr( "Invalid JSON object, can not contain a texFormula and texFormulas field.", "CVariableInfo::fromJson" );
        return {};
    }

    std::list< QString > texFormulas;
    NSABUtils::fromJson( texFormulas, obj, "texFormulas" );

    QString texFormula;
    NSABUtils::fromJson( texFormula, obj, "texFormula" );
    if ( !texFormula.isEmpty() )
        texFormulas.push_back( texFormula );

    if ( obj.contains( "jsFormula" ) && obj.contains( "jsFormulas" ) )
    {
        errorMsg = QObject::tr( "Invalid JSON object, can not contain a jsFormula and jsFormulas field.", "CVariableInfo::fromJson" );
        return {};
    }

    std::list< QString > jsFormulas;
    NSABUtils::fromJson( jsFormulas, obj, "jsFormulas" );

    QString jsFormula;
    NSABUtils::fromJson( jsFormula, obj, "jsFormula" );
    if ( !jsFormula.isEmpty() )
        jsFormulas.push_back( jsFormula );

    auto retVal = std::make_shared< CVariableInfo >( name, description, unitType.value(), location.value(), SForceShared() );

    if ( !texFormulas.empty() )
        retVal->setTexFormulas( texFormulas );

    if ( !texFormulas.empty() && jsFormulas.empty() )
    {
        for ( const auto &ii : texFormulas )
        {
            jsFormulas.push_back( NUtilities::texToJS( ii ) );
        }
    }

    if ( !jsFormulas.empty() )
        retVal->setJSFormulas( jsFormulas );

    if ( imperial.has_value() )
        retVal->setUnitOverride( unitType.value(), imperial.value() );

    retVal->setIsIntermediate( intermediate );

    if ( obj.contains( "range" ) && obj.contains( "rangeList" ) )
    {
        errorMsg = QObject::tr( "Invalid JSON object, can not contain a range and rangeList field.", "CVariableInfo::fromJson" );
        return {};
    }

    if ( obj.contains( "values" ) && obj.contains( "valuesList" ) )
    {
        errorMsg = QObject::tr( "Invalid JSON object, can not contain a values and valuesList field.", "CVariableInfo::fromJson" );
        return {};
    }

    if ( ( obj.contains( "range" ) || obj.contains( "rangeList" ) ) && ( obj.contains( "values" ) || obj.contains( "valuesList" ) ) )
    {
        errorMsg = QObject::tr( "Invalid JSON object, can not contain a range or rangeList field and a values or valuesList field.", "CVariableInfo::fromJson" );
        return {};
    }

    if ( !loadRange( obj, errorMsg, retVal ) )
        return {};

    if ( !loadRangeList( obj, errorMsg, retVal ) )
        return {};

    TNamedValueItemList valueList;
    if ( !loadValues( obj, errorMsg, retVal, valueList ) )
        return {};

    if ( !loadValuesList( obj, errorMsg, retVal ) )
        return {};

    return retVal;
}

bool CVariableInfo::loadRange( const QJsonObject &obj, std::optional< QString > &errorMsg, TVariableInfo &varInfo )
{
    if ( !obj.contains( "range" ) )
        return true;

    auto rangeVal = obj[ "range" ];
    if ( !rangeVal.isObject() )
    {
        errorMsg = QObject::tr( "Invalid JSON object, range field is not an object.", "CVariableInfo::fromJson" );
        return false;
    }

    auto range = SRange::fromJson( rangeVal.toObject(), errorMsg );
    if ( !range.has_value() )
        return false;

    varInfo->addRange( {}, {}, range.value() );
    return true;
}

bool CVariableInfo::loadRangeList( const QJsonObject &obj, std::optional< QString > &errorMsg, TVariableInfo &varInfo )
{
    if ( !obj.contains( "rangeList" ) )
        return true;

    auto rangeListVal = obj[ "rangeList" ];
    if ( !rangeListVal.isArray() )
    {
        errorMsg = QObject::tr( "Invalid JSON object, rangeList field is not an array.", "CVariableInfo::fromJson" );
        return false;
    }

    auto rangeArray = rangeListVal.toArray();
    for ( auto &&ii : rangeArray )
    {
        if ( !ii.isObject() )
        {
            errorMsg = QObject::tr( "Invalid JSON object, rangeList item is not an object.", "CVariableInfo::fromJson" );
            return false;
        }
        auto currObj = ii.toObject();
        if ( !currObj.contains( "range" ) )
        {
            errorMsg = QObject::tr( "Invalid JSON object, rangeList item does not contain a range.", "CVariableInfo::fromJson" );
            return false;
        }

        std::optional< bool > imperial;
        std::optional< bool > seaWater;
        NSABUtils::fromJson( imperial, currObj, "imperial" );
        NSABUtils::fromJson( seaWater, currObj, "seaWater" );

        auto range = SRange::fromJson( currObj[ "range" ].toObject(), errorMsg );
        if ( !range.has_value() )
            return false;
        varInfo->addRange( imperial, seaWater, range.value() );
    }
    return true;
}

bool CVariableInfo::loadValues( const QJsonObject &obj, std::optional< QString > &errorMsg, TVariableInfo &varInfo, TNamedValueItemList &valueList )
{
    if ( !obj.contains( "values" ) )
        return true;

    auto valuesVal = obj[ "values" ];
    if ( !valuesVal.isArray() )
    {
        errorMsg = QObject::tr( "Invalid JSON object, values field is not an array.", "CVariableInfo::fromJson" );
        return false;
    }

    auto valuesArray = valuesVal.toArray();
    for ( auto &&currValue : valuesArray )
    {
        if ( !currValue.isObject() )
        {
            errorMsg = QObject::tr( "Invalid JSON object, invalid value field.", "CVariableInfo::fromJson" );
            return false;
        }

        auto currObj = currValue.toObject();
        QString key;
        if ( !NSABUtils::fromJson( key, currObj, "valueName" ) || key.isEmpty() )
        {
            errorMsg = QObject::tr( "Invalid JSON object, invalid valueName field.", "CVariableInfo::fromJson" );
            return false;
        }

        TOptionalDouble value;
        if ( currObj.contains( "value" ) )
        {
            if ( !NSABUtils::fromJson( value, currObj, "value" ) )
            {
                errorMsg = QObject::tr( "Invalid JSON object, invalid value field.", "CVariableInfo::fromJson" );
                return false;
            }
        }
        valueList.emplace_back( key, value );
    }

    if ( varInfo )
        varInfo->addValues( {}, {}, valueList );
    return true;
}

bool CVariableInfo::loadValuesList( const QJsonObject &obj, std::optional< QString > &errorMsg, TVariableInfo &varInfo )
{
    if ( !obj.contains( "valuesList" ) )
        return true;

    auto valuesListVal = obj[ "valuesList" ];
    if ( !valuesListVal.isArray() )
    {
        errorMsg = QObject::tr( "Invalid JSON object, valuesList field is not an array.", "CVariableInfo::fromJson" );
        return false;
    }

    auto rangeArray = valuesListVal.toArray();
    for ( auto &&ii : rangeArray )
    {
        if ( !ii.isObject() )
        {
            errorMsg = QObject::tr( "Invalid JSON object, valuesList item is not an object.", "CVariableInfo::fromJson" );
            return false;
        }

        auto currObj = ii.toObject();
        if ( !currObj.contains( "values" ) )
        {
            errorMsg = QObject::tr( "Invalid JSON object, valuesList item does not contain values.", "CVariableInfo::fromJson" );
            return false;
        }

        std::optional< bool > imperial;
        std::optional< bool > seaWater;
        NSABUtils::fromJson( imperial, currObj, "imperial" );
        NSABUtils::fromJson( seaWater, currObj, "seaWater" );
        TNamedValueItemList valueItemList;
        auto tmp = TVariableInfo();
        if ( !loadValues( currObj, errorMsg, tmp, valueItemList ) )
        {
            return false;
        }
        varInfo->addValues( imperial, seaWater, valueItemList );
    }

    return true;
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

std::optional< SRange > SRange::fromJson( const QJsonObject &rangeObj, std::optional< QString > &errorMsg )
{
    if ( rangeObj.isEmpty() )
    {
        errorMsg = QObject::tr( "Invalid JSON object, range field is not an object.", "CVariableInfo::fromJson" );
        return {};
    }

    SRange range;
    if ( !NSABUtils::fromJson( range.fMin, rangeObj, "min" ) )
    {
        errorMsg = QObject::tr( "Invalid JSON object, range field missing min value.", "CVariableInfo::fromJson" );
        return {};
    }

    if ( !NSABUtils::fromJson( range.fMax, rangeObj, "max" ) )
    {
        errorMsg = QObject::tr( "Invalid JSON object, range field missing max value.", "CVariableInfo::fromJson" );
        return {};
    }

    if ( !NSABUtils::fromJson( range.fStep, rangeObj, "step" ) )
    {
        errorMsg = QObject::tr( "Invalid JSON object, range field missing step value.", "CVariableInfo::fromJson" );
        return {};
    }

    NSABUtils::fromJson( range.fDefaultValue, rangeObj, "defaultValue" );
    return range;
}

bool SRange::operator==( const SRange &rhs )
{
    return ( fMin == rhs.fMin )   //
           && ( fMax == rhs.fMax )   //
           && ( fDefaultValue == rhs.fDefaultValue )   //
           && ( fStep == rhs.fStep );
}
