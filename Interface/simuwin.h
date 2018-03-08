#ifndef SIMUWIN_H
#define SIMUWIN_H


/// This is the main interface to perform complex tasks with a model/experiment.
/// The job is separated into two main classes :
///
/// - manageSims, which is independent of any graphical library.
///   it can perform simulations, optimizations, load configuration files, saving results as tables, etc.
///
/// - simuWin, which is extending manageSims with a graphical QT interface.
///   some actions are only possible with simuWin, such as directly saving simulations as plots.
///
///     As Qt doesn't support plotting graphs, two additional libraries are proposed(one is enough)
///     - Qwt           (older one, incompatible between different versions --> often raises compiling errors)
///     - QCustomPlot   (more recent, makes beautiful plots, a bit slower, might cause trouble if plots a whole optimization)
///
///     The simplest way to use simuWin is given here :

/* Here is A minimum piece of code to run the graphical interface from a model/experiment. Note :QT should be allowed (WITHOUT_QT)

#include "common.h"

#ifndef WITHOUT_QT
#ifdef QT5
#include <QMainWindow>
#include <QApplication>
#endif
#ifdef QT4
#include <QtGui/QMainWindow>
#include <QtGui/QApplication>
#endif

#include "simuwin.h"
#include "../Framework/modele.h"
#include "../Framework/Experiment.h"
#include "../THDiff/expThs.h"                                   // will also include the names for Th
#include "../THDiff/M6a-LatentTbet2/modeleLatentTbet2.h"

int anOldMain(int argc, char *argv[])
{
    QApplication b(argc, argv);
    Modele* currentModel = new modeleLatentTbet2();
    Experiment* currentExperiment = new expThs(currentModel);
    simuWin* p = new simuWin(currentExperiment);
    p->show();
    return b.exec();
    return 0;
}
#endif  */


// In case you want to compile without using the optimizer.
#define ALLOW_OPTIMIZE

#include "../Framework/modele.h"
#include "../Framework/tableCourse.h"
#include "../Framework/Experiment.h"
#include "../Framework/parameterSets.h"

#include <vector>
#include <sstream>
#include <stdlib.h>     // for atof
using namespace std;

#include "common.h"
#ifndef WITHOUT_QT
#ifdef USE_QWT
#include "graphe2.h"
#endif
#ifdef USE_QCP
#include "grapheCustom.h"
#endif
#ifdef QT5
#include <QMainWindow>
#include <QStandardItemModel>
#include <QWidget>
#include <QListWidget>
#include <QTableWidget>
#endif
#ifdef QT4
#include <QtGui/QMainWindow>
#include <QtGui/QStandardItemModel>
#include <QtGui/QWidget>
#include <QtGui/QListWidget>
#include <QtGui/QTableWidget>
#endif
#endif

// penser à  update model from table after load
#ifdef ALLOW_OPTIMIZE
#include "optselect.h"
#include "../Extreminator/optimizers/baseOptimizer.h"
#include "../Extreminator/common/myRandom.h"
#include "../Extreminator/baseProblem.h"
#include "../Extreminator/optimizers/baseOptimizer.h"
#endif


/// Data structure to track the evolution of a variable with time, such as the cost during the optimization.
/// In order to avoid reallocating a bigger table every time, it records by blocs (of size 'SizeGroups'),
/// and everytime a block is finished, computes min, max, mean and stddev of this block and starts a new block

struct evolution {
    evolution(int _sizeGroups);

private:
    int sizeGroups;
    int currentGroup;
    int nbInCurrentGroup;
    vector< vector<double>* > tables;

public:
    void clear();
    void newValue(double _v);
    string print();

    vector<double> stddevs; // size : currentGroup, attention !
    vector<double> mins;
    vector<double> maxs;
    vector<double> means;
    vector<double> Xs;      // the number of completed values for each group
};

//function defined in the .cpp, independently of any class, to cut an interval in nbPoints (for identifiability/sensitivity)
vector<double> cutSpace(int nbPoints, bool logarithmic, double base, double vstart, double vending);
pSets readHistory(string _name);

// a thread class, not used yet
class Optimizer;


/// mother class, to keep all the functions we might want to use without graphical interface
/// Basically, we give it to the optimizer, optimizer does what he wants and this class records the
/// sets it was asked to simulate.
// Common storage and functions to be shared between this mother class and the interface
enum modes {MONO_EXPERIMENT, MULTI_EXPERIMENT};

