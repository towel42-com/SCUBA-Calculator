#include "Calculator.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "EAD" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual bool isWaterTypeBased() const override { return true; }

    virtual TVariableInfoList getMyVariables() const override;
    
    virtual std::optional< QString > myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo & unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Equivalent Air Depth (EAD)" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Partial Pressure Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "ead", tr( "Equivalent Air Depth EAD" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "fn2", tr( "FN2" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "depth", tr( "Depth" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::eFN2AtSurfaceConst ),   //
            std::make_shared< CVariableInfo >( EVariableType::eDepthToSingleATMConst ),   //
        };
}

std::optional< QString > CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return QString( R"__(<ead> = [(\frac{<fn2>}{<%2>}) \times (<depth> + <%1>)] - <%1>)__" ).arg( NUtilities::NConstants::kDepthToSingleATMConstFieldName ).arg( NUtilities::NConstants::kFN2AtSurfaceFieldName );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "ead" )
    {
        return getBaseFormula();
    }
    else if ( unsetVar->name() == "fn2" )
    {
        return QString( R"__(<fn2> = \frac{[<%2> \times (<ead>+<%1>)]}{(<depth>+<%1>)})__" ).arg( NUtilities::NConstants::kDepthToSingleATMConstFieldName ).arg( NUtilities::NConstants::kFN2AtSurfaceFieldName );
    }
    else if ( unsetVar->name() == "depth" )
    {
        return QString( R"__(<depth> = [\frac{(<ead>+<%1>)}{\frac{<fn2>}{<%2>}}]-<%1>)__" ).arg( NUtilities::NConstants::kDepthToSingleATMConstFieldName ).arg( NUtilities::NConstants::kFN2AtSurfaceFieldName );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo & unsetVar )
{
    auto ead = getVariable( "ead" );
    auto fn2 = getVariable( "fn2" );
    auto depth = getVariable( "depth" );

    if ( unsetVar == ead )
    {
        ead->setValue( ( ( fn2->value() / NUtilities::NConstants::percentN2AtSurface() ) * ( depth->value() + NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) ) - NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) );
    }
    else if ( unsetVar == fn2 )
    {
        fn2->setValue( ( NUtilities::NConstants::percentN2AtSurface() * ( ead->value() + NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) ) / ( depth->value() + NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) );
    }
    else if ( unsetVar == depth )
    {
        depth->setValue( ( ( ead->value() + NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) / ( fn2->value() / NUtilities::NConstants::percentN2AtSurface() ) ) - NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) );
    }
}