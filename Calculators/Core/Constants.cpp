#include "Utilities.h"
#include <QString>
#include <QObject>

namespace NUtilities
{
    namespace NConstants
    {
        const char *kPressurePerDegreeConstFieldName = "pressurePerDegree";
        const char *kWeightPerVolumeOfWaterConstFieldName = "weightPerVolumeOfWater";
        const char *kVolumePerWeightOfWaterConstFieldName = "volumePerWeightOfWater";
        const char *kIdealGasConstantFieldName = "idealGasConstant";
        const char *kFN2AtSurfaceFieldName = "FN2AtSurface";
        const char *kFO2AtSurfaceFieldName = "FO2AtSurface";
        const char *kDepthToSingleATMConstFieldName = "depthToSingleAtmosphere";
        const char *kFeetToMetersConstFieldName = "feetToMeters";
        const char *kMetersToFeetConstFieldName = "metersToFeet";
        const char *kLbsPerKgsConstFieldName = "lbsToKgs";
        const char *kKgsPerLbsConstFieldName = "kgsToLbs";
        const char *kFreshWaterToSeaWaterConstFieldName = "freshWaterToSeaWater";
        const char *kSeaWaterToFreshWaterConstFieldName = "seaWaterToFreshWater";
        const char *kPSIToBarConstFieldName = "psiToBar";
        const char *kBarToPSIConstFieldName = "barToPSI";
        const char *kAbsZeroOffsetConstFieldName = "absZeroOffset";
        const char *kPressureAtSurfaceConstFieldName = "pressureAtSurface";
        const char *kBaseMETofSCUBAConstFieldName = "baseMETOfScuba";
        const char *kFillRateAirConstFieldName = "fillRateAir";
        const char *kFillRateO2ConstFieldName = "fillRateO2";
        const char *kCubicFeetToLitersFieldName = "cubicFeetToLiters";
        const char *kLitersToCubicFeetFieldName = "litersToCubicFeet";
        const char *kSafetyStopDepthConstFieldName = "safetyStopConst";
        const char *kWaterWeightAdjustmentFieldName = "waterWeightAdjustment";

        double absZeroOffset( bool imperial )
        {
            return imperial ? 460.0 : 273.0;
        }

        double pressureChangePerDegreeChange( bool imperial )
        {
            return imperial ? 5 : 0.6;
        }

        double weightPerVolumeOfWater( bool imperial, bool seaWater )
        {
            auto retVal = imperial ? 62.4 : 1.0;
            retVal *= seaWater ? 1.0 : freshWaterToSeaWater();
            return retVal;
        }

        double volumePerWeightOfWater( bool imperial, bool seaWater )
        {
            return 1.0 / weightPerVolumeOfWater( imperial, seaWater );
        }

        // psi and atm
        double pressureAtSurface( bool imperial )
        {
            return imperial ? 14.7 : 1.0;
        }

        double pressureLossPerAltitude( bool imperial )
        {
            auto retVal = 0.1 / 1000.0;   // ata/m

            if ( imperial )
            {
                retVal *= pressureAtSurface( imperial );   // psi/m
                retVal *= metersPerFeet();   // psi/ft
            }
            return retVal;
        }

        double safetyStopDepth( bool imperial, bool seaWater )
        {
            auto retVal = imperial ? NConversions::metersToFeet( 5 ) : 5;
            if ( !seaWater )
                retVal = NConversions::depthSeawaterToFreshwater( retVal );
            return retVal;
        }

        double waterWeightAdjustment( bool imperial, bool seaWater )
        {
            double retVal = seaWater ? 3.0 : 0.0;
            if ( imperial )
                retVal = NConversions::kgsToLbs( retVal );
            return retVal;
        }

        double depthToSingleAtmosphere( bool imperial, bool seaWater )
        {
            auto retVal = imperial ? ( seaWater ? 33.0 : 34.0 ) : ( seaWater ? 10.0 : 10.3 );
            return retVal;
        }

        double idealGasConstant( bool imperial )
        {
            if ( imperial )
            {
                return 10.731577089016;
            }
            else
            {
                return 0.08206;
            }
        }

        double freshWaterToSeaWater()
        {
            return 1.03;
        }

        double seaWaterToFreshWater()
        {
            return 1 / freshWaterToSeaWater();
        }

        double metersPerFeet()
        {
            return 0.3048;
        }

        double feetPerMeters()
        {
            return 1 / metersPerFeet();
        }

        double percentN2AtSurface()
        {
            return 1.0 - percentO2AtSurface();
        }

        double percentO2AtSurface()
        {
            return 0.209;
        }

        double barToPSI()
        {
            return 14.5037738;
        }

        double psiToBar()
        {
            return 1 / barToPSI();
        }

        double lbsPerKGs()
        {
            return 2.20462;
        }

        double kgsPerLbs()
        {
            return 1 / lbsPerKGs();
        }

        double cubicFeetPerLiter()
        {
            return 1 / litersPerCubicFeet();
        }

        double litersPerCubicFeet()
        {
            return 28.3168;
        }

        // the base unit of 7.0 is
        // kcal/(kg*hour)
        // we return kcal/(kg*min) by dividing by 60
        double baseMETForScuba()
        {
            return 7.0 / 60.0;
        }

        double fillRateAir( bool imperial )
        {
            double retVal = 600;   // psi/min
            if ( !imperial )
            {
                retVal = NUtilities::NConversions::psiToBar( retVal );
            }
            return retVal;
        }

        double fillRateO2( bool imperial )
        {
            double retVal = 60;   // psi/min
            if ( !imperial )
            {
                retVal = NUtilities::NConversions::psiToBar( retVal );
            }
            return retVal;
        }

        QString pressureChangePerDegreeChange( bool imperial, bool useAbbreviations, bool tex )
        {
            return ratio( NConstants::pressureChangePerDegreeChange( imperial ), NUnitStrings::pressureUnit( imperial, useAbbreviations, tex ), NUnitStrings::tempUnit( imperial, useAbbreviations, tex ), tex );
        }

        QString lbsPerKgs( bool useAbbreviations, bool tex )
        {
            return ratio( NConstants::lbsPerKGs(), NUnitStrings::weightUnit( true, useAbbreviations, tex ), NUnitStrings::weightUnit( false, useAbbreviations, tex ), tex );
        }

        QString kgsPerLbs( bool useAbbreviations, bool tex )
        {
            return ratio( NConstants::kgsPerLbs(), NUnitStrings::weightUnit( false, useAbbreviations, tex ), NUnitStrings::weightUnit( true, useAbbreviations, tex ), tex );
        }

        QString scubaMET( bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( NConstants::baseMETForScuba() ).arg( NUnitStrings::metUnits( useAbbreviations, tex ) );
            return retVal;
        }

