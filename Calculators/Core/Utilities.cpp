#include "Utilities.h"
#include "VariableInfo.h"
#include "FormulaString.h"

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

    TFormulaStringList sortAndUniquifyFormulas( const TFormulaStringList &formulas )
    {
        auto tmp = formulas;

        std::unordered_map< TVariableInfo, TFormulaStringList::const_iterator > lastLocOfVar;

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

        TFormulaStringList retVal;

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

    TFormulaString ratioFormula( const TConstVariableInfo &returnVariable, const TConstVariableInfo &numerator, const TConstVariableInfo &denominator )
    {
        return std::make_shared< CFormulaString >( returnVariable, ratio( numerator->fieldName(), denominator->fieldName(), true ) );
    }

    TFormulaString ratioFormula( const TConstVariableInfo &returnVariable, const TConstVariableInfo &numerator, const QString &denominator )
    {
        return std::make_shared< CFormulaString >( returnVariable, ratio( numerator->fieldName(), denominator, true ) );
    }

    TFormulaString ratioFormula( const TConstVariableInfo &returnVariable, const QString &numerator, const TConstVariableInfo &denominator )
    {
        return std::make_shared< CFormulaString >( returnVariable, ratio( numerator, denominator->fieldName(), true ) );
    }

    TFormulaString ratioFormula( const TConstVariableInfo &returnVariable, const TConstVariableInfo &numerator, EVariableType denominator )
    {
        return std::make_shared< CFormulaString >( returnVariable, ratio( numerator->fieldName(), NUtilities::fieldNameForType( denominator ), true ) );
    }

    TFormulaString ratioFormula( const TConstVariableInfo &returnVariable, EVariableType numerator, const TConstVariableInfo &denominator )
    {
        return std::make_shared< CFormulaString >( returnVariable, ratio( NUtilities::fieldNameForType( numerator ), denominator->fieldName(), true ) );
    }

    QString descForType( EVariableType type )
    {
        switch ( type )
        {
            case EVariableType::eIntermediate:
            case EVariableType::eVariable:
                return {};
            case EVariableType::ePressurePerDegreeConst:
                return QObject::tr( "Pressure Change per Degree Change", "descForType" );
            case EVariableType::eWeightPerVolumeOfWaterConst:
                return QObject::tr( "Weight of Water", "descForType" );
            case EVariableType::eVolumePerWeightOfWaterConst:
                return QObject::tr( "Volume of Water", "descForType" );
            case EVariableType::eIdealGasConst:
                return QObject::tr( "Ideal Gas Constant", "descForType" );
            case EVariableType::eFN2AtSurfaceConst:
                return QObject::tr( "Partial Pressure of Nitrogen at Surface", "descForType" );
            case EVariableType::eFO2AtSurfaceConst:
                return QObject::tr( "Partial Pressure of Oxygen at Surface", "descForType" );
            case EVariableType::eDepthToSingleATMConst:
                return QObject::tr( "Depth of a Single Atmosphere", "descForType" );
            case EVariableType::eFeetToMetersConst:
                return QObject::tr( "How many Feet to a Meter", "descForType" );
            case EVariableType::eMetersToFeetConst:
                return QObject::tr( "How many Meters to a Foot", "descForType" );
            case EVariableType::eLbsPerKgsConst:
                return QObject::tr( "How many Pounds per Kilogram", "descForType" );
            case EVariableType::eKgsPerLbsConst:
                return QObject::tr( "How many Kilograms per Pound", "descForType" );
            case EVariableType::eFreshWaterToSeaWaterConst:
                return QObject::tr( "How much depth of Freshwater to Seawater", "descForType" );
            case EVariableType::eSeaWaterToFreshWaterConst:
                return QObject::tr( "How much depth of Seawater to Freshwater", "descForType" );
            case EVariableType::ePSIToBarConst:
                return QObject::tr( "PSI to Bar", "descForType" );
            case EVariableType::eBarToPSIConst:
                return QObject::tr( "Bar to PSI", "descForType" );
            case EVariableType::eAbsZeroOffsetConst:
                return QObject::tr( "Absolute Zero Offset", "descForType" );
            case EVariableType::ePressureAtSurfaceConst:
                return QObject::tr( "Pressure at Surface", "descForType" );
            case EVariableType::ePressureLossPerAltitudeGainConst:
                return QObject::tr( "Pressure Loss Per Altitude Gain", "descForType" );
            case EVariableType::eSafetyStopDepthConst:
                return QObject::tr( "Safety Stop Depth", "descForType" );
            case EVariableType::eWaterWeightAdjustmentConst:
                return QObject::tr( "Water Weight Adjustment", "descForType" );
            case EVariableType::eBaseMETofSCUBAConst:
                return QObject::tr( "Base MET Value for SCUBA", "descForType" );
            case EVariableType::eFillRateAirConst:
                return QObject::tr( "Fill Rate for Air", "descForType" );
            case EVariableType::eFillRateO2Const:
                return QObject::tr( "Fill Rate for Pure O2", "descForType" );
            case EVariableType::eCubicFeetToLitersConst:
                return QObject::tr( "Cubic Feet to Liters", "descForType" );
            case EVariableType::eLitersToCubicFeetConst:
                return QObject::tr( "Liters to Cubic Feet", "descForType" );
        };
        return {};
    }

    bool isConstantVariable( EVariableType type )
    {
        switch ( type )
        {
            case EVariableType::ePressurePerDegreeConst:
            case EVariableType::eWeightPerVolumeOfWaterConst:
            case EVariableType::eVolumePerWeightOfWaterConst:
            case EVariableType::eIdealGasConst:
            case EVariableType::eFN2AtSurfaceConst:
            case EVariableType::eFO2AtSurfaceConst:
            case EVariableType::eDepthToSingleATMConst:
            case EVariableType::eFeetToMetersConst:
            case EVariableType::eMetersToFeetConst:
            case EVariableType::eLbsPerKgsConst:
            case EVariableType::eKgsPerLbsConst:
            case EVariableType::eFreshWaterToSeaWaterConst:
            case EVariableType::eSeaWaterToFreshWaterConst:
            case EVariableType::ePSIToBarConst:
            case EVariableType::eBarToPSIConst:
            case EVariableType::eAbsZeroOffsetConst:
            case EVariableType::ePressureAtSurfaceConst:
            case EVariableType::ePressureLossPerAltitudeGainConst:
            case EVariableType::eSafetyStopDepthConst:
            case EVariableType::eWaterWeightAdjustmentConst:
            case EVariableType::eBaseMETofSCUBAConst:
            case EVariableType::eFillRateAirConst:
            case EVariableType::eFillRateO2Const:
            case EVariableType::eCubicFeetToLitersConst:
            case EVariableType::eLitersToCubicFeetConst:
                return true;
            case EVariableType::eIntermediate:
            case EVariableType::eVariable:
                return false;
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

    std::optional< QString > joinFormulas( bool imperial, bool seaWater, const TFormulaStringList &formulaStrings )
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
