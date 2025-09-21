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

class QLineEdit;
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
    virtual std::optional< std::vector< std::optional< double > > > compute( const std::vector< std::optional< double > > &values ) const = 0;

    virtual void setImperial( bool imperial ) final;
    virtual void setMetric( bool metric ) final;

    virtual bool imperial() const final;
    virtual bool metric() const final;

    virtual double weightOfWater( bool saltWater ) const final;
    virtual double depthToSingleAtmosphere( bool saltWater ) const final;

    virtual double absZero() const final;
    virtual double pressureOffset() const final;
    virtual double absZeroBasedTemp( double temp ) const final;
    virtual double fromAbsZeroBasedTemp( double temp ) const final;
    virtual double idealGasConstant() const final;
    virtual double pressurePerTemp() const final;
    virtual double percentN2AtSurface() const final;
    virtual double percentO2AtSurface() const final;

    virtual std::size_t numEmpty( const std::vector< std::optional< double > > &values ) const final;
    virtual bool numEmptyOK( const std::vector< std::optional< double > > &values ) const final;

    void calculateDepthToFromPressure( bool saltWater, std::optional< double > &pressure, std::optional< double > &depth ) const;

public:
protected:
    virtual CSCUBACalculatorPage *constructPage( QWidget *parent ) const = 0;
    mutable CSCUBACalculatorPage *fPage{ nullptr };

public:
};

class CSCUBACalculatorPage : public QWidget
{
    Q_OBJECT;

public:
    Q_PROPERTY( bool showUnits READ showUnits );
    CSCUBACalculatorPage( const CSCUBACalculator *calculator, QWidget *parent );
    virtual ~CSCUBACalculatorPage();

    virtual const CSCUBACalculator *calculator() const final { return fCalculator; }

    virtual void setImperial( bool imperial ) final;
    virtual void setMetric( bool metric ) final;

    virtual bool imperial() const final { return fImperial; }
    virtual bool metric() const final { return !fImperial; }

    virtual void setUpdateFromRHS( bool updateFromRHS ) final;
    virtual bool updateFromRHS() const final { return fUpdateFromRHS; }

    virtual void updateValues( QWidget *widget ) = 0;
    virtual void addWidgets( bool rhs, const std::list< QWidget * > &widgets );
    virtual void addWidget( bool rhs, QWidget *widget );

    QString doubleToString( const std::optional< double > &value, int numDecimal ) const;
    std::optional< double > getValue( const QString &text ) const;
    virtual void setValue( QLineEdit *le, const std::optional< double > &value, int numDecimal = 1 );

    virtual QString volumeUnit( bool singular ) const final;
    virtual QString weightUnit( bool singular ) const final;
    virtual QString lengthUnit( bool singular ) const final;
    virtual QString pressureUnit() const final;
    virtual QString pressurePerTemp() const final;
    virtual QString tempUnit( bool absZero ) const final;

    virtual QString weightOfWater( bool seaWater ) const final;
    virtual QString idealGasConstant() const final;

    virtual bool showUnits() const { return true; }
private Q_SLOTS:
    void slotWidgetChanged( QWidget * );
Q_SIGNALS:
    void sigUnitsChanged();

private:
    bool fImperial{ false };
    bool fUpdateFromRHS{ true };

    const CSCUBACalculator *fCalculator{ nullptr };
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
