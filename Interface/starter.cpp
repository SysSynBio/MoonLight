#include "starter.h"
#include "ui_starter.h"
#include "simuwin.h"
#include <QFileDialog>

Starter::Starter(string workingFolder, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Starter)
{
    ready = false;
    ui->setupUi(this);
    currentModel =      NULL;
    currentExperiment = NULL;
    manS =              NULL;
    ui->plainTextEditWorkingFolder->setPlainText(QString(workingFolder.c_str()));
    if(workingFolder.size() < 1) ui->plainTextEditWorkingFolder->setPlainText(QDir::currentPath());
    ui->radioButton6CanoDiff->setChecked(true);

    QObject::connect(ui->comboBoxModel,                         SIGNAL(currentIndexChanged(int)),   this, SLOT(modelChanged(int)));
    QObject::connect(ui->pushButtonWorkingFolderChange,         SIGNAL(released()),                 this, SLOT(askNewWorkingFolder()));
    QObject::connect(ui->pushButtonConfigurationChange,         SIGNAL(released()),                 this, SLOT(askNewConfigurationFile()));
    QObject::connect(ui->pushButtonParameterSetChange,          SIGNAL(released()),                 this, SLOT(askNewParamSet()));
    QObject::connect(ui->checkBoxFitOnlyComb,                   SIGNAL(stateChanged(int)),          this, SLOT(checkFitteOnlyConf(int)));
    QObject::connect(ui->checkBoxIdentifOnlyParam,              SIGNAL(stateChanged(int)),          this, SLOT(checkIdentifOnlyParam(int)));
    QObject::connect(ui->spinBoxCombToIdentif,                  SIGNAL(valueChanged(int)),          this, SLOT(combToIdentifChanged(int)));


    QObject::connect(ui->radioButton1CanoFitPerComb,            SIGNAL(toggled(bool)),              this, SLOT(radioButtonsChanged()));
    QObject::connect(ui->radioButton2CanoFitAround,             SIGNAL(toggled(bool)),              this, SLOT(radioButtonsChanged()));
    QObject::connect(ui->radioButton3CanoIdentifPerComb,        SIGNAL(toggled(bool)),              this, SLOT(radioButtonsChanged()));
    QObject::connect(ui->radioButton4CanoIdentifAround,         SIGNAL(toggled(bool)),              this, SLOT(radioButtonsChanged()));
    QObject::connect(ui->radioButton5Sensitiv,                  SIGNAL(toggled(bool)),              this, SLOT(radioButtonsChanged()));
    QObject::connect(ui->radioButton6CanoDiff,                  SIGNAL(toggled(bool)),              this, SLOT(radioButtonsChanged()));
    QObject::connect(ui->radioButton7SwitchMedium,              SIGNAL(toggled(bool)),              this, SLOT(radioButtonsChanged()));
    QObject::connect(ui->radioButton8TitrCytok,                 SIGNAL(toggled(bool)),              this, SLOT(radioButtonsChanged()));
    QObject::connect(ui->radioButton9TitrCellDensity,           SIGNAL(toggled(bool)),              this, SLOT(radioButtonsChanged()));
    QObject::connect(ui->radioButtonUseParameterDefault,        SIGNAL(toggled(bool)),              this, SLOT(radioButtonsChanged()));
    QObject::connect(ui->radioButtonUseParameterFromFile,       SIGNAL(toggled(bool)),              this, SLOT(radioButtonsChanged()));
    QObject::connect(ui->plainTextEditParameterSet,             SIGNAL(textChanged()),              this, SLOT(radioButtonsChanged()));

    QButtonGroup* mainChoice = new QButtonGroup(this);
    mainChoice->addButton(ui->radioButton1CanoFitPerComb);
    mainChoice->addButton(ui->radioButton2CanoFitAround);
    mainChoice->addButton(ui->radioButton3CanoIdentifPerComb);
    mainChoice->addButton(ui->radioButton4CanoIdentifAround);
    mainChoice->addButton(ui->radioButton5Sensitiv);
    mainChoice->addButton(ui->radioButton6CanoDiff);
    mainChoice->addButton(ui->radioButton6CanoDiffAB);
    mainChoice->addButton(ui->radioButton6CanoDiffJ4);
    mainChoice->addButton(ui->radioButton7SwitchMedium);
    mainChoice->addButton(ui->radioButton8TitrCytok);
    mainChoice->addButton(ui->radioButton9TitrCellDensity);
    mainChoice->addButton(ui->radioButton10MultiObjective);


    ui->buttonBox->setEnabled(true);
    ui->labelPleaseSelectParamSet->hide();
    ui->radioButtonUseParameterFromFile->setChecked(true);


    // Predefined options for Th differentiation
    QStringList DiffFrom;   DiffFrom << QString("Th1") << QString("Th2") << QString("iTreg") << QString("Th17") << QString("Th0");
    ui->comboBoxConditionToSwitchFrom->addItems(DiffFrom);
    ui->doubleSpinBoxTimeSwitch->setValue(20);
    /*cerr << "A).   Changing differentiation condition in the middle (default p = 20 hrs) : " << endl;
    cerr << "- 1 :         From Th1,   change to other ones at t = p hrs   " << endl;
    cerr << "- 2 :         From Th2,   change to other ones at t = p hrs   " << endl;
    cerr << "- 3 :         From iTreg, change to other ones at t = p hrs   " << endl;
    cerr << "- 4 :         From Th17,  change to other ones at t = p hrs   " << endl;
    cerr << "- 5 :         From Th0,   change to other ones at t = p hrs   " << endl;*/

    ui->comboBoxConditionForCellDensity->addItems(DiffFrom);
    /*cerr << "B).   Titrations of cell density in the well : " << endl;
    cerr << "- 10 :        Th1   diff, Different Densities of cells               " << endl;
    cerr << "- 11 :        Th2   diff, Different Densities of cells               " << endl;
    cerr << "- 12 :        iTreg diff, Different Densities of cells               " << endl;
    cerr << "- 13 :        Th17  diff, Different Densities of cells               " << endl;
    cerr << "- 14 :        Th10  diff, Different Densities of cells               " << endl;*/

    ui->comboBoxConditionForCytokTitration->addItems(DiffFrom);
    QStringList Cytoks;   Cytoks << QString("IL2") << QString("IL4") << QString("IL6") << QString("IL12") << QString("IL17")  << QString("IL21")  << QString("IFNg");
    ui->comboBoxCytokToTitrate->addItems(Cytoks);
    /*cerr << "C).   Titrations of cytokines : " << endl;
    cerr << "- 20 :        Th1,  Doses of IL2                         " << endl;
    cerr << "- 21 :        Th1,  Doses of IL4                         " << endl;
    cerr << "- 22 :        Th1,  Doses of IL6                         " << endl;
    cerr << "- 23 :        Th1,  Doses of IL12                        " << endl;
    cerr << "- 24 :        Th1,  Doses of IL17                        " << endl;
    cerr << "- 25 :        Th1,  Doses of IL21                        " << endl;
    cerr << "- 26 :        Th1,  Doses of IFNG                        " << endl;
    cerr << "" << endl;
    cerr << "- 30 :        Th2,  Doses of IL2                         " << endl;
    cerr << "- 31 :        Th2,  Doses of IL4                         " << endl;
    cerr << "- 32 :        Th2,  Doses of IL6                         " << endl;
    cerr << "- 33 :        Th2,  Doses of IL12                        " << endl;
    cerr << "- 34 :        Th2,  Doses of IL17                        " << endl;
    cerr << "- 35 :        Th2,  Doses of IL21                        " << endl;
    cerr << "- 36 :        Th2,  Doses of IFNG                        " << endl;
    cerr << "" << endl;
    cerr << "- 40 :        iTreg,  Doses of IL2                         " << endl;
    cerr << "- 41 :        iTreg,  Doses of IL4                         " << endl;
    cerr << "- 42 :        iTreg,  Doses of IL6                         " << endl;
    cerr << "- 43 :        iTreg,  Doses of IL12                        " << endl;
    cerr << "- 44 :        iTreg,  Doses of IL17                        " << endl;
    cerr << "- 45 :        iTreg,  Doses of IL21                        " << endl;
    cerr << "- 46 :        iTreg,  Doses of IFNG                        " << endl;
    cerr << "" << endl;
    cerr << "- 50 :        Th17,  Doses of IL2                         " << endl;
    cerr << "- 51 :        Th17,  Doses of IL4                         " << endl;
    cerr << "- 52 :        Th17,  Doses of IL6                         " << endl;
    cerr << "- 53 :        Th17,  Doses of IL12                        " << endl;
    cerr << "- 54 :        Th17,  Doses of IL17                        " << endl;
    cerr << "- 55 :        Th17,  Doses of IL21                        " << endl;
    cerr << "- 56 :        Th17,  Doses of IFNG                        " << endl;
    cerr << "" << endl;
    cerr << "- 60 :        Th0,  Doses of IL2                         " << endl;
    cerr << "- 61 :        Th0,  Doses of IL4                         " << endl;
    cerr << "- 62 :        Th0,  Doses of IL6                         " << endl;
    cerr << "- 63 :        Th0,  Doses of IL12                        " << endl;
    cerr << "- 64 :        Th0,  Doses of IL17                        " << endl;
    cerr << "- 65 :        Th0,  Doses of IL21                        " << endl;
    cerr << "- 66 :        Th0,  Doses of IFNG                        " << endl;
    cerr << "" << endl;
    cerr << "- 100 :       Perform all with various parameters and saves all simulations       " << endl;*/
}

