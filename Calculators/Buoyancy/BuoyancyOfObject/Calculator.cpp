#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/Formula.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "BuoyancyOfObject" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual bool showWaterType() const override { return true; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual TOptionalFormulaList myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual TOptionalFormulaList getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
};

extern "C" CCalculatorBase *instantiateCalculator()
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
            CVariableInfo::create( "buoyancy", tr( "Buoyancy" ), EUnit::eWeight, EVariableLoc::eLHS ),   //
            CVariableInfo::create( "weightOfObject", tr( "Weight" ), EUnit::eWeight, EVariableLoc::eRHS ),   //
            CVariableInfo::create( "volumeDisplaced", tr( "Volume" ), EUnit::eVolume, EVariableLoc::eRHS ),   //
        };
}

TOptionalFormulaList CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { std::make_shared< CFormula >( getVariable( "buoyancy" ), QString( R"__(<weightOfObject> - [<volumeDisplaced> \times %1])__" ).arg( NUtilities::fieldNameForType( EConstantType::eWeightPerVolumeOfWaterConst ) ) ) } );
}

TOptionalFormulaList CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "buoyancy" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "weightOfObject" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(<buoyancy> + <volumeDisplaced> \times %1)__" ).arg( NUtilities::fieldNameForType( EConstantType::eWeightPerVolumeOfWaterConst ) ) ) } );
    }
    else if ( unsetVar == "volumeDisplaced" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__((<weightOfObject> - <buoyancy>) \times %1)__" ).arg( NUtilities::fieldNameForType( EConstantType::eVolumePerWeightOfWaterConst ) ) ) } );
    }
    return {};
}

void CCalculator::computeVariableValues()
{
    auto buoyancy = getVariable( "buoyancy" );
    auto weightOfObject = getVariable( "weightOfObject" );
    auto volumeDisplaced = getVariable( "volumeDisplaced" );

    if ( !buoyancy->has_value() && buoyancy->dependenciesSatisfied() )
    {
        buoyancy->setValue( weightOfObject->value() - ( volumeDisplaced->value() * NUtilities::NConstants::weightPerVolumeOfWater( imperial(), seaWater() ) ) );
    }

    if ( !weightOfObject->has_value() && weightOfObject->dependenciesSatisfied() )
    {
        weightOfObject->setValue( buoyancy->value() + ( volumeDisplaced->value() * NUtilities::NConstants::weightPerVolumeOfWater( imperial(), seaWater() ) ) );
    }

    if ( !volumeDisplaced->has_value() && volumeDisplaced->dependenciesSatisfied() )
    {
        volumeDisplaced->setValue( ( weightOfObject->value() - buoyancy->value() ) / NUtilities::NConstants::weightPerVolumeOfWater( imperial(), seaWater() ) );
    }
}
