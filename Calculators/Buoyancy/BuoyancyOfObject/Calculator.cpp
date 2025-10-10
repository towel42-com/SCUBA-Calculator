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
    return tr( "Buoyancy Characteristics of an Object" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Buoyancy Calculations" ) };
}

std::list< std::shared_ptr< SVariableInfo > > CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< SVariableInfo >( "buoyancy", tr( "Buoyancy" ), EVariableType::eVariable, EUnit::eWeight, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "weightOfObject", tr( "Weight" ), EVariableType::eVariable, EUnit::eWeight, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "volumeDisplaced", tr( "Volume" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "weightOfWater", tr( "Weight of Water" ), EVariableType::eWeightOfWaterConstant, EUnit::eWeight, EVariableLoc::eRHS )   //
        };
}

QString CCalculator::getBaseFormula() const
{
    return R"__(<buoyancy>=<weightOfObject> - [<volumeDisplaced> \times <weightOfWater>])__";
}

std::optional< QString > CCalculator::getCurrentFormula() const
{
    auto numUnset = numUnsetVariables();
    if ( numUnset != 1 )
        return getBaseFormula();
    else if ( numUnset != 1 )
        return {};

    auto buoyancy = getVariable( "buoyancy" );
    auto weightOfObject = getVariable( "weightOfObject" );
    auto volumeDisplaced = getVariable( "volumeDisplaced" );
    if ( !buoyancy->has_value() )
    {
        return getBaseFormula();
    }
    else if ( !weightOfObject->has_value() )
    {
        return R"__(<weightOfObject>=<buoyancy> + <volumeDisplaced> \times <weightOfWater>)__";
    }
    else if ( !volumeDisplaced->has_value() )
    {
        volumeDisplaced->setValue( ( weightOfObject->value() - buoyancy->value() ) / NUtilities::NConstants::weightOfWater( imperial(), seaWater() ) );
        return R"__(<volumeDisplaced>=\frac{(<weightOfObject> - <buoyancy>)}{<weightOfWater>})__";
    }
    return {};
}

void CCalculator::computeValues() const
{
    auto buoyancy = getVariable( "buoyancy" );
    auto weightOfObject = getVariable( "weightOfObject" );
    auto volumeDisplaced = getVariable( "volumeDisplaced" );

    QString formula;
    bool aOK = numUnsetVariables() == 1;
    if ( !aOK )
    {
        return;
    }
    
    if ( !buoyancy->has_value() )
        buoyancy->setValue( weightOfObject->value() - ( volumeDisplaced->value() * NUtilities::NConstants::weightOfWater( imperial(), seaWater() ) ) );
    else if ( !weightOfObject->has_value() )
        weightOfObject->setValue( buoyancy->value() + ( volumeDisplaced->value() * NUtilities::NConstants::weightOfWater( imperial(), seaWater() ) ) );
    else if ( !volumeDisplaced->has_value() )
        volumeDisplaced->setValue( ( weightOfObject->value() - buoyancy->value() ) / NUtilities::NConstants::weightOfWater( imperial(), seaWater() ) );
}