Starter::~Starter()
{
    delete ui;
}

void Starter::addModel(string nameInCombo, Modele *pointerOfCreatedModel, string configFile){
    if(!pointerOfCreatedModel) {cerr << "ERR: Starter::addModel, NULL model given. Should be created before.\n"; return;}
    ui->comboBoxModel->addItem(QString(nameInCombo.c_str()));
    modelList.push_back(pointerOfCreatedModel);
    configFilesList.push_back(configFile);
    //if(!ready) {
        QObject::disconnect(ui->comboBoxModel,                         SIGNAL(currentIndexChanged(int)),   this, SLOT(modelChanged(int)));
        ready = true;
        cout << "   -> Checking the config file given for model " << nameInCombo << endl;
        modelChanged(modelList.size() -1);
        ui->comboBoxModel->setCurrentIndex(modelList.size() -1);
        QObject::connect(ui->comboBoxModel,                         SIGNAL(currentIndexChanged(int)),   this, SLOT(modelChanged(int)));
    //}
}

void Starter::setDefaultModel(string nameInCombo){
    cout << "\n   Default model set to " << nameInCombo << endl;
    for(int i = 0; i < (int) modelList.size(); ++i){
        if(!(ui->comboBoxModel->itemText(i).toStdString().compare(nameInCombo))) {
            modelChanged(i);
            ui->comboBoxModel->setCurrentIndex(i);
            return;
        }
    }
}

