#include "VariableInfo.h"
#include "SCUBACalculatorPage.h"
#include "Utilities.h"
#include "SABUtils/DelayLineEdit.h"

#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QComboBox>

CVariableInfo::CVariableInfo( const QString &name, const QString &desc, EVariableType type, EUnit unitType, EVariableLoc variableLocation ) :
    fName( name ),
    fDescription( desc ),
    fType( type ),
    fUnit( unitType ),
    fVariableLocation( variableLocation )
{
}

CVariableInfo::CVariableInfo( EVariableType type ) :
    CVariableInfo( NUtilities::fieldNameForType( type ), NUtilities::descForType( type ), type, EUnit::eNone, EVariableLoc::eRHS )
{
    Q_ASSERT( ( fType != EVariableType::eIntermediate ) && ( fType != EVariableType::eVariable ) );
}

bool CVariableInfo::createWidgets( CSCUBACalculatorPage *page, QFormLayout *formLayout )
{
    if ( fType != EVariableType::eVariable )
        return false;

    fLabel = new QLabel( page );
    fLabel->setText( QString( "%1:" ).arg( fDescription ) );

    QHBoxLayout *fieldLayout = nullptr;
    if ( fRange.has_value() )
    {
        auto spinBox = new QDoubleSpinBox( page );
        fField = spinBox;
        spinBox->setObjectName( fName.data() );
        spinBox->setDecimals( 2 );
        spinBox->setMinimum( fRange.value().fMin );
        spinBox->setMaximum( fRange.value().fMax );
        spinBox->setSingleStep( fRange.value().fStep );
        spinBox->setValue( fRange.value().fDefaultValue );
    }
    else if ( fValues.has_value() )
    {
        auto comboBox = new QComboBox( page );
        comboBox->setEditable( false );
        comboBox->setObjectName( fName.data() );
        for ( auto &&ii : fValues.value() )
        {
            comboBox->addItem( ii.first, ii.second.has_value() ? ii.second.value() : QVariant() );
        }
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

    updateLabels( page->imperial(), page->seaWater() );
    return true;
}

void CVariableInfo::updateLabels( bool imperial, bool seaWater )
{
    if ( fType != EVariableType::eVariable )
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
    if ( fUnitText.has_value() )
        return fUnitText.value();

    switch ( fUnit )
    {
        case EUnit::eNone:
            return {};
        case EUnit::eVolume:
            return NUtilities::NUnitStrings::volumeUnit( imperial, true, tex );
        case EUnit::eCalories:
            return NUtilities::NUnitStrings::energyUnit( imperial, true, tex );
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
                if ( tex && ( formulaType == EFormulaType::eBaseFormula ) )
                    return NUtilities::NUnitStrings::absZeroTempUnit( imperial, true, true );
                else
                    return NUtilities::NUnitStrings::tempUnit( imperial, true, false );
            }
        case EUnit::ePercent:
            return NUtilities::NUnitStrings::percentUnit( imperial, true, tex );
        case EUnit::eTime:
            return NUtilities::NUnitStrings::timeUnit( imperial, true, tex );
        default:
            return {};
    }
}

void CVariableInfo::resetValue( bool updateUI, bool notifyUI )
{
    fValue.reset();
    if ( !updateUI )
        return;
    updateFieldFromValue( notifyUI );
}

