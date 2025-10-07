#ifndef BUOYANCY_PAGE_H
#define BUOYANCY_PAGE_H

#include "SCUBACalculatorPage.h"

#include <QWidget>
#include <memory>

namespace Ui
{
    class CPage;
}

class CPage : public CSCUBACalculatorPage
{
    Q_OBJECT
public:
    CPage( CSCUBACalculator *calculator, QWidget *parent = nullptr );
    ~CPage();

public:

Q_SIGNALS:

public Q_SLOTS:
protected:
    virtual void updateValuesInternal( QWidget *widgetChanged );

protected:
private:
    std::unique_ptr< Ui::CPage > fImpl;
};

#endif
