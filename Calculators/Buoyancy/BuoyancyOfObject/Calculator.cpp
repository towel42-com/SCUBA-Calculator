#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "BuoyancyOfObject" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual bool isWaterTypeBased() const override { return true; }

    virtual TVariableInfoList getMyVariables() const override;

    virtual std::optional< QStringList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QStringList > getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

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
        };
}

std::optional< QStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return QStringList() << QString( R"__(<buoyancy>=<weightOfObject> - [<volumeDisplaced> \times <%1>])__" ).arg( NUtilities::fieldNameForType( EVariableType::eWeightPerVolumeOfWaterConst ) );
    ;
}

std::optional< QStringList > CCalculator::getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar->name() == "buoyancy" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    if ( unsetVar->name() == "weightOfObject" )
    {
        return QStringList() << QString( R"__(<weightOfObject>=<buoyancy> + <volumeDisplaced> \times <%1>)__" ).arg( NUtilities::fieldNameForType( EVariableType::eWeightPerVolumeOfWaterConst ) );
    }
    if ( unsetVar->name() == "volumeDisplaced" )
    {
        return QStringList() << QString( R"__(<volumeDisplaced>=(<weightOfObject> - <buoyancy>) \times <%1>)__" ).arg( NUtilities::fieldNameForType( EVariableType::eVolumePerWeightOfWaterConst ) );
    }
    return {};
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto buoyancy = getVariable( "buoyancy" );
    auto weightOfObject = getVariable( "weightOfObject" );
    auto volumeDisplaced = getVariable( "volumeDisplaced" );

    if ( unsetVar == buoyancy )
        buoyancy->setValue( weightOfObject->value() - ( volumeDisplaced->value() * NUtilities::NConstants::weightPerVolumeOfWater( imperial(), seaWater() ) ) );
    else if ( unsetVar == weightOfObject )
        weightOfObject->setValue( buoyancy->value() + ( volumeDisplaced->value() * NUtilities::NConstants::weightPerVolumeOfWater( imperial(), seaWater() ) ) );
    else if ( unsetVar == volumeDisplaced )
        volumeDisplaced->setValue( ( weightOfObject->value() - buoyancy->value() ) / NUtilities::NConstants::weightPerVolumeOfWater( imperial(), seaWater() ) );
}
