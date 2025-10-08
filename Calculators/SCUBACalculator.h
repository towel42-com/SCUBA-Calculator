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
class QWidget;
class QSvgWidget;
class QFrame;

class CALCULATORS_EXPORT CSCUBACalculator : public QObject
{
    Q_OBJECT;

    friend class CSCUBACalculatorPage;

public:
    Q_PROPERTY( bool showUnits READ showUnits );
    Q_PROPERTY( bool isWaterTypeBased READ isWaterTypeBased );

    CSCUBACalculator( QObject *parent = nullptr );
    virtual void init( bool imperial, bool seaWater ) final;   // initializes the default equations and sets the equations to the current setup
    virtual ~CSCUBACalculator();

    virtual QString calculatorName() const = 0;
    virtual QStringList calculatorPath() const = 0;

    virtual CSCUBACalculatorPage *getPage( QWidget *parent ) final;
    virtual CSCUBACalculatorPage *getPage() const final;

    virtual bool imperial() const final;
    virtual void setImperial( bool imperial ) final;

    virtual bool seaWater() const final;
    virtual void setSeaWater( bool seaWater ) final;

    virtual bool isWaterTypeBased() const { return false; }
    virtual bool showUnits() const { return true; }

    virtual void setUpdateFormulaFunc( const TUpdateFormulaFunc &func ) final;
    virtual void renderDefaultFormulas() const final;
    virtual void compute( EVariableLoc updateFromSide, QWidget *triggerWidget ) final;

    virtual const TVariableInfoList &getVariables() const;

    virtual void resetVariables();

    virtual QFrame *svgFrame() const { return fSvgFrame; }
    virtual QSvgWidget *svgWidget() const { return fSvgWidget; }

protected:
    std::size_t numUnsetVariables() const;
    TVariableInfo getVariable( const QString &varName ) const;

protected:
    virtual TVariableInfoList &getVariables();

    virtual void notifyOfNewFormula( const QString &eq, bool baseFormula ) const final;
    virtual void updateFields( QWidget *triggerWidget ) const final;
    virtual QString finalizeFormula( bool imperial, bool seaWater, const QString &formula, bool isBaseFormula ) const final;
    virtual QString finalizeFormula( const QString &formula, bool isBaseFormula ) const final;

    virtual TVariableInfoList getMyVariables() const = 0;
    virtual QString getDefaultFormula() const = 0;
    virtual QString computeAndGenerateFormula() const = 0;   // updates values and returns the formula

    virtual void determineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget ) final;

    virtual TVariableInfo customDetermineVariableToUnset( EVariableLoc updateFromSide, QWidget *triggerWidget, bool preDefaultBehavior );
    // only necessary if the number of variables on either side is greater than two.

protected:
    CSCUBACalculatorPage *fPage{ nullptr };
    QSvgWidget *fSvgWidget{ nullptr };
    QFrame *fSvgFrame{ nullptr };
    TUpdateFormulaFunc fUpdateFormulaFunc;
    TVariableInfoList fLHSVariables;
    TVariableInfoList fRHSVariables;
    TVariableInfoList fGlobalVariables;
    TVariableInfoList fVariables;

    std::unordered_map< QString, TVariableInfo > fVariableMap;
    std::size_t fNumVariables{ 0 };   // if there are constants in the variable list, this value will not equal fVariables.size();
};

extern "C" CALCULATORS_EXPORT CSCUBACalculatorPage *getPage( CSCUBACalculator *calculator, QWidget *parentWidget, bool *needsInit );
extern "C" CALCULATORS_EXPORT void setImperial( CSCUBACalculator *calculator, bool imperial );
extern "C" CALCULATORS_EXPORT void setSeaWater( CSCUBACalculator *calculator, bool seaWater );
extern "C" CALCULATORS_EXPORT void setUpdateFormulaFunc( CSCUBACalculator *calculator, const TUpdateFormulaFunc & );
extern "C" CALCULATORS_EXPORT void initCalculatorPage( CSCUBACalculator *calculator, bool imperial, bool seawater );

#endif
