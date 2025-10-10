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
    return tr( "PSI to BAR" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Pressure and Depth Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< SVariableInfo >( "bar", tr( "BAR" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "psi", tr( "PSI" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "psiToBar", tr( "PSI to BAR" ), EVariableType::ePSIToBarConstant, EUnit::eNone, EVariableLoc::eRHS ),   //
        } );

    retVal.front()->setUnitLabel( NUtilities::NUnitStrings::pressureUnit( false, true, false ) );
    ( *std::next( retVal.begin() ) )->setUnitLabel( NUtilities::NUnitStrings::pressureUnit( true, true, false ) );
    return retVal;
}

QString CCalculator::getBaseFormula() const
{
    return NUtilities::psiToBarFormula( "psi", "bar", "psiToBar" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar ) const
{
    auto psi = getVariable( "psi" );
    auto bar = getVariable( "bar" );

    if ( unsetVar->name() == "bar" )
    {
        return getBaseFormula();
    }
    else if ( unsetVar->name() == "psi" )
    {
        return NUtilities::barToPSIFormula( "psi", "bar", "psiToBar" );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo & unsetVar )
{
    auto psi = getVariable( "psi" );
    auto bar = getVariable( "bar" );

    if ( unsetVar == bar )
    {
        bar->setValue( NUtilities::psiToBar( psi->value() ) );
    }
    else if ( unsetVar == psi )
    {
        psi->setValue( NUtilities::barToPSI( bar->value() ) );
    }
}