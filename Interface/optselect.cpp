#include "optselect.h"
#include <sstream>
#include <iostream>
using namespace std;

#ifndef WITHOUT_QT
#include "ui_optselect.h"
optSelect::optSelect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::optSelect)
{
    ui->setupUi(this);


    QObject::connect(ui->pushButtonGenerate, SIGNAL(released()), this, SLOT(generate()));
/*    geneticAlgorithm	14
    A0	int type of algo	0=CEP, 1= SSGA, 2= GGA +10 = all
    A1	setparent policy
    A2	A3	setcrossOverPolicy
    A4 	setMutationPolicy
    A5	setReplacementPolicy
    A6	setSelectPolicy
    A7	A8	setStrategyPolicy
        int num_tries = (int) argument(9);
        int maxCalls = (int) argument(10);
        int popSize = (int) argument(11);
        double propCross = (double) argument(12);
        double forkCoeff = (double) argument(13);
*/
    ui->comboBoxOptimizer->addItem(QString("geneticAlgorithm"));

    ui->comboBox1Algo->addItem(QString("CEP  - Classical Evolutionary Programming"));
    ui->comboBox1Algo->addItem(QString("SSGA - SteadyState Genetic Algorithm"));
    ui->comboBox1Algo->addItem(QString("GGA  - "));
    ui->comboBox1Algo->addItem(QString("CEP  - All Mut&Cross"));
    ui->comboBox1Algo->addItem(QString("SSGA - All Mut&Cross"));
    ui->comboBox1Algo->addItem(QString("GGA  - All Mut&Cross"));

    QStringList parents;
    parents.push_back("Select Best");
    parents.push_back("Rank-based selection");
    parents.push_back("Random selection");
    parents.push_back("Useless");
    parents.push_back("Tournament 5%");
    parents.push_back("Tournament 10%");
    parents.push_back("Tournament 25%");
    parents.push_back("Tournament 50%");
    parents.push_back("Proportional / From Worst / Basic Sampling");
    parents.push_back("Proportional / From Worst / Roulette");
    parents.push_back("Proportional / From Best / Basic Sampling");
    parents.push_back("Proportional / From Best / Roulette");
    parents.push_back("Proportional / No Corr. / Basic Sampling");
    parents.push_back("Proportional / No Corr. / Roulette");
    ui->comboBox2Parents->addItems(parents);

    QStringList crossOver;
    crossOver.push_back("ONE_POINT_CROSSOVER");
    crossOver.push_back("TWO_POINT_CROSSOVER");
    crossOver.push_back("WRIGHT1_CROSSOVER");
    crossOver.push_back("WRIGHT2_CROSSOVER");
    crossOver.push_back("ARITHMETIC_CROSSOVER");
    crossOver.push_back("BLXALPHA_CROSSOVER");
    crossOver.push_back("GEOMETRIC_CROSSOVER");
    crossOver.push_back("SBX_CROSSOVER");
    crossOver.push_back("RANDOMIZE_ONE_POINT_CROSSOVER");
    crossOver.push_back("UNDX_CROSSOVER_3P");
    crossOver.push_back("UNDX_CROSSOVER_4P");
    ui->comboBox3CrossOver->addItems(crossOver);
    //if((idCross >= 4) && (idCross <= 7)){
    //    double param_cross_over = 	-1;
    //}

    QStringList mutation;
    mutation.push_back("Normal - One point");
    mutation.push_back("Normal - All points");
    mutation.push_back("Cauchy - One point");
    mutation.push_back("Cauchy - All points");
    mutation.push_back("Random - One point");
    mutation.push_back("Random - All points");
    mutation.push_back("Exponential - One point");
    mutation.push_back("Exponential - All points");
    mutation.push_back("Combined N+C - One point");
    mutation.push_back("Combined N+C - One point");
    mutation.push_back("Uniform Box - All points");
    mutation.push_back("Uniform Box - All points");
    ui->comboBox4Mutation->addItems(mutation);

    QStringList replacement;
    replacement.push_back("NO_NEED_REPLACEMENT");
    replacement.push_back("REPLACE_WORST_PARENT");
    replacement.push_back("REPLACE_WORST_PARENT_METROPOLIS");
    replacement.push_back("REPLACE_WORST");
    replacement.push_back("REPLACE_RANDOM");
    replacement.push_back("REPLACE_RANDOM_METROPOLIS");
    replacement.push_back("KILL_TOURNAMENT");
    replacement.push_back("REPLACE_OLDEST");
    replacement.push_back("CONSERVATIVE_SELECTION");
    replacement.push_back("ELITIST");
    ui->comboBox5Replace->addItems(replacement);

    QStringList selection;
    selection.push_back("Select Best");
    selection.push_back("Rank-based selection");
    selection.push_back("Random selection");
    selection.push_back("Useless");
    selection.push_back("Tournament 5%");
    selection.push_back("Tournament 10%");
    selection.push_back("Tournament 25%");
    selection.push_back("Tournament 50%");
    selection.push_back("Proportional / From Worst / Basic Sampling");
    selection.push_back("Proportional / From Worst / Roulette");
    selection.push_back("Proportional / From Best / Basic Sampling");
    selection.push_back("Proportional / From Best / Roulette");
    selection.push_back("Proportional / No Corr. / Basic Sampling");
    selection.push_back("Proportional / No Corr. / Roulette");
    ui->comboBox6Selection->addItems(selection);

//    strategy_parameter = d;
//    if(idStrat <= 5) strategy_parameter = 1.0;

    QStringList strategy;
    strategy.push_back("CONSTANT");
    strategy.push_back("EXPONENTIAL_DOWN");
    strategy.push_back("DISTANCE_BEST");
    strategy.push_back("DISTANCE_BEST_SEPARATED");
    strategy.push_back("PROPORTIONAL_NORMALIZED");
    strategy.push_back("LOGNORMAL");
    strategy.push_back("MUTATIVE");
//                if (d < 0) strategy_parameter = 5./100.;
    strategy.push_back("MUTATIVE_SEPARATED");
//                if (d <= 0) strategy_parameter = 1./1000.;
    ui->comboBox7Strategy->addItems(strategy);

    ui->doubleSpinBoxForkCoeff->setMinimum(0.001);
    ui->doubleSpinBoxForkCoeff->setMaximum(100);
    ui->doubleSpinBoxForkCoeff->setDecimals(5);

    ui->doubleSpinBoxParamCrossOver->setMinimum(0.0);
    ui->doubleSpinBoxParamCrossOver->setMaximum(100.0);
    ui->doubleSpinBoxParamCrossOver->setDecimals(5);

    ui->doubleSpinBoxParamStrategy->setMinimum(0.0);
    ui->doubleSpinBoxParamStrategy->setMaximum(10.0);
    ui->doubleSpinBoxParamStrategy->setDecimals(5);

    ui->doubleSpinBoxPropCrossOver->setMinimum(0.0);
    ui->doubleSpinBoxPropCrossOver->setMaximum(1.0);
    ui->doubleSpinBoxPropCrossOver->setDecimals(5);

    ui->spinBoxMaxCosts->setMinimum(10);
    ui->spinBoxMaxCosts->setMaximum(1e9);
    ui->spinBoxNbRepeats->setMinimum(1);
    ui->spinBoxNbRepeats->setMaximum(1e5);
    ui->spinBoxPopSize->setMinimum(10);
    ui->spinBoxPopSize->setMaximum(1e6);

    // basic values
    ui->spinBoxNbRepeats->setValue(1);  // repeats only once
    ui->spinBoxPopSize->setValue(250);  // population size
    ui->spinBoxMaxCosts->setValue(50000);
    ui->doubleSpinBoxForkCoeff->setValue(0.5);
    ui->doubleSpinBoxPropCrossOver->setValue(0.2);
    ui->comboBox1Algo->setCurrentIndex(0); // CEP all
    ui->comboBox2Parents->setCurrentIndex(8); // parents : proportional selection, correction from worst
    ui->comboBox4Mutation->setCurrentIndex(1);
    ui->comboBox3CrossOver->setCurrentIndex(7);
    ui->doubleSpinBoxParamCrossOver->setValue(1);
    ui->comboBox5Replace->setCurrentIndex(0);
    ui->comboBox6Selection->setCurrentIndex(0); // se4lection of population : select the best
    ui->comboBox7Strategy->setCurrentIndex(7);  // mutative each dimension,
    ui->doubleSpinBoxParamStrategy->setValue(0.005); //
}

