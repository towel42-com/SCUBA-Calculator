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
        const char *kPressureLossPerAltitudeGainConstFieldName = "pressureLossPerAltitudeGain";
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

        double pressureLossPerAltitudeGain( bool imperial )
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

        double singleATMPerDepth( bool imperial, bool seaWater )
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

        double psiPerBAR()
        {
            return 14.5037738;
        }

        double barPerPSI()
        {
            return 1 / psiPerBAR();
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

        QString ratioConstant(
            bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description,   //
            const std::function< double() > &constantFunc,   //
            const std::function< QString() > &descFunction,   //
            const std::function< QString( bool imperial, bool seaWater, bool useAbbreviations, bool tex ) > &numeratorUnitFunc,   //
            const std::function< QString( bool imperial, bool seaWater, bool useAbbreviations, bool tex ) > &denominatorUnitFunc,   //
            std::optional< int > numDecimal )
        {
            auto numUnit = numeratorUnitFunc( imperial, seaWater, useAbbreviations, tex );
            auto denUnit = denominatorUnitFunc( imperial, seaWater, useAbbreviations, tex );
            if ( description )
            {
                auto desc = descFunction();
                auto descUnits = ( tex ? QObject::tr( R"__(\frac{%1}{%2})__" ) : QObject::tr( "%1/%2" ) ).arg( numeratorUnitFunc( imperial, seaWater, false, tex ) ).arg( denominatorUnitFunc( imperial, seaWater, false, tex ) );
                return ratio( desc, numUnit, denUnit, tex );
            }
            else
            {
                auto value = constantFunc();
                return ratio( value, numUnit, denUnit, tex, numDecimal );
            }
        }

        QString pressureChangePerDegreeChange( bool imperial, bool useAbbreviations, bool tex, bool description )
        {
            return ratioConstant(
                false, false, useAbbreviations, tex, description,   //
                [ imperial ]() -> double { return NConstants::pressureChangePerDegreeChange( imperial ); },   //
                [ tex ]() -> QString { return ratio( QObject::tr( "Pressure" ), QObject::tr( "Degree Change" ), tex ); },   //
                []( bool imperial, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::pressureUnit( imperial, useAbbreviations, tex ); },   //
                []( bool imperial, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::tempUnit( imperial, useAbbreviations, tex ); },   //
                {} );
        }

        QString lbsPerKgs( bool useAbbreviations, bool tex, bool description )
        {
            return ratioConstant(
                false, false, useAbbreviations, tex, description,   //
                []() -> double { return NConstants::lbsPerKGs(); },   //
                [ tex ]() -> QString { return ratio( QObject::tr( "Pounds" ), QObject::tr( "Kilogram" ), tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::weightUnit( true, useAbbreviations, tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::weightUnit( false, useAbbreviations, tex ); },   //
                {} );
        }

        QString kgsPerLbs( bool useAbbreviations, bool tex, bool description )
        {
            return ratioConstant(
                false, false, useAbbreviations, tex, description,   //
                []() -> double { return NConstants::lbsPerKGs(); },   //
                [ tex ]() -> QString { return ratio( QObject::tr( "Kilograms" ), QObject::tr( "Pound" ), tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::weightUnit( false, useAbbreviations, tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::weightUnit( true, useAbbreviations, tex ); },   //
                {} );
        }

        QString scubaMET( bool useAbbreviations, bool tex, bool description )
        {
            QString retVal = ( tex && !description ) ? "%1%2" : "%1 (%2)";
            if ( description )
                retVal = retVal.arg( QObject::tr( "Base MET for SCUBA" ) );
            else
                retVal = retVal.arg( NConstants::baseMETForScuba() );
            retVal = retVal.arg( NUnitStrings::metUnits( useAbbreviations, tex ) );
            return retVal;
        }

        QString fillRateO2( bool imperial, bool useAbbreviations, bool tex, bool description )
        {
            QString retVal = ( tex && !description ) ? "%1%2" : "%1 (%2)";
            if ( description )
                retVal = retVal.arg( QObject::tr( "O2 Fill Rate" ) );
            else
                retVal = retVal.arg( NConstants::fillRateO2( imperial ) );

            retVal = retVal.arg( NUnitStrings::pressurePerMinuteUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString fillRateAir( bool imperial, bool useAbbreviations, bool tex, bool description )
        {
            QString retVal = ( tex && !description ) ? "%1%2" : "%1 (%2)";
            if ( description )
                retVal = retVal.arg( QObject::tr( "Air Fill Rate" ) );
            else
                retVal = retVal.arg( NConstants::fillRateAir( imperial ) );

            retVal = retVal.arg( NUnitStrings::pressurePerMinuteUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString weightPerVolumeOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description )
        {
            if ( description )
                return ratio( ( tex ? QObject::tr( R"__(\frac{%1 of Water}{%2})__" ) : QObject::tr( "%1 of Water/%2" ) ).arg( NUnitStrings::weightUnit( imperial, false, false ) ).arg( NUnitStrings::volumeUnit( imperial, false, false ) ), NUnitStrings::weightUnit( imperial, useAbbreviations, tex ), NUnitStrings::volumeUnit( imperial, useAbbreviations, tex ), tex );
            else
                return ratio( NConstants::weightPerVolumeOfWater( imperial, seaWater ), NUnitStrings::weightUnit( imperial, useAbbreviations, tex ), NUnitStrings::volumeUnit( imperial, useAbbreviations, tex ), tex, 3 );
        }

        QString volumePerWeightOfWater( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description )
        {
            if ( description )
                return ratio( ( tex ? QObject::tr( R"__(\frac{%1 of Water}{%2})__" ) : QObject::tr( "%1 of Water/%2" ) ).arg( NUnitStrings::volumeUnit( imperial, false, tex ) ).arg( NUnitStrings::weightUnit( imperial, false, tex ) ), NUnitStrings::volumeUnit( imperial, useAbbreviations, tex ), NUnitStrings::weightUnit( imperial, useAbbreviations, tex ), tex );
            else
                return ratio( NConstants::volumePerWeightOfWater( imperial, seaWater ), NUnitStrings::volumeUnit( imperial, useAbbreviations, tex ), NUnitStrings::weightUnit( imperial, useAbbreviations, tex ), tex, 3 );
        }

        QString idealGasConstant( bool imperial, bool useAbbreviations, bool tex, bool description )
        {
            // pv = nrt
            QString retVal = ( tex && !description ) ? "%1%2" : "%1 (%2)";
            if ( description )
                retVal = retVal.arg( QObject::tr( "Ideal Gas Constant" ) );
            else
                retVal = retVal.arg( doubleToString( NConstants::idealGasConstant( imperial ), 5 ) );

            retVal = retVal.arg( NUnitStrings::idealGasConstantUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString percentN2AtSurface( bool imperial, bool useAbbreviations, bool tex, bool description )
        {
            if ( description )
                return QObject::tr( "% of N2 at Surface" );
            else
                return QObject::tr( R"__(%1%2)__", "percentN2AtSurface" ).arg( NConstants::percentN2AtSurface() ).arg( NUnitStrings::percentUnit( imperial, useAbbreviations, tex ) );
        }

        QString percentO2AtSurface( bool imperial, bool useAbbreviations, bool tex, bool description )
        {
            if ( description )
                return QObject::tr( "% of O2 at Surface" );
            else
                return QObject::tr( R"__(%1%2)__", "percentO2AtSurface" ).arg( NConstants::percentO2AtSurface() ).arg( NUnitStrings::percentUnit( imperial, useAbbreviations, tex ) );
        }

        QString singleATMPerDepth( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description )
        {
            QString retVal = ( tex && !description ) ? "%1%2" : "%1 (%2)";
            if ( description )
                retVal = retVal.arg( QObject::tr( "Depth of a Single Atmosphere" ) );
            else
                retVal = retVal.arg( doubleToString( NConstants::singleATMPerDepth( imperial, seaWater ), 2 ) );

            retVal = retVal.arg( NUnitStrings::lengthUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString metersPerFoot( bool useAbbreviations, bool tex, bool description )
        {
            return ratioConstant(
                false, false, useAbbreviations, tex, description,   //
                []() -> double { return NConstants::feetPerMeters(); },   //
                [ tex ]() -> QString { return ratio( QObject::tr( "Meters" ), QObject::tr( "Foot" ), tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::lengthUnit( true, useAbbreviations, tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::lengthUnit( false, useAbbreviations, tex ); },   //
                {} );
        }

        QString feetPerMeter( bool useAbbreviations, bool tex, bool description )
        {
            return ratioConstant(
                false, false, useAbbreviations, tex, description,   //
                []() -> double { return NConstants::feetPerMeters(); },   //
                [ tex ]() -> QString { return ratio( QObject::tr( "Feet" ), QObject::tr( "Meter" ), tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::lengthUnit( false, useAbbreviations, tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::lengthUnit( true, useAbbreviations, tex ); },   //
                {} );
        }

        QString freshWaterToSeaWater( bool imperial, bool useAbbreviations, bool tex, bool description )
        {
            return ratioConstant(
                imperial, false, useAbbreviations, tex, description,   //
                []() -> double { return NConstants::freshWaterToSeaWater(); },   //
                [ tex ]() -> QString { return QObject::tr( "Fresh Water to Sea Water" ); },   //
                []( bool imperial, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::depthUnit( imperial, false, useAbbreviations, tex ); },   //
                []( bool imperial, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::depthUnit( imperial, true, useAbbreviations, tex ); },   //
                2 );
        }

        QString seaWaterToFreshWater( bool imperial, bool useAbbreviations, bool tex, bool description )
        {
            return ratioConstant(
                imperial, false, useAbbreviations, tex, description,   //
                []() -> double { return NConstants::seaWaterToFreshWater(); },   //
                [ tex ]() -> QString { return QObject::tr( "Sea Water to Fresh Water" ); },   //
                []( bool imperial, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::depthUnit( imperial, true, useAbbreviations, tex ); },   //
                []( bool imperial, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::depthUnit( imperial, false, useAbbreviations, tex ); },   //
                2 );
        }

        QString barPerPSI( bool useAbbreviations, bool tex, bool description )
        {
            return ratioConstant(
                false, false, useAbbreviations, tex, description,   //
                []() -> double { return NConstants::barPerPSI(); },   //
                [ tex ]() -> QString { return ratio( QObject::tr( "BAR" ), QObject::tr( "PSI" ), tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::pressureUnit( true, useAbbreviations, tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::pressureUnit( false, useAbbreviations, tex ); },   //
                2 );
        }

        QString psiPerBAR( bool useAbbreviations, bool tex, bool description )
        {
            return ratioConstant(
                false, false, useAbbreviations, tex, description,   //
                []() -> double { return NConstants::barPerPSI(); },   //
                [ tex ]() -> QString { return ratio( QObject::tr( "PSI" ), QObject::tr( "BAR" ), tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::pressureUnit( false, useAbbreviations, tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::pressureUnit( true, useAbbreviations, tex ); },   //
                2 );
        }

        QString absZeroOffset( bool imperial, bool useAbbreviations, bool tex, bool description )
        {
            QString retVal = ( tex && !description ) ? "%1%2" : "%1 (%2)";
            if ( description )
                retVal = retVal.arg( QObject::tr( "Absolute Zero Offset" ) );
            else
                retVal = retVal.arg( doubleToString( NConstants::absZeroOffset( imperial ), 0 ) );

            retVal = retVal.arg( NUnitStrings::tempUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString pressureAtSurface( bool imperial, bool useAbbreviations, bool tex, bool description )
        {
            QString retVal = ( tex && !description ) ? "%1%2" : "%1 (%2)";
            if ( description )
                retVal = retVal.arg( QObject::tr( "Pressure at Sea Level" ) + " " );
            else
                retVal = retVal.arg( doubleToString( NConstants::pressureAtSurface( imperial ), 1 ) );

            retVal = retVal.arg( NUnitStrings::atmosphereUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString pressureLossPerAltitudeGain( bool imperial, bool useAbbreviations, bool tex, bool description )
        {
            auto retVal = ratioConstant(
                false, false, useAbbreviations, tex, description,   //
                [ imperial ]() -> double { return NConstants::pressureLossPerAltitudeGain( imperial ); },   //
                [ tex ]() -> QString { return ratio( QObject::tr( "Pressure Loss" ), QObject::tr( "Altitude Gain" ), tex ); },   //
                []( bool imperial, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::atmosphereUnit( imperial, useAbbreviations, tex ); },   //
                []( bool imperial, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::lengthUnit( imperial, useAbbreviations, tex ); },   //
                4 );
            return retVal;
        }

        QString safetyStop( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description )
        {
            QString retVal = ( tex && !description ) ? "%1%2" : "%1 (%2)";
            if ( description )
                retVal = retVal.arg( QObject::tr( "Safety Stop Depth" ) );
            else
                retVal = retVal.arg( doubleToString( NConstants::safetyStopDepth( imperial, seaWater ), 1 ) );

            retVal = retVal.arg( NUnitStrings::depthUnit( imperial, seaWater, useAbbreviations, tex ) );
            return retVal;
        }

        QString waterWeightAdjustment( bool imperial, bool seaWater, bool useAbbreviations, bool tex, bool description )
        {
            QString retVal = ( tex && !description ) ? "%1%2" : "%1 (%2)";
            if ( description )
                retVal = retVal.arg( QObject::tr( "Weight Adjustment for Water" ) );
            else
                retVal = retVal.arg( doubleToString( NConstants::waterWeightAdjustment( imperial, seaWater ), 1 ) );

            retVal = retVal.arg( NUnitStrings::weightUnit( imperial, useAbbreviations, tex ) );
            return retVal;
        }

        QString litersPerCubicFoot( bool useAbbreviations, bool tex, bool description )
        {
            return ratioConstant(
                false, false, useAbbreviations, tex, description,   //
                []() -> double { return NConstants::cubicFeetPerLiter(); },   //
                [ tex ]() -> QString { return ratio( QObject::tr( "Liters" ), QObject::tr( "Cubic Foot" ), tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::volumeUnit( true, useAbbreviations, tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::volumeUnit( false, useAbbreviations, tex ); },   //
                4 );
        }

        QString cubicFootPerLiter( bool useAbbreviations, bool tex, bool description )
        {
            return ratioConstant(
                false, false, useAbbreviations, tex, description,   //
                []() -> double { return NConstants::litersPerCubicFeet(); },   //
                [ tex ]() -> QString { return ratio( QObject::tr( "Cubic Feet" ), QObject::tr( "Liter" ), tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::volumeUnit( false, useAbbreviations, tex ); },   //
                []( bool /*imperial*/, bool /*seaWater*/, bool useAbbreviations, bool tex ) -> QString { return NUnitStrings::volumeUnit( true, useAbbreviations, tex ); },   //
                4 );
        }

        QString constantString( bool imperial, bool seaWater, EVariableType constantType, bool description )
        {
            switch ( constantType )
            {
                case EVariableType::eIntermediate:
                case EVariableType::eVariable:
                    Q_ASSERT( isConstantVariable( constantType ) );
                    break;
                case EVariableType::ePressurePerDegreeConst:
                    return pressureChangePerDegreeChange( imperial, true, true, description );
                case EVariableType::eWeightPerVolumeOfWaterConst:
                    return weightPerVolumeOfWater( imperial, seaWater, true, true, description );
                case EVariableType::eVolumePerWeightOfWaterConst:
                    return volumePerWeightOfWater( imperial, seaWater, true, true, description );
                case EVariableType::eIdealGasConst:
                    return idealGasConstant( imperial, true, true, description );
                case EVariableType::eFN2AtSurfaceConst:
                    return percentN2AtSurface( imperial, true, true, description );
                case EVariableType::eFO2AtSurfaceConst:
                    return percentO2AtSurface( imperial, true, true, description );
                case EVariableType::eDepthToSingleATMConst:
                    return singleATMPerDepth( imperial, seaWater, true, true, description );
                case EVariableType::eFeetToMetersConst:
                    return metersPerFoot( true, true, description );
                case EVariableType::eMetersToFeetConst:
                    return feetPerMeter( true, true, description );
                case EVariableType::eLbsPerKgsConst:
                    return lbsPerKgs( true, true, description );
                case EVariableType::eKgsPerLbsConst:
                    return kgsPerLbs( true, true, description );
                case EVariableType::eFreshWaterToSeaWaterConst:
                    return freshWaterToSeaWater( imperial, true, true, description );
                case EVariableType::eSeaWaterToFreshWaterConst:
                    return seaWaterToFreshWater( imperial, true, true, description );
                case EVariableType::ePSIToBarConst:
                    return barPerPSI( true, true, description );
                case EVariableType::eBarToPSIConst:
                    return psiPerBAR( true, true, description );
                case EVariableType::eAbsZeroOffsetConst:
                    return absZeroOffset( imperial, true, true, description );
                case EVariableType::ePressureAtSurfaceConst:
                    return pressureAtSurface( imperial, true, true, description );
                case EVariableType::ePressureLossPerAltitudeGainConst:
                    return pressureLossPerAltitudeGain( imperial, true, true, description );
                case EVariableType::eSafetyStopDepthConst:
                    return safetyStop( imperial, seaWater, true, true, description );
                case EVariableType::eWaterWeightAdjustmentConst:
                    return waterWeightAdjustment( imperial, seaWater, true, true, description );
                case EVariableType::eBaseMETofSCUBAConst:
                    return scubaMET( true, true, description );
                case EVariableType::eFillRateO2Const:
                    return fillRateO2( imperial, true, true, description );
                case EVariableType::eFillRateAirConst:
                    return fillRateAir( imperial, true, true, description );
                case EVariableType::eCubicFeetToLitersConst:
                    return litersPerCubicFoot( true, true, description );
                case EVariableType::eLitersToCubicFeetConst:
                    return cubicFootPerLiter( true, true, description );
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
                                           EVariableType::ePressureLossPerAltitudeGainConst,
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

    QString fieldNameForType( EVariableType type )
    {
        QString retVal;
        switch ( type )
        {
            case EVariableType::eIntermediate:
            case EVariableType::eVariable:
                return {};
            case EVariableType::ePressurePerDegreeConst:
                retVal = NConstants::kPressurePerDegreeConstFieldName;
                break;
            case EVariableType::eWeightPerVolumeOfWaterConst:
                retVal = NConstants::kWeightPerVolumeOfWaterConstFieldName;
                break;
            case EVariableType::eVolumePerWeightOfWaterConst:
                retVal = NConstants::kVolumePerWeightOfWaterConstFieldName;
                break;
            case EVariableType::eIdealGasConst:
                retVal = NConstants::kIdealGasConstantFieldName;
                break;
            case EVariableType::eFN2AtSurfaceConst:
                retVal = NConstants::kFN2AtSurfaceFieldName;
                break;
            case EVariableType::eFO2AtSurfaceConst:
                retVal = NConstants::kFO2AtSurfaceFieldName;
                break;
            case EVariableType::eDepthToSingleATMConst:
                retVal = NConstants::kDepthToSingleATMConstFieldName;
                break;
            case EVariableType::eFeetToMetersConst:
                retVal = NConstants::kFeetToMetersConstFieldName;
                break;
            case EVariableType::eMetersToFeetConst:
                retVal = NConstants::kMetersToFeetConstFieldName;
                break;
            case EVariableType::eLbsPerKgsConst:
                retVal = NConstants::kLbsPerKgsConstFieldName;
                break;
            case EVariableType::eKgsPerLbsConst:
                retVal = NConstants::kKgsPerLbsConstFieldName;
                break;
            case EVariableType::eFreshWaterToSeaWaterConst:
                retVal = NConstants::kFreshWaterToSeaWaterConstFieldName;
                break;
            case EVariableType::eSeaWaterToFreshWaterConst:
                retVal = NConstants::kSeaWaterToFreshWaterConstFieldName;
                break;
            case EVariableType::ePSIToBarConst:
                retVal = NConstants::kPSIToBarConstFieldName;
                break;
            case EVariableType::eBarToPSIConst:
                retVal = NConstants::kBarToPSIConstFieldName;
                break;
            case EVariableType::eAbsZeroOffsetConst:
                retVal = NConstants::kAbsZeroOffsetConstFieldName;
                break;
            case EVariableType::ePressureAtSurfaceConst:
                retVal = NConstants::kPressureAtSurfaceConstFieldName;
                break;
            case EVariableType::ePressureLossPerAltitudeGainConst:
                retVal = NConstants::kPressureLossPerAltitudeGainConstFieldName;
                break;
            case EVariableType::eSafetyStopDepthConst:
                retVal = NConstants::kSafetyStopDepthConstFieldName;
                break;
            case EVariableType::eWaterWeightAdjustmentConst:
                retVal = NConstants::kWaterWeightAdjustmentFieldName;
                break;
            case EVariableType::eBaseMETofSCUBAConst:
                retVal = NConstants::kBaseMETofSCUBAConstFieldName;
                break;
            case EVariableType::eFillRateAirConst:
                retVal = NConstants::kFillRateAirConstFieldName;
                break;
            case EVariableType::eFillRateO2Const:
                retVal = NConstants::kFillRateO2ConstFieldName;
                break;
            case EVariableType::eCubicFeetToLitersConst:
                retVal = NConstants::kCubicFeetToLitersFieldName;
                break;
            case EVariableType::eLitersToCubicFeetConst:
                retVal = NConstants::kLitersToCubicFeetFieldName;
                break;
        };
        retVal = QString( "%1" ).arg( retVal );
        return retVal;
    }
}
