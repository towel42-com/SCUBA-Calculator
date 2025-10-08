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
    virtual QString computeAndGenerateFormula() const override;
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Buoyancy Characteristics of an Object";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Buoyancy Calculations" };
}

std::list< std::shared_ptr< SVariableInfo > > CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< SVariableInfo >( "buoyancy", tr( "Buoyancy" ), EVariableType::eVariable, EUnit::eWeight, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "weightOfObject", tr( "Weight" ), EVariableType::eVariable, EUnit::eWeight, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "volumeDisplaced", tr( "Volume" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "weightOfWater", tr( "Weight of Water" ), EVariableType::eWeightOfWaterConst, EUnit::eWeight, EVariableLoc::eRHS )   //
        };
}

QString CCalculator::getDefaultFormula() const
{
    return tr( R"__(<buoyancy>=<weightOfObject> - [<volumeDisplaced> \times <weightOfWater>])__" );
}

QString CCalculator::computeAndGenerateFormula() const
{
    auto buoyancy = getVariable( "buoyancy" );
    auto weightOfObject = getVariable( "weightOfObject" );
    auto volumeDisplaced = getVariable( "volumeDisplaced" );

    QString formula;
    bool aOK = numUnsetVariables() == 1;
    if ( !aOK || !buoyancy->has_value() )
    {
        if ( aOK )
            buoyancy->setValue( weightOfObject->value() - ( volumeDisplaced->value() * NUtilities::NConstants::weightOfWater( imperial(), seaWater() ) ) );
        formula = getDefaultFormula();
    }
    else if ( !weightOfObject->has_value() )
    {
        weightOfObject->setValue( buoyancy->value() + ( volumeDisplaced->value() * NUtilities::NConstants::weightOfWater( imperial(), seaWater() ) ) );
        formula = tr( R"__(<weightOfObject>=<buoyancy> + <volumeDisplaced> \times <weightOfWater>)__" );
    }
    else if ( !volumeDisplaced->has_value() )
    {
        volumeDisplaced->setValue( ( weightOfObject->value() - buoyancy->value() ) / NUtilities::NConstants::weightOfWater( imperial(), seaWater() ) );
        formula = tr( R"__(<volumeDisplaced>=\frac{(<weightOfObject> - <buoyancy>)}{<weightOfWater>})__" );
    }

    return formula;
}
