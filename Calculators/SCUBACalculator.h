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

#ifndef CALCULATORS_EXPORT_H
#define CALCULATORS_EXPORT_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <optional>

#include <unordered_map>

#if defined( WINDOWS ) || defined( WIN32 ) || defined( Q_OS_WINDOWS )
    #ifdef CALCULATORS_INTERNAL
        #define CALCULATORS_EXPORT __declspec( dllexport )
    #else
        #define CALCULATORS_EXPORT __declspec( dllimport )
    #endif
#else
    #define CALCULATORS_EXPORT
#endif

class QWidget;
class CSCUBACalculatorPage;
class CALCULATORS_EXPORT CSCUBACalculator
{
public:
    CSCUBACalculator();
    virtual ~CSCUBACalculator();

    virtual QString calculatorName() const = 0;
    virtual QStringList calculatorPath() const = 0;
    virtual CSCUBACalculatorPage *getPage( QWidget *parent = nullptr ) const final;

    virtual void setImperial( bool imperial ) final;
    virtual void setMetric( bool metric ) final;

    virtual bool imperial() const final;
    virtual bool metric() const final;

protected:
    virtual CSCUBACalculatorPage *constructPage( QWidget *parent ) const = 0;
    mutable CSCUBACalculatorPage *fPage{ nullptr };
};

class CSCUBACalculatorPage : public QWidget
{
    Q_OBJECT;

public:
    CSCUBACalculatorPage( QWidget *parent );
    virtual ~CSCUBACalculatorPage();

    virtual void setImperial( bool imperial ) final;
    virtual void setMetric( bool metric ) final;

    virtual bool imperial() const final { return fImperial; }
    virtual bool metric() const final { return !fImperial; }

    virtual void setUpdateFromRHS( bool updateFromRHS ) final;
    virtual bool updateFromRHS() const final { return fUpdateFromRHS; }

protected:
    virtual void updateValues( QWidget * widget ) = 0;
    virtual void addWidgets( bool rhs, const std::list< QWidget * > &widgets );
    virtual void addWidget( bool rhs, QWidget *widget );

    std::optional< double > getValue( const QString & text ) const;

private Q_SLOTS:
    void slotWidgetChanged( QWidget * );
Q_SIGNALS:
    void sigUnitsChanged();

private:
    bool fImperial{ false };
    bool fUpdateFromRHS{ true };
    std::unordered_map< QObject *, bool > fWidgets;
};

using TInstantiateCalcFunc = CSCUBACalculator *(*)();
using TGetPageFunc = CSCUBACalculatorPage *(*)( CSCUBACalculator *calculator, QWidget * );
using TSetImperialFunc = void ( * )( CSCUBACalculator *calculator, bool );
using TSetMetricFunc = void ( * )( CSCUBACalculator *calculator, bool );

extern "C" CALCULATORS_EXPORT CSCUBACalculatorPage *getPage( CSCUBACalculator *calculator, QWidget *parentWidget );
extern "C" CALCULATORS_EXPORT void setImperial( CSCUBACalculator *calculator, bool imperial );
extern "C" CALCULATORS_EXPORT void setMetric( CSCUBACalculator *calculator, bool metric );

#endif
