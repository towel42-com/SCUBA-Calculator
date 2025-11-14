#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"
#include "Core/Formula.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CCalculatorBase
{
public:
    CCalculator() { setObjectName( "OffsetNegativeBuoyancy" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual bool isWaterTypeBased() const override { return true; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual std::optional< TFormulaList > myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< TFormulaList > getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values
};

extern "C" CCalculatorBase *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Offsetting Negative Buoyancy" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Buoyancy Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables( bool * /*preReversed*/ ) const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "volumeDisplaced", tr( "Volume Displaced" ), EUnit::eVolume, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "negativeBuoyancy", tr( "Negative Buoyancy" ), EUnit::eWeight, EVariableLoc::eRHS ),   //
        };
}

std::optional< TFormulaList > CCalculator::myBaseFormulas( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return TFormulaList( { std::make_shared< CFormula >( getVariable( "volumeDisplaced" ), QString( R"__(<negativeBuoyancy> \times %1)__" ).arg( NUtilities::fieldNameForType( EConstantType::eVolumePerWeightOfWaterConst ) ) ) } );
}

std::optional< TFormulaList > CCalculator::getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const
{
    if ( unsetVar == "volumeDisplaced" )
    {
        return myBaseFormulas( imperial, seaWater );
    }
    else if ( unsetVar == "negativeBuoyancy" )
    {
        return TFormulaList( { std::make_shared< CFormula >( getVariable( unsetVar ), QString( R"__(<volumeDisplaced> \times %1)__" ).arg( NUtilities::fieldNameForType( EConstantType::eWeightPerVolumeOfWaterConst ) ) ) } );
    }
    return {};
}

void CCalculator::computeVariableValues()
{
    auto negativeBuoyancy = getVariable( "negativeBuoyancy" );
    auto volumeDisplaced = getVariable( "volumeDisplaced" );

    if ( !volumeDisplaced->has_value() && volumeDisplaced->dependenciesSatisfied() )
    {
        volumeDisplaced->setValue( negativeBuoyancy->value() / NUtilities::NConstants::weightPerVolumeOfWater( imperial(), seaWater() ) );
    }

    if ( !negativeBuoyancy->has_value() && negativeBuoyancy->dependenciesSatisfied() )
    {
        negativeBuoyancy->setValue( volumeDisplaced->value() * NUtilities::NConstants::weightPerVolumeOfWater( imperial(), seaWater() ) );
    }
}
