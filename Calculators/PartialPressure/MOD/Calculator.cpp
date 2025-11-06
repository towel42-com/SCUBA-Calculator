#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "MOD" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual bool isWaterTypeBased() const override { return true; }

    virtual TVariableInfoList getMyVariables() const override;
    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior ) override;

    virtual std::optional< TFormulaStringList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaStringList > getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Maximum Operating Depth (MOD)" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Partial Pressure Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< CVariableInfo >( "mod", tr( "Maximum Operating Depth (MOD)" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "maxPO2", tr( "Maximum PO2" ), EUnit::ePercent, EVariableLoc::eRHS, SBaseInfo< SRange >( {}, {}, SRange( { 0.21, 2.0, 1.4, 0.1 } ) ) ),   //
            std::make_shared< CVariableInfo >( "fo2", tr( "FO2" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS ),   //
        } );
    return retVal;
}

TVariableInfo CCalculator::determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior )
{
    TVariableInfo retVal;
    if ( preDefaultBehavior )
    {
        if ( getVariable( "mod" )->isWidget( triggerWidget ) )
        {
            retVal = getVariable( "fo2" );
        }
    }
    else
        retVal = CSCUBACalculator::determineVariableToUnset( updateFromSide, triggerWidget, preDefaultBehavior );
    return retVal;
}

std::optional< TFormulaStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { TFormulaString( getVariable( "mod" ), QString( R"__([(\frac{<maxPO2>}{<fo2>})-1] \times <%1>)__" ).arg( NUtilities::NConstants::kDepthToSingleATMConstFieldName ) ) } );
}

std::optional< TFormulaStringList > CCalculator::getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar->name() == "mod" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar->name() == "maxPO2" )
    {
        return TFormulaStringList( { TFormulaString( unsetVar, QString( R"__(<fo2> \times [(\frac{<mod>}{<%1>})+1])__" ).arg( NUtilities::NConstants::kDepthToSingleATMConstFieldName ) ) } );
    }
    else if ( unsetVar->name() == "fo2" )
    {
        return TFormulaStringList( { TFormulaString( unsetVar, QString( R"__(\frac{<maxPO2>}{(\frac{<mod>}{<%1>})+1})__" ).arg( NUtilities::NConstants::kDepthToSingleATMConstFieldName ) ) } );
    }

    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto mod = getVariable( "mod" );
    auto maxPO2 = getVariable( "maxPO2" );
    auto fo2 = getVariable( "fo2" );

    if ( unsetVar == mod )
    {
        mod->setValue( ( ( maxPO2->value() / fo2->value() ) - 1 ) * NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) );
    }
    else if ( unsetVar == maxPO2 )
    {
        maxPO2->setValue( fo2->value() * ( ( mod->value() / NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) + 1 ) );
    }
    else if ( unsetVar == fo2 )
    {
        if ( mod->value() != 0.0 )
            fo2->setValue( maxPO2->value() / ( ( mod->value() / NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) + 1 ) );
    }
}