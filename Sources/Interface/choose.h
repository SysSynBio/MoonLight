#ifndef CHOOSE_H
#define CHOOSE_H

#include <QWidget>

namespace Ui {
class choose;
}

class choose : public QWidget
{
    Q_OBJECT

public:
    explicit choose(QWidget *parent = 0);
    ~choose();

public slots:
    void modelSelected(int mod);
    void expSelected(int exp);
    void subExpSelected(int subexp);
    void dataSetSelected(int ds);



private:
    Ui::choose *ui;
};

#endif // CHOOSE_H
