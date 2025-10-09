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
    virtual QString computeAndGenerateFormula( bool &isBaseFormula ) const override;
    virtual TVariableInfo customDetermineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior );
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Calculating Pressure Following a Change in Temperature";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure, Temperature and Volume Calculations" };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< SVariableInfo >( "p1", tr( "Pressure 1" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "t1", tr( "Temperature 1" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "p2", tr( "Pressure 2" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "t2", tr( "Temperature 2" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "absOffset", tr( "Absolute Temperature Offset" ), EVariableType::eAbsZeroOffsetConstant, EUnit::eNone, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "pressureOffset", tr( "Pressure Offset" ), EVariableType::ePressureOffsetConstant, EUnit::eNone, EVariableLoc::eRHS ),   //
        };
}

QString CCalculator::getDefaultFormula() const
{
    return tr( R"__(<p2> = [<t2> \times \frac{(<p1> + <pressureOffset>)}{<t1>}] - <pressureOffset>)__" );
}

TVariableInfo CCalculator::customDetermineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior )
{
    TVariableInfo retVal;

    if ( updateFromSide == EVariableLoc::eLHS )
    {
        if ( getVariable( "p1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "p2" );
        else if ( getVariable( "t1" )->isWidget( triggerWidget ) )
            retVal = getVariable( "t2" );
    }
    else if ( updateFromSide == EVariableLoc::eRHS )
    {
        if ( getVariable( "p2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "p1" );
        else if ( getVariable( "t2" )->isWidget( triggerWidget ) )
            retVal = getVariable( "t1" );
    }
    return CSCUBACalculator::customDetermineVariableToUnset( updateFromSide, triggerWidget, preDefaultBehavior );
}

QString CCalculator::computeAndGenerateFormula( bool &isBaseFormula ) const
{
    auto p1 = getVariable( "p1" );
    auto t1 = getVariable( "t1" );

    auto p2 = getVariable( "p2" );
    auto t2 = getVariable( "t2" );

    QString formula;
    bool aOK = numUnsetVariables() == 1;
    isBaseFormula = false;
    if ( !aOK )
    {
        formula = getDefaultFormula();
        isBaseFormula = true;
    }
    else if ( !p1->has_value() )
    {
        p1->setValue( ( NUtilities::toAbsZeroBasedTemp( imperial(), t1->value() ) * ( ( p2->value() + NUtilities::NConstants::pressureOffset( imperial() ) ) / NUtilities::toAbsZeroBasedTemp( imperial(), t2->value() ) ) ) - NUtilities::NConstants::pressureOffset( imperial() ) );
        formula = tr( R"__(<p1> = [(<t1> + <absOffset>) \times \frac{(<p2> + <pressureOffset>)}{(<t2> + <absOffset>}] - <pressureOffset>)__" );
    }
    else if ( !p2->has_value() )
    {
        p2->setValue( ( NUtilities::toAbsZeroBasedTemp( imperial(), t2->value() ) * ( ( p1->value() + NUtilities::NConstants::pressureOffset( imperial() ) ) / NUtilities::toAbsZeroBasedTemp( imperial(), t1->value() ) ) ) - NUtilities::NConstants::pressureOffset( imperial() ) );
        formula = tr( R"__(<p2> = [(<t2> + <absOffset>) \times \frac{(<p1> + <pressureOffset>)}{(<t1> + <absOffset>}] - <pressureOffset>)__" );
    }
    else if ( !t1->has_value() )
    {
        formula = tr( R"__(<t1> = [\frac{(<p1> + <pressureOffset>) \times (<t2> + <absOffset>)}{<p2> + <pressureOffset>}] - <absOffset>)__" );
        t1->setValue( NUtilities::fromAbsZeroBasedTemp( imperial(), ( ( p1->value() + NUtilities::NConstants::pressureOffset( imperial() ) ) * NUtilities::toAbsZeroBasedTemp( imperial(), t2->value() ) ) / ( p2->value() + NUtilities::NConstants::pressureOffset( imperial() ) ) ) );
    }
    else if ( !t2->has_value() )
    {
        formula = tr( R"__(<t2> = [\frac{(<p2> + <pressureOffset>) \times (<t1> + <absOffset>)}{<p1> + <pressureOffset>}] - <absOffset>)__" );
        t2->setValue( NUtilities::fromAbsZeroBasedTemp( imperial(), ( ( p2->value() + NUtilities::NConstants::pressureOffset( imperial() ) ) * NUtilities::toAbsZeroBasedTemp( imperial(), t1->value() ) ) / ( p1->value() + NUtilities::NConstants::pressureOffset( imperial() ) ) ) );
    }

    return formula;
}
