#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/Formula.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "AltitudeDiving" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual bool isWaterTypeBased() const override { return true; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;
    virtual void setupCustomDependencies() override;

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
    return tr( "Altitude Diving" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Miscellaneous" ) };
}

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
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

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool imperial, bool /*seaWater*/ ) const
{
    auto formulas = TFormulaList( { NUtilities::NConversions::surfacePressureAtAltitudeFormula( imperial, getVariable( "surfacePressure" ), getVariable( "altitude" ) ) } );

    formulas.emplace_back( std::make_shared< CFormula >( getVariable( "theoreticalDepth" ), QString( R"__([(<depth> + %1) * \frac{%2}{<surfacePressure>}] - %1)__" ).arg( NUtilities::fieldNameForType( EVariableType::eDepthToSingleATMConst ) ).arg( NUtilities::fieldNameForType( EVariableType::ePressureAtSurfaceConst ) ) ) );
    formulas.emplace_back( std::make_shared< CFormula >( getVariable( "safetyStop" ), QString( R"__([(%3 + %1) * \frac{%2}{<surfacePressure>}] - %1)__" ).arg( NUtilities::fieldNameForType( EVariableType::eDepthToSingleATMConst ) ).arg( NUtilities::fieldNameForType( EVariableType::ePressureAtSurfaceConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eSafetyStopDepthConst ) ) ) );

    return formulas;
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    TFormulaList retVal;

    if ( unsetVar == "theoreticalDepth" )
    {
        return myBaseFormulas( imperial, seaWater );
    }

    if ( unsetVar == "altitude" )
    {
        retVal = TFormulaList( { NUtilities::NConversions::altitudeForSurfacePressureFormula( imperial, getVariable( "surfacePressure" ), getVariable( "altitude" ) ) } );
    }

    if ( unsetVar == "depth" )
    {
        retVal = TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__([(<theoreticalDepth> + %1) \times \frac{<surfacePressure>}{%2}] - %1)__" ).arg( NUtilities::fieldNameForType( EVariableType::eDepthToSingleATMConst ) ).arg( NUtilities::fieldNameForType( EVariableType::ePressureAtSurfaceConst ) ) ) } );
    }

    if ( !getVariable( "surfacePressure" )->has_value() && getVariable( "surfacePressure" )->dependenciesSatisfied() )
    {
        retVal.emplace_back( NUtilities::NConversions::surfacePressureAtAltitudeFormula( imperial, getVariable( "surfacePressure" ), getVariable( "altitude" ) ) );
    }

    if ( !getVariable( "altitude" )->has_value() && getVariable( "altitude" )->dependenciesSatisfied() )
    {
        retVal.emplace_back( NUtilities::NConversions::altitudeForSurfacePressureFormula( imperial, getVariable( "surfacePressure" ), getVariable( "altitude" ) ) );
    }

    if ( !getVariable( "safetyStop" )->has_value() )
    {
        retVal.emplace_back( std::make_shared< CFormula >( getVariable( "safetyStop" ), QString( R"__([(%3 + %1) * \frac{%2}{<surfacePressure>}] - %1)__" ).arg( NUtilities::fieldNameForType( EVariableType::eDepthToSingleATMConst ) ).arg( NUtilities::fieldNameForType( EVariableType::ePressureAtSurfaceConst ) ).arg( NUtilities::fieldNameForType( EVariableType::eSafetyStopDepthConst ) ) ) );
    }

    return retVal;
}

void CCalculator::setupCustomDependencies()
{
    setDependencies( "surfacePressure", "altitude" );
    setDependencies( "altitude", "surfacePressure" );
    setDependencies( "safetyStop", "surfacePressure" );
    setDependencies( "theoreticalDepth", { "depth", "surfacePressure" } );
    setDependencies( "depth", { "theoreticalDepth", "surfacePressure" } );
}

void CCalculator::computeVariableValues()
{
    auto altitude = getVariable( "altitude" );
    auto depth = getVariable( "depth" );
    auto theoreticalDepth = getVariable( "theoreticalDepth" );
    auto surfacePressure = getVariable( "surfacePressure" );
    auto safetyStop = getVariable( "safetyStop" );

    if ( !surfacePressure->has_value() && surfacePressure->dependenciesSatisfied() )
    {
        surfacePressure->setValue( NUtilities::NConversions::surfacePressureAtAltitude( imperial(), altitude->value() ) );
    }

    if ( !altitude->has_value() && altitude->dependenciesSatisfied() )
    {
        altitude->setValue( NUtilities::NConversions::altitudeForSurfacePressure( imperial(), surfacePressure->value() ) );
    }

    if ( !safetyStop->has_value() && safetyStop->dependenciesSatisfied() )
    {
        safetyStop->setValue( ( NUtilities::NConstants::safetyStopDepth( imperial(), seaWater() ) + NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() ) ) * ( NUtilities::NConstants::pressureAtSurface( imperial() ) / surfacePressure->value() ) - NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() ) );
    }

    if ( !theoreticalDepth->has_value() && theoreticalDepth->dependenciesSatisfied() )
    {
        theoreticalDepth->setValue( ( depth->value() + NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() ) ) * ( NUtilities::NConstants::pressureAtSurface( imperial() ) / surfacePressure->value() ) - NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() ) );
    }

    if ( !depth->has_value() && depth->dependenciesSatisfied() )
    {
        depth->setValue( ( ( theoreticalDepth->value() + NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() ) ) * ( surfacePressure->value() / NUtilities::NConstants::pressureAtSurface( imperial() ) ) ) - NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() ) );
    }
}