pair<int, float> Starter::getFinalChoice(){
    if(ui->radioButton6CanoDiff->isChecked())
        return pair<int, float>((int) ui->comboBoxModel->currentIndex(), (float) -1.0);
    if(ui->radioButton6CanoDiffAB->isChecked())
        return pair<int, float>((int) 19, (float) 0.0);
    if(ui->radioButton6CanoDiffJ4->isChecked())
        return pair<int, float>((int) 18, (float) 0.0);

    //if(ui->radioButton1->isChecked())

    if(ui->radioButton7SwitchMedium->isChecked())
        return pair<int, float>(ui->comboBoxConditionToSwitchFrom->currentIndex()+1 + 5*(ui->comboBoxConditionFromOrTo->currentIndex()), (float)  ui->doubleSpinBoxTimeSwitch->value() );
    /* QStringList DiffFrom;   DiffFrom << QString("Th1") << QString("Th2") << QString("iTreg") << QString("Th17") << QString("Th0");
    ui->comboBoxConditionToSwitchFrom->addItems(DiffFrom);
    ui->doubleSpinBoxTimeSwitch->setValue(20);
    cout << "A).   Changing differentiation condition in the middle (default p = 20 hrs) : " << endl;
    cout << "- 1 :         From Th1,   change to other ones at t = p hrs   " << endl;
    cout << "- 2 :         From Th2,   change to other ones at t = p hrs   " << endl;
    cout << "- 3 :         From iTreg, change to other ones at t = p hrs   " << endl;
    cout << "- 4 :         From Th17,  change to other ones at t = p hrs   " << endl;
    cout << "- 5 :         From Th0,   change to other ones at t = p hrs   " << endl;
    cout << "- 6 :         To Th1,   change to other ones at t = p hrs   " << endl;
    cout << "- 7 :         To Th2,   change to other ones at t = p hrs   " << endl;
    cout << "- 8 :         To iTreg, change to other ones at t = p hrs   " << endl;
    cout << "- 9 :         To Th17,  change to other ones at t = p hrs   " << endl;
    cout << "- 10 :        To Th0,   change to other ones at t = p hrs   " << endl;*/


    if(ui->radioButton9TitrCellDensity->isChecked())
        return pair<int, float>(11 +ui->comboBoxConditionForCellDensity->currentIndex(), (float)  0.0 );
    /* ui->comboBoxConditionForCellDensity->addItems(DiffFrom);
    cerr << "B).   Titrations of cell density in the well : " << endl;
    cout << "- 11 :        Th1   diff, Different Densities of cells               " << endl;
    cout << "- 12 :        Th2   diff, Different Densities of cells               " << endl;
    cout << "- 13 :        iTreg diff, Different Densities of cells               " << endl;
    cout << "- 14 :        Th17  diff, Different Densities of cells               " << endl;
    cout << "- 15 :        Th10  diff, Different Densities of cells               " << endl;*/

    if(ui->radioButton8TitrCytok->isChecked())
        return pair<int, float>(20 + 10*ui->comboBoxConditionForCytokTitration->currentIndex()+ui->comboBoxCytokToTitrate->currentIndex(), (float) 0.0);
    /* ui->comboBoxConditionForCytokTitration->addItems(DiffFrom);
    cerr << "C).   Titrations of cytokines : " << endl;
    cerr << "- 20 :        Th1,  Doses of IL2                         " << endl;
    cerr << "- 21 :        Th1,  Doses of IL4                         " << endl;
    cerr << "- 22 :        Th1,  Doses of IL6                         " << endl;
    cerr << "- 23 :        Th1,  Doses of IL12                        " << endl;
    cerr << "- 24 :        Th1,  Doses of IL17                        " << endl;
    cerr << "- 25 :        Th1,  Doses of IL21                        " << endl;
    cerr << "- 26 :        Th1,  Doses of IFNG                        " << endl;
    cerr << "" << endl;
    cerr << "- 30 :        Th2,  Doses of IL2                         " << endl;
    cerr << "- 31 :        Th2,  Doses of IL4                         " << endl;
    cerr << "- 32 :        Th2,  Doses of IL6                         " << endl;
    cerr << "- 33 :        Th2,  Doses of IL12                        " << endl;
    cerr << "- 34 :        Th2,  Doses of IL17                        " << endl;
    cerr << "- 35 :        Th2,  Doses of IL21                        " << endl;
    cerr << "- 36 :        Th2,  Doses of IFNG                        " << endl;
    cerr << "" << endl;
    cerr << "- 40 :        iTreg,  Doses of IL2                         " << endl;
    cerr << "- 41 :        iTreg,  Doses of IL4                         " << endl;
    cerr << "- 42 :        iTreg,  Doses of IL6                         " << endl;
    cerr << "- 43 :        iTreg,  Doses of IL12                        " << endl;
    cerr << "- 44 :        iTreg,  Doses of IL17                        " << endl;
    cerr << "- 45 :        iTreg,  Doses of IL21                        " << endl;
    cerr << "- 46 :        iTreg,  Doses of IFNG                        " << endl;
    cerr << "" << endl;
    cerr << "- 50 :        Th17,  Doses of IL2                         " << endl;
    cerr << "- 51 :        Th17,  Doses of IL4                         " << endl;
    cerr << "- 52 :        Th17,  Doses of IL6                         " << endl;
    cerr << "- 53 :        Th17,  Doses of IL12                        " << endl;
    cerr << "- 54 :        Th17,  Doses of IL17                        " << endl;
    cerr << "- 55 :        Th17,  Doses of IL21                        " << endl;
    cerr << "- 56 :        Th17,  Doses of IFNG                        " << endl;
    cerr << "" << endl;
    cerr << "- 60 :        Th0,  Doses of IL2                         " << endl;
    cerr << "- 61 :        Th0,  Doses of IL4                         " << endl;
    cerr << "- 62 :        Th0,  Doses of IL6                         " << endl;
    cerr << "- 63 :        Th0,  Doses of IL12                        " << endl;
    cerr << "- 64 :        Th0,  Doses of IL17                        " << endl;
    cerr << "- 65 :        Th0,  Doses of IL21                        " << endl;
    cerr << "- 66 :        Th0,  Doses of IFNG                        " << endl;
    cerr << "" << endl;
    cerr << "- 100 :       Perform all with various parameters and saves all simulations       " << endl;*/

    if(ui->radioButton10MultiObjective->isChecked())
        return pair<int, float>(-2, -1.0);

    return pair<int, float>(-1, -1.0);
}

