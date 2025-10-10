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
    return tr( "Offsetting Negative Buoyancy" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Buoyancy Calculations" ) };
}

std::list< std::shared_ptr< SVariableInfo > > CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< SVariableInfo >( "volumeDisplaced", tr( "Volume Displaced" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "negativeBuoyancy", tr( "Negative Buoyancy" ), EVariableType::eVariable, EUnit::eWeight, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "weightOfWater", tr( "Weight of Water" ), EVariableType::eWeightOfWaterConstant, EUnit::eWeight, EVariableLoc::eRHS )   //
        };
}

QString CCalculator::getBaseFormula() const
{
    return R"__(<volumeDisplaced>=\frac{<negativeBuoyancy>}{<weightOfWater>})__";
}

QString CCalculator::computeAndGenerateFormula( bool &isBaseFormula ) const
{
    auto negativeBuoyancy = getVariable( "negativeBuoyancy" );
    auto volumeDisplaced = getVariable( "volumeDisplaced" );
    QString formula;
    bool aOK = numUnsetVariables() == 1;
    isBaseFormula = false;
    if ( !aOK )
    {
        formula = getBaseFormula();
        isBaseFormula = true;
    }
    else if ( !volumeDisplaced->has_value() )
    {
        volumeDisplaced->setValue( negativeBuoyancy->value() / NUtilities::NConstants::weightOfWater( imperial(), seaWater() ) );
        formula = getBaseFormula();
    }
    else if ( !negativeBuoyancy->has_value() )
    {
        formula = tr( R"__(<negativeBuoyancy>=<volumeDisplaced> \times <weightOfWater>)__" );
        negativeBuoyancy->setValue( volumeDisplaced->value() * NUtilities::NConstants::weightOfWater( imperial(), seaWater() ) );
    }

    return formula;
}
