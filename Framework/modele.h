// Philippe Robert, 19-09-2014 - philippe-robert.com
// modele.h and modele.cpp  are using the TableCourse structure defined in tablecourse.h and tablecourse.cpp
//                          and the Evaluator structure. Remove/comment #define USE_EVALUATORS to use the model class witout evaluators
#ifndef MODELE_H
#define MODELE_H

//#define COMPILE_AUTOGEN

#define USE_EVALUATORS
#define USE_OVERRIDERS
//#define USE_BOOST_SOLVER

#define STOP_VAL 1e8
#define MIN_VAL 1e-10

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>      // for std::setprecision
#include <cmath>
#include <fstream>
#include "tableCourse.h"
#ifdef USE_OVERRIDERS
#include "overrider.h"
#endif
#ifdef USE_EVALUATORS
#include "evaluator.h"
#endif
using namespace std;

// you can define the backgrounds as you like, but it is important that each time parameters are loaded, the background is reseted,
// because backgrounds are touching parameters --> changing parameters can screw the background --> background has to be set again each
// time by using initialize. Better using 1 instead of 0 as default, for binary combinations of backgrounds.
#define DEFAULT_BACKGROUND_VALUE    1

/// ================= A general mother class for ODE simulations. =========================

struct Modele {
    string name;                                            /// Name of the model (to separate different subclasses ...)

    /// The general constraints the submodel should follow are here :
    /// 0 - Calling the constructor : the sub-model calls says the nb of variables and parameters to allocate
    Modele(int _nbVars,  int _nbParams);

    /// 1 - the storage is provided by the mother class memory, (only accesssible by the sub-class but not b outside)
    ///     note that the mother class only allocate them with a good size, but never operates/changes values in these 6 fields,
    ///     so the subclass should do everything
protected:
    int nbVars;
    int nbParams;
    vector<double> init;                                    /// for initial values of variables
    vector<double> params;                                  /// for parameter values
    vector<string> names;                                   /// for the names of the variables
    vector<string> paramNames;

    ///     for running a simulation, these variables will be used
    double t;                                               /// updated during simulation / advised to do 't=0;' in the subclass function initialise
    vector<double> val;                                     /// for variables at time t

public:
    /// 2 - The functions the sub-model HAS TO implement : time evolution for dt at time t, initialise and base parameter values
    virtual void derivatives(const vector<double> &x, vector<double> &dxdt, const double t); 	/// computes the derivatives (at t) from position x
    virtual void setBaseParameters();                       /// gives a correct set of parameters, if you don't load other set.
    virtual void initialise(long long _background = DEFAULT_BACKGROUND_VALUE);           /// initialise, (parameters should be set before initializing) - sets t = 0 if necessary.
                                                            /// to avoid confusion, this function should not change the parameter values !!
                                                            /// the background parameter allows to give options of simulation (such as deficient mice)

    /// 2b - This function is already implemented in the mother class, but as an option you can override it in the subclass
    virtual void loadParameters(string file_name);          /// reads parameters from a text file with format : "NB_PARAMS\tparam1\tparam2\tparam3 ..."
    virtual void saveParameters(string file_name);          /// writes parameters from a text file with format : "NB_PARAMS\tparam1\tparam2\tparam3 ..."

    /// 2c - A function to perform personnalized actions by the model, without a specific function, but by a name of action, (ex : adding a cytokine at day 2)
    virtual void action(string name, double parameter);
    virtual void action(string name, vector<double> parameters);

protected:
    /// 2d - the 'event' functions that the virtual functions of the sub-classes should call, so the mother class knows what's happening,
    void initialiseDone();
    void setBaseParametersDone();
    void loadParametersDone();

public:
    void needNewParameterSet();

public:
/// Now, the interests of using the Modele mother class comes :
/// whatever the sub-class model, the following functions are implemented and can be used to simulate, or do interesting manipulations of the model.

#ifndef USE_EVALUATORS
    typedef void Evaluator;
#endif

/// 3 - functions for simulating (automatically calling the sub-class derivatives function, using the solver)

