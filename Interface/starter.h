#ifndef STARTER_H
#define STARTER_H

#include <QDialog>
#include "../Framework/modele.h"
#include "../Framework/Experiment.h"
#include "simuwin.h"


namespace Ui {
class Starter;
}

class Starter : public QDialog
{
    Q_OBJECT

public:
    explicit Starter(string workingFolder = string(""), QWidget *parent = 0);
    ~Starter();

    vector<Modele*> modelList;
    vector<string> configFilesList;

    Modele* currentModel;
    Experiment* currentExperiment;
    manageSims* manS;
    int nbCombs;

    void addModel(string nameInCombo, Modele* pointerOfCreatedModel, string configFile = string(""));
    void setDefaultModel(string nameInCombo);
    pair<int, float> getFinalChoice();
public slots:
    void modelChanged(int);
    void askNewWorkingFolder();
    void askNewConfigurationFile();
    void askNewParamSet();
    void checkFitteOnlyConf(int newState);
    void checkIdentifOnlyParam(int newState);
    void combToIdentifChanged(int newCombinationValue);
    void radioButtonsChanged();

    void go();
    void quit();

private:
    Modele* modelForTest;
    Ui::Starter *ui;
    bool ready;     // to wait that there is at least one loaded model
    vector<int> parametersInConfig;     // the list of parameters that are in the selected config
};

#endif // STARTER_H
