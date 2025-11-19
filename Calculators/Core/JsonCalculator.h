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

#ifndef JSONCALCULATOR_H
#define JSONCALCULATOR_H

#include "CalculatorBase.h"
#include "CalculatorFwd.h"
#include <optional>
#include <list>
#include <QString>
#include <QStringList>
#include <QJsonValue>

class QJsonDocument;
class QJsonObject;
class QJsonValueRef;
class CJsonCalculator : public CCalculatorBase
{
    Q_OBJECT;

    CJsonCalculator( const QString &projectName, const QString &groupName, QObject *parent = nullptr );
    CJsonCalculator( const QString &objName, const QJsonObject &jsonObj, QObject *parent = nullptr );
    CJsonCalculator( const QString &objName, const QJsonValue &jsonObj, QObject *parent = nullptr );

public:
    static CJsonCalculator *create( const QString &projectName, const QString &groupName, QObject *parent );
    static CJsonCalculator *create( const QString &objName, const QJsonValue &jsonObj, QObject *parent );
    static CJsonCalculator *create( const QString &objName, const QJsonObject &jsonObj, QObject *parent );
    static std::optional< std::list< CJsonCalculator * > > create( const QString &jsonFile, QObject *parent, std::optional< QString > &errorMsg );
    virtual ~CJsonCalculator() override;

    bool hasError() const { return fErrorMsg.has_value(); }
    QString errorMsg() const { return fErrorMsg.has_value() ? fErrorMsg.value() : QString(); }

    virtual bool isReversible() const override { return fIsReversible; }
    virtual std::optional< std::pair< QString, QString > > fromToLabels() const override { return fFromToLabels; }

    virtual QStringList myCalculatorPath() const override { return fPath; }
    virtual QStringList myReversedCalculatorPath() const override { return fReversePath; }

    virtual QString myCalculatorName() const override;
    virtual QString myReversedCalculatorName() const override;

    virtual QString calculatorProjectName() const override;
    virtual QString calculatorGroupName() const override;

    virtual bool showUnits() const { return fShowUnits; }
    virtual bool showWaterType() const { return fShowWaterType; }

    virtual TVariableInfoList getMyVariables( bool * /*preReversed*/ ) const override;

    virtual TOptionalFormulaList myBaseFormulas( bool imperial, bool seaWater ) const override;   // for descriptive purposes
    virtual TOptionalFormulaList myReversedBaseFormulas( bool imperial, bool seaWater ) const override;
    virtual TOptionalFormulaList getFormulasForVar( const QString &unsetVar, bool imperial, bool seaWater ) const override;   // returns the current formula in use

    virtual void computeVariableValues() override;   // updates all values

    const QJsonValue &jsonObject() const { return fJsonValue; }

private:
    TVariableInfo findVariable( const QString &var ) const;
    TOptionalFormulaList formulasForBaseFormula( bool imperial, bool seaWater, const std::optional< QString > &formula ) const;

    bool loadJson( const QString &jsonFile );
    bool loadJson();
    static std::optional< QJsonDocument > loadJsonDocument( const QString &jsonFile, std::optional< QString > &errorMsg );

    QString fProjectName;
    QString fGroupName;
    QJsonValue fJsonValue;

    std::optional< QString > fErrorMsg;

    bool fIsReversible{ false };
    std::optional< std::pair< QString, QString > > fFromToLabels;
    std::optional< QString > fName;
    std::optional< QString > fReversedName;
    QStringList fPath;
    QStringList fReversePath;
    bool fShowUnits{ false };
    bool fShowWaterType{ false };
    std::optional< QString > fBaseFormula;
    std::optional< QString > fReversedBaseFormula;

    TVariableInfoList fVariables;
};

#endif
