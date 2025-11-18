#include "Calculator.h"
#include "CalculatorDef.h"

#include "Core/JsonCalculator.h"

CCalculatorBase *instantiateCalculator()
{
    Q_INIT_RESOURCE( calculator );

    return CJsonCalculator::create( kProjectName, kGroupName, nullptr );
}
