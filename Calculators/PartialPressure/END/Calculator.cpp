#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/FormulaString.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "END" ); }
    virtual ~CCalculator() override {}
    virtual bool isWaterTypeBased() const override { return true; }

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual std::optional< TFormulaStringList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaStringList > getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
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
            std::make_shared< CVariableInfo >( "depth", tr( "Depth" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "fhe", tr( "Helium Percentage" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "end", tr( "Equivalent Narcotic Depth (END)" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eLHS ),   //
        } );
    return retVal;
}

std::optional< TFormulaStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( "end" ), QString( R"__([(<depth> + %1) \times ( 1.0 - <fhe> ) ] - %1 )__" ).arg( NUtilities::fieldNameForType( EVariableType::eDepthToSingleATMConst ) ) ) } );
}

std::optional< TFormulaStringList > CCalculator::getFormulasForVar( const QString &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar == "end" )
        return myBaseFormulas( false, false );
    else if ( unsetVar == "fhe" )
        return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( unsetVar ), QString( R"__(1.0 - \frac{<end> + %1}{<depth> + %1} )__" ).arg( NUtilities::fieldNameForType( EVariableType::eDepthToSingleATMConst ) ) ) } );
    else if ( unsetVar == "depth" )
        return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( unsetVar ), QString( R"__(\frac{<end> + %1}{1.0 - <fhe>} - %1))__" ).arg( NUtilities::fieldNameForType( EVariableType::eDepthToSingleATMConst ) ) ) } );
    return {};
}

void CCalculator::computeVariableValues()
{
    auto depth = getVariable( "depth" );
    auto fhe = getVariable( "fhe" );
    auto end = getVariable( "end" );

    auto depthToSingleATM = NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() );
    if ( !end->has_value() && depth->has_value() && fhe->has_value() )
    {
        end->setValue( ( ( depth->value() + depthToSingleATM ) * ( 1.0 - fhe->value() ) ) - depthToSingleATM );
    }
    
    if ( end->has_value() && depth->has_value() && !fhe->has_value() )
    {
        fhe->setValue( 1.0 - ( end->value() + depthToSingleATM ) / ( depth->value() + depthToSingleATM ) );
    }
    
    if ( end->has_value() && !depth->has_value() && fhe->has_value() )
    {
        depth->setValue( ( ( end->value() + depthToSingleATM ) / ( 1.0 - fhe->value() ) ) - depthToSingleATM );
    }
}