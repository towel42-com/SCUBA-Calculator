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

    virtual void resetVariables() override { CSCUBACalculator::resetVariables(); }
    virtual QFrame *svgFrame() const override { return CSCUBACalculator::svgFrame(); }
    virtual QSvgWidget *svgWidget() const override { return CSCUBACalculator::svgWidget(); }

    virtual bool isWaterTypeBased() const override { return true; }

    virtual std::list< std::shared_ptr< SVariableInfo > > getMyVariables() const override;
    virtual QString getDefaultFormula() const override;
    virtual QString computeAndGenerateFormula( bool & isBaseFormula ) const override;
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Calculating Partial Pressure for Given Depth";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Partial Pressure Calculations" };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< SVariableInfo >( "ata", tr( "Absolute Pressure at Depth" ), EVariableType::eHidden, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "partialPressureAtDepth", tr( "Partial Pressure at Depth" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "depth", tr( "Depth" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "partialPressureAtSurface", tr( "Partial Pressure at Surface" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "depthToSingleAtmosphere", tr( "Depth to Single Atmosphere" ), EVariableType::eDepthToSingleAtmosphereConst, EUnit::eLength, EVariableLoc::eRHS ),   //
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

QString CCalculator::getDefaultFormula() const
{
    auto depthToPressureFormula = NUtilities::depthToPressureFormula( "ata", "depth", "depthToSingleAtmosphere" );
    auto ppatDepth = QString( R"__(<partialPressureAtDepth> = <ata_value> \times <partialPressureAtSurface>)__" );
    return depthToPressureFormula + R"( \newline\newline )" + ppatDepth;
}

QString CCalculator::computeAndGenerateFormula( bool & isBaseFormula ) const
{
    auto ata = getVariable( "ata" );
    auto partialPressureAtDepth = getVariable( "partialPressureAtDepth" );
    auto depth = getVariable( "depth" );
    auto partialPressureAtSurface = getVariable( "partialPressureAtSurface" );

    QString formula;
    bool aOK = numUnsetVariables() == 1;
    isBaseFormula = false;
    if ( !aOK )
    {
        formula = getDefaultFormula();
        isBaseFormula = true;
    }
    else if ( !partialPressureAtDepth->has_value() )
    {
        ata->setValue( NUtilities::depthToPressure( imperial(), seaWater(), depth->value() ) );
        partialPressureAtDepth->setValue( ata->value() * partialPressureAtSurface->value() );
        formula = getDefaultFormula();
    }
    else if ( !depth->has_value() )
    {
        depth->setValue( NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) * ( ( ( partialPressureAtDepth->value() / partialPressureAtSurface->value() ) ) - 1 ) );
    }
    else if ( !partialPressureAtSurface->has_value() )
    {
        ata->setValue( NUtilities::depthToPressure( imperial(), seaWater(), depth->value() ) );
        partialPressureAtSurface->setValue( partialPressureAtDepth->value() / ata->value() );
    }

    return formula;
}