    void simulate(double _sec_max, Evaluator* E = NULL);    /// Simulates from t (not changed), to t + _sec_max, by calling one_step
                                                            /// the evaluator, if given, is a structure that says when to store data from the simulation, and avoids to store everything.
                                                            /// if the simulation diverges, it is stopped and a penalty is computed in the field penalities (of the mother class)

    public:    double dt;                                   /// minimum dt used for simulations. The tunable dt will start at dt*10 and be limited between dt and dt*100
    public:    double penalities;                           /// automatically updated by the mother class : put to zero when initialiseDone() called and increased when the simulation diverges.

public:
/// 4 - I/O functions


/// 4a - to print the state of a simulation



/// 4b - to get simulations as tables of values (kinetics) of simulation

    /// before a simulation, one has to activate the kinetic mode.
    /// Then, for every simulation, the simulation data will be put in a new table, that can be retrieved by getCinetique.
    /// Note that 'initialiseDone' clears the current table
public:
    bool saveKinetics;                                         /// sets the mode : simulates a kinetics to record or just simulate what you ask without stopping
    double print_all_secs;                                  /// frequency of saving/displaying data for kinetics
    void setPrintMode(bool z, double _print_all_secs);      /// to set the 'recording mode' to ON/OFF, and the frequency of recording
    TableCourse getCinetique();                             /// then, each time initialiseDone() is called, the kinetics is cleared, and
    vector<double> getInit();
    double getT();

/// 4c - to get the value of a particular variable at a particular time point during a simulation,

/// then, the best way is to create an optimizer* E, give him the data points you want, and do the simulation with simulate(int _sec_max, E)
/// so the simulation is filling automatically the evaluator with those data points. See the class Optimizer for that.

/// 4d to print most informations about the model

    string print();
    string printVarNames(double _t);
    string printVarValues(double _t);
    string printParNames();
    string printParValues();

/// 5 - Accessing variables from an external name, and defining backgrounds
///     even if the model has its internal variable names, you might want to say that this vairable represents this cytokine,
///     in this case, a model can attribute an 'external' name to each variable
///     additionally, a model can declare the list of backgrounds it is able to simulate.
///     in that way, it is possible to interrogate what a model is able to simulate (variables, background).

///     vectors that can be filled by the subclass :
protected:
    vector<int> extNames;           /// for each variable, the model can give a global ID
    vector<long long> backSimulated;      /// the model can list in here the IDs of backgrounds it is able to simulate.
                                    /// then, this information can be used inside the sub-class functions (derivative ...)

///     Then, the values of variables can be accessed with the 'external ID' with the following functions
///     these already implemented functions can be overrided if wanted (for instance if changing the value of a variable needs to be done in a particular way)
public:
    virtual void   setValue(int idGlobalVariable, double val);      /// to modify the value of a variable from the global ID of it
    virtual void   addValue(int idGlobalVariable, double val);      /// to modify the value of a variable from the global ID of it
    virtual double getValue(int idGlobalVariable);                  /// to get the value of a variable from its global ID
    virtual bool   isSimuBack(long long idGlobalBack);                    /// to know if a background can be simulated by the model
    virtual bool   isVarKnown(int idGlobalVariable);                 /// to know if a variable can be simulated by the model (from its global ID)
    virtual int    internValName(int idGlobalVariable);             /// to know the index of a variable inside the tables

/// 7 - Working directly with the parameters of a model (important for managing the interface with the optimizer (fitting class))

    int getNbParams();
    void setParam(int i, double v);
    double getParam(int i);
    vector<double> getParameters();
    void setParameters(vector<double> &newParamSet);

/// 8 - to get information on the (internal) variables.

