#include "choose.h"
#include "ui_choose.h"

choose::choose(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::choose)
{
    ui->setupUi(this);

    QObject::connect(ui->comboBoxModele, SIGNAL(activated(int)), this, SLOT(modelSelected(int)));
    QObject::connect(ui->comboBoxDataSet, SIGNAL(activated(int)), this, SLOT(dataSetSelected(int)));
    QObject::connect(ui->comboBoxExperiment, SIGNAL(activated(int)), this, SLOT(expSelected(int)));
    QObject::connect(ui->comboBoxSubExperiment, SIGNAL(activated(int)), this, SLOT(subExpSelected(int)));


}


void choose::reset(){
    AvailModels.clear();
    AvailModels.append("Choose a model");
    AvailModels.append("M110 - ");
    AvailModels.append("M111 - ");
    AvailModels.append("M112 - ");
    AvailModels.append("M113 - ");
    AvailModels.append("M114 - ");
    AvailModels.append("M110L - ");
    AvailModels.append("M111L - ");
    AvailModels.append("M112L - ");
    AvailModels.append("M113L - ");
    AvailModels.append("M114L - ");
    AvailModels.append("M1V1 - ");
    AvailModels.append("MThV5genAuto - ");
    ui->comboBoxModele->clear();
    ui->comboBoxModele->addItems(AvailModels);

    AvailDataSets.clear();
    AvailDataSets.append("Choose a dataset");
    AvailDataSets.append("Th1 - Höfer");
    AvailDataSets.append("MyData");
    ui->comboBoxDataSet->clear();
    ui->comboBoxDataSet->addItems(AvailDataSets);

    AvailExperiments.clear();
    AvailExperiments.append("Choose an Experimental Setup");
    AvailExperiments.append("Exp Th1s");
    AvailExperiments.append("Exp ThAlls");
    AvailExperiments.append("My Diffs");
    ui->comboBoxExperiment->clear();
    ui->comboBoxExperiment->addItems(AvailExperiments);

    AvailSubExperiments.clear();

    currentModel = NULL;
    currentExperiment = NULL;
}


void choose::modelSelected(int mod){
    //if(currentModel) delete currentModel; // implement destructor first ...
    currentModel = NULL;

    switch(mod){
    case 1: {currentModel = new Modele110(); loadOptim(QString("C:/Users/parobert/Desktop/Optimisation/ThModelesV3/Modeles/M110.txt")); break;}
    case 2: {currentModel = new Modele111(); loadOptim(QString("C:/Users/parobert/Desktop/Optimisation/ThModelesV3/Modeles/M111.txt")); break;}
    case 3: {currentModel = new Modele112(); loadOptim(QString("C:/Users/parobert/Desktop/Optimisation/ThModelesV3/Modeles/M112.txt")); break;}
    case 4: {currentModel = new Modele113(); loadOptim(QString("C:/Users/parobert/Desktop/Optimisation/ThModelesV3/Modeles/M113.txt")); break;}
    case 5: {currentModel = new Modele114(); loadOptim(QString("C:/Users/parobert/Desktop/Optimisation/ThModelesV3/Modeles/M114.txt")); break;}
    case 6: {currentModel = new Modele110L(); loadOptim(QString("C:/Users/parobert/Desktop/Optimisation/ThModelesV3/Modeles/M110L.txt")); break;}
    case 7: {currentModel = new Modele111L(); loadOptim(QString("C:/Users/parobert/Desktop/Optimisation/ThModelesV3/Modeles/M111L.txt")); break;}
    case 8: {currentModel = new Modele112L(); loadOptim(QString("C:/Users/parobert/Desktop/Optimisation/ThModelesV3/Modeles/M112L.txt")); break;}
    case 9: {currentModel = new Modele113L(); loadOptim(QString("C:/Users/parobert/Desktop/Optimisation/ThModelesV3/Modeles/M113L.txt")); break;}
    case 10:{currentModel = new Modele114L(); loadOptim(QString("C:/Users/parobert/Desktop/Optimisation/ThModelesV3/Modeles/M114L.txt")); break;}
    case 11:{currentModel = new Modele1v1(); break;}
    //case 12:{currentModel = new ModeleThTot5(); break;}
    }
    if(currentModel){
        currentModel->setBaseParameters();
        currentModel->initialise();
        currentModel->print();
        ui->textEditParamSet->clear();
        for(int i = 0; i < currentModel->paramSize(); ++i){
            ui->textEditParamSet->append(QString::number(currentModel->getParam(i)) + QString("\t"));
        }
        ui->comboBoxVariable->clear();
        for(int i = 0; i < currentModel->getNbVars(); ++i){
            ui->comboBoxVariable->addItem(QString(currentModel->getName(i).c_str()));
        }
    }

    /*switch(mod){
    case 1: {loadSet(QString("C:/Users/parobert/Desktop/Optimisation/ThModeles/Modeles/BestSetM110.txt"));break;}
    case 2: {loadSet(QString("C:/Users/parobert/Desktop/Optimisation/ThModeles/Modeles/BestSetM111.txt"));break;}
    case 3: {loadSet(QString("C:/Users/parobert/Desktop/Optimisation/ThModeles/Modeles/BestSetM112.txt"));break;}
    case 4: {break;}
    case 5: {break;}
    case 6: {loadSet(QString("C:/Users/parobert/Desktop/Optimisation/ThModeles/Modeles/BestSetM110L.txt"));break;}
    case 7: {loadSet(QString("C:/Users/parobert/Desktop/Optimisation/ThModeles/Modeles/BestSetM111L.txt"));break;}
    case 8: {loadSet(QString("C:/Users/parobert/Desktop/Optimisation/ThModeles/Modeles/BestSetM112L.txt"));break;}
    case 9: {break;}
    case 10:{break;}
    case 11:{break;}
    case 12:{break;}
    }*/
    dataSetSelected(ui->comboBoxDataSet->currentIndex());   // to update
}

void choose::dataSetSelected(int ds){
    //if(currentExperiment) delete currentExperiment;   // implement destructor first
    currentExperiment = NULL;

    switch(ds){
    case 1:{
        if(currentModel) currentExperiment = new ExpTH1(currentModel);
        //else ui->comboBoxDataSet->setCurrentIndex(0);
    }

    }

    if(currentExperiment){
        int nb = currentExperiment->nbExp();
        cerr << "       Nb Exps : " << nb << endl;
        ui->comboBoxExperiment->clear();
        for(int i = 0; i < nb; ++i){
            if(currentExperiment->isDoable(i)){
                ui->comboBoxExperiment->addItem(currentExperiment->expName(i).c_str());
                cerr << "   - " << currentExperiment->expName(i).c_str() << endl;
            } else {
                cerr << "   X " << currentExperiment->expName(i).c_str() << endl;
            }
        }

    }
}

void choose::expSelected(int exp){}
void choose::subExpSelected(int subexp){}

choose::~choose()
{
    delete ui;
}
