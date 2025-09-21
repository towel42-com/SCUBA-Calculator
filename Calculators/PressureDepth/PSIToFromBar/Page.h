#ifndef BUOYANCY_PAGE_H
#define BUOYANCY_PAGE_H

#include "SCUBACalculator.h"

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
    CPage( const CSCUBACalculator *calculator, QWidget *parent = 0 );
    ~CPage();

public:
    virtual bool showUnits() const { return false; }

Q_SIGNALS:

public Q_SLOTS:
protected:
    virtual void updateValues( QWidget *widgetChanged ) override;

protected:
private:
    std::unique_ptr< Ui::CPage > fImpl;
};

#endif
