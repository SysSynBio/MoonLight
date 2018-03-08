#include "play.h"
#include "ui_play.h"
#include <QFileDialog>


// Main idea : being able to choose a model and an experiment,
// , then the experiment class can say what experiments can be performed

Play::Play(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Play)
{
    ui->setupUi(this);
    currentGraphe = new Graphe2(ui->widgetGraphe);
    currentGraphe->Example();


    QObject::connect(ui->pushButtonGo, SIGNAL(released()), this, SLOT(simulate()));
    QObject::connect(ui->pushButtonOptimize, SIGNAL(released()), this, SLOT(fitte()));
    QObject::connect(ui->pushButtonSensitivity, SIGNAL(released()), this, SLOT(sensivity()));
    QObject::connect(ui->pushButtonIdentifiability, SIGNAL(released()), this, SLOT(identifiability()));

    QObject::connect(ui->pushButtonLoadOpt, SIGNAL(released()), this, SLOT(loadOptim()));
    QObject::connect(ui->pushButtonLoadSet, SIGNAL(released()), this, SLOT(loadSet()));
    QObject::connect(ui->comboBoxVariable, SIGNAL(activated(int)), this, SLOT(varChanged(int)));


    reset();
}

void Play::reset(){
    ui->textEditOptimizerFile->clear();
}

void Play::simulate(){
    if(currentExperiment){
        // initialize is done by simulationsPart
        currentExperiment->m->setPrintMode(true, 20);
        currentExperiment->simulate(ui->comboBoxExperiment->currentIndex()); //dangereux le 'currentIndex' !!
    }
    varChanged(ui->comboBoxVariable->currentIndex());
   /* if(currentExperiment && (currentExperiment->m->getCinetique())){
        QString title = QString("Modele: ") + ui->comboBoxModele->currentText() + QString("Modele: ") + ui->comboBoxDataSet->currentText() + QString("Modele: ") + ui->comboBoxExperiment->currentText();
        currentExperiment->m->getCinetique()->save(string("C:/Users/parobert/Desktop/Optimisation/ThModeles/Modeles/SimulOutput.txt"), title.toStdString());
    }*/

}

void Play::varChanged(int i){
    cerr << "var changed " << endl;
    /*if(currentExperiment && currentExperiment->m->cinetique){
        currentGraphe->Plot(currentExperiment->m->cinetique->getTimeCourse(i), currentExperiment->m->cinetique->getTimePoints(), QString(""));
    }*/
}

void Play::loadOptim(QString _name){

     if(!_name.compare(QString(""))){
             _name = QFileDialog::getOpenFileName(this);
     }
     QFile file(_name);
     file.open(QFile::ReadOnly | QFile::Text);

     QTextStream ReadFile(&file);
     ui->textEditOptimizerFile->setText(ReadFile.readAll());

     cerr << "Reading file " << _name.toStdString() << endl;
     ifstream fichier(_name.toStdString().c_str(), ios::in);
     if(!fichier) cerr << "file not found\n" << endl;
     fichier.close();
}

void Play::fitte(){
    ofstream fichier("C:/Users/parobert/Desktop/Optimisation/build-ThModeles-Desktop_Qt_5_3_0_MinGW_32bit-Release/RunningModel.txt", ios::out);
    fichier << ui->textEditOptimizerFile->toPlainText().toStdString();
    fichier.close();
    if(currentExperiment) {
        currentExperiment->m->setPrintMode(false, 20);
        //Optimize(string("C:/Users/parobert/Desktop/Optimisation/build-ThModeles-Desktop_Qt_5_3_0_MinGW_32bit-Release/RunningModel.txt"), currentExperiment );
    }
}

void Play::loadSet(QString dest){
    if(currentModel) {
         if(!dest.compare(QString(""))) dest = QFileDialog::getOpenFileName(this,"Open ...",QDir::currentPath().toStdString().c_str(), tr("Text Files (*.txt)"));
         currentModel->loadParameters(dest.toStdString());
         ui->textEditParamSet->clear();
         QString res = QString("");
         for(int i = 0; i < currentModel->paramSize(); ++i){
             res.append(QString::number(currentModel->getParam(i)) + QString("\t"));
         }
         ui->textEditParamSet->append(res);
         // Experiments fera le initialize
    }
}

Play::~Play()
{
    delete ui;
}


void Play::sensitivity(){}
void Play::identifiability(){}

void Play::saveSet(){}

