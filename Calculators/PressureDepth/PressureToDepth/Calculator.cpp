#include "Calculator.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "PressureToDepth" ); }
    virtual ~CCalculator() override {}

    virtual bool isReversible() const override { return true; }
    virtual std::pair< QString, QString > fromToLabels() const override;

    virtual QStringList myCalculatorPath() const override;

    virtual TVariableInfoList getMyVariables() const override;

    virtual QString myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual QString myReversedBaseFormula() const override;
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
    virtual bool isWaterTypeBased() const override { return true; }
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

std::pair< QString,QString > CCalculator::fromToLabels() const
{
    return { tr( "Pressure" ), tr( "Depth" ) };
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure and Depth Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "depth", tr( "depth" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "pressure", tr( "Pressure" ), EVariableType::eVariable, EUnit::eAtmospheres, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::eDepthToSingleATMConst ),   //
        };
}

QString CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return NUtilities::NConversions::pressureToDepthFormula( "pressure", "depth" );
}

QString CCalculator::myReversedBaseFormula() const
{
    return NUtilities::NConversions::depthToPressureFormula( "pressure", "depth" );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "depth" )
    {
        return getBaseFormula();
    }
    else if ( unsetVar->name() == "pressure" )
    {
        return NUtilities::NConversions::depthToPressureFormula( "pressure", "depth" );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto pressure = getVariable( "pressure" );
    auto depth = getVariable( "depth" );

    if ( unsetVar == depth )
    {
        depth->setValue( NUtilities::NConversions::pressureToDepth( imperial(), seaWater(), pressure->value() ) );
    }
    else if ( unsetVar == pressure )
    {
        pressure->setValue( NUtilities::NConversions::depthToPressure( imperial(), seaWater(), depth->value() ) );
    }
}