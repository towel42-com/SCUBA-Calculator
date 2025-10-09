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

    virtual std::list< std::shared_ptr< SVariableInfo > > getMyVariables() const override;
    virtual QString getDefaultFormula() const override;
    virtual QString computeAndGenerateFormula( bool & isBaseFormula ) const override;
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return tr( "Ideal Gas Law" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Pressure, Temperature and Volume Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< SVariableInfo >( "p", tr( "Pressure" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "v", tr( "Volume" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "numMoles", tr( "Number of Moles" ), EVariableType::eVariable, EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "t", tr( "Temperature" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "idealGasConstant", tr( "Ideal Gas Constant" ), EVariableType::eIdealGasConstant, EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "absOffset", tr( "Absolute Temperature Offset" ), EVariableType::eAbsZeroOffsetConstant, EUnit::eNone, EVariableLoc::eRHS ),   //
        };
}

QString CCalculator::getDefaultFormula() const
{
    return R"__(<p> \times <v> = <numMoles> \times <idealGasConstant> \times <t>)__";
}

QString CCalculator::computeAndGenerateFormula( bool &isBaseFormula ) const
{
    auto p = getVariable( "p" );
    auto v = getVariable( "v" );
    auto numMoles = getVariable( "numMoles" );
    auto t = getVariable( "t" );

    QString formula;
    bool aOK = numUnsetVariables() == 1;
    isBaseFormula = false;
    if ( !aOK )
    {
        formula = getDefaultFormula();
        isBaseFormula = true;
    }
    else if ( !p->has_value() )
    {
        // p = nrt/v
        p->setValue( numMoles->value() * NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::toAbsZeroBasedTemp( imperial(), t->value() ) / v->value() );
        formula = tr( R"__(<p> = \frac{<numMoles> \times <idealGasConstant> \times (<t> + <absOffset>)}{<v>})__" );
    }
    else if ( !v->has_value() )
    {
        // v = nrt/p
        v->setValue( numMoles->value() * NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::toAbsZeroBasedTemp( imperial(), t->value() ) / p->value() );
        formula = tr( R"__(<v> = \frac{<numMoles> \times <idealGasConstant> \times (<t> + <absOffset>)}{<p>})__" );
    }
    else if ( !numMoles->has_value() )
    {
        // n = pv/rt
        numMoles->setValue( ( p->value() * v->value() ) / ( NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::toAbsZeroBasedTemp( imperial(), t->value() ) ) );
        formula = tr( R"__(<numMoles> = \frac{<p> \times <v>}{<idealGasConstant> \times (<t> + <absOffset>)})__" );
    }
    else if ( !t->has_value() )
    {
        // t = pv/nr
        t->setValue( NUtilities::fromAbsZeroBasedTemp( imperial(), ( p->value() * v->value() ) / ( NUtilities::NConstants::idealGasConstant( imperial() ) * numMoles->value() ) ) );
        formula = tr( R"__(<t> = (\frac{<p> \times <v>}{<idealGasConstant> \times <numMoles>}) - <absOffset>)__" );
    }

    return formula;
}
