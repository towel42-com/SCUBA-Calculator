#include "Calculator.h"
#include "VariableInfo.h"
#include "Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() {}
    virtual ~CCalculator() override {}

    virtual bool isReversable() const override { return true; }
    virtual QString myReversedCalculatorName() const override;
    virtual QString myReversedBaseFormula() const override;

    virtual QString myCalculatorName() const override;
    virtual QStringList calculatorPath() const override;

    virtual bool showUnits() const { return false; }

    virtual TVariableInfoList getMyVariables() const override;

    virtual QString myBaseFormula() const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "PSI to BAR" );
}

QString CCalculator::myReversedCalculatorName() const
{
    return tr( "BAR to PSI" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Miscellaneous" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< CVariableInfo >( "bar", tr( "Pressure" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "psi", tr( "Pressure" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::ePSIToBarConst ),   //
        } );

    retVal.front()->setUnitLabel( NUtilities::NUnitStrings::pressureUnit( false, true, false ) );
    ( *std::next( retVal.begin() ) )->setUnitLabel( NUtilities::NUnitStrings::pressureUnit( true, true, false ) );
    return retVal;
}

QString CCalculator::myBaseFormula() const
{
    return NUtilities::NConversions::psiToBarFormula( "psi", "bar" );
}

QString CCalculator::myReversedBaseFormula() const
{
    return NUtilities::NConversions::barToPSIFormula( "psi", "bar" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar ) const
{
    auto psi = getVariable( "psi" );
    auto bar = getVariable( "bar" );

    if ( unsetVar->name() == "bar" )
    {
        return NUtilities::NConversions::psiToBarFormula( "psi", "bar" );
    }
    else if ( unsetVar->name() == "psi" )
    {
        return NUtilities::NConversions::barToPSIFormula( "psi", "bar" );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto psi = getVariable( "psi" );
    auto bar = getVariable( "bar" );

    if ( unsetVar == bar )
    {
        bar->setValue( NUtilities::NConversions::psiToBar( psi->value() ) );
    }
    else if ( unsetVar == psi )
    {
        psi->setValue( NUtilities::NConversions::barToPSI( bar->value() ) );
    }
}