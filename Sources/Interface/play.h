#ifndef PLAY_H
#define PLAY_H

#include <QMainWindow>
#include "graphe2.h"
#include "../Framework/modele.h"
//#include "fitte.h"
#include "../Framework/experiments.h"
#include <vector>
#include <QStandardItemModel>
using namespace std;

namespace Ui {
class Play;
}

class Play : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Play(QWidget *parent = 0);
    ~Play();
    void reset();

    Modele* currentModel;
    Experiments* currentExperiment;
    QStandardItemModel* m;

public slots:
    void simulate();
    void fitte();
    void sensitivity();
    void identifiability();

    void loadOptim(QString _name = QString(""));
    void loadSet(QString dest = QString(""));
    void saveSet();

    void varChanged(int i);

private:
    Ui::Play *ui;
    Graphe2* currentGraphe;
    QStringList AvailModels;
    QStringList AvailDataSets;
    QStringList AvailExperiments;
    QStringList AvailSubExperiments;


};

#endif // PLAY_H
