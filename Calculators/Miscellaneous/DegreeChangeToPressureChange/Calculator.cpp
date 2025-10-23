#include "Calculator.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "DegreeChangeToPressureChange" ); }
    virtual ~CCalculator() override {}

    virtual bool isReversible() const override { return true; }
    virtual std::pair< QString, QString > fromToLabels() const override;
    virtual QString myCalculatorName() const override;
    virtual QString myReversedCalculatorName() const override;

    virtual QStringList myCalculatorPath() const override;

    virtual TVariableInfoList getMyVariables() const override;

    virtual std::optional< QString > myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QString > myReversedBaseFormula() const override;
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

std::pair< QString, QString > CCalculator::fromToLabels() const
{
    return { tr( "Pressure Change" ), tr( "Temperature Change" ) };
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Quick Estimate for Pressure when Temperature Changes" );
}

QString CCalculator::myReversedCalculatorName() const
{
    return tr( "Quick Estimate for Temperature when Pressure Changes" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Miscellaneous" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "p1", tr( "Pressure Change" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "t1", tr( "Temperature Change" ), EVariableType::eVariable, EUnit::eTemperature, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::ePressurePerDegreeConst ),   //
        };
}

std::optional< QString > CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return NUtilities::NConversions::pressureChangeForDegreeChangeFormula( "t1", "p1" );
}

std::optional< QString > CCalculator::myReversedBaseFormula() const
{
    return NUtilities::NConversions::degreeChangeForPressureChangeFormula( "t1", "p1" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "p1" )
    {
        return NUtilities::NConversions::pressureChangeForDegreeChangeFormula( "t1", "p1" );
    }
    else if ( unsetVar->name() == "t1" )
    {
        return NUtilities::NConversions::degreeChangeForPressureChangeFormula( "t1", "p1" );
    }

    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto p1 = getVariable( "p1" );
    auto t1 = getVariable( "t1" );

    if ( unsetVar == p1 )
    {
        p1->setValue( NUtilities::NConversions::pressureChangeForDegreeChange( imperial(), t1->value() ) );
    }
    else if ( unsetVar == t1 )
    {
        t1->setValue( NUtilities::NConversions::degreeChangeForPressureChange( imperial(), p1->value() ) );
    }
}