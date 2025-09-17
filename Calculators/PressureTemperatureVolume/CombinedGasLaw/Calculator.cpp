#include "VolumePressureChangeInPressureVolume.h"

class CALCULATORS_EXPORT CVolumePressureChangeInPressureVolume : public CSCUBACalculator
{
public:
    CVolumePressureChangeInPressureVolume() {}
    virtual ~CVolumePressureChangeInPressureVolume() override {}

    QString calculatorName() const override;
    QStringList calculatorPath() const override;

    CSCUBACalculatorPage *getPage( QWidget * /*parent*/ ) const override { return nullptr; };
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CVolumePressureChangeInPressureVolume;
}

QString CVolumePressureChangeInPressureVolume::calculatorName() const
{
    return "Calculating Volume/Pressure Following a Change in Pressure/Volume";
}

QStringList CVolumePressureChangeInPressureVolume::calculatorPath() const
{
    return { "Pressure and Volume Calculations" };
}

