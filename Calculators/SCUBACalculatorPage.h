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

#ifndef SCUBACALCULATORPAGE_H
#define SCUBACALCULATORPAGE_H

#include "SCUBACalculatorFwd.h"

#include <QWidget>
#include <tuple>

class QSvgWidget;
class QFrame;

class CSCUBACalculatorPage : public QWidget
{
    friend struct SVariableInfo;

public:
    Q_OBJECT;

public:
    Q_PROPERTY( bool showUnits READ showUnits );
    Q_PROPERTY( bool isWaterTypeBased READ isWaterTypeBased );

    CSCUBACalculatorPage( CSCUBACalculator *calculator, QWidget *parent );
    virtual ~CSCUBACalculatorPage();

    static std::tuple< CSCUBACalculatorPage *, QFrame*, QSvgWidget *, std::size_t > constructPage( CSCUBACalculator *calculator, QWidget *parent );

    virtual void init( bool imperial, bool saltWater ) final;

    virtual bool imperial() const final { return fImperial; }
    virtual void setImperial( bool imperial ) final;

    virtual bool saltWater() const final { return fSaltWater; }
    virtual void setSaltWater( bool saltWater ) final;

    virtual bool showUnits() const;
    virtual bool isWaterTypeBased() const;

    virtual bool needsInit() const;

    virtual const CSCUBACalculator *calculator() const final { return fCalculator; }

protected:
    virtual void addWidgets( EVariableLoc varLocation, const std::list< QWidget * > &widgets );
    virtual void addWidget( EVariableLoc varLocation, QWidget *widget );
    virtual void updateValues( QWidget *widget ) final;

private Q_SLOTS:
    void slotWidgetChanged( QWidget * );
Q_SIGNALS:
    void sigUpdateValues();

private:
    virtual CSCUBACalculator *calculator() final { return fCalculator; }

    virtual void setUpdateFromSide( EVariableLoc updateFromSide ) final;
    virtual EVariableLoc updateFromSide() const final { return fUpdateFromSide; }

private:
    bool fNeedsInit{ true };
    bool fImperial{ false };
    bool fSaltWater{ false };
    EVariableLoc fUpdateFromSide{ EVariableLoc::eRHS };
    CSCUBACalculator *fCalculator{ nullptr };
    std::unordered_map< QObject *, EVariableLoc > fVariables;
};

#endif
