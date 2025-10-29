#include "Calculator.h"
#include "CalculatorDef.h"
#include "Core/VariableInfo.h"
#include "Core/Utilities.h"

#include <memory>

class CALCULATORS_EXPORT CCalculator : public CSCUBACalculator
{
public:
    CCalculator() { setObjectName( "DiveWeightCalculator" ); }
    virtual ~CCalculator() override {}

    virtual bool isWaterTypeBased() const override { return true; }

    virtual QString myCalculatorName() const override;
    virtual QStringList myCalculatorPath() const override;

    virtual QString calculatorProjectName() const override { return kProjectName; }
    virtual QString calculatorGroupName() const override { return kGroupName; }

    virtual TVariableInfoList getMyVariables() const override;

    virtual std::optional< QString > myBaseFormula( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual std::optional< QString > getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeValueForVar( TVariableInfo &unsetVar ) override;   // updates all values
};

extern "C" CSCUBACalculator *instantiateCalculator()
{
    return new CCalculator;
}

QString CCalculator::myCalculatorName() const
{
    return tr( "Dive Weight Calculator" );
}

QStringList CCalculator::myCalculatorPath() const
{
    return { tr( "Miscellaneous" ) };
}

TVariableInfoList CCalculator::getMyVariables() const
{
    auto retVal =   //
        TVariableInfoList( {
            std::make_shared< CVariableInfo >( "lead", tr( "Lead Needed" ), EVariableType::eVariable, EUnit::eWeight, EVariableLoc::eLHS ),   //
            std::make_shared< CVariableInfo >( "yourWeight", tr( "Your Weight" ), EVariableType::eVariable, EUnit::eWeight, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >(
                "experience", tr( "Diving Experience Adjustment" ), EUnit::eWeight, EVariableLoc::eRHS,   //
                SBaseInfo< TNamedValueItemList >(
                    false, {},
                    TNamedValueItemList( {
                        { tr( "Open Water (0-20 dives)" ), 2.0 },   //
                        { tr( "Advanced Adventure (25-100 dives)" ), 1.0 },
                        { tr( "Advanced (100-150 dives)" ), -1.0 },
                        { tr( "Master Diver/Instructor (150+ dives)" ), -2.0 }   //
                    } ) ) ),   //
            std::make_shared< CVariableInfo >(
                "exposureSuit", tr( "Exposure Suit Adjustment" ), EUnit::eWeight, EVariableLoc::eRHS,
                SBaseInfo< TNamedValueItemList >(
                    false, {},
                    TNamedValueItemList( {
                        { tr( "None (Swimsuit/Dive Skin)" ), 0.0 },   //
                        { tr( "3mm (Tropical Waters)" ), 2.5 },
                        { tr( "5mm (Temperate Waters)" ), 4.5 },
                        { tr( "7mm (Cool Waters)" ), 7.0 },   //
                        { tr( "Tri-Lam Dry Suit" ), 10.0 },   //
                        { tr( "Neoprene Dry Suit" ), 12.0 },   //
                    } ) ) ),   //
            std::make_shared< CVariableInfo >(
                "tankMaterial", tr( "Tank Material Adjustment" ), EUnit::eWeight, EVariableLoc::eRHS,
                SBaseInfo< TNamedValueItemList >(
                    false, {},
                    TNamedValueItemList( {
                        { tr( "Aluminum" ), 1.5 },
                        { tr( "Steel" ), -2.0 },   //
                        { tr( "Carbon Fiber" ), -1.0 },
                    } ) ) ),   //
            std::make_shared< CVariableInfo >(
                "additionalEquipment", tr( "Additional Equipment Adjustment" ), EUnit::eWeight, EVariableLoc::eRHS,
                SBaseInfo< TNamedValueItemList >(
                    false, {},
                    TNamedValueItemList( {
                        { tr( "Light (Camera, Small Accessories)" ), 0.0 },   //
                        { tr( "Medium (Camera, Lights, SMB)" ), 1.0 },
                        { tr( "Heavy (Camera System, Multiple Accessories)" ), 2.0 },
                    } ) ) ),   //
            std::make_shared< CVariableInfo >( "baseLeadWeight", tr( "Base Lead Weight" ), EVariableType::eIntermediate, EUnit::eWeight, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( "adjustments", tr( "Total Adjustments" ), EVariableType::eIntermediate, EUnit::eWeight, EVariableLoc::eRHS ),   //
            std::make_shared< CVariableInfo >( EVariableType::eWaterWeightAdjustmentConst ),   //
        } );

    auto pos = std::next( std::next( retVal.begin() ) );
    ( *pos )->addValues( SBaseInfo< TNamedValueItemList >(
        true, {},
        TNamedValueItemList( {
            { "Open Water (0-20 dives)", NUtilities::NConversions::kgsToLbs( 2.0 ) },   //
            { "Advanced Adventure (25-100 dives)", NUtilities::NConversions::kgsToLbs( 1.0 ) },
            { "Advanced (100-150 dives)", NUtilities::NConversions::kgsToLbs( -1.0 ) },
            { "Master Diver/Instructor (150+ dives)", NUtilities::NConversions::kgsToLbs( -2.0 ) }   //
        } ) ) );

    pos++;
    ( *pos )->addValues( SBaseInfo< TNamedValueItemList >(
        true, {},
        TNamedValueItemList( {
            { "None (Swimsuit/Dive Skin)", NUtilities::NConversions::kgsToLbs( 0.0 ) },   //
            { "3mm (Tropical Waters)", NUtilities::NConversions::kgsToLbs( 2.5 ) },
            { "5mm (Temperate Waters)", NUtilities::NConversions::kgsToLbs( 4.5 ) },
            { "7mm (Cool Waters)", NUtilities::NConversions::kgsToLbs( 7.0 ) },   //
            { "Tri-Lam Dry Suit ()", NUtilities::NConversions::kgsToLbs( 10.0 ) },   //
            { "Neoprene Dry Suit ()", NUtilities::NConversions::kgsToLbs( 12.0 ) },   //
        } ) ) );

    pos++;
    ( *pos )->addValues( SBaseInfo< TNamedValueItemList >(
        true, {},
        TNamedValueItemList( {
            { "Aluminum", NUtilities::NConversions::kgsToLbs( 1.5 ) },
            { "Steel", NUtilities::NConversions::kgsToLbs( -2.0 ) },   //
            { "Carbon Fiber", NUtilities::NConversions::kgsToLbs( -1.0 ) },
        } ) ) );

    pos++;
    ( *pos )->addValues( SBaseInfo< TNamedValueItemList >(
        true, {},
        TNamedValueItemList( {
            { "Light (Camera, Small Accessories)", NUtilities::NConversions::kgsToLbs( 0.0 ) },   //
            { "Medium (Camera, Lights, SMB)", NUtilities::NConversions::kgsToLbs( 1.0 ) },
            { "Heavy (Camera System, Multiple Accessories)", NUtilities::NConversions::kgsToLbs( 2.0 ) },
        } ) ) );

    return retVal;
}
/*'
* https://swimmingcalculators.com/scuba-diving-weight-calculator/

function calculateWeight() {
    const weight = parseFloat(weightInput.value);

    if (isNaN(weight) || weight < 30 || weight > 200) {
        alert('Please enter a valid weight between 30 and 200 kg');
        return;
    }

    // Base calculation: 10% of body weight
    let baseWeight = weight * 0.1;

    // Experience adjustment
    const experience = document.getElementById('experience').value;
    let experienceAdjustment = 0;

    switch (experience) {
    case 'beginner':
        experienceAdjustment = 2;
        break;
    case 'intermediate':
        experienceAdjustment = 1;
        break;
    case 'advanced':
        experienceAdjustment = -1;
        break;
    case 'instructor':
        experienceAdjustment = -2;
        break;
    }

    // Water type adjustment
    const waterType = document.querySelector('input[name="waterType"]:checked').value;
    const waterAdjustment = waterType === 'saltwater' ? 3 : 0;

    // Wetsuit adjustment
    const wetsuit = document.getElementById('wetsuit').value;
    let wetsuitAdjustment = 0;

    switch (wetsuit) {
    case 'none':
        wetsuitAdjustment = 0;
        break;
    case '3mm':
        wetsuitAdjustment = 2.5;
        break;
    case '5mm':
        wetsuitAdjustment = 4.5;
        break;
    case '7mm':
        wetsuitAdjustment = 7;
        break;
    case 'drysuit':
        wetsuitAdjustment = 10;
        break;
    }

    // Tank adjustment
    const tank = document.getElementById('tank').value;
    let tankAdjustment = 0;

    switch (tank) {
    case 'aluminum':
        tankAdjustment = 1.5;
        break;
    case 'steel':
        tankAdjustment = -2;
        break;
    case 'carbon':
        tankAdjustment = -1;
        break;
    }

    // Equipment adjustment
    const equipment = document.getElementById('equipment').value;
    let equipmentAdjustment = 0;

    switch (equipment) {
    case 'none':
        equipmentAdjustment = 0;
        break;
    case 'light':
        equipmentAdjustment = 1;
        break;
    case 'medium':
        equipmentAdjustment = 2;
        break;
    case 'heavy':
        equipmentAdjustment = 3.5;
        break;
    }

    // Total weight
    const totalWeight = baseWeight + experienceAdjustment + waterAdjustment + wetsuitAdjustment + tankAdjustment + equipmentAdjustment;

    // Update display
    resultValue.textContent = totalWeight.toFixed(1);
    poundsDisplay.textContent = (totalWeight * 2.20462).toFixed(1);

    // Update chart
    weightChart.data.datasets[0].data = [baseWeight, experienceAdjustment, waterAdjustment, wetsuitAdjustment, tankAdjustment, equipmentAdjustment];

    weightChart.update();
}
*/
std::optional< QString > CCalculator::myBaseFormula( bool /*imperial*/, bool /*seaWater*/ ) const
{
    QStringList formulas;

    formulas << R"__(<baseLeadWeight> = <yourWeight> \times 0.1)__";
    formulas << QString( R"__(<adjustments> = <%1> + <experience> + <exposureSuit> + <tankMaterial> + <additionalEquipment>)__" ).arg( NUtilities::fieldNameForType( EVariableType::eWaterWeightAdjustmentConst ) );
    formulas << R"__(<lead> = <baseLeadWeight> + <adjustments>)__";

    return NUtilities::joinFormulas( formulas );
}

std::optional< QString > CCalculator::getFormulaForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const
{
    QStringList formulas;
    if ( unsetVar->name() == "lead" )
        return myBaseFormula( imperial, seaWater );
    else if ( unsetVar->name() == "yourWeight" )
    {
        formulas << QString( R"__(<adjustments> = <%1> + <experience> + <exposureSuit> + <tankMaterial> + <additionalEquipment>)__" ).arg( NUtilities::fieldNameForType( EVariableType::eWaterWeightAdjustmentConst ) );
        formulas << R"__(<baseLeadWeight> = <lead> - <adjustments>)__";
        formulas << R"__(<yourWeight> = \frac{<baseLeadWeight>}{0.1})__";
    }

    return NUtilities::joinFormulas( formulas );
}

void CCalculator::computeValueForVar( TVariableInfo &unsetVar )
{
    auto lead = getVariable( "lead" );
    auto yourWeight = getVariable( "yourWeight" );
    auto experience = getVariable( "experience" );
    auto exposureSuit = getVariable( "exposureSuit" );
    auto tankMaterial = getVariable( "tankMaterial" );
    auto additionalEquipment = getVariable( "additionalEquipment" );
    auto adjustments = getVariable( "adjustments" );
    auto baseLeadWeight = getVariable( "baseLeadWeight" );

    auto waterAdjustment = NUtilities::NConstants::waterWeightAdjustment( imperial(), seaWater() );

    if ( ( unsetVar == lead ) || ( unsetVar == yourWeight ) )
    {
        auto experienceAdjustment = experience->value();
        auto exposureSuitAdjustment = exposureSuit->value();
        auto tankMaterialAdjustment = tankMaterial->value();
        auto additionalEquipmentAdjustment = additionalEquipment->value();

        auto adjustmentsValue = experienceAdjustment + waterAdjustment + exposureSuitAdjustment + tankMaterialAdjustment + additionalEquipmentAdjustment;
        adjustments->setValue( adjustmentsValue );

        if ( unsetVar == lead )
        {
            auto baseWeight = yourWeight->value() * 0.1;
            baseLeadWeight->setValue( baseWeight );
            lead->setValue( baseWeight + adjustmentsValue );
        }
        else
        {
            baseLeadWeight->setValue( lead->value() - adjustments->value() );
            yourWeight->setValue( baseLeadWeight->value() / 0.1 );
        }
    }
}