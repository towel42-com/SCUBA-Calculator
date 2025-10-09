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
    return tr( "Quick Estimate for Temperature when Pressure Changes" );
}

QStringList CCalculator::calculatorPath() const
{
    return { tr( "Pressure, Temperature and Volume Calculations" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            std::make_shared< SVariableInfo >( "p1", tr( "Pressure 1" ), EVariableType::eVariable, EUnit::ePressure, EVariableLoc::eRHS ),   //
            std::make_shared< SVariableInfo >( "t1", tr( "Temperature 1" ), EVariableType::eVariable, EUnit::eAbsZeroTemperature, EVariableLoc::eLHS ),   //
            std::make_shared< SVariableInfo >( "pressurePerDegree", tr( "Pressure Change Per Degree" ), EVariableType::ePressurePerDegreeConst, EUnit::eNone, EVariableLoc::eRHS ),   //
        };
}

QString CCalculator::getDefaultFormula() const
{
    QString formula;
    formula = NUtilities::quickPressureChangeToDegreeFormula( "t1", "p1", "pressurePerDegree" );
    return formula;
}

QString CCalculator::computeAndGenerateFormula( bool &isBaseFormula ) const
{
    auto p1 = getVariable( "p1" );
    auto t1 = getVariable( "t1" );

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
        p1->setValue( NUtilities::quickDegreeChangeToPressure( imperial(), t1->value() ) );
        formula = NUtilities::quickDegreeChangeToPressureFormula( "t1", "p1", "pressurePerDegree" );
    }
    else if ( !t1->has_value() )
    {
        t1->setValue( NUtilities::quickPressureChangeToDegree( imperial(), p1->value() ) );
        formula = NUtilities::quickPressureChangeToDegreeFormula( "t1", "p1", "pressurePerDegree" );
    }

    return formula;
}
