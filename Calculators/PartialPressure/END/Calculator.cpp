#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

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

    virtual TVariableInfoList getMyVariables() const override;

    virtual std::optional< QStringList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QStringList > getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
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

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal =   //
        TVariableInfoList( {
            std::make_shared< CVariableInfo >( "depth", tr( "Depth" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "fhe", tr( "Helium Percentage" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "end", tr( "Equivalent Narcotic Depth (END)" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eLHS ),   //
        } );
    return retVal;
}

std::optional< QStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return QStringList() << QString( R"__(<end> = [(<depth> + <%1>) \times ( 1.0 - <fhe> ) ] - <%1> )__" ).arg( NUtilities::fieldNameForType( EVariableType::eDepthToSingleATMConst ) );
}

std::optional< QStringList > CCalculator::getFormulasForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "end" )
        return myBaseFormulas( false, false );
    else if ( unsetVar->name() == "fhe" )
        return QStringList() << QString( R"__(<fhe> =1.0 - \frac{<end> + %1}{<depth> + <%1>} )__" ).arg( NUtilities::fieldNameForType( EVariableType::eDepthToSingleATMConst ) );
    else if ( unsetVar->name() == "depth" )
        return QStringList() << QString( R"__(<depth> = \frac{<end> + %1}{1.0 - <fhe>} - <%1>))__" ).arg( NUtilities::fieldNameForType( EVariableType::eDepthToSingleATMConst ) );
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto depth = getVariable( "depth" );
    auto fhe = getVariable( "fhe" );
    auto end = getVariable( "end" );

    auto depthToSingleATM = NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() );
    if ( unsetVar == end )
    {
        end->setValue( ( ( depth->value() + depthToSingleATM ) * ( 1.0 - fhe->value() ) ) - depthToSingleATM );
    }
    else if ( unsetVar == fhe )
    {
        fhe->setValue( 1.0 - ( end->value() + depthToSingleATM ) / ( depth->value() + depthToSingleATM ) );
    }
    else if ( unsetVar == depth )
    {
        depth->setValue( ( ( end->value() + depthToSingleATM ) / ( 1.0 - fhe->value() ) ) - depthToSingleATM );
    }
}