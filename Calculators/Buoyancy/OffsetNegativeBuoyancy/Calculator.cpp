#include "Calculator.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "OffsetNegativeBuoyancy" ); }
    virtual ~CCalculator() override {}

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual bool isWaterTypeBased() const override { return true; }

    virtual TVariableInfoList getMyVariables() const override;

    virtual std::optional< QString > myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo & unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
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

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< CVariableInfo >( "volumeDisplaced", tr( "Volume Displaced" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "negativeBuoyancy", tr( "Negative Buoyancy" ), EVariableType::eVariable, EUnit::eWeight, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::eWeightOfWaterConst )   //
        };
}

std::optional< QString > CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    return R"__(<volumeDisplaced>=\frac{<negativeBuoyancy>}{<weightOfWater>})__";
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool /*imperial*/, bool /*seaWater*/ ) const
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