std::string optSelect::generate(){
    stringstream ss;
    ss << ui->comboBoxOptimizer->currentText().toStdString() << "\t" << 14 << "\n";
    int algo = ui->comboBox1Algo->currentIndex();
    if(algo > 2) algo += 7;
    ss << algo << "\t#" << ui->comboBox1Algo->currentText().toStdString() << endl;
    ss << ui->comboBox2Parents->currentIndex() << "\t#" << ui->comboBox2Parents->currentText().toStdString() << endl;
    ss << ui->comboBox3CrossOver->currentIndex() << "\t" << ui->doubleSpinBoxParamCrossOver->value() << "\t#" << ui->comboBox3CrossOver->currentText().toStdString() << endl;
    ss << ui->comboBox4Mutation->currentIndex() << "\t#" << ui->comboBox4Mutation->currentText().toStdString() << endl;
    ss << ui->comboBox5Replace->currentIndex() << "\t#" << ui->comboBox5Replace->currentText().toStdString() << endl;
    ss << ui->comboBox6Selection->currentIndex() << "\t#" << ui->comboBox6Selection->currentText().toStdString() << endl;
    ss << ui->comboBox7Strategy->currentIndex() << "\t" << ui->doubleSpinBoxParamStrategy->value() <<"\t#" << ui->comboBox7Strategy->currentText().toStdString() <<  endl;
    ss << ui->spinBoxNbRepeats->value() << "\t#Nb Repeats" << endl;
    ss << ui->spinBoxMaxCosts->value() << "\t#Max nb of simulations-costs" << endl;
    ss << ui->spinBoxPopSize->value() << "\t#Population Size" << endl;
    ss << ui->doubleSpinBoxPropCrossOver->value() << "\t#Proportion of CrossOver (vs offspring) in new individuals" << endl;
    ss << ui->doubleSpinBoxForkCoeff->value() << "\t#Fork coeff (%renewed each generation)" << endl;

    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(QString(ss.str().c_str()));
    return ss.str();
}

optSelect::~optSelect()
{
    delete ui;
}

#endif
