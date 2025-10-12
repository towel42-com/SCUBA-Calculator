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
    class CMathJaxWidget;
}

struct SFormulas
{
    std::optional< QString > formula( EFormulaType formulaType ) const;
    void setFormula( const QString &formula, EFormulaType formulaType );
    std::optional< QString > fBaseFormula;
    std::optional< QString > fCurrFormula;
    std::optional< QString > fCurrValueFormula;
};

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
    void slotResetCurrentPage();
    void slotGenerateAllFormulas();

private:
    CSCUBACalculator *currentCalculator() const;
    CSCUBACalculatorPage *currentCalculatorPage() const;

    void setCurrentPage( QTreeWidgetItem *item, CSCUBACalculatorPage *page, bool initPage );
    void loadSettings();
    void saveSettings();

    void loadCache();

    void showUnits( bool show );
    void showWaterType( bool show );
    void addCalculator( CSCUBACalculator *calculator );

    void loadFormulasForPage( CSCUBACalculatorPage *page );

    bool renderSVG( const QString &formula );
    std::optional< QString > formulaForPage( QWidget *page, EFormulaType formulaType );

private:
    std::optional< QString > formulaForFormulaType( EFormulaType formulaType ) const;
    NTowel42::CMathJaxWidget *mathJaxForFormulaType( EFormulaType formulaType ) const;
    void setMathJaxWidgetsVisible( bool visible );

private:
    void updateSVGSizes();
    void updateSVGSize( EFormulaType formulaType );

    void setFormulaForPage( CSCUBACalculatorPage *page, const QString &formula, EFormulaType formulaType );

    CSCUBACalculator *getCalculator( QTreeWidgetItem *leaf ) const;
    CSCUBACalculator *getCalculator( QWidget *page ) const;
    QTreeWidgetItem *getItemForPage( QWidget *page ) const;

    //TGetPageFunc getGetPageFunc( QTreeWidgetItem *leaf ) const;
    //TSetBoolFunc getSetImperialFunc( QTreeWidgetItem *leaf ) const;
    //TSetBoolFunc getSetSeaWaterFunc( QTreeWidgetItem *leaf ) const;
    //TInitFunc getInitFunc( QTreeWidgetItem *leaf ) const;

    QTreeWidgetItem *findItem( QTreeWidgetItem *parent, const QStringList &path, bool createIfNecessary );
    std::unique_ptr< Ui::CMainWindow > fImpl;

    QWidget *fBlankPage{ nullptr };

    //struct SPageInfo
    //{
    //CSCUBACalculator *fCalculator{ nullptr };
    //TInitFunc fInitFunc{ nullptr };
    //TGetPageFunc fGetPageFunc{ nullptr };
    //TSetBoolFunc fSetImperialFunc{ nullptr };
    //TSetBoolFunc fSetSeaWaterFunc{ nullptr };
    //};
    std::unordered_map< QTreeWidgetItem *, CSCUBACalculator * > fCalculators;
    std::unordered_map< QWidget *, QTreeWidgetItem * > fPageToItem;
    std::unordered_map< QWidget *, SFormulas > fPageToFormulasMap;

    NTowel42::CQt6MathJax *fRenderingEngine{ nullptr };
};

#endif
