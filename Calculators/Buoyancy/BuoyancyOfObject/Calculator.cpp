#include "Calculator.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "BuoyancyOfObject" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual bool isWaterTypeBased() const override { return true; }

    virtual TVariableInfoList getMyVariables() const override;

    virtual std::optional< QString > myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Buoyancy Characteristics of an Object" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Buoyancy Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "buoyancy", tr( "Buoyancy" ), EVariableType::eVariable, EUnit::eWeight, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "weightOfObject", tr( "Weight" ), EVariableType::eVariable, EUnit::eWeight, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "volumeDisplaced", tr( "Volume" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::eWeightOfWaterConst )   //
        };
}

std::optional< QString > CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return R"__(<buoyancy>=<weightOfObject> - [<volumeDisplaced> \times <weightOfWater>])__";
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
{
    if ( unsetVar->name() == "buoyancy" )
    {
        return getBaseFormula();
    }
    if ( unsetVar->name() == "weightOfObject" )
    {
        return R"__(<weightOfObject>=<buoyancy> + <volumeDisplaced> \times <weightOfWater>)__";
    }
    if ( unsetVar->name() == "volumeDisplaced" )
    {
        return R"__(<volumeDisplaced>=\frac{(<weightOfObject> - <buoyancy>)}{<weightOfWater>})__";
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto buoyancy = getVariable( "buoyancy" );
    auto weightOfObject = getVariable( "weightOfObject" );
    auto volumeDisplaced = getVariable( "volumeDisplaced" );

    if ( unsetVar == buoyancy )
        buoyancy->setValue( weightOfObject->value() - ( volumeDisplaced->value() * NUtilities::NConstants::weightOfWater( imperial(), seaWater() ) ) );
    else if ( unsetVar == weightOfObject )
        weightOfObject->setValue( buoyancy->value() + ( volumeDisplaced->value() * NUtilities::NConstants::weightOfWater( imperial(), seaWater() ) ) );
    else if ( unsetVar == volumeDisplaced )
        volumeDisplaced->setValue( ( weightOfObject->value() - buoyancy->value() ) / NUtilities::NConstants::weightOfWater( imperial(), seaWater() ) );
}
