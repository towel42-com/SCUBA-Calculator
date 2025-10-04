// The MIT License( MIT )
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
#include <variant>
#include <vector>
#include <unordered_map>
#include <functional>

class QLineEdit;

#if defined( WINDOWS ) || defined( WIN32 ) || defined( Q_OS_WINDOWS )
    #ifdef CALCULATORS_INTERNAL
        #define CALCULATORS_EXPORT __declspec( dllexport )
    #else
        #define CALCULATORS_EXPORT __declspec( dllimport )
    #endif
#else
    #define CALCULATORS_EXPORT
#endif

using TVariant = std::variant< bool, double >;
using TOptionalVariant = std::optional< TVariant >;
using TOptionalVariantVector = std::vector< TOptionalVariant >;

class QWidget;
class CSCUBACalculatorPage;
using TUpdateFormulaFunc = std::function< void( CSCUBACalculatorPage *, const QString & ) >;

QString doubleToString( const TOptionalVariant &value, int numDecimal );

class CALCULATORS_EXPORT CSCUBACalculator
{
public:
    CSCUBACalculator();
    virtual ~CSCUBACalculator();

    virtual QString calculatorName() const = 0;
    virtual QStringList calculatorPath() const = 0;
    virtual CSCUBACalculatorPage *getPage( QWidget *parent ) final;
    virtual CSCUBACalculatorPage *getPage() const final;
    virtual std::optional< TOptionalVariantVector > compute( const TOptionalVariantVector &values ) const = 0;
    virtual std::optional< TOptionalVariantVector > setupValues( bool updateFromRHS, std::size_t triggerPos, const TOptionalVariantVector &values ) const = 0;

    virtual void init( bool imperial, bool saltWater ) final;
    virtual void setImperial( bool imperial ) final;
    virtual void setSaltWater( bool saltWater ) final;
    virtual void setUpdateFormulaFunc( const TUpdateFormulaFunc &func ) final;

    virtual bool imperial() const final;
    virtual bool saltWater() const final;

    virtual double weightOfWater( bool saltWater ) const final;
    virtual double depthToSingleAtmosphere( bool saltWater ) const final;

    virtual QString volumeUnit( bool singular, bool tex ) const final;
    virtual QString weightUnit( bool singular, bool tex ) const final;
    virtual QString lengthUnit( bool singular, bool tex ) const final;
    virtual QString pressureUnit( bool tex ) const final;
    virtual QString pressurePerTemp( bool tex ) const final;
    virtual QString tempUnit( bool absZero, bool tex ) const final;

    virtual QString weightOfWater( bool seaWater, bool tex ) const final;
    virtual QString idealGasConstant( bool tex ) const final;

    virtual double absZero() const final;
    virtual double pressureOffset() const final;
    virtual double absZeroBasedTemp( double temp ) const final;
    virtual double fromAbsZeroBasedTemp( double temp ) const final;
    virtual double idealGasConstant() const final;
    virtual double pressurePerTemp() const final;
    virtual double percentN2AtSurface() const final;
    virtual double percentO2AtSurface() const final;

    virtual std::size_t numEmpty( const TOptionalVariantVector &values ) const final;
    virtual bool valuesValid( const TOptionalVariantVector &values, bool checkNumEmpty = true ) const final;

    void calculateDepthToFromPressure( bool saltWater, TOptionalVariant &pressure, TOptionalVariant &depth ) const;

    virtual bool usesSaltWater() const { return false; }

    virtual void updateEquation( const QString &eq ) const final;

public:
protected:
    virtual CSCUBACalculatorPage *constructPage( QWidget *parent ) const = 0;
    CSCUBACalculatorPage *fPage{ nullptr };

private:
    TUpdateFormulaFunc fUpdateEqFunc;
};

class CSCUBACalculatorPage : public QWidget
{
    Q_OBJECT;

public:
    Q_PROPERTY( bool showUnits READ showUnits );
    Q_PROPERTY( bool showWaterType READ showWaterType );
    CSCUBACalculatorPage( const CSCUBACalculator *calculator, QWidget *parent );
    virtual ~CSCUBACalculatorPage();

