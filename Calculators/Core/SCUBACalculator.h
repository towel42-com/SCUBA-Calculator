// The MIT License( MIT )
//
// Copyright( c ) 2025 Scott Aron Bloom
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sub-license, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef SCUBACALCULATOR_H
#define SCUBACALCULATOR_H

#include <unordered_map>
#include <unordered_set>
#include "SCUBACalculatorFwd.h"
#include <QStringList>
#include <QString>
#include <QObject>
#include <QJsonArray>
#include <memory>

namespace NTowel42
{
    class CMathJaxQt6;
}

class QWidget;
class QFrame;

class CALCULATORS_EXPORT CSCUBACalculator : public QObject
{
    Q_OBJECT;

    Q_PROPERTY( bool showUnits READ showUnits );
    Q_PROPERTY( bool isWaterTypeBased READ isWaterTypeBased );
    Q_PROPERTY( bool isReversible READ isReversible );

public:
    // used inside the DLL
    CSCUBACalculator( QObject *parent = nullptr );
    virtual ~CSCUBACalculator();

public:
    // used outside the DLL
    // dont know why, but making them final means they can not be imported
    virtual void init( bool imperial, bool seaWater ) /*final*/;   // initializes the default equations and sets the equations to the current setup

    virtual bool isReversible() const { return false; }
    virtual bool isReversed() const { return fReversed.second; }
    virtual void setIsReversed( CSCUBACalculator *nonReversedCalc, bool isReversed ) /*final*/;

    virtual QStringList calculatorPath() const /*final*/;
    virtual QString calculatorName() const /*final*/;

    virtual QString calculatorProjectName() const = 0;
    virtual QString calculatorGroupName() const = 0;

    virtual QWidget *getPage( QWidget *parent ) /*final*/;
    virtual QWidget *getPage() const /*final*/;

    virtual void setUpdateFormulaFunc( const TUpdateFormulaFunc &func ) /*final*/;
    virtual void setImperial( bool imperial ) /*final*/;
    virtual void setSeaWater( bool seaWater ) /*final*/;

    virtual void resetVariables() /*final*/;

    virtual std::shared_ptr< CGeneratedFormulaData > getAllFormulas( const std::function< bool( const QString &formula ) > &beenCreated ) /*final*/;

    virtual void initResources() const /*final*/;

public:
    // used inside the DLL
    virtual void compute( EVariableLoc updateFromSide, QWidget *triggerWidget ) final;
    virtual bool showUnits() const { return true; }
    virtual bool isWaterTypeBased() const { return false; }

    virtual TVariableInfoList &getLHSVariables() final;
    virtual TVariableInfoList &getRHSVariables() final;

    TConstVariableInfo getVariable( const QString &varName ) const;
    TVariableInfo getVariable( const QString &varName );

    virtual void computeValueForVar( TVariableInfo &unsetVar ) = 0;

protected:
    virtual bool imperial() const final;
    virtual bool seaWater() const final;

    virtual TVariableInfoList &getVariables() final;

    virtual const TVariableInfoList &getVariables() const final;
    virtual const TVariableInfoList &getLHSVariables() const final;
    virtual const TVariableInfoList &getRHSVariables() const final;

    TFormulaList getFormulaList();
    TFormulaList getNamedFormulas() const;
    TVariableValuePairVectorVector getAllVariableValueCombinations() const;

    virtual std::pair< QString, QString > fromToLabels() const { return {}; }

    virtual QStringList myCalculatorPath() const = 0;
    virtual QStringList myReversedCalculatorPath() const { return {}; }

    virtual QString myCalculatorName() const;
    virtual QString myReversedCalculatorName() const;

    virtual TFormulaStringList getBaseFormulas() const final;   // for descriptive purposes

    virtual std::optional< TFormulaStringList > getCurrentFormulas() const final;   // returns the current formula in use
    virtual std::optional< TFormulaStringList > myBaseFormulas( bool imperial, bool seaWater ) const = 0;   // when the formula depends on watertype and/or units but not just in units
    virtual std::optional< TFormulaStringList > myReversedBaseFormulas( bool imperial, bool seaWater ) const;   // when the formula depends on watertype and/or units but not just in units

    void initVariables();
    TVariableInfoList unsetVariables() const;
    std::size_t numUnsetVariables() const;
    bool allVariablesUnset() const;

    virtual void notifyOfNewFormula( const QString &formula, bool finished ) const final;
    virtual void updateFields( QWidget *triggerWidget ) const final;

    virtual QString finalizeFormulas( bool imperial, bool seaWater, const TFormulaStringList &formulas ) final;
    virtual TFormulaStringList finalizeFormula( bool imperial, bool seaWater, const TFormulaString &formula ) final;
    QString postProcessFormula( const QString &retVal ) const;

    virtual TFormulaString applyVariables( bool imperial, bool seaWater, const TFormulaString &formula, EFormulaType formulaType ) const final;

    virtual TVariableInfoList getMyVariables() const = 0;
    virtual TVariableInfoList getMyVariables( bool *preReversed ) const;

    virtual void determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget ) final;

    virtual TVariableInfo determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior );
    // only necessary if the number of variables on either side is greater than two.

    virtual void computeValues() final;   // updates all values
    virtual bool valuesSetProperly() const;

protected:
    virtual int numAllowedUnset() const { return 1; }

    TVariableInfoList getUnsetVariables() const;
    TVariableInfo getFirstUnsetVariable() const;
    virtual std::optional< TFormulaStringList > getFormulasForVar( const TConstVariableInfo &unsetVar, bool imperial, bool seaWater ) const = 0;   // returns the current formula in use

    TVariableInfo getFirstVariable( EVariableLoc side ) const;
    TVariableInfo getLastVariable( EVariableLoc side ) const;
    std::size_t numVariables( EVariableLoc side ) const;

protected:
    CSCUBACalculatorPage *fPage{ nullptr };
    TUpdateFormulaFunc fUpdateFormulaFunc;
    TVariableInfoList fLHSVariables;
    TVariableInfoList fRHSVariables;
    TVariableInfoList fVariables;

    std::unordered_map< QString, TVariableInfo > fVariableMap;
    std::size_t fNumVariables{ 0 };   // if there are constants in the variable list, this value will not equal fVariables.size();
    std::pair< CSCUBACalculator *, bool > fReversed{ nullptr, false };
};

extern "C" CALCULATORS_EXPORT CSCUBACalculator *instantiateCalculator();

#endif
