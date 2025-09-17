#include "Calculator.h"
#include "Page.h"

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() {}
    virtual ~CCalculator() override {}

    QString calculatorName() const override;
    QStringList calculatorPath() const override;

    CSCUBACalculatorPage *constructPage( QWidget *parent ) const override;
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::calculatorName() const
{
    return "Offsetting Negative Buoyancy";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Buoyancy Calculations" };
}


CSCUBACalculatorPage *CCalculator::constructPage( QWidget *parent ) const
{
    return new CPage( parent );
}