    virtual const CSCUBACalculator *calculator() const final { return fCalculator; }

    virtual void init( bool imperial, bool saltWater ) final;
    virtual void setImperial( bool imperial ) final;
    virtual void setSaltWater( bool saltWater ) final;

    virtual bool imperial() const final { return fImperial; }
    virtual bool saltWater() const final { return fSaltWater; }

    virtual void setUpdateFromRHS( bool updateFromRHS ) final;
    virtual bool updateFromRHS() const final { return fUpdateFromRHS; }

    virtual void updateValues( QWidget *widget ) final;
    virtual void addWidgets( bool rhs, const std::list< QWidget * > &widgets );
    virtual void addWidget( bool rhs, QWidget *widget );

    TOptionalVariant getValue( const QString &text ) const;
    virtual void setValue( QLineEdit *le, const TOptionalVariant &origValue, const TOptionalVariant &value, int numDecimal = 1 );

    virtual QString volumeUnit( bool singular, bool tex ) const final;
    virtual QString weightUnit( bool singular, bool tex ) const final;
    virtual QString lengthUnit( bool singular, bool tex ) const final;
    virtual QString pressureUnit( bool tex ) const final;
    virtual QString pressurePerTemp( bool tex ) const final;
    virtual QString tempUnit( bool absZero, bool tex ) const final;

    virtual QString weightOfWater( bool seaWater, bool tex ) const final;
    virtual QString idealGasConstant( bool tex ) const final;

    virtual bool showUnits() const { return true; }
    virtual bool showWaterType() const { return false; }

    virtual bool needsInit() const;

private Q_SLOTS:
    void slotWidgetChanged( QWidget * );
Q_SIGNALS:
    void sigUpdateValues();

protected:
    virtual void updateValuesInternal( QWidget *widget ) = 0;
    bool fNeedsInit{ true };
    bool fImperial{ false };
    bool fSaltWater{ false };
    bool fUpdateFromRHS{ true };
    const CSCUBACalculator *fCalculator{ nullptr };
    std::unordered_map< QObject *, bool > fWidgets;
};

using TInstantiateCalcFunc = CSCUBACalculator *(*)();
using TGetPageFunc = CSCUBACalculatorPage *(*)( CSCUBACalculator *calculator, QWidget *, bool *needsInit );
using TSetBoolFunc = void ( * )( CSCUBACalculator *calculator, bool );
using TInitFunc = void ( * )( CSCUBACalculator *calculator, bool imperial, bool saltWater );
using TSetUpdateFormulaFunc = void ( * )( CSCUBACalculator *calculator, const TUpdateFormulaFunc & );

constexpr char kInstantiateCalcFuncName[] = "instantiateCalculator";
constexpr char kGetPageFuncName[] = "getPage";
constexpr char kSetImperialFuncName[] = "setImperial";
constexpr char kSetSaltWaterFuncName[] = "setSaltWater";
constexpr char kSetUpdateFormulaFuncName[] = "setUpdateFormulaFunc";
constexpr char kInitFuncName[] = "initCalculatorPage";

extern "C" CALCULATORS_EXPORT CSCUBACalculatorPage *getPage( CSCUBACalculator *calculator, QWidget *parentWidget, bool *needsInit );
extern "C" CALCULATORS_EXPORT void setImperial( CSCUBACalculator *calculator, bool imperial );
extern "C" CALCULATORS_EXPORT void setSaltWater( CSCUBACalculator *calculator, bool saltWater );
extern "C" CALCULATORS_EXPORT void setUpdateFormulaFunc( CSCUBACalculator *calculator, const TUpdateFormulaFunc & );
extern "C" CALCULATORS_EXPORT void initCalculatorPage( CSCUBACalculator *calculator, bool imperial, bool saltwater );


#endif
