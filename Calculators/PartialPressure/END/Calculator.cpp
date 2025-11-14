#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/Formula.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "END" ); }
    virtual ~CCalculator() override {}
    virtual bool showWaterType() const override { return true; }

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

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
    return tr( "Equivalent Narcotic Depth w/Helium (END)" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Partial Pressure Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
{
    auto retVal =   //
        TVariableInfoList( {
            std::make_shared< CVariableInfo >( "depth", tr( "Depth" ), EUnit::eDepth, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "fhe", tr( "Helium Percentage" ), EUnit::ePercent, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "end", tr( "Equivalent Narcotic Depth (END)" ), EUnit::eDepth, EVariableLoc::eLHS ),   //
        } );
    return retVal;
}

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { std::make_shared< CFormula >( getVariable( "end" ), QString( R"__([(<depth> + %1) \times ( 1.0 - <fhe> ) ] - %1 )__" ).arg( NUtilities::fieldNameForType( EConstantType::eDepthToSingleATMConst ) ) ) } );
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar == "end" )
        return myBaseFormulas( false, false );
    else if ( unsetVar == "fhe" )
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(1.0 - \frac{<end> + %1}{<depth> + %1} )__" ).arg( NUtilities::fieldNameForType( EConstantType::eDepthToSingleATMConst ) ) ) } );
    else if ( unsetVar == "depth" )
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(\frac{<end> + %1}{1.0 - <fhe>} - %1))__" ).arg( NUtilities::fieldNameForType( EConstantType::eDepthToSingleATMConst ) ) ) } );
    return {};
}

void CCalculator::computeVariableValues()
{
    auto depth = getVariable( "depth" );
    auto fhe = getVariable( "fhe" );
    auto end = getVariable( "end" );

    auto depthToSingleATM = NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() );
    if ( !end->has_value() && end->dependenciesSatisfied() )
    {
        end->setValue( ( ( depth->value() + depthToSingleATM ) * ( 1.0 - fhe->value() ) ) - depthToSingleATM );
    }
    
    if ( !fhe->has_value() && fhe->dependenciesSatisfied() )
    {
        fhe->setValue( 1.0 - ( end->value() + depthToSingleATM ) / ( depth->value() + depthToSingleATM ) );
    }
    
    if ( !depth->has_value() && depth->dependenciesSatisfied() )
    {
        depth->setValue( ( ( end->value() + depthToSingleATM ) / ( 1.0 - fhe->value() ) ) - depthToSingleATM );
    }
}