    int getNbVars();
    string getName(int internalID);
    int getGlobalID(int internalID);

/// 9 - Overriding certain variables with data,

#ifdef USE_OVERRIDERS
    /// Idea : provide data as an overrider:
    ///      overrider(i) says if you override or not this variable (global name/ID)
    ///      overrider(i,t) gives forced value for variable i (global name) at time t (linear or spline interpolation)
    ///
    ///      for simulating, the solver might be confused if the x value is changed from external data (the error might be big depending on delta t)
    ///      solution : the solver will force the data each time-step before calling the solver
public :
    void setOverrider(overrider* newOverrider = NULL);
protected:
    overrider* currentOverrider;
public:
    void applyOverride(vector<double> &x, double t);
    void clearOverride(vector<double> &x, vector<double> &dxdt);
#endif
    bool over(int indexLocal);      /// says if a variable is overrided (then, no need to calculate derivatives)
                                    /// if no USE_OVERRIDERS defined, then returns false ! So a program can use this
                                    /// function independently of the define options

/// 10 - names and boundaries for parameters

protected:
    vector<double> paramUpBounds;
    vector<double> paramLowBounds;
public:
    string getParamName(int i);
    double getLowerBound(int i);
    double getUpperBound(int i);
    void setBounds(int i, double vLow, double vHi);
    void setBounds(vector<double> upVals, vector<double> lowVals);

/// 11 - Internal functions

    /// internal way of managing kinetics (automatically used)
private:
    TableCourse* cinetique;
    void deleteCinetique();
    void newCinetiqueIfRequired();
    void save_state(double _t);                                /// the simule function call this to record data of a time point into the Cinetique.
protected:
    double max(double, double);                                     // Auxiliary functions, not available by default ...
    bool parametersLoaded;                                          // to make sure setparameters is done before initialize
    bool checkDivergence();
    bool stopCriterionReached;
};





/// Example of a minimalistic model :
/// for simulating : Creation: --(KF)--> A --(K12)--> B --(K23)--> C --(KFB)--| A,
///                  with respecitve death/degradation rates KD1, KD2, KD3 for A,B and C
///                  and with a mutant where K12 is modified into K12m
///
/// struct ModeleA1 : public Modele {
///     ModeleA1();
///     enum {A, B, C, NBVAR};							// List of variables
///     enum {KF, KD1, KD2, KD3, K12, K23, KFB, K12m, NBPARAM};	// List of parameters
///     enum {WT, MUT, NBBACK};                         // List of backgrounds
///     long long  background;
///
///     void initialise(long long _background = WT);
///     void setBaseParameters();
///     void one_step(double _t, double _delta_t);
/// };
///
/// ModeleA1::ModeleA1() : Modele(NBVAR, NBPARAM), background(WT) {
///     names[A] = string("PopA");
///     names[B] = string("PopB");
///     names[C] = string("PopC");
/// }
///
/// void ModeleA1::setBaseParameters(){
///     params.clear(); params.resize(NBPARAM);     // to erase previous values
///     params[KF]   = 1e-3;
///     params[KD1]  = 1e-4;
///     params[KD2]  = 1e-6;
///     params[KD3]  = 1e-6;
///     params[K12]  = 1e-6;
///     params[K23]  = 1e-5;
///     params[KFB]  = 0.1;
///     params[K12m] = 1e-4;
///     setBaseParametersDone();
/// }
///
/// void ModeleA1::initialise(long long _background){
///     background = _background;
///     val.clear();     val.resize(NBVAR, 0.0);        // to erase previous values
///     valTemp.clear(); valTemp.resize(NBVAR, 0.0);
///     init.clear();    init.resize(NBVAR, 0.0);
///     for(int i = 0; i < NBVAR; ++i){val[i] = init[i];}
///     t = 0;
///     initialiseDone();
/// }
///
/// void ModeleA1::one_step(double _t, double delta_t){
///     double pK12 = (background == WT) ? params[K12] : params[K12m];
///     valTemp[A] = delta_t * (params[KF] * (1- val[C] / params[KFB]) - (params[KD1] + pK12) * val[A]);
///     valTemp[B] = delta_t * (pK12 * val[A] - (params[KD2] + params[K23]) * val[B]);
///     valTemp[C] = delta_t* (params[K23] * val[B] - params[KD3] * val[C]);
///     for(int i = 0; i < NBVAR; ++i){val[i] += valTemp[i];}
/// } */
///
/// // To perform simulations with this model (see below for 'simulate'):
///
/// ModelA1 A();
/// A.setBaseParameters();
/// // or : A.loadParameters(string("Params.txt"));, where Params.txt contains "8\t1e-3\t1e-4\t1e-6\t1e-6\t1e-6\t1e-5\t0.1\t1e-4\n"
/// A.initialise(A::WT);
/// A.simulate(100);
/// A.initialise(A::MUT);
/// A.simulate(100);



