#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/Formula.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "PartialPressureForDepth" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual bool showWaterType() const override { return true; }

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
    return tr( "Calculating Partial Pressure for Given Depth" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Partial Pressure Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< CVariableInfo >( "ata", tr( "Absolute Pressure at Depth" ), EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "partialPressureAtDepth", tr( "Partial Pressure at Depth" ), EUnit::ePercent, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "depth", tr( "Depth" ), EUnit::eDepth, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >(
                "partialPressureAtSurface", tr( "Partial Pressure at Surface" ), EUnit::ePercent, EVariableLoc::eRHS,
                SBaseInfo< TNamedValueItemList >(
                    {}, {},
                    TNamedValueItemList( {
                        //
                        std::make_pair( tr( "Oxygen" ), NUtilities::NConstants::percentO2AtSurface() ),   //
                        std::make_pair( tr( "Nitrogen" ), NUtilities::NConstants::percentN2AtSurface() ),   //
                        std::make_pair( tr( "Other" ), TOptionalDouble() )   //
                    } ) ) ),   //
        } );

    auto pos = retVal.begin();
    ( *pos )->setIsIntermediate( true );

    return retVal;
}

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    auto formulas = TFormulaList( { NUtilities::NConversions::depthToATAFormula( getVariable( "ata" ), getVariable( "depth" ) ) } );
    formulas.emplace_back( std::make_shared< CFormula >( getVariable( "partialPressureAtDepth" ), QString( R"__(<ata_value> \times <partialPressureAtSurface>)__" ) ) );
    return formulas;
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "partialPressureAtDepth" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "depth" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(%1 \times (\frac{<partialPressureAtDepth>}{<partialPressureAtSurface>} - 1))__" ).arg( NUtilities::fieldNameForType( EConstantType::eDepthToSingleATMConst ) ) ) } );
    }
    else if ( unsetVar == "partialPressureAtSurface" )
    {
        auto formulas = TFormulaList( { NUtilities::NConversions::depthToATAFormula( getVariable( "ata" ), getVariable( "depth" ) ) } );
        formulas.emplace_back( std::make_shared< CFormula >( getVariable( unsetVar ), R"__(\frac{< partialPressureAtDepth > } {< ata_value > })__" ) );
        return formulas;
    }

    return {};
}

void CCalculator::setupCustomDependencies()
{
    setDependencies( "ata", "depth" );
    setDependencies( "partialPressureAtDepth", { "ata", "partialPressureAtSurface" } );
    setDependencies( "depth", { "partialPressureAtDepth", "partialPressureAtSurface" } );
    setDependencies( "partialPressureAtSurface", { "partialPressureAtDepth", "ata" } );
}

void CCalculator::computeVariableValues()
{
    auto ata = getVariable( "ata" );
    auto partialPressureAtDepth = getVariable( "partialPressureAtDepth" );
    auto depth = getVariable( "depth" );
    auto partialPressureAtSurface = getVariable( "partialPressureAtSurface" );

    if ( !ata->has_value() && ata->dependenciesSatisfied() )
    {
        ata->setValue( NUtilities::NConversions::depthToATA( imperial(), seaWater(), depth->value() ) );
    }

    if ( !partialPressureAtDepth->has_value() && partialPressureAtDepth->dependenciesSatisfied() )
    {
        partialPressureAtDepth->setValue( ata->value() * partialPressureAtSurface->value() );
    }

    if ( !depth->has_value() && depth->dependenciesSatisfied() )
    {
        depth->setValue( NUtilities::NConstants::singleATMPerDepth( imperial(), seaWater() ) * ( ( ( partialPressureAtDepth->value() / partialPressureAtSurface->value() ) ) - 1 ) );
    }

    if ( !partialPressureAtSurface->has_value() && partialPressureAtSurface->dependenciesSatisfied() )
    {
        if ( ata->value() != 0.0 )
            partialPressureAtSurface->setValue( partialPressureAtDepth->value() / ata->value() );
    }
}