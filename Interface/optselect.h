#ifndef OPTSELECT_H
#define OPTSELECT_H

#include "common.h"
#ifndef WITHOUT_QT

#ifdef QT5
#include <QWidget>
#endif
#ifdef QT4
#include <QtGui/QWidget>
#endif
namespace Ui {
class optSelect;
}

class optSelect : public QWidget
{
    Q_OBJECT

public:
    explicit optSelect(QWidget *parent = 0);
    ~optSelect();

public slots:
    std::string generate();

private:
    Ui::optSelect *ui;
};
#endif

#endif // OPTSELECT_H