/*  // an example using all the possibilities :

namespace N     {enum {IL12, STAT4P, TBET, IFNG, R12, RL12, STAT4, RIFN, IFNGR, STAT1, STAT1P, SOCS1, NB_GLOB_VARS};}
namespace Back  {enum {WT, IFNGRKO, STAT4KO, TBETKO, NB_GLOB_BACK};}

struct Modele110L : public Modele {
    Modele110L();
    enum  {IL12, STAT4P, TBET, NBVAR};
    enum {KD12, KDS4P, KDTBET, S4PEQ, CS4P, TBETEQ, CTBET, CTCR, BS4P, BTBET, NBPARAM};
    long long background;
    void one_step(double _t, double _delta_t);
    void initialise(long long _background = Back::WT);
    void setBaseParameters();
};

Modele110L::Modele110L() : Modele(NBVAR, NBPARAM), background(Back::WT) {
    // overwrites the values of the generic constructor Modele::Modele()
    dt = 0.2;
    print_all_secs = 1200;

    names[IL12] = string("IL12");
    names[STAT4P] = string("STAT4P");
    names[TBET] = string("TBET");

    extNames[IL12] = N::IL12;
    extNames[STAT4P] = N::STAT4P;
    extNames[TBET] = N::TBET;

    backSimulated.clear();
    backSimulated.push_back(Back::WT);
    backSimulated.push_back(Back::STAT4KO);
    backSimulated.push_back(Back::TBETKO);

    #ifdef PRINT_KIN
    cerr << "Modele for Th1 differentiation (M110L) Nb of parameters : " << NBPARAM << ", variables :" << NBVAR << endl;
    #endif
}

void Modele110L::setBaseParameters(){
    params.clear();
    params.resize(NBPARAM);
    params[KD12] = 1.1e-4;
    params[KDS4P] = 2e-5;
    params[KDTBET] = 1e-5;
    params[S4PEQ] = 0.05;
    params[CS4P] = 1.0;
    params[TBETEQ] = 0.05;
    params[CTBET] = 1.0;
    params[CTCR] = 1.0;
    // params[BS4P] and params[BTBET] will be defined in initialize
    setBaseParametersDone();
}

void Modele110L::initialise(long long _background){

    background = _background;
    val.clear();        val.resize(NBVAR, 0.0);
    valTemp.clear();    valTemp.resize(NBVAR, 0.0);
    init.clear();       init.resize(NBVAR, 0.0);

    // parameters that depend on other ones are better here in case one wants to use loadParameters
    params[BS4P]  = params[KDS4P] * params[S4PEQ];
    params[BTBET] = params[KDTBET] * params[TBETEQ] - params[CTBET] * params[S4PEQ] * params[TBETEQ] * (1 + params[CTCR]);
    if(params[BS4P]  < 0) params[BS4P] = 0;
    if(params[BTBET] < 0) params[BTBET] = 0;

    init[IL12]   = 0.0;
    init[TBET]   = params[TBETEQ];
    init[STAT4P] = params[S4PEQ];

    if(background == Back::STAT4KO) init[STAT4P] = 0.0;
    if(background == Back::TBETKO) init[TBET] = 0.0;

    for(int i = 0; i < NBVAR; ++i){val[i] = init[i];}
    t = 0;
}

void Modele110L::one_step(double _t, double delta_t){
    valTemp[IL12] = delta_t * (- params[KD12] * val[IL12]);
    valTemp[STAT4P] = delta_t * (- params[KDS4P] * val[STAT4P] + params[BS4P] + params[CS4P] * val[IL12] * val[STAT4P]);
    valTemp[TBET] = delta_t * (- params[KDTBET] * val[TBET] + params[BTBET] + params[CTBET] * val[STAT4P] * val[TBET] * (1 + params[CTCR]));
    for(int i = 0; i < NBVAR; ++i){val[i] = valTemp[i];}
}


*/

#endif // MODELE_H


