#include "Utilities.h"
#include "VariableInfo.h"
#include "Formula.h"

#include <QString>
#include <QObject>
#include <QRegularExpression>

#include <iterator>
#include <list>

namespace NUtilities
{
    QStringList getVariables( const QString &formula )
    {
        QStringList retVal;

        auto regEx = QRegularExpression( R"__(\<((?:[A-Za-z][A-Za-z0-9]+)|(?:%\d+))\>)__" );
        auto matchII = regEx.globalMatch( formula );
        int offset = 0;
        while ( matchII.hasNext() )
        {
            auto match = matchII.next();
            retVal << match.captured( 1 );
        }
        return retVal;
    }

    bool hasVariable( const QString &formula )
    {
        auto regEx = QRegularExpression( R"__(\<([A-Za-z]+)|(%\d+)\>)__" );
        return regEx.match( formula ).hasMatch();
    }

    TFormulaList sortAndUniquifyFormulas( const TFormulaList &formulas )
    {
        auto tmp = formulas;

        std::unordered_map< TVariableInfo, TFormulaList::const_iterator > lastLocOfVar;

        for ( auto &&ii = tmp.begin(); ii != tmp.end(); )
        {
            auto currVariable = ( *ii )->variable();
            auto pos = lastLocOfVar.find( currVariable );
            if ( pos == lastLocOfVar.end() )
            {
                lastLocOfVar[ currVariable ] = ii;
                ++ii;
            }
            else
            {
                auto curr = *ii;
                ii = tmp.erase( ii );
                lastLocOfVar[ currVariable ] = tmp.insert( std::next( ( *pos ).second ), curr );
            }
        }

        TFormulaList retVal;

        // check for duplicates
        for ( auto &&ii = tmp.begin(); ii != tmp.end(); ++ii )
        {
            bool found = false;
            for ( auto &&jj = std::next( ii ); !found && ( jj != tmp.end() ); ++jj )
            {
                found = ( *ii == *jj );
            }
            if ( !found )
                retVal.push_back( *ii );
        }
        return retVal;
    }

    QString ratio( const QString &numerator, const QString &denominator, bool tex )
    {
        QString retVal = tex ? R"__(\frac{%2}{%3})__" : "(%2/%3)";
        retVal = retVal.arg( numerator ).arg( denominator );
        return retVal;
    }

    QString ratio( const QString &desc, const QString &numerator, const QString &denominator, bool tex )
    {
        auto units = ratio( numerator, denominator, tex );
        QString retVal = desc;
        if ( !retVal.isEmpty() )
            retVal += " ";
        retVal += units;
        return retVal;
    }

    QString ratio( double value, const QString &numerator, const QString &denominator, bool tex, std::optional< int > numDecimal /*= {}*/ )
    {
        QString valueString;

        if ( !numDecimal.has_value() )
            valueString = QString( "%1" ).arg( value );
        else
            valueString = NUtilities::doubleToString( value, numDecimal.value() );

        return ratio( valueString, numerator, denominator, tex );
    }

    TFormula ratioFormula( const TConstVariableInfo &returnVariable, const TConstVariableInfo &numerator, const TConstVariableInfo &denominator )
    {
        return std::make_shared< CFormula >( returnVariable, ratio( numerator->fieldName(), denominator->fieldName(), true ) );
    }

    TFormula ratioFormula( const TConstVariableInfo &returnVariable, const TConstVariableInfo &numerator, const QString &denominator )
    {
        return std::make_shared< CFormula >( returnVariable, ratio( numerator->fieldName(), denominator, true ) );
    }

    TFormula ratioFormula( const TConstVariableInfo &returnVariable, const QString &numerator, const TConstVariableInfo &denominator )
    {
        return std::make_shared< CFormula >( returnVariable, ratio( numerator, denominator->fieldName(), true ) );
    }

    TFormula ratioFormula( const TConstVariableInfo &returnVariable, const TConstVariableInfo &numerator, EConstantType denominator )
    {
        return std::make_shared< CFormula >( returnVariable, ratio( numerator->fieldName(), NUtilities::fieldNameForType( denominator ), true ) );
    }

    TFormula ratioFormula( const TConstVariableInfo &returnVariable, EConstantType numerator, const TConstVariableInfo &denominator )
    {
        return std::make_shared< CFormula >( returnVariable, ratio( NUtilities::fieldNameForType( numerator ), denominator->fieldName(), true ) );
    }