class manageSims
        #ifdef ALLOW_OPTIMIZE
        : public GeneralImplementation
        #endif
{
public:
    modes currentMode;
    // first way to launch it : only one experiment
    manageSims(Experiment* _Exp) :
        #ifdef ALLOW_OPTIMIZE
        GeneralImplementation(_Exp->m->getNbParams()),
        #endif
        currentMode(MONO_EXPERIMENT), listExperiments(NULL), currentExperiment(_Exp), currentModel(_Exp->m), history(1000, _Exp->m->getNbParams()), costRecords(50), recording(false)
    {
        resetConfigFromModel();
        sensitivities.resize(_Exp->m->getNbParams(), NULL);
        identifiabilities.resize(_Exp->m->getNbParams(), NULL);
        optFileNamesIdentifiability.resize(_Exp->m->getNbParams());
    }

    // second way to launch it : with a group of experiments (MultiExperiments)
    manageSims(MultiExperiments* _Exp) :
        #ifdef ALLOW_OPTIMIZE
        GeneralImplementation(_Exp->m->getNbParams()),
        #endif
        currentMode(MULTI_EXPERIMENT), listExperiments(_Exp), currentExperiment(NULL), currentModel(_Exp->m), history(1000, _Exp->m->getNbParams()), costRecords(50), recording(false)
    {
        if(_Exp->nbBigExp() < 1) {cerr << "ERR: manageSims constructor can not be called with an empty multiExperiments\n"; return;}
        currentExperiment = _Exp->getExperiment(0);
        resetConfigFromModel();
        sensitivities.resize(_Exp->m->getNbParams(), NULL);
        identifiabilities.resize(_Exp->m->getNbParams(), NULL);
        optFileNamesIdentifiability.resize(_Exp->m->getNbParams());
    }




    // Module 1 (independent) : performing a simulation
    MultiExperiments* listExperiments;  // the list of experiments that are all simulated (NULL = mode one experiment only)
    Experiment* currentExperiment;      // the experiment that is currently DISPLAYED
    Modele* currentModel;
    virtual void simulate();            // call the simulate function for each sub-experiment
    vector<TableCourse> currentData;    // to save the kinetics of the last simulation

    bool isMultiExperiments(){return (currentMode == MULTI_EXPERIMENT);}
    // Module 2 (depends on module 1) : optimization :
    //     -> vector<double> parameters; is inherited from GeneralImplementation, together with the functions setParameter, etc
    //     -> the function getCost has to be implemented and will be called from the optimizer
    virtual double getCost();

    //     -> additionally, these fields record what's happening (i.e. which parameter sets the optimizer was asking)
    //        it allows, whatever the optimization toolbox, to make plots analyze the efficiency of optimization over time
    pSets history;
    virtual void saveHistory(string _name = string(""));
    virtual void loadHistory(string _name = string(""));
    vector<double> bestSetFromHistory();
    void saveBestSetFromHistory(string _name);
    vector<double> useParamSetFromHistory(int indexSet, int indexCombToOverride = -1);
    int nbCostCalls;
    evolution costRecords;


    // Module 3 : handling configurations (only depends/works on the currentModel)
    // note : manageSims is not made to manually modify a configuration. Only reads/writes file. simuWin provides on top the graphical interface to modifu the configuration; if you don't want to use the GUI, then change the text file of the configuration itself and update it.
    int nbCombs;
    vector< vector<string> > currentConfig;
    virtual string loadConfig(string _name);
    string resetParamSetFromConfig(string _name);
    void updateConfigParamsFromModel();
    void resetConfigFromModel(); // to start with a non-empty config. Note that IF the graphical interface (simuWin) is used, currentConfig DOES NOT follow the changes made in the table.
    bool isInConfig(int idParameter, int idConfig);
    vector<int> parametersInConfig(int idConfig);



    // Module 4 : (independent) get track of the function called and gives the C++ code equivalent to it.
    bool recording;
    stringstream currentMacro;
    int macroID;
    void startMacro(string experimentName = string("MyExperiment"));
    void addMacro(string text);
    void addFileTextToMacro(string text, string fileName = string(""));
    void addFileToMacro(string fileName);
    string getMacro();


    // Module 5 : Elaborate functions of the mother class to replace what the interface was doing
    void motherOverrideUsingComb(int newIndex);
    void motherOptimize(string optText, int nbSetsToRecord);
    string motherCreateOptimizerFile(int indexComb, string optMethodText, int parameterToExclude = -1);
    string makeTextReportParamSet(string _folder, int configuration, double simDt = -1, double displayDt = -1);

    //string makeMultiExpReport();

    void motherSensitivity(vector<double> &initialSet, int parameterIndex);
    void motherRecursiveSensitivity(vector<double>& initialSet, int parameterIndex, int nbPoints, bool logarithmic, double base, double vstart, double vending, int deepLevel);

    void motherIdentifiability(vector<double> &initialSet, int parameterIndex);
    void motherRecursiveIdentifibiality(vector<double> &initialSet, int parameterIndex, int nbPoints, bool logarithmic, double base, double vstart, double vending, int deepLevel);

    vector<oneParameterAnalysis* > sensitivities;
    vector<oneParameterAnalysis* > identifiabilities;
    vector<string> optFileNamesIdentifiability; // one file per parameter.

    void makeIdentifibialityReport(int parameterID, string existingBaseFolder, int currentConfigID);
    void prepareOptFilesForIdentifibiality(string folder, int parameterIndex, int indexComb, string optMethodText);

    virtual ~manageSims(){} // for the subclass simuWin





};