TOptionalDouble CVariableInfo::optValue() const
{
    TOptionalDouble currValue;
    if ( lineEdit() )
        currValue = valueForString( lineEdit()->text() );
    else if ( doubleSpinBox() )
        currValue = doubleSpinBox()->value();
    else if ( comboBox() )
    {
        if ( !comboBox()->currentData().isNull() )
            currValue = comboBox()->currentData().toDouble();
        else if ( dynamic_cast< QLineEdit * >( fExtraInputWidgets.front() ) )
            currValue = valueForString( dynamic_cast< QLineEdit * >( fExtraInputWidgets.front() )->text() );
    }
    return currValue;
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

void CVariableInfo::updateValueFromField()
{
    if ( fType != EVariableType::eVariable )
        return;

    fValue.reset();
    Q_ASSERT( fField );
    if ( !fField )
        return;

    fValue = optValue();
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

void CVariableInfo::updateFormula( bool imperial, bool seaWater, QString &formula, EFormulaType formulaType ) const
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
        switch ( fType )
        {
            case EVariableType::eIntermediate:
            case EVariableType::eVariable:
                {
                    value = fDescription;
                    format = QString( "%1 (%2)" );
                    break;
                }
            case EVariableType::ePressurePerDegreeConst:
                {
                    value = NUtilities::NUnitStrings::pressureChangePerDegreeChange( imperial, true, true );
                    format = "%1";
                }
                break;
            case EVariableType::eWeightOfWaterConst:
                {
                    value = NUtilities::NUnitStrings::weightOfWater( imperial, seaWater, true, true );
                    format = "%1";
                }
                break;
            case EVariableType::eIdealGasConst:
                {
                    value = NUtilities::NUnitStrings::idealGasConstant( imperial, true, true );
                    format = "%1";
                }
                break;
            case EVariableType::eFN2AtSurfaceConst:
                {
                    value = NUtilities::NUnitStrings::percentN2AtSurface( imperial, true, true );
                    format = "%1";
                }
                break;
            case EVariableType::eFO2AtSurfaceConst:
                {
                    value = NUtilities::NUnitStrings::percentO2AtSurface( imperial, true, true );
                    format = "%1";
                }
                break;
            case EVariableType::eDepthToSingleATMConst:
                {
                    value = NUtilities::NUnitStrings::depthToSingleAtmosphere( imperial, seaWater, true, true );
                    format = "%1";
                }
                break;
            case EVariableType::eFeetToMetersConst:
                {
                    value = NUtilities::NUnitStrings::feetToMeters( true, true );
                    format = "%1";
                }
                break;
            case EVariableType::eMetersToFeetConst:
                {
                    value = NUtilities::NUnitStrings::metersToFeet( true, true );
                    format = "%1";
                }
                break;
            case EVariableType::eLbsPerKgsConst:
                {
                    value = NUtilities::NUnitStrings::lbsPerKgs( true, true );
                    format = "%1";
                }
                break;
            case EVariableType::eFreshWaterToSeaWaterConst:
                {
                    value = NUtilities::NUnitStrings::freshWaterToSeaWater( imperial, true, true );
                    format = "%1";
                }
                break;
            case EVariableType::ePSIToBarConst:
                {
                    value = NUtilities::NUnitStrings::psiToBar( true, true );
                    format = "%1";
                }
                break;
            case EVariableType::eAbsZeroOffsetConst:
                {
                    value = NUtilities::NUnitStrings::absZeroOffset( imperial, true, true );
                    format = "%1";
                }
                break;
            case EVariableType::ePressureOffsetConst:
                {
                    value = NUtilities::NUnitStrings::pressureOffset( imperial, true, true );
                    format = "%1";
                }
                break;
            case EVariableType::eBaseMETofSCUBAConst:
                {
                    value = NUtilities::NUnitStrings::scubaMET( true, true );
                    format = "%1";
                }
                break;
        };
    }

    auto newString = QString( format ).arg( value );
    if ( ( fType == EVariableType::eVariable ) || ( fType == EVariableType::eIntermediate ) )
    {
        auto unit = unitText( imperial, seaWater, true, formulaType );
        newString = newString.arg( unit );
        newString.replace( " ()", "" );
    }

    auto token = QString( "<%1>" ).arg( name() );
    if ( fType == EVariableType::eIntermediate )
    {
        auto labelString = QString( "%1 (%2)" ).arg( fDescription );
        labelString = labelString.arg( unitText( imperial, seaWater, true, formulaType ) );
        labelString.replace( " ()", "" );
        formula = formula.replace( token, labelString );

        token = QString( "<%1_value>" ).arg( name() );
    }

    formula = formula.replace( token, newString );
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

void CVariableInfo::clearField( bool notifyUI )
{
    if ( !notifyUI )
        fField->blockSignals( true );

    if ( lineEdit() )
        lineEdit()->clear();
    else if ( doubleSpinBox() )
        doubleSpinBox()->setValue( fRange.value().fDefaultValue );
    else if ( comboBox() )
        comboBox()->setCurrentIndex( 0 );

    fValue = optValue();

    if ( !notifyUI )
        fField->blockSignals( false );
}

void CVariableInfo::updateFieldFromValue( bool notifyUI )
{
    if ( fType != EVariableType::eVariable )
        return;

    Q_ASSERT( fField );
    if ( !fField )
        return;

    if ( !has_value() )
        clearField( notifyUI );

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