void Starter::modelChanged(int newComboIndex){
    if(!ready) return;  // don't know why but this function is called at first run
    if((newComboIndex < 0) || (newComboIndex >= (int) modelList.size())){cerr << "Err: starter::modelChanged(" << newComboIndex << "), the combo index is out of bounds of modelList" << endl; return;}
    if(modelList.size() != configFilesList.size()) cerr << "Err : different sizes between modelList and configFilesList" << endl;
    currentModel = modelList[newComboIndex];
    //cout << "\n   -> Model changed for " << ui->comboBoxModel->itemText(newComboIndex).toStdString() << endl;
    if(!currentModel) cerr << "Err: Starter::modelChanged, NULL model after selection" << endl;
    ui->plainTextEditConfiguration->setPlainText(QString(configFilesList[newComboIndex].c_str()));
    delete manS;
    delete currentExperiment;
    currentExperiment = new Experiment(currentModel, 0);
    manS = new manageSims(currentExperiment);
    cerr << manS->loadConfig(ui->plainTextEditWorkingFolder->toPlainText().toStdString() + ui->plainTextEditConfiguration->toPlainText().toStdString());
    nbCombs = manS->nbCombs;
    if(nbCombs > 0) ui->spinBoxCombToIdentif->setMaximum(nbCombs-1);
    else ui->spinBoxCombToIdentif->setMaximum(0);
    ui->spinBoxCombToIdentif->setMinimum(0);
    ui->spinBoxCombToFit->setMaximum(nbCombs-1);
    ui->spinBoxCombToFit->setMinimum(0);
    // restores the initial checking values
    ui->spinBoxCombToFit->setEnabled(false);
    ui->spinBoxCombToIdentif->setValue(0);
    ui->checkBoxFitOnlyComb->setChecked(false);
    ui->comboBoxParamToIdentif->setEnabled(false);
    ui->checkBoxIdentifOnlyParam->setChecked(false);
    if(nbCombs > 0) combToIdentifChanged(0);
    ui->checkBoxFitOnlyComb->setEnabled(nbCombs>0);
    //ui->radioButton1CanoFitPerComb->setChecked(true);
    //cerr << "   Modele loaded" << endl;
}

