#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include "Calculators/ScubaCalculator.h"
#include <QMainWindow>
#include <unordered_map>
#include <memory>
#include <tuple>

namespace Ui
{
    class CMainWindow;
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

public:
Q_SIGNALS:
public Q_SLOTS:
    void slotSelectCalculator( QTreeWidgetItem *item );
    void hideUnits( bool hide );

        private:
    void addCalculator( CSCUBACalculator *calculator, TGetPageFunc getPageFunc, TSetImperialFunc setImperialFunc, TSetMetricFunc setMetricFunc );

private:
    CSCUBACalculator *getCalculator( QTreeWidgetItem *leaf ) const;
    TGetPageFunc getGetPageFunc( QTreeWidgetItem *leaf ) const;
    TSetImperialFunc getSetImperialFunc( QTreeWidgetItem *leaf ) const;
    TSetMetricFunc getSetMetricFunc( QTreeWidgetItem *leaf ) const;

    QTreeWidgetItem *findItem( QTreeWidgetItem *parent, const QStringList &path, bool createIfNecessary );
    std::unique_ptr< Ui::CMainWindow > fImpl;

    QWidget *fBlankPage{ nullptr };
    std::unordered_map< QTreeWidgetItem *, std::tuple< CSCUBACalculator *, TGetPageFunc, TSetImperialFunc, TSetMetricFunc > > fCalculators;
};

#endif
