#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/FormulaString.h"

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

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual std::optional< TFormulaStringList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaStringList > getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
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

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "buoyancy", tr( "Buoyancy" ), EVariableType::eVariable, EUnit::eWeight, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "weightOfObject", tr( "Weight" ), EVariableType::eVariable, EUnit::eWeight, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "volumeDisplaced", tr( "Volume" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eRHS ),   //
        };
}

std::optional< TFormulaStringList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( "buoyancy" ), QString( R"__(<weightOfObject> - [<volumeDisplaced> \times %1])__" ).arg( NUtilities::fieldNameForType( EVariableType::eWeightPerVolumeOfWaterConst ) ) ) } );
}

std::optional< TFormulaStringList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "buoyancy" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "weightOfObject" )
    {
        return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( unsetVar ), QString( R"__(<buoyancy> + <volumeDisplaced> \times %1)__" ).arg( NUtilities::fieldNameForType( EVariableType::eWeightPerVolumeOfWaterConst ) ) ) } );
    }
    else if ( unsetVar == "volumeDisplaced" )
    {
        return TFormulaStringList( { std::make_shared< CFormulaString >( getVariable( unsetVar ), QString( R"__((<weightOfObject> - <buoyancy>) \times %1)__" ).arg( NUtilities::fieldNameForType( EVariableType::eVolumePerWeightOfWaterConst ) ) ) } );
    }
    return {};
}

void CCalculator::computeVariableValues()
{
    auto buoyancy = getVariable( "buoyancy" );
    auto weightOfObject = getVariable( "weightOfObject" );
    auto volumeDisplaced = getVariable( "volumeDisplaced" );

    if ( !buoyancy->has_value() && weightOfObject->has_value() && volumeDisplaced->has_value() )
    {
        buoyancy->setValue( weightOfObject->value() - ( volumeDisplaced->value() * NUtilities::NConstants::weightPerVolumeOfWater( imperial(), seaWater() ) ) );
    }

    if ( buoyancy->has_value() && !weightOfObject->has_value() && volumeDisplaced->has_value() )
    {
        weightOfObject->setValue( buoyancy->value() + ( volumeDisplaced->value() * NUtilities::NConstants::weightPerVolumeOfWater( imperial(), seaWater() ) ) );
    }

    if ( !buoyancy->has_value() && weightOfObject->has_value() && !volumeDisplaced->has_value() )
    {
        volumeDisplaced->setValue( ( weightOfObject->value() - buoyancy->value() ) / NUtilities::NConstants::weightPerVolumeOfWater( imperial(), seaWater() ) );
    }
}
