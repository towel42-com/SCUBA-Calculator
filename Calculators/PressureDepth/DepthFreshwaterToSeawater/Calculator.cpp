#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "DepthFreshwaterToSeawater" ); }
    virtual ~CCalculator() override {}

    virtual bool isReversible() const override { return true; }
    virtual std::optional< std::pair< QString, QString > > fromToLabels() const override;

    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual TOptionalFormulaList myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual TOptionalFormulaList myReversedBaseFormulas( bool imperial, bool seaWater ) const override;
    virtual TOptionalFormulaList getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
};

extern "C" CCalculatorBase *instantiateCalculator()
{
    return new CCalculator;
}

std::optional< std::pair< QString, QString > > CCalculator::fromToLabels() const
{
    return std::make_pair(  tr( "Depth Freshwater" ), tr( "Depth Seawater" ) );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Pressure and Depth Conversions" ) };
}

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
{
    auto retVal = TVariableInfoList(   //
        {
            CVariableInfo::create( "seaWater", tr( "Seawater" ), EUnit::eDepth, EVariableLoc::eLHS ),   //
            CVariableInfo::create( "freshWater", tr( "Freshwater" ), EUnit::eDepth, EVariableLoc::eRHS ),   //
        } );

    return retVal;
}

TOptionalFormulaList CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { NUtilities::NConversions::depthFreshwaterToSeawaterFormula( getVariable( "freshWater" ), getVariable( "seaWater" ) ) } );
}

TOptionalFormulaList CCalculator::myReversedBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { NUtilities::NConversions::depthSeawaterToFreshwaterFormula( getVariable( "freshWater" ), getVariable( "seaWater" ) ) } );
}

TOptionalFormulaList CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "seaWater" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "freshWater" )
    {
        return myReversedBaseFormulas( imperial, seaWater );
    }
    return {};
}

void CCalculator::computeVariableValues()
{
    auto freshWater = getVariable( "freshWater" );
    auto seaWater = getVariable( "seaWater" );

    if ( !seaWater->has_value() && seaWater->dependenciesSatisfied() )
    {
        seaWater->setValue( NUtilities::NConversions::depthFreshwaterToSeawater( freshWater->value() ) );
    }
    
    if ( !freshWater->has_value() && freshWater->dependenciesSatisfied() )
    {
        freshWater->setValue( NUtilities::NConversions::depthSeawaterToFreshwater( seaWater->value() ) );
    }
}