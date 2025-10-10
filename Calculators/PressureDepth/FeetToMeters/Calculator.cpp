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

    virtual bool showUnits() const { return false; }

    virtual void resetVariables() override { CSCUBACalculator::resetVariables(); }
    virtual QFrame *svgFrame() const override { return CSCUBACalculator::svgFrame(); }
    virtual QSvgWidget *svgWidget() const override { return CSCUBACalculator::svgWidget(); }

    virtual TVariableInfoList getMyVariables() const override;

    virtual QString getBaseFormula() const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo & unsetVar ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo & unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return tr( "Feet to Meters" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Pressure and Depth Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< SVariableInfo >( "meters", tr( "Meters" ), EVariableType::eVariable, EUnit::eNone, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "feet", tr( "Feet" ), EVariableType::eVariable, EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "metersToFeet", tr( "Meters To Feet" ), EVariableType::eMetersToFeetConstant, EUnit::eNone, EVariableLoc::eRHS ),   //
        } );

    retVal.front()->setUnitLabel( NUtilities::NUnitStrings::lengthUnit( false, true, false ) );
    ( *std::next( retVal.begin() ) )->setUnitLabel( NUtilities::NUnitStrings::lengthUnit( true, true, false ) );
    return retVal;
}

QString CCalculator::getBaseFormula() const
{
    return NUtilities::feetToMetersFormula( "feet", "meters", "metersToFeet" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo & unsetVar ) const
{
    if ( unsetVar->name() == "meters" )
    {
        return getBaseFormula();
    }
    else if ( unsetVar->name() == "feet" )
    {
        return NUtilities::metersToFeetFormula( "feet", "meters", "metersToFeet" );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo & unsetVar )
{
    auto feet = getVariable( "feet" );
    auto meters = getVariable( "meters" );

    if ( unsetVar == meters )
    {
        meters->setValue( NUtilities::feetToMeters( feet->value() ) );
    }
    else if ( unsetVar == feet )
    {
        feet->setValue( NUtilities::metersToFeet( meters->value() ) );
    }
}