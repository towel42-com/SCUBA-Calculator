#include "Calculator.h"
#include "VariableInfo.h"
#include "Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() {}
    virtual ~CCalculator() override {}

    virtual QString calculatorName() const override;
    virtual QStringList calculatorPath() const override;

    virtual bool isWaterTypeBased() const override { return true; }

    virtual void resetVariables() override { CSCUBACalculator::resetVariables(); }
    virtual QFrame *svgFrame() const override { return CSCUBACalculator::svgFrame(); }
    virtual QSvgWidget *svgWidget() const override { return CSCUBACalculator::svgWidget(); }

    virtual TVariableInfoList getMyVariables() const override;

    virtual QString getBaseFormula() const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo & unsetVar ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo & unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return tr( "Calculating Partial Pressure for Given Depth" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Partial Pressure Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< CVariableInfo >( "ata", tr( "Absolute Pressure at Depth" ), EVariableType::eHidden, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "partialPressureAtDepth", tr( "Partial Pressure at Depth" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "depth", tr( "Depth" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "partialPressureAtSurface", tr( "Partial Pressure at Surface" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "depthToSingleAtmosphere", tr( "Depth to Single Atmosphere" ), EVariableType::eDepthToSingleAtmosphereConstant, EUnit::eLength, EVariableLoc::eRHS ),   //
        } );

    ( *std::prev( std::prev( retVal.end() ) ) )
        ->setValues(   //
            TOptionalNamedValueItemList( {
                //
                std::make_pair( tr( "Oxygen" ), NUtilities::NConstants::percentO2AtSurface() ),   //
                std::make_pair( tr( "Nitrogen" ), NUtilities::NConstants::percentN2AtSurface() ),   //
                std::make_pair( tr( "Other" ), TOptionalDouble() )   //
            } ) );   //
    return retVal;
}

QString CCalculator::getBaseFormula() const
{
    auto formula = NUtilities::depthToPressureFormula( "ata", "depth", "depthToSingleAtmosphere" );
    formula += R"__( \newline\newline )__";
    formula += R"__(<partialPressureAtDepth> = <ata_value> \times <partialPressureAtSurface>)__";
    return formula;
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo & unsetVar ) const
{
    if ( unsetVar->name() == "partialPressureAtDepth" )
    {
        return getBaseFormula();
    }
    else if ( unsetVar->name() == "depth" )
    {
        return R"__(<depth> = <depthToSingleAtmosphere> * (\frac{<partialPressureAtDepth>}{<partialPressureAtSurface>} - 1))__";
    }
    else if ( unsetVar->name() == "partialPressureAtSurface" )
    {
        auto formula = NUtilities::depthToPressureFormula( "ata", "depth", "depthToSingleAtmosphere" );
        formula += R"__( \newline\newline )__";
        formula += R"__(<partialPressureAtSurface> = \frac{<partialPressureAtDepth>}{<ata_value>})__";
        return formula;

    }

    return {};
}

void CCalculator::computeValueForVar( TVariableInfo & unsetVar )
{
    auto ata = getVariable( "ata" );
    auto partialPressureAtDepth = getVariable( "partialPressureAtDepth" );
    auto depth = getVariable( "depth" );
    auto partialPressureAtSurface = getVariable( "partialPressureAtSurface" );

    if ( unsetVar == partialPressureAtDepth )
    {
        ata->setValue( NUtilities::depthToPressure( imperial(), seaWater(), depth->value() ) );
        partialPressureAtDepth->setValue( ata->value() * partialPressureAtSurface->value() );
    }
    else if ( unsetVar == depth )
    {
        depth->setValue( NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) * ( ( ( partialPressureAtDepth->value() / partialPressureAtSurface->value() ) ) - 1 ) );
    }
    else if ( unsetVar == partialPressureAtSurface )
    {
        ata->setValue( NUtilities::depthToPressure( imperial(), seaWater(), depth->value() ) );
        partialPressureAtSurface->setValue( partialPressureAtDepth->value() / ata->value() );
    }
}