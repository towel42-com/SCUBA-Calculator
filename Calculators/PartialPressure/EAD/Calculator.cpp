#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/Formula.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "EAD" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual bool isWaterTypeBased() const override { return true; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual std::optional< TFormulaList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaList > getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
};

extern "C" CCalculatorBase *instantiateCalculator()
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

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "ead", tr( "Equivalent Air Depth EAD" ), EUnit::eDepth, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "fn2", tr( "FN2" ), EUnit::ePercent, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "depth", tr( "Depth" ), EUnit::eDepth, EVariableLoc::eRHS ),   //
        };
}

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { std::make_shared< CFormula >( getVariable( "ead" ), QString( R"__([(\frac{<fn2>}{%2}) \times (<depth> + %1)] - %1)__" ).arg( NUtilities::fieldNameForType( EConstantType::eDepthToSingleATMConst ) ).arg( NUtilities::fieldNameForType( EConstantType::eFN2AtSurfaceConst ) ) ) } );
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "ead" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "fn2" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(\frac{[%2 \times (<ead>+<%1>)]}{(<depth>+%1)})__" ).arg( NUtilities::fieldNameForType( EConstantType::eDepthToSingleATMConst ) ).arg( NUtilities::fieldNameForType( EConstantType::eFN2AtSurfaceConst ) ) ) } );
    }
    else if ( unsetVar == "depth" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__([\frac{<ead>+%1}{\frac{<fn2>}{%2}}]-%1)__" ).arg( NUtilities::fieldNameForType( EConstantType::eDepthToSingleATMConst ) ).arg( NUtilities::fieldNameForType( EConstantType::eFN2AtSurfaceConst ) ) ) } );
    }
    return {};
}

void CCalculator::computeVariableValues()
{
    auto ead = getVariable( "ead" );
    auto fn2 = getVariable( "fn2" );
    auto depth = getVariable( "depth" );

    if ( !ead->has_value() && ead->dependenciesSatisfied() )
    {
        ead->setValue( ( ( fn2->value() / NUtilities::NConstants::percentN2AtSurface() ) * ( depth->value() + NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() ) ) ) - NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() ) );
    }
    
    if ( !fn2->has_value() && fn2->dependenciesSatisfied() )
    {
        fn2->setValue( ( NUtilities::NConstants::percentN2AtSurface() * ( ead->value() + NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() ) ) ) / ( depth->value() + NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() ) ) );
    }
    
    if ( !depth->has_value() && depth->dependenciesSatisfied() )
    {
        depth->setValue( ( ( ead->value() + NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() ) ) / ( fn2->value() / NUtilities::NConstants::percentN2AtSurface() ) ) - NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() ) );
    }
}