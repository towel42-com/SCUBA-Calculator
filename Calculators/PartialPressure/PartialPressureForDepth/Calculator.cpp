#include "Calculator.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "PartialPressureForDepth" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual bool isWaterTypeBased() const override { return true; }

    virtual TVariableInfoList getMyVariables() const override;

    virtual QString myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
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

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< CVariableInfo >( "ata", tr( "Absolute Pressure at Depth" ), EVariableType::eIntermediate, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "partialPressureAtDepth", tr( "Partial Pressure at Depth" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "depth", tr( "Depth" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "partialPressureAtSurface", tr( "Partial Pressure at Surface" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS,
                TNamedValueItemList( {
                    //
                    std::make_pair( tr( "Oxygen" ), NUtilities::NConstants::percentO2AtSurface() ),   //
                    std::make_pair( tr( "Nitrogen" ), NUtilities::NConstants::percentN2AtSurface() ),   //
                    std::make_pair( tr( "Other" ), TOptionalDouble() )   //
                } ) ),   //
            std::make_shared< CVariableInfo >( EVariableType::eDepthToSingleATMConst ),   //
        } );

    return retVal;
}

QString CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    QStringList formulas;
    formulas << NUtilities::NConversions::depthToPressureFormula( "ata", "depth" );
    formulas << R"__(<partialPressureAtDepth> = <ata_value> \times <partialPressureAtSurface>)__";
    return NUtilities::joinFormulas( formulas );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "partialPressureAtDepth" )
    {
        return getBaseFormula();
    }
    else if ( unsetVar->name() == "depth" )
    {
        return QString( R"__(<depth> = <%1> \times (\frac{<partialPressureAtDepth>}{<partialPressureAtSurface>} - 1))__" ).arg( NUtilities::NConstants::kDepthToSingleATMConstFieldName );
    }
    else if ( unsetVar->name() == "partialPressureAtSurface" )
    {
        QStringList formulas;
        formulas << NUtilities::NConversions::depthToPressureFormula( "ata", "depth" );
        formulas << R"__(<partialPressureAtSurface> = \frac{<partialPressureAtDepth>}{<ata_value>})__";
        return NUtilities::joinFormulas( formulas );
    }

    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto ata = getVariable( "ata" );
    auto partialPressureAtDepth = getVariable( "partialPressureAtDepth" );
    auto depth = getVariable( "depth" );
    auto partialPressureAtSurface = getVariable( "partialPressureAtSurface" );

    if ( unsetVar == partialPressureAtDepth )
    {
        ata->setValue( NUtilities::NConversions::depthToPressure( imperial(), seaWater(), depth->value() ) );
        partialPressureAtDepth->setValue( ata->value() * partialPressureAtSurface->value() );
    }
    else if ( unsetVar == depth )
    {
        depth->setValue( NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) * ( ( ( partialPressureAtDepth->value() / partialPressureAtSurface->value() ) ) - 1 ) );
    }
    else if ( unsetVar == partialPressureAtSurface )
    {
        ata->setValue( NUtilities::NConversions::depthToPressure( imperial(), seaWater(), depth->value() ) );
        partialPressureAtSurface->setValue( partialPressureAtDepth->value() / ata->value() );
    }
}