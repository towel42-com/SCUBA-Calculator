#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <unordered_map>
#include <memory>
#include <optional>

#include "Calculators/Core/ScubaCalculatorFwd.h"
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
class CMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    CMainWindow( QWidget *parent = 0 );
    ~CMainWindow();

public:
    void loadCalculators();

private:

Q_SIGNALS:

public Q_SLOTS:
    void slotSelectCalculator( QTreeWidgetItem *item );
    void slotUnitsChanged();
    void slotWaterChanged();
    void slotResetCurrentPage();
    void slotGenerateAllFormulas();
    void slotGenerateUpdatedFormulas();

private:
    void generateFormulas( bool updatedOnly );

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
    void setFormulaForPage( CSCUBACalculatorPage *page, const QString &formula, EFormulaType formulaType );

    CSCUBACalculator *getCalculator( QTreeWidgetItem *leaf ) const;
    CSCUBACalculator *getCalculator( QWidget *page ) const;
    QTreeWidgetItem *getItemForPage( QWidget *page ) const;

    QTreeWidgetItem *findItem( QTreeWidgetItem *parent, const QStringList &path, bool createIfNecessary );
    std::unique_ptr< Ui::CMainWindow > fImpl;

    QWidget *fBlankPage{ nullptr };

    std::unordered_map< QTreeWidgetItem *, CSCUBACalculator * > fCalculators;
    std::unordered_map< QWidget *, QTreeWidgetItem * > fPageToItem;
    std::unordered_map< QWidget *, SFormulas > fPageToFormulasMap;

    NTowel42::CQt6MathJax *fRenderingEngine{ nullptr };
};

#endif
