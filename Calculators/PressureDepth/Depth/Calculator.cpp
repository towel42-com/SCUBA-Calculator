#include "Calculator.h"
#include "VariableInfo.h"
#include "Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() {}
    virtual ~CCalculator() override {}

    virtual QString calculatorName() const override;
    virtual QStringList calculatorPath() const override;

    virtual void resetVariables() override { CSCUBACalculator::resetVariables(); }
    virtual QFrame *svgFrame() const override { return CSCUBACalculator::svgFrame(); }
    virtual QSvgWidget *svgWidget() const override { return CSCUBACalculator::svgWidget(); }

    virtual std::list< std::shared_ptr< SVariableInfo > > getMyVariables() const override;
    virtual QString getDefaultFormula() const override;
    virtual QString computeAndGenerateFormula() const override;

    virtual bool showUnits() const { return false; }
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Depth";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure and Depth Conversions" };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList (//
        {
            std::make_shared< SVariableInfo >( "meters", tr( "Meters" ), EVariableType::eVariable, EUnit::eNone, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "feet", tr( "Feet" ), EVariableType::eVariable, EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "feetToMeters", tr( "Feet To Meters" ), EVariableType::eFeetToMetersConst, EUnit::eNone, EVariableLoc::eRHS ),   //
        } );

    retVal.front()->setUnitLabel( NUtilities::NUnitStrings::lengthUnit( true, true, false ) );
    (*std::next( retVal.begin() ) )->setUnitLabel( NUtilities::NUnitStrings::lengthUnit( false, true, false ) );
    return retVal;
}

QString CCalculator::getDefaultFormula() const
{
    return tr( R"__(<meters>=<feet> \times <feetToMeters>)__" );
}

QString CCalculator::computeAndGenerateFormula() const
{
    auto feet = getVariable( "feet" );
    auto meters = getVariable( "meters" );

    QString formula;
    bool aOK = numUnsetVariables() == 1;
    if ( !aOK || !meters->has_value() )
    {
        if ( aOK )
            meters->setValue( feet->value() * NUtilities::NConstants::feetToMeters() );
        formula = getDefaultFormula();
    }
    else if ( !feet->has_value() )
    {
        feet->setValue( meters->value() / NUtilities::NConstants::feetToMeters() );
        return tr( R"__(<feet>=\frac{<meters>}{<feetToMeters>})__" );
    }
    return formula;
}