#ifndef WITHOUT_QT
namespace Ui {
    class simuWin;
}

class simuWin : public QWidget, public manageSims
{
    Q_OBJECT
private:
    Ui::simuWin *ui;

public:
    explicit simuWin(Experiment* _Exp, QWidget *parent = 0);
    explicit simuWin(MultiExperiments* _Exp, QWidget *parent = 0);
    //explicit simuWin(Choose* Launcher, QWidget *parent = 0);
    ~simuWin();

    QListWidget         *listSubExps;
    QStandardItemModel  *tableHistory;
    QTableWidget        *currentTable;
    QStandardItemModel  *tableMultiExpCosts;

#ifdef ALLOW_OPTIMIZE
    optSelect *optChoice;
#endif

public slots:

    /// Initialization of the window
    void reset();

    /// Display another variable in the graph (without simulating, according to recorded data)
    void varChanged(int i);

    /// To refresh the table from model's values
    void updateParmsFromModel();

    /// To refresh the table of best parameter sets
    void refreshHistory();

    /// To save the current combination (of parameters to fit and variables to override) into a new column
    void createComb();

    /// To change the buttons from "Use" to "delete" and vice-versa
    void removeCombs();

    ///
    void showOnlyCombChecked(int newState);

    /// Tu use (or delete) a saved combination
    void useComb(int IDComb = -1);
    void costReport();

    void refreshMacro();
    void resetMacro();
    void changeRecord(int);


    /// Use set of parameter (inside the model) to simulate and record/display the kinetics
    void simulate();

    /// From the mother class generalImplementation, takes the velues in parameters[], put them in
    /// the model, do simulate, put the set of parameters with its cost in the pSets, and returns the cost
    double getCost();

    /// Not reimplemented :
    /// save the current list of best parameters in a file
    void saveHistory(QString _name = QString(""));
    void loadHistory(QString _name = QString(""));

    /// When user clicks to override or not a variable in the table
    void changeOverride(int newState);

    void optimize();
    void optimizeThread();

    void currentExperimentChanged();

    string createOptimizerFile();

    //void SettingsChangedFromTable(QModelIndex, QModelIndex);
    void cellClicked(int row, int column);
    void cellChanged(int row, int column);
    void cellPressed(int row, int column);
    void coefficientsChanged(QModelIndex left, QModelIndex right);
    void paramSetDoubleClickedFromHistory(const QModelIndex &QMI);

    void loadOptim(string _name = string(""));
    void loadSet(string dest = string(""));
    void saveSet(string _name = string(""));
    string loadConfig(string _name = string(""));
    void saveConfig(string _name = string (""));
    void resetParams();
    void paramChangedFromBox(double newValue);

    // returns the list of created folders (in the multiexperiment case)
    vector<string> makeFigReportParamSet(string _folder = string(""));
    void setColorScale(int newScale);
    void expandPlot();

    void sensitivity();
    void identifiability();
    void stopOptimization();

private:
    //QStandardItemModel* m;
    int nbGraphes;
    enum {MINS, MAXS, MEANS, STDDEVS};  // name of curves in costGraphe
#ifdef USE_QWT
    Graphe2* currentGraphe;
    Graphe2* costGraphe;
    vector<Graphe2*> currentGraphes;
#endif
#ifdef USE_QCP
    grapheCustom* currentGraphe;
    grapheCustom* costGraphe;
    vector<grapheCustom*> currentGraphes;
#endif

    int lastParamID;
    int paramSetID;
    bool removeMode;
    bool stopOpt;

    QThread* thread;
    Optimizer* opt;
};

class Optimizer : public QObject {
    Q_OBJECT

public:
    Optimizer(simuWin* _currentWindow);
    ~Optimizer();

public slots:
    void process();

signals:
    void finished();
    void error(QString err);

private:
    simuWin* currentWindow;
    // add your variables here
};
#endif
#endif // SIMUWIN_H



/*int NP = currentModel->getNbParams();
sensitivityXs.resize(NP, NULL);
sensitivityCosts.resize(NP, NULL);
sensitivityCostsPerVariable(NP, NULL);
for(int i = ; i < NP; ++i){
    sensitivityXs = new vector<double>();
    sensitivityCosts  = new vector<double>();
    sensitivityCostsPerVariable = new vector< vector<double> > ();
}
vector<double> initialParameterSet;
vector< vector<double>* > sensitivityXs;                // (*sensitivityXs[i])[indexTestedValue];
vector< vector<double>* > sensitivityCosts;             // (*sensitivityXs[i])[indexTestedValue];
vector<  vector< vector<double> > * > sensitivityCostsPerVariable;   // (*sensitivityCosts[IDVar])[indexTestedValue][variableInternalIndex)]
*/