        QString fillRateO2( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( NConstants::fillRateO2( imperial ) ).arg( NUnitStrings::pressurePerMinuteUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString fillRateAir( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( NConstants::fillRateAir( imperial ) ).arg( NUnitStrings::pressurePerMinuteUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString weightPerVolumeOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex )
        {
            return ratio( NConstants::weightPerVolumeOfWater( imperial, seaWater ), NUnitStrings::weightUnit( imperial, useAbbreviations, tex ), NUnitStrings::volumeUnit( imperial, useAbbreviations, tex ), tex, 2 );
        }

        QString volumePerWeightOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex )
        {
            return ratio( NConstants::volumePerWeightOfWater( imperial, seaWater ), NUnitStrings::volumeUnit( imperial, useAbbreviations, tex ), NUnitStrings::weightUnit( imperial, useAbbreviations, tex ), tex, 2 );
        }

        QString idealGasConstant( bool imperial, bool useAbbreviations, bool tex )
        {
            // pv = nrt
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( doubleToString( NConstants::idealGasConstant( imperial ), 5 ) ).arg( NUnitStrings::idealGasConstantUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString percentN2AtSurface( bool imperial, bool useAbbreviations, bool tex )
        {
            return QObject::tr( R"__(%1%2)__", "percentN2AtSurface" ).arg( NConstants::percentN2AtSurface() ).arg( NUnitStrings::percentUnit( imperial, useAbbreviations, tex ) );
        }

        QString percentO2AtSurface( bool imperial, bool useAbbreviations, bool tex )
        {
            return QObject::tr( R"__(%1%2)__", "percentO2AtSurface" ).arg( NConstants::percentO2AtSurface() ).arg( NUnitStrings::percentUnit( imperial, useAbbreviations, tex ) );
        }

        QString depthToSingleAtmosphere( bool imperial, bool seaWater, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( doubleToString( NConstants::depthToSingleAtmosphere( imperial, seaWater ), 2 ) ).arg( NUnitStrings::lengthUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString feetToMeters( bool useAbbreviations, bool tex )
        {
            return ratio( NConstants::feetPerMeters(), NUnitStrings::lengthUnit( false, useAbbreviations, tex ), NUnitStrings::lengthUnit( true, useAbbreviations, tex ), tex, 3 );
        }

        QString metersToFeet( bool useAbbreviations, bool tex )
        {
            return ratio( NConstants::metersPerFeet(), NUnitStrings::lengthUnit( true, useAbbreviations, tex ), NUnitStrings::lengthUnit( false, useAbbreviations, tex ), tex, 3 );
        }

        QString freshWaterToSeaWater( bool imperial, bool useAbbreviations, bool tex )
        {
            return ratio( NConstants::freshWaterToSeaWater(), NUnitStrings::depthUnit( imperial, false, useAbbreviations, tex ), NUnitStrings::depthUnit( imperial, true, useAbbreviations, tex ), tex, 2 );
        }

        QString seaWaterToFreshWater( bool imperial, bool useAbbreviations, bool tex )
        {
            return ratio( NConstants::seaWaterToFreshWater(), NUnitStrings::depthUnit( imperial, true, useAbbreviations, tex ), NUnitStrings::depthUnit( imperial, false, useAbbreviations, tex ), tex, 2 );
        }

        QString psiToBar( bool useAbbreviations, bool tex )
        {
            return ratio( NConstants::psiToBar(), NUnitStrings::pressureUnit( true, useAbbreviations, tex ), NUnitStrings::pressureUnit( false, useAbbreviations, tex ), tex, 3 );
        }

        QString barToPSI( bool useAbbreviations, bool tex )
        {
            return ratio( NConstants::barToPSI(), NUnitStrings::pressureUnit( false, useAbbreviations, tex ), NUnitStrings::pressureUnit( true, useAbbreviations, tex ), tex, 3 );
        }

        QString absZeroOffset( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( doubleToString( NConstants::absZeroOffset( imperial ), 0 ) ).arg( NUnitStrings::tempUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString pressureAtSurface( bool imperial, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( doubleToString( NConstants::pressureAtSurface( imperial ), 1 ) ).arg( NUnitStrings::atmosphereUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString pressureLossPerAltitude( bool imperial, bool useAbbreviations, bool tex )
        {
            return ratio( NConstants::pressureLossPerAltitude( imperial ), NUnitStrings::atmosphereUnit( imperial, useAbbreviations, tex ), NUnitStrings::lengthUnit( imperial, useAbbreviations, tex ), tex, 4 );
        }

        QString safetyStop( bool imperial, bool seaWater, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( doubleToString( NConstants::safetyStopDepth( imperial, seaWater ), 1 ) ).arg( NUnitStrings::depthUnit( imperial, seaWater, useAbbreviations, tex ) );
            return retVal;
        }

        QString waterWeightAdjustment( bool imperial, bool seaWater, bool useAbbreviations, bool tex )
        {
            QString retVal = tex ? "%1%2" : "%1 (%2)";
            retVal = retVal.arg( doubleToString( NConstants::waterWeightAdjustment( imperial, seaWater ), 1 ) ).arg( NUnitStrings::weightUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString cubicFeetToLiters( bool useAbbreviations, bool tex )
        {
            return ratio( NConstants::cubicFeetPerLiter(), NUnitStrings::volumeUnit( true, useAbbreviations, tex ), NUnitStrings::lengthUnit( false, useAbbreviations, tex ), tex, 4 );
        }

        QString litersToCubicFeet( bool useAbbreviations, bool tex )
        {
            return ratio( NConstants::litersPerCubicFeet(), NUnitStrings::volumeUnit( false, useAbbreviations, tex ), NUnitStrings::lengthUnit( true, useAbbreviations, tex ), tex, 4 );
        }

        QString constantString( bool imperial, bool seaWater, EVariableType constantType )
        {
            switch ( constantType )
            {
                case EVariableType::eIntermediate:
                case EVariableType::eVariable:
                    Q_ASSERT( isConstantVariable( constantType ) );
                    break;
                case EVariableType::ePressurePerDegreeConst:
                    return NUtilities::NConstants::pressureChangePerDegreeChange( imperial, true, true );
                case EVariableType::eWeightPerVolumeOfWaterConst:
                    return NUtilities::NConstants::weightPerVolumeOfWater( imperial, seaWater, true, true );
                case EVariableType::eVolumePerWeightOfWaterConst:
                    return NUtilities::NConstants::volumePerWeightOfWater( imperial, seaWater, true, true );
                case EVariableType::eIdealGasConst:
                    return NUtilities::NConstants::idealGasConstant( imperial, true, true );
                case EVariableType::eFN2AtSurfaceConst:
                    return NUtilities::NConstants::percentN2AtSurface( imperial, true, true );
                case EVariableType::eFO2AtSurfaceConst:
                    return NUtilities::NConstants::percentO2AtSurface( imperial, true, true );
                case EVariableType::eDepthToSingleATMConst:
                    return NUtilities::NConstants::depthToSingleAtmosphere( imperial, seaWater, true, true );
                case EVariableType::eFeetToMetersConst:
                    return NUtilities::NConstants::feetToMeters( true, true );
                case EVariableType::eMetersToFeetConst:
                    return NUtilities::NConstants::metersToFeet( true, true );
                case EVariableType::eLbsPerKgsConst:
                    return NUtilities::NConstants::lbsPerKgs( true, true );
                case EVariableType::eKgsPerLbsConst:
                    return NUtilities::NConstants::kgsPerLbs( true, true );
                case EVariableType::eFreshWaterToSeaWaterConst:
                    return NUtilities::NConstants::freshWaterToSeaWater( imperial, true, true );
                case EVariableType::ePSIToBarConst:
                    return NUtilities::NConstants::psiToBar( true, true );
                case EVariableType::eBarToPSIConst:
                    return NUtilities::NConstants::barToPSI( true, true );
                case EVariableType::eAbsZeroOffsetConst:
                    return NUtilities::NConstants::absZeroOffset( imperial, true, true );
                case EVariableType::ePressureAtSurfaceConst:
                    return NUtilities::NConstants::pressureAtSurface( imperial, true, true );
                case EVariableType::eSafetyStopDepthConst:
                    return NUtilities::NConstants::safetyStop( imperial, seaWater, true, true );
                case EVariableType::eWaterWeightAdjustmentConst:
                    return NUtilities::NConstants::waterWeightAdjustment( imperial, seaWater, true, true );
                case EVariableType::eBaseMETofSCUBAConst:
                    return NUtilities::NConstants::scubaMET( true, true );
                case EVariableType::eFillRateO2Const:
                    return NUtilities::NConstants::fillRateO2( imperial, true, true );
                case EVariableType::eFillRateAirConst:
                    return NUtilities::NConstants::fillRateAir( imperial, true, true );
                case EVariableType::eCubicFeetToLitersConst:
                    return NUtilities::NConstants::cubicFeetToLiters( true, true );
                case EVariableType::eLitersToCubicFeetConst:
                    return NUtilities::NConstants::litersToCubicFeet( true, true );
            }
            return {};
        }

        void foreachConstantType( const std::function< void( EVariableType ) > &onConstType )
        {
            for ( auto &&currConstType : { //
                                           EVariableType::ePressurePerDegreeConst,   //
                                           EVariableType::eWeightPerVolumeOfWaterConst,   //
                                           EVariableType::eVolumePerWeightOfWaterConst,   //
                                           EVariableType::eIdealGasConst,   //
                                           EVariableType::eFN2AtSurfaceConst,
                                           EVariableType::eFO2AtSurfaceConst,
                                           EVariableType::eDepthToSingleATMConst,
                                           EVariableType::eFeetToMetersConst,
                                           EVariableType::eMetersToFeetConst,
                                           EVariableType::eLbsPerKgsConst,
                                           EVariableType::eKgsPerLbsConst,
                                           EVariableType::eFreshWaterToSeaWaterConst,
                                           EVariableType::eSeaWaterToFreshWaterConst,
                                           EVariableType::ePSIToBarConst,
                                           EVariableType::eBarToPSIConst,
                                           EVariableType::eAbsZeroOffsetConst,
                                           EVariableType::ePressureAtSurfaceConst,
                                           EVariableType::eBaseMETofSCUBAConst,
                                           EVariableType::eFillRateO2Const,
                                           EVariableType::eFillRateAirConst,
                                           EVariableType::eCubicFeetToLitersConst,
                                           EVariableType::eLitersToCubicFeetConst,
                                           EVariableType::eSafetyStopDepthConst,
                                           EVariableType::eWaterWeightAdjustmentConst } )
            {
                onConstType( currConstType );
            }
        }
    }
}