    QString descForType( EConstantType type )
    {
        switch ( type )
        {
            case EConstantType::ePressurePerDegreeConst:
                return QObject::tr( "Pressure Change per Degree Change", "descForType" );
            case EConstantType::eWeightPerVolumeOfWaterConst:
                return QObject::tr( "Weight of Water", "descForType" );
            case EConstantType::eVolumePerWeightOfWaterConst:
                return QObject::tr( "Volume of Water", "descForType" );
            case EConstantType::eIdealGasConst:
                return QObject::tr( "Ideal Gas Constant", "descForType" );
            case EConstantType::eFN2AtSurfaceConst:
                return QObject::tr( "Partial Pressure of Nitrogen at Surface", "descForType" );
            case EConstantType::eFO2AtSurfaceConst:
                return QObject::tr( "Partial Pressure of Oxygen at Surface", "descForType" );
            case EConstantType::eDepthToSingleATMConst:
                return QObject::tr( "Depth of a Single Atmosphere", "descForType" );
            case EConstantType::eFeetToMetersConst:
                return QObject::tr( "How many Feet to a Meter", "descForType" );
            case EConstantType::eMetersToFeetConst:
                return QObject::tr( "How many Meters to a Foot", "descForType" );
            case EConstantType::eLbsPerKgsConst:
                return QObject::tr( "How many Pounds per Kilogram", "descForType" );
            case EConstantType::eKgsPerLbsConst:
                return QObject::tr( "How many Kilograms per Pound", "descForType" );
            case EConstantType::eFreshWaterToSeaWaterConst:
                return QObject::tr( "How much depth of Freshwater to Seawater", "descForType" );
            case EConstantType::eSeaWaterToFreshWaterConst:
                return QObject::tr( "How much depth of Seawater to Freshwater", "descForType" );
            case EConstantType::ePSIToBarConst:
                return QObject::tr( "PSI to Bar", "descForType" );
            case EConstantType::eBarToPSIConst:
                return QObject::tr( "Bar to PSI", "descForType" );
            case EConstantType::eAbsZeroOffsetConst:
                return QObject::tr( "Absolute Zero Offset", "descForType" );
            case EConstantType::ePressureAtSurfaceConst:
                return QObject::tr( "Pressure at Surface", "descForType" );
            case EConstantType::ePressureLossPerAltitudeGainConst:
                return QObject::tr( "Pressure Loss Per Altitude Gain", "descForType" );
            case EConstantType::eSafetyStopDepthConst:
                return QObject::tr( "Safety Stop Depth", "descForType" );
            case EConstantType::eWaterWeightAdjustmentConst:
                return QObject::tr( "Water Weight Adjustment", "descForType" );
            case EConstantType::eBaseMETofSCUBAConst:
                return QObject::tr( "Base MET Value for SCUBA", "descForType" );
            case EConstantType::eFillRateAirConst:
                return QObject::tr( "Fill Rate for Air", "descForType" );
            case EConstantType::eFillRateO2Const:
                return QObject::tr( "Fill Rate for Pure O2", "descForType" );
            case EConstantType::eCubicFeetToLitersConst:
                return QObject::tr( "Cubic Feet to Liters", "descForType" );
            case EConstantType::eLitersToCubicFeetConst:
                return QObject::tr( "Liters to Cubic Feet", "descForType" );
        };
        return {};
    }

    bool isConstantVariable( EConstantType type )
    {
        switch ( type )
        {
            case EConstantType::ePressurePerDegreeConst:
            case EConstantType::eWeightPerVolumeOfWaterConst:
            case EConstantType::eVolumePerWeightOfWaterConst:
            case EConstantType::eIdealGasConst:
            case EConstantType::eFN2AtSurfaceConst:
            case EConstantType::eFO2AtSurfaceConst:
            case EConstantType::eDepthToSingleATMConst:
            case EConstantType::eFeetToMetersConst:
            case EConstantType::eMetersToFeetConst:
            case EConstantType::eLbsPerKgsConst:
            case EConstantType::eKgsPerLbsConst:
            case EConstantType::eFreshWaterToSeaWaterConst:
            case EConstantType::eSeaWaterToFreshWaterConst:
            case EConstantType::ePSIToBarConst:
            case EConstantType::eBarToPSIConst:
            case EConstantType::eAbsZeroOffsetConst:
            case EConstantType::ePressureAtSurfaceConst:
            case EConstantType::ePressureLossPerAltitudeGainConst:
            case EConstantType::eSafetyStopDepthConst:
            case EConstantType::eWaterWeightAdjustmentConst:
            case EConstantType::eBaseMETofSCUBAConst:
            case EConstantType::eFillRateAirConst:
            case EConstantType::eFillRateO2Const:
            case EConstantType::eCubicFeetToLitersConst:
            case EConstantType::eLitersToCubicFeetConst:
                return true;
        };
        return false;
    }

    std::size_t numEmpty( const TOptionalDoubleVector &values )
    {
        std::size_t numEmpty = 0;

        for ( auto &&value : values )
        {
            numEmpty += value.has_value() ? 0 : 1;
        }
        return numEmpty;
    }

    QString doubleToString( const TOptionalDouble &value, int numDecimal )
    {
        QString retVal;
        if ( value.has_value() )
            retVal = QString( "%1" ).arg( value.value(), 0, 'f', numDecimal );
        return retVal;
    }

    std::optional< QString > joinFormulas( bool imperial, bool seaWater, const TFormulaList &formulaStrings )
    {
        if ( formulaStrings.empty() )
            return {};

        QStringList formulas;
        std::optional< TVariableInfo > prevVar;
        for ( auto &&ii : formulaStrings )
        {
            if ( !ii )
                continue;

            if ( ( prevVar.has_value() && ( prevVar.value() != ii->variable() ) ) )
                formulas << QString();

            formulas << ii->equation( imperial, seaWater );
            prevVar = ii->variable();
        }

        auto retVal = formulas.join( R"( \newline )"
                                     "\n" );
        if ( formulas.size() > 1 )
        {
            retVal = QString( R"__(\begin{align})__"
                              "\n" )
                     + retVal
                     + QString( "\n"
                                R"__(\end{align})__"
                                "\n" );
            retVal.replace( "=", "& =" );
        }
        return retVal;
    }
}
