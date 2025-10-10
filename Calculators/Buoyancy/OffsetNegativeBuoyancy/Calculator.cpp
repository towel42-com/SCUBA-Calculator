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
    return tr( "Offsetting Negative Buoyancy" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Buoyancy Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "volumeDisplaced", tr( "Volume Displaced" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "negativeBuoyancy", tr( "Negative Buoyancy" ), EVariableType::eVariable, EUnit::eWeight, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "weightOfWater", tr( "Weight of Water" ), EVariableType::eWeightOfWaterConstant, EUnit::eWeight, EVariableLoc::eRHS )   //
        };
}

QString CCalculator::getBaseFormula() const
{
    return R"__(<volumeDisplaced>=\frac{<negativeBuoyancy>}{<weightOfWater>})__";
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo & unsetVar ) const
{
    if ( unsetVar->name() == "volumeDisplaced" )
    {
        return getBaseFormula();
    }
    if ( unsetVar->name() == "negativeBuoyancy" )
    {
        return R"__(<negativeBuoyancy>=<volumeDisplaced> \times <weightOfWater>)__";
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo & unsetVar )
{
    auto negativeBuoyancy = getVariable( "negativeBuoyancy" );
    auto volumeDisplaced = getVariable( "volumeDisplaced" );
    if ( unsetVar == volumeDisplaced )
    {
        volumeDisplaced->setValue( negativeBuoyancy->value() / NUtilities::NConstants::weightOfWater( imperial(), seaWater() ) );
    }
    else if ( unsetVar == negativeBuoyancy )
    {
        negativeBuoyancy->setValue( volumeDisplaced->value() * NUtilities::NConstants::weightOfWater( imperial(), seaWater() ) );
    }
}
