#ifndef Experiment_H
#define Experiment_H

#include "evaluator.h"
#include "modele.h"
#include <vector>
#include <string>
#include <iostream>
using namespace std;

int getIdNameFromTable(string s, vector<string> & _NamesVariablesInTheirIndex);
int getPosInKinetics(string sname, TableCourse* Tc);

struct Experiment{

    Experiment(Modele* _m, int _nbConditions);   // An experiment has to be intanciated with an existing model
    Modele* m;
    int nbConditions;                             // Nb of simulations/curves inside this experiment

    // informations hat should be filled by the subclass:
    string Identification;                  // a name for this experiment !
    vector<string> names_exp;               // List of defined conditions the subclass can run
    vector<bool> doable;                    // A table to store if each condition is doable with the model.
                                            // Aim : the given model m might be suitable or not, so you can use the constructor of your experiment to ask the given model if he simulates the good variables and write it down in doable[]

    // that should be reimplemented
    virtual void simulate(int IdExp, Evaluator* E = NULL, bool force = false);  // simulating one of the experiments. Do it only if the experiment is doable. If the estimator doesn't need values from this experiment, will not be simulated, unless you use force = true.
                    // Should describe how the model has to be simulated, for each condition,
                    // by putting initial values to variables, using Modele::simulate for
                    // a certain duration, and by applying perturbations.
                    // The evaluator E will contain the experimental data and has to be passed
                    // to the Modele::simulate() function to store simulated data.
                    // If force = false, will only simulate simulations that have data in E.

    // that can be reimplemented. The original code is in the motherXXX functions
    virtual void    init(){motherInit();}
    virtual double  costPart(int speciesGlobalID = -1){return motherCostPart(speciesGlobalID);}        // returns the cost of an experiment compared to data (you have to provide the data & cost yourself)
    virtual double  costExp(int ExpID = -1){return motherCostExp(ExpID);}        // returns the cost of an experiment compared to data (you have to provide the data & cost yourself)
    virtual void    reset(){motherReset();}                     // does nothing (except if reimplemented)
    virtual void    simulateAll(bool force = false){motherSimulateAll(force);}         // to do all the simulations

    // The code pre-implemented for all of them
    void            motherInit();
    double          motherCostPart(int speciesGlobalID = -1);
    double          motherCostExp(int expID = -1);
    void            motherReset();
    bool            motherPrepareSimulate(int IdExp, Evaluator *&E, bool force); // Checks if doable,  // what the hell *& ??
    void            motherSimulateAll(bool force = false);

    // Basic I/O functions
    int nbCond();                    // gives names_exp.size()
    bool isDoable(int i);           // reads doable[i]
    string expName(int i);          // reads names_exp[i]
    string print();

    // In order to store a list of points to simulate and record (avoids to record all the kinetics of all variables at all dt ...)
    vector<Evaluator*> VTG; // "Values to get", one evaluator per experiment
    double V(int IDexp, int Species, int time_sec){return VTG[IDexp]->getVal(m->internValName(Species), time_sec);}
    double expV(int IDexp, int Species, int time_sec){return VTG[IDexp]->getExpVal(m->internValName(Species), time_sec);}
    double totalPenalities;
    double getPenalities();

    // this should be programmed independently
    // In order to give experimental data with StdDev and compare with simulations or give to evaluators
    vector<TableCourse*> ExpData;
    vector<TableCourse*> ExpStdDevs;
    vector<string> NamesVariablesInTheirIndex;
    void giveData(TableCourse* kineticData, int IdExp);
    void giveStdDevs(TableCourse* kineticStds, int IdExp);
    void giveHowToReadNamesInKinetics(vector<string> _NamesVariablesInTheirIndex);
    void loadEvaluators();

    // In order to replace variables by pre-defined curves
    vector<overrider*> Overs;
    void setOverrider(int IdExp, overrider* _ov = NULL);
    void overrideVariable(int IdGlobalName, bool override = true, int IdExp = -1);
    bool canOverride(int IdGlobalVariable, int IdExp = -1);

    // a super cool function to extract a table of results from simulation of different experiments
    string extractData(vector<int> timePoints, vector<int> idGlobalVariables, vector<string> namesAllGlobVars, vector<int> IDexps = vector<int>());

    string costReport();
    virtual ~Experiment(){}
};


struct MultiExperiments{
    MultiExperiments(Modele* _m);   // m is the model that will be used to do simulations
    Modele* m;

    int NbBigExps;
    vector<Experiment*> ListBigExperiments;
    void AddExperiment(Experiment* Ex);
    Experiment* getExperiment(int BigExpID);

    vector<double> coefficients;
    int cptNbSimus;

    // function called by the cost functions, need to be re-implemented, and can change the coefficients whenever.
    virtual void politics(){}
    virtual void postPolitics(){}


    // that should be reimplemented
    virtual void simulate(int IdExp, Evaluator* E = NULL, bool force = false);  // simulating one of the experiments. Do it only if the experiment is doable. If the estimator doesn't need values from this experiment, will not be simulated, unless you use force = true.


    // just call the same function in all the Experiment of the list
    void    init();
    virtual double  costPart(int speciesGlobalID = -1);
    virtual double  costBigExp(int BigExpID = -1);
    void    reset();
    void    simulateAll(bool force = false);

    // Basic I/O functions
    int nbBigExp();                    // gives names_exp.size()
    string BigExpName(int i);          //
    string print();

    double getCoefficient(int BigExpID);
    void setCoefficient(int BigExpID, double newValue);

    double getPenalities();
    void loadEvaluators();
    void overrideVariable(int IdGlobalName, bool override);

    string costReport();


    virtual ~MultiExperiments(){}
};

#endif // Experiment_H
