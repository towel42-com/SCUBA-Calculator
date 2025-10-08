#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include "Calculators/ScubaCalculator.h"
#include <QMainWindow>
#include <unordered_map>
#include <memory>
#include <optional>

namespace Ui
{
    class CMainWindow;
}

namespace NTowel42
{
    class CQt6MathJax;

}

class CSCUBACalculator;
class QTreeWidgetItem;
class QSvgWidget;
class CMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    CMainWindow( QWidget *parent = 0 );
    ~CMainWindow();

public:
    void loadCalculators();
	
private:
    bool eventFilter( QObject *obj, QEvent *event );

Q_SIGNALS:

public Q_SLOTS:
    void slotSelectCalculator( QTreeWidgetItem *item );
    void slotUnitsChanged();
    void slotWaterChanged();
    void slotFormulaRendered( const QString &formula, const QByteArray &svg );
    void slotResetCurrentPage();

private:
    CSCUBACalculator *currentCalculator() const;
    void setCurrentPage( QTreeWidgetItem *item, CSCUBACalculatorPage *page, bool initPage );
    void loadSettings();
    void saveSettings();
    void showUnits( bool show );
    void showWaterType( bool show );
    void addCalculator( CSCUBACalculator *calculator, TGetPageFunc getPageFunc, TSetBoolFunc setImperialFunc, TSetBoolFunc setSaltWaterFunc, TSetUpdateFormulaFunc setUpdateEqFunc, TInitFunc initFunc );

    void loadFormulasForPage( CSCUBACalculatorPage *page );

    bool renderSVG( const QString &formula );
    std::optional< QString > formulaForPage( QWidget *page, bool baseFormula );

    void loadSVG( const QString &formula, const QByteArray &svg );

    void updateSVGSizes();
    void updateSVGSize( bool defaultFormula );

    void setFormulaForPage( CSCUBACalculatorPage *page, const QString &formula, bool baseFormula );

    CSCUBACalculator *getCalculator( QTreeWidgetItem *leaf ) const;
    CSCUBACalculator *getCalculator( QWidget *page ) const;
    QTreeWidgetItem *getItemForPage( QWidget *page ) const;

    TGetPageFunc getGetPageFunc( QTreeWidgetItem *leaf ) const;
    TSetBoolFunc getSetImperialFunc( QTreeWidgetItem *leaf ) const;
    TSetBoolFunc getSetSaltWaterFunc( QTreeWidgetItem *leaf ) const;
    TInitFunc getInitFunc( QTreeWidgetItem *leaf ) const;

    QTreeWidgetItem *findItem( QTreeWidgetItem *parent, const QStringList &path, bool createIfNecessary );
    std::unique_ptr< Ui::CMainWindow > fImpl;

    QWidget *fBlankPage{ nullptr };

    struct SPageInfo
    {
        CSCUBACalculator *fCalculator{ nullptr };
        TInitFunc fInitFunc{ nullptr };
        TGetPageFunc fGetPageFunc{ nullptr };
        TSetBoolFunc fSetImperialFunc{ nullptr };
        TSetBoolFunc fSetSaltWaterFunc{ nullptr };
    };
    std::unordered_map< QTreeWidgetItem *, SPageInfo > fCalculators;
    std::unordered_map< QWidget *, QTreeWidgetItem * > fPageToItem;
    using TWidgetToFormulaMap = std::unordered_map< QWidget *, QString >;
    TWidgetToFormulaMap fPageToBaseFormulaMap;   //widget -> base --> will be cached
    TWidgetToFormulaMap fPageToResultFormulaMap;   //widget -> current result
    std::unordered_map< QString, QByteArray > fFormulaToSVGMap;
    NTowel42::CQt6MathJax *fRenderingEngine{ nullptr };
    std::pair< QString, QString > fCurrFormulas;
};

#endif
