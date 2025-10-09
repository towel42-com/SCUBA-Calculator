#include "Calculator.h"
#include "VariableInfo.h"

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
    return "Ideal Gas Law";
}

QStringList CCalculator::calculatorPath() const
{
    return { "Pressure, Temperature and Volume Calculations" };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    return   //
        {
            //std::make_shared< SVariableInfo >( "volumeDisplaced", tr( "Volume Displaced" ), EVariableType::eVariable, EUnit::eVolume, EVariableLoc::eLHS ),   //
        };
}

QString CCalculator::getDefaultFormula() const
{
    return {};
}

QString CCalculator::computeAndGenerateFormula( bool & isBaseFormula ) const
{
    isBaseFormula = true;
    return {};
    //if ( !NUtilities::valuesValid( values ) )
    //    return {};

    //auto p = values[ 0 ];
    //auto v = values[ 1 ];
    //auto numMoles = values[ 2 ];
    //auto t = values[ 3 ];

    //// p*v = numMoles * R * t
    //if ( !p.has_value() )
    //{
    //    p = numMoles.value() * NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::toAbsZeroBasedTemp( imperial(), t.value() ) / v.value();
    //}
    //else if ( !v.has_value() )
    //{
    //    v = numMoles.value() * NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::toAbsZeroBasedTemp( imperial(), t.value() ) / p.value();
    //}
    //else if ( !numMoles.has_value() )
    //{
    //    numMoles = ( p.value() * v.value() ) / ( NUtilities::NConstants::idealGasConstant( imperial() ) * NUtilities::toAbsZeroBasedTemp( imperial(), t.value() ) );
    //}
    //else if ( !t.has_value() )
    //{
    //    t = NUtilities::fromAbsZeroBasedTemp( imperial(), ( p.value() * v.value() ) / ( NUtilities::NConstants::idealGasConstant( imperial() ) * numMoles.value() ) );
    //}
    //return TOptionalDoubleVector( { p, v, numMoles, t } );
}

