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

    virtual std::list< std::shared_ptr< SVariableInfo > > getMyVariables() const override;
    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior ) override;

    virtual QString getBaseFormula() const override;   // for descriptive purposes
    virtual std::optional< QString > getCurrentFormula() const override;   // returns the current formula in use

    virtual void computeValues() const override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return tr( "Maximum Operating Depth (MOD)" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Partial Pressure Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal = TVariableInfoList(   //
        {
            std::make_shared< SVariableInfo >( "mod", tr( "Maximum Operating Depth (MOD)" ), EVariableType::eVariable, EUnit::eLength, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "maxPO2", tr( "Maximum PO2" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "fo2", tr( "FO2" ), EVariableType::eVariable, EUnit::ePercent, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "depthToSingleAtmosphere", tr( "Depth to Single Atmosphere" ), EVariableType::eDepthToSingleAtmosphereConstant, EUnit::eLength, EVariableLoc::eRHS ),   //
        } );
    ( *std::next( retVal.begin() ) )->setRange( SRange( { 0.21, 2.0, 1.4, 0.1 } ) );
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

QString CCalculator::getBaseFormula() const
{
    return R"__(<mod>=[(\frac{<maxPO2>}{<fo2>})-1] \times <depthToSingleAtmosphere>)__";
}

QString CCalculator::computeAndGenerateFormula( bool &isBaseFormula ) const
{
    auto mod = getVariable( "mod" );
    auto maxPO2 = getVariable( "maxPO2" );
    auto fo2 = getVariable( "fo2" );

    QString formula;
    bool aOK = ( numUnsetVariables() == 1 ) && ( fo2->value() != 0.0 );
    isBaseFormula = false;
    if ( !aOK )
    {
        formula = getBaseFormula();
        isBaseFormula = true;
    }
    else if ( !mod->has_value() )
    {
        mod->setValue( ( ( maxPO2->value() / fo2->value() ) - 1 ) * NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) );
        formula = getBaseFormula();
    }
    else if ( !maxPO2->has_value() )
    {
        maxPO2->setValue( fo2->value() * ( ( mod->value() / NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) + 1 ) );
        formula = tr( R"__(<maxPO2>=<fo2> \times [(\frac{<mod>}{<depthToSingleAtmosphere>})+1])__" );
    }
    else if ( !fo2->has_value() )
    {
        if ( mod->value() != 0.0 )
            fo2->setValue( maxPO2->value() / ( ( mod->value() / NUtilities::NConstants::depthToSingleAtmosphere( imperial(), seaWater() ) ) + 1 ) );

        formula = tr( R"__(<fo2>=\frac{<maxPO2>}{(\frac{<mod>}{<depthToSingleAtmosphere>})+1})__" );
    }
    return formula;
}
