#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <unordered_map>
#include <memory>
#include <optional>

class QDir;
class QProgressDialog;

#include "Calculators/Core/ScubaCalculatorFwd.h"
namespace Ui
{
    class CMainWindow;
}

namespace NTowel42
{
    class CMathJaxQt6;
    class CMathJaxQt6GroupBox;
}

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
    std::size_t generateSVGs( QProgressDialog *progress, const TFormulaMap &allFormulas, std::size_t numToBeRendered, std::size_t totalFormulas ) const;
    std::pair< std::size_t, std::size_t > computeTotals( QProgressDialog *progress, TFormulaMap &allFormulas ) const;   // returns total, numToBeRendered
    void saveJSONFiles( QProgressDialog *progress, const QDir &dir, const TFormulaMap &allFormulas, bool needUpdatingOnly ) const;

    CSCUBACalculator *currentCalculator() const;
    CSCUBACalculatorPage *currentCalculatorPage() const;

    void setCurrentPage( QTreeWidgetItem *item, CSCUBACalculatorPage *page, bool initPage );
    void loadSettings();
    void saveSettings();

    void loadCache( std::optional< QString > wildCard = {} );
    void loadCacheForCalc( CSCUBACalculator *calc );
    void loadCacheFiles( const QStringList &fileNames );
    std::size_t loadCacheFile( QProgressDialog *progress, const QString &fileName, std::size_t numFiles, std::size_t currSVGNum, std::size_t currTotal );

    void showUnits( bool show );
    void showWaterType( bool show );
    void addCalculator( CSCUBACalculator *calculator );

    void loadFormulasForPage( CSCUBACalculatorPage *page );

    bool renderSVG( const QString &formula );
    std::optional< QString > formulaForPage( QWidget *page );

    void initMathJaxWidgets();
    NTowel42::CMathJaxQt6GroupBox *mathJaxGoupBox() const;
    void setMathJaxWidgetsVisible( bool visible );

private:
    void setFormulaForPage( CSCUBACalculatorPage *page, const QString &formula, bool finished );

    CSCUBACalculator *getCalculator( QTreeWidgetItem *leaf ) const;
    CSCUBACalculator *getCalculator( QWidget *page ) const;
    QTreeWidgetItem *getItemForPage( QWidget *page ) const;

    QTreeWidgetItem *findItem( QTreeWidgetItem *parent, const QStringList &path, bool createIfNecessary );
    std::unique_ptr< Ui::CMainWindow > fImpl;

    QWidget *fBlankPage{ nullptr };

    std::unordered_map< QTreeWidgetItem *, CSCUBACalculator * > fCalculators;
    std::unordered_map< QWidget *, QTreeWidgetItem * > fPageToItem;
    std::unordered_map< QWidget *, std::optional< QString > > fPageToFormulasMap;

    std::shared_ptr< NTowel42::CMathJaxQt6 > fRenderingEngine;
    std::unordered_set< QString > fLoadedCacheFiles;
};

#endif
