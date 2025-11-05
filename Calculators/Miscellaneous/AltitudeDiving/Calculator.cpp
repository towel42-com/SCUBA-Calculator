#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "AltitudeDiving" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual bool isWaterTypeBased() const override { return true; }

    virtual TVariableInfoList getMyVariables() const override;

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
    return tr( "Altitude Diving" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Miscellaneous" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal =   //
        TVariableInfoList( {
            std::make_shared< CVariableInfo >( "altitude", tr( "Altitude" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "depth", tr( "Actual Depth" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "theoreticalDepth", tr( "Theoretical Depth" ), EVariableType::eVariable, EUnit::eDepth, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "safetyStop", tr( "Safety Stop" ), EVariableType::eIntermediate, EUnit::eDepth, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "surfacePressure", tr( "Surface Air Pressure @ Altitude" ), EVariableType::eIntermediate, EUnit::ePressure, EVariableLoc::eLHS ),   //
        } );
    return retVal;
}

std::optional< TFormulaStringList > CCalculator::myBaseFormulas( bool imperial, bool /*seaWater*/ ) const
{
    auto formulas = TFormulaStringList( { NUtilities::NConversions::surfacePressureAtAltitudeFormula( imperial, "surfacePressure", "altitude" ) } );

    formulas.emplace_back( R"__(<theoreticalDepth>)__", QString( R"__([(<depth> + <%1>) * \frac{<%2>}{<surfacePressure>}] - <%1>)__" ).arg( NUtilities::fieldNameForType( EVariableType::eDepthToSingleATMConst ) ).arg( NUtilities::fieldNameForType( EVariableType::ePressureAtSurfaceConst ) ) );
    formulas.emplace_back( R"__(<safetyStop>)__", QString( R"__([(<%3> + <%1>) * \frac{<%2>}{<surfacePressure>}] - <%1>)__" ).arg( NUtilities::fieldNameForType( EVariableType::eDepthToSingleATMConst ) ).arg( NUtilities::fieldNameForType( EVariableType::ePressureAtSurfaceConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eSafetyStopDepthConst ) ) );

    return formulas;
}

std::optional< TFormulaStringList > CCalculator::getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar->name() == "theoreticalDepth" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar->name() == "altitude" )
    {
        return TFormulaStringList( { NUtilities::NConversions::altitudeForSurfacePressureFormula( imperial, "surfacePressure", "altitude" ) } );
    }
    else if ( unsetVar->name() == "depth" )
    {
        return TFormulaStringList( { TFormulaString( R"__(<depth>)__", QString( R"__([(<theoreticalDepth> + <%1>) \times \frac{<surfacePressure>}{<%2>}] - <%1>)__" ).arg( NUtilities::fieldNameForType( EVariableType::eDepthToSingleATMConst ) ).arg( NUtilities::fieldNameForType( EVariableType::ePressureAtSurfaceConst ) ) ) } );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto altitude = getVariable( "altitude" );
    auto depth = getVariable( "depth" );
    auto theoreticalDepth = getVariable( "theoreticalDepth" );
    auto surfacePressure = getVariable( "surfacePressure" );

    if ( unsetVar == theoreticalDepth )
    {
        surfacePressure->setValue( NUtilities::NConversions::surfacePressureAtAltitude( imperial(), altitude->value() ) );
        theoreticalDepth->setValue( ( depth->value() + NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) * ( NUtilities::NConstants::pressureAtSurface( imperial() ) / surfacePressure->value() ) - NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) );
        getVariable( "safetyStop" )->setValue( ( NUtilities::NConstants::safetyStopDepth( imperial(), seaWater() ) + NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) * ( NUtilities::NConstants::pressureAtSurface( imperial() ) / surfacePressure->value() ) - NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) );
    }
    else if ( unsetVar == altitude )
    {
        altitude->setValue( NUtilities::NConversions::altitudeForSurfacePressure( imperial(), surfacePressure->value() ) );
    }
    else if ( unsetVar == depth )
    {
        depth->setValue( ( ( theoreticalDepth->value() + NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) * ( surfacePressure->value() / NUtilities::NConstants::pressureAtSurface( imperial() ) ) ) - NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) );
    }
}