void Starter::radioButtonsChanged(){
    bool showButtons = true;
    bool enableParamWidget = false;

    if(ui->radioButton2CanoFitAround->isChecked()){
        showButtons = (ui->radioButtonUseParameterDefault->isChecked() || (ui->plainTextEditParameterSet->toPlainText().size() > 0));
        enableParamWidget = true;
    }

    if(ui->radioButton4CanoIdentifAround->isChecked()){
        showButtons = (ui->radioButtonUseParameterDefault->isChecked() || (ui->plainTextEditParameterSet->toPlainText().size() > 0));
        enableParamWidget = true;
    }

    if(ui->radioButton5Sensitiv->isChecked()){
        showButtons = (ui->radioButtonUseParameterDefault->isChecked() || (ui->plainTextEditParameterSet->toPlainText().size() > 0));
        enableParamWidget = true;
    }

    ui->widgetParameterSet->setEnabled(enableParamWidget);
    ui->buttonBox->setEnabled(showButtons);
    ui->labelPleaseSelectParamSet->setHidden(showButtons);
}

void Starter::askNewWorkingFolder(){
    QString folder = QFileDialog::getExistingDirectory(this, QString("Please select a working folder."));
    ui->plainTextEditWorkingFolder->setPlainText(folder);
}

void Starter::askNewConfigurationFile(){
    QString confFile = QFileDialog::getOpenFileName(this, QString("Please select a configuration file."), ui->plainTextEditWorkingFolder->toPlainText());
    ui->plainTextEditConfiguration->setPlainText(confFile);
}

void Starter::askNewParamSet(){
    QString paramSetFile = QFileDialog::getOpenFileName(this, QString("Please select a parameter set file (syntax : NB_PARAMS\\nP1\\tP2\\t...."), ui->plainTextEditWorkingFolder->toPlainText());
    ui->plainTextEditParameterSet->setPlainText(paramSetFile);
}

void Starter::checkFitteOnlyConf(int newState){
    if(newState) ui->labelFittingAroundBestSet->hide(); else ui->labelFittingAroundBestSet->show();
    ui->spinBoxCombToFit->setEnabled(newState != 0);
}

void Starter::checkIdentifOnlyParam(int newState){
    ui->comboBoxParamToIdentif->setEnabled(newState != 0);
}

void Starter::combToIdentifChanged(int newCombinationValue){
    if((newCombinationValue < 0) || (newCombinationValue >= nbCombs)) {cerr << "ERR: Starter::combToIdentifChanged(" << newCombinationValue << "), out of bounds index of combination to change\n"; return;}
    ui->comboBoxParamToIdentif->clear();
    parametersInConfig = manS->parametersInConfig(newCombinationValue);
    for(int i = 0; i < (int) parametersInConfig.size(); ++i){
        ui->comboBoxParamToIdentif->addItem(QString::number(parametersInConfig[i]) + QString("->") + QString(currentModel->getParamName(parametersInConfig[i]).c_str()));
    }
}

void Starter::go(){

}

void Starter::quit(){

}
