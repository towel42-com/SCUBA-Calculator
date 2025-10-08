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

SVariableInfo::SVariableInfo( const QString &name, const QString &desc, EVariableType type, EUnit unitType, EVariableLoc variableLocation ) :
    fName( name ),
    fDescription( desc ),
    fType( type ),
    fUnit( unitType ),
    fVariableLocation( variableLocation )
{
}

bool SVariableInfo::createWidgets( CSCUBACalculatorPage *page, QFormLayout *formLayout )
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

    updateLabels( page->imperial(), page->saltWater() );
    return true;
}

void SVariableInfo::updateLabels( bool imperial, bool /*saltWater*/ )
{
    if ( fType != EVariableType::eVariable )
        return;

    Q_ASSERT( fField && fUnitLabel );

    auto unitText = NUtilities::NUnitStrings::getUnitLabel( imperial, fUnit, false );

    if ( lineEdit() )
        lineEdit()->setPlaceholderText( QString( "%1 (%2)" ).arg( fDescription ).arg( unitText ) );

    if ( fUnitLabel )
        fUnitLabel->setText( unitText );
}

void SVariableInfo::resetValue( bool updateUI, bool notifyUI )
{
    fValue.reset();
    if ( !updateUI )
        return;
    updateFieldFromValue( notifyUI );
}

TOptionalDouble SVariableInfo::currFieldValue() const
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

QLineEdit *SVariableInfo::lineEdit() const
{
    return ( dynamic_cast< QLineEdit * >( fField ) );
}

QDoubleSpinBox *SVariableInfo::doubleSpinBox() const
{
    return ( dynamic_cast< QDoubleSpinBox * >( fField ) );
}

QComboBox *SVariableInfo::comboBox() const
{
    return ( dynamic_cast< QComboBox * >( fField ) );
}

void SVariableInfo::updateValueFromField()
{
    if ( fType != EVariableType::eVariable )
        return;

    fValue.reset();
    Q_ASSERT( fField );
    if ( !fField )
        return;

    fValue = currFieldValue();
}

TOptionalDouble SVariableInfo::valueForString( const QString &text ) const
{
    if ( text.trimmed().isEmpty() )
        return {};
    bool aOK = false;
    auto retVal = text.trimmed().toDouble( &aOK );
    if ( !aOK )
        return {};
    return retVal;
}

void SVariableInfo::updateFormula( bool imperial, bool saltWater, QString &formula, bool isBaseFormula ) const
{
    QString value;
    QString format;
    if ( !isBaseFormula && has_value() )
    {
        value = NUtilities::doubleToString( formulaValue(), numDecimals() );
        format = QString( "%1%2" );
    }
    else
    {
        switch ( fType )
        {
            case EVariableType::eHidden:
            case EVariableType::eVariable:
                {
                    value = fDescription;
                    format = QString( "%1 (%2)" );
                    break;
                }
            case EVariableType::ePressurePerTempConst:
                {
                    value = NUtilities::NUnitStrings::pressurePerTemp( imperial, true );
                    format = "%1";
                }
                break;
            case EVariableType::eWeightOfWaterConst:
                {
                    value = NUtilities::NUnitStrings::weightOfWater( imperial, saltWater, true );
                    format = "%1";
                }
                break;
            case EVariableType::eIdealGasConst:
                {
                    value = NUtilities::NUnitStrings::idealGasConstant( imperial, true );
                    format = "%1";
                }
                break;
            case EVariableType::eFN2AtSurfaceConst:
                {
                    value = NUtilities::NUnitStrings::percentN2AtSurface( true );
                    format = "%1";
                }
                break;
            case EVariableType::eFO2AtSurfaceConst:
                {
                    value = NUtilities::NUnitStrings::percentO2AtSurface( true );
                    format = "%1";
                }
                break;
            case EVariableType::eDepthToSingleAtmosphereConst:
                {
                    value = NUtilities::NUnitStrings::depthToSingleAtmosphere( imperial, saltWater, true );
                    format = "%1";
                }
                break;
        };
    }

    auto newString = QString( format ).arg( value );
    if ( ( fType == EVariableType::eVariable ) || ( fType == EVariableType::eHidden ) )
        newString = newString.arg( NUtilities::NUnitStrings::getUnitLabel( imperial, fUnit, true ) );

    auto token = QString( "<%1>" ).arg( name() );
    if ( fType == EVariableType::eHidden )
    {
        auto labelString = QString( "%1 (%2)" ).arg( fDescription );
        labelString = labelString.arg( NUtilities::NUnitStrings::getUnitLabel( imperial, fUnit, true ) );
        formula = formula.replace( token, labelString );

        token = QString( "<%1_value>" ).arg( name() );
    }

    formula = formula.replace( token, newString );
}

double SVariableInfo::value() const
{
    if ( ( fUnit == EUnit::ePercent ) && ( fValue.value() >= 5 ) )
    {
        return fValue.value() / 100;
    }
    return fValue.value();
}

bool SVariableInfo::isWidget( QWidget *widget ) const
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

double SVariableInfo::formulaValue() const
{
    if ( !has_value() )
        return 0.0;

    if ( fUnit == EUnit::ePercent )
    {
        return value() * 100;
    }
    return value();
}

void SVariableInfo::clearField( bool notifyUI )
{
    if ( !notifyUI )
        fField->blockSignals( true );

    if ( lineEdit() )
        lineEdit()->clear();
    else if ( doubleSpinBox() )
        doubleSpinBox()->setValue( fRange.value().fDefaultValue );
    else if ( comboBox() )
        comboBox()->setCurrentIndex( 0 );

    fValue = currFieldValue();

    if ( !notifyUI )
        fField->blockSignals( false );
}

void SVariableInfo::updateFieldFromValue( bool notifyUI )
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

void SVariableInfo::updateFieldFromValue( QDoubleSpinBox *spinBox, bool notifyUI )
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

void SVariableInfo::updateFieldFromValue( QLineEdit *lineEdit, bool notifyUI )
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

void SVariableInfo::updateFieldFromValue( QComboBox *comboBox, bool notifyUI )
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