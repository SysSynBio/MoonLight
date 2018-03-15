#include "modele.h"
#include <algorithm>
#include <sstream>
using namespace std;

//#include "../Extreminator/common/myTimes.h"

#ifdef USE_BOOST_SOLVER
#include <boost/numeric/odeint.hpp>
using namespace boost::numeric::odeint;
#endif

/// ======================= ODE Solver (defined with the same interface than BOOST) ===============================

/// This class is the interface to use boost::integrate functions (a class where the operator () gives the derivatives)
/// I don't use boost, but I keep this formalism so it's easy to use boost when we want
/// And I use this class to override variables with data. The strategy is to force x[overrided variable] = data
/// each time before calling the derivatives, and to remove it after (put 0), so the solver is not confused
/// (there is no variation on these variables. if not, the solver would compute an 'error'and might adapt its dt to the forced data curves).

class mySim {
public:
    Modele* mm;
    mySim( Modele* _mm ) : mm(_mm) {}
    void operator()(const vector<double> &_x, vector<double> &_dxdt, const double t){
        #ifdef USE_OVERRIDERS
        mm->applyOverride((vector<double>&) _x, (double) t);
        #endif

        mm->derivatives(_x,_dxdt,t);

        #ifdef USE_OVERRIDERS
        mm->clearOverride((vector<double>&) _x, (vector<double>&) _dxdt);
        #endif
    }
};

/// a function to evalute the percent of error between two vectors (for comparing dt time step versus two times dt/2)
double evalErr(vector<double> &v1ref, vector<double> &v2, int size){
    double res = 0;
    for(int i = 0; i < size; ++i)
        if(v1ref[i] != 0) res = max(res, fabs((v2[i] - v1ref[i]) / (v1ref[i])));
    return res/size;
}

/// Integrates the ODEs (in the model in mySim), updating the start vector, from t to nxt, with the minimum dt dtmin (and maximum dt dtmin * 500, starting from dtmin * 10)
size_t myintegrate( mySim BS , vector<double> &start , double t , double nxt , double dtmin ){
    bool diverge = false;
    int size = start.size();
    vector<double> oneshotdxdt  = vector<double>(size, 0.0);
    vector<double> oneshotpoint = vector<double>(size, 0.0);
    vector<double> halfdxdt     = vector<double>(size, 0.0);
    size_t nbSteps = 0;

    double dtlocal = min(dtmin*10, (nxt-t) / 10);
    if(dtlocal == 0) return 0;  // simulation finished
    while(t < nxt){
        // if integrating one step of dtlocal
        BS(start, oneshotdxdt, t);
        for(int i = 0; i < size; ++i){
            oneshotpoint[i] = start[i] + dtlocal * oneshotdxdt[i];
            //if(isnan(oneshotpoint[i]) || (isinf(oneshotpoint[i])) || (fabs(oneshotpoint[i]) > 1e12)) {/*cerr << "Divergence ! " << endl; */ return 0; }
        }

        // versus if integrating two steps of dtlocal/2 (the result from this technique is the one kept (more accurate than one step), but the next dt will be chosen depending if one step is accurate enough (close to two steps) or not
        //BS(start, halfdxdt, t);
        for(int i = 0; i < size; ++i)
            {start[i] = start[i] + 0.5* dtlocal * oneshotdxdt[i];}
        BS(start, halfdxdt, t + 0.5* dtlocal);
        for(int i = 0; i < size; ++i){
            start[i] = start[i] + 0.5* dtlocal * halfdxdt[i];
            if((start[i] < MIN_VAL) && (start[i] > (-1.0 * MIN_VAL))) start[i] = 0;
            if(fabs(start[i]) > STOP_VAL) diverge = true;}  // maybe better to do it every 10 or 20 loops ...
        t += dtlocal;

        double compare = evalErr(start, oneshotpoint, size);
        if(compare < 0.002) dtlocal *= 1.15; else dtlocal *= 0.5;
        if(dtlocal < dtmin) {dtlocal = dtmin;}          // minimum dt
        if(dtlocal > (nxt-t)) dtlocal = (nxt-t);        // should not exceed the stopping time point
        if(dtlocal > (500*dtmin)) dtlocal = 500*dtmin;  // should not exceed 500 * dtmin
        nbSteps += 2;                                   // for information, reminds the number of steps

        if(diverge) return nbSteps;
    }
    return nbSteps;
}


#ifdef USE_BOOST_SOLVER
runge_kutta4<vector<double>> rk4 = runge_kutta4<vector<double>> ();

/// Integrates the ODEs (in the model in mySim), updating the start vector, from t to nxt, with the minimum dt dtmin (and maximum dt dtmin * 500, starting from dtmin * 10)
size_t RK4integrate( mySim BS , vector<double> &start , double t , double nxt , double dtmin ){

    bool diverge = false;
    int size = start.size();
    vector<double> oneshotdxdt  = vector<double>(size, 0.0);
    vector<double> oneshotpoint = vector<double>(size, 0.0);
    vector<double> halfdxdt     = vector<double>(size, 0.0);
    size_t nbSteps = 0;

    double dtlocal = min(dtmin*10, (nxt-t) / 10);
    if(dtlocal == 0) return 0;  // simulation finished
    while(t < nxt){
        // if integrating one step of dtlocal

        rk4.do_step( BS , oneshotpoint , t , dtlocal );
        /*BS(start, oneshotdxdt, t);
        for(int i = 0; i < size; ++i){
            oneshotpoint[i] = start[i] + dtlocal * oneshotdxdt[i];
            //if(isnan(oneshotpoint[i]) || (isinf(oneshotpoint[i])) || (fabs(oneshotpoint[i]) > 1e12)) { return 0; }
        }*/

        // versus if integrating two steps of dtlocal/2 (the result from this technique is the one kept (more accurate than one step), but the next dt will be chosen depending if one step is accurate enough (close to two steps) or not

        //implicitely (already called before) BS(start, halfdxdt, t + 0.5* dtlocal);
        /*for(int i = 0; i < size; ++i)
            {start[i] = start[i] + 0.5* dtlocal * oneshotdxdt[i];}*/
        rk4.do_step( BS , start , t , 0.5* dtlocal );               // In-place transformation of inout

        //BS(start, halfdxdt, t + 0.5* dtlocal);
        rk4.do_step( BS , start , t , 0.5* dtlocal );


        for(int i = 0; i < size; ++i){
            // start[i] = start[i] + 0.5* dtlocal * halfdxdt[i];
            if((start[i] < MIN_VAL) && (start[i] > (-1.0 * MIN_VAL))) start[i] = 0;
            if(fabs(start[i]) > STOP_VAL) diverge = true;
        }                                       // maybe better to do it every 10 or 20 loops ...
        t += dtlocal;

        double compare = evalErr(start, oneshotpoint, size);
        if(compare < 0.002) dtlocal *= 1.15; else dtlocal *= 0.5;
        if(dtlocal < dtmin) {dtlocal = dtmin;}          // minimum dt
        if(dtlocal > (nxt-t)) dtlocal = (nxt-t);        // should not exceed the stopping time point
        if(dtlocal > (500*dtmin)) dtlocal = 500*dtmin;  // should not exceed 500 * dtmin
        nbSteps += 2;                                   // for information, reminds the number of steps

        if(diverge) return nbSteps;
    }
    return nbSteps;
}
#endif

/// function to simulate from t to t+sec_max. The value of modele::t is updated
void Modele::simulate(double sec_max, Evaluator* E){
    if(!E) cerr << "No Evaluator\n";
    if(saveKinetics) newCinetiqueIfRequired(); else deleteCinetique();
    mySim BS = mySim(this);

    double nxt = 0;         // nxt will be the next stopping point each time (for recording kinetics or for getting specific value)
    double t_init = t;
    size_t steps = 0;

    while(nxt < t_init + sec_max){       // when finished, 1e8

        // finds the next wanted time point
        nxt = t_init + sec_max;
        if(saveKinetics){nxt = min(nxt, print_all_secs * (double) ((int) (t / print_all_secs + 1e-8) + 1));} // the 1e-8 is due to compensate the errors. Indeed, (0.03 / 0.01) = 3 but (int) (0.03 / 0.01) was giving 2 ...
        #ifdef USE_EVALUATORS
        if(E) nxt = min(nxt,E->nextPoint());
        #endif

        // integrates until this time point
        #ifdef USE_OVERRIDERS
        clearOverride(val, val);            // cheating, but can be improved (for val, val) : the solver doesn't see the overrided variables (they are =0)
        #endif

#ifdef USE_BOOST_SOLVER
// Version 1 : boost adaptive runge kuutta => freezes
//typedef controlled_runge_kutta< runge_kutta_dopri5<vector<double>> > ctrl_rkck54;
//steps += integrate_adaptive( ctrl_rkck54(), BS, val , (double) t , (double) nxt , (double) dt /*, my_observer */ );

// Version 2 : same adaptive as aeuler but with rk4 inside
steps += RK4integrate( BS , val , (double) t , (double) nxt , (double) dt );     // this line can be directly replaced by the boost solver ...

#else
        steps += myintegrate( BS , val , (double) t , (double) nxt , (double) dt );     // this line can be directly replaced by the boost solver ...
#endif

        t = nxt;    // this might not be right if the simulation stopped before ...
        #ifdef USE_OVERRIDERS
        applyOverride(val, t);              // now that the solver did the job, the variables can be overrided with the real value of the overrider.
        #endif

        if(checkDivergence()) return;

        // gets values from this time point for the evaluator or for saving/printing kinetics
        #ifdef USE_EVALUATORS
        if(E && (E->takeDataAtThisTime(t))){
            int z = E->speciesToUpdate();
            while(z >= 0){ // id of the next variable, -1 if finished
                if( z < (int) val.size()) {
                    E->setValNow(z, val[z]);
                    //cerr << "Set t= " << t << "\tV(" << z << ") = " << val[z] << endl;
                } else {
                    E->setValNow(z, 0);
                    //cerr << "OUT t= " << t << "\tV(" << z << ")\n";
                }
                z = E->speciesToUpdate();}   // the next one
        }
        #endif
        if(saveKinetics){if( fabs((double) ((int) (t / print_all_secs + 1e-8)) - t / print_all_secs) < 1e-8) save_state(t);} // to account for division errors
    }
    //{ // in case, finishes the simulation (but in theory, nxt should have reached t+sec_max)
    //    steps += myintegrate( BS , val , (double) t , (double) t_init +sec_max , dt );
    //    t = t_init + sec_max;
    //}
}

bool Modele::checkDivergence(){
    if(stopCriterionReached) return true;
    for(int i = 0; i < nbVars; ++i){
        if(std::isnan(val[i]) || std::isinf(val[i]) || (fabs(val[i]) > STOP_VAL))  {
            penalities += 100 * (1.0 / (0.01 + log(((double) t) / 100.0 + 1)));
            stopCriterionReached = true;
        }
    }
    return false;
}







// ======================================= 2 Basic definitions ==========================

Modele::Modele(int _nbVars,  int _nbParams) : nbVars(_nbVars), nbParams(_nbParams), penalities(0), saveKinetics(false), currentOverrider(NULL), cinetique(NULL) {
    t = 0; stopCriterionReached = false; dt = 0.02; print_all_secs = 1200;	// basal values for simulation, can be changed by the subclass
    params.clear();         params.resize(nbParams, 0.0);
    val.clear();            val.resize(nbVars, 0.0);
    names.clear();          names.resize(nbVars, string(""));
    init.clear();           init.resize(nbVars, 0.0);
    extNames.clear();       extNames.resize(nbVars, 0);
    paramNames.clear();     paramNames.resize(nbParams, string(""));
    paramLowBounds.clear(); paramLowBounds.resize(nbParams, 1e-12);
    paramUpBounds.clear();  paramUpBounds.resize(nbParams, 1e12);
    backSimulated.clear();}
void Modele::loadParameters(string file_name){
    cout << "      -> loading paramter set from " << file_name << endl;
    ifstream fichier(file_name.c_str(), ios::in);
        if(fichier){int nb_p = 0; fichier >> nb_p;
        for(int i = 0; i < min(nb_p, (int) nbParams); ++i){
            double tampon = 0.0;
            fichier >> tampon;
            if(tampon != 0) params[i] = tampon;
            //cout << "Loading p" << i << " = " << params[i] << endl;
        }}
    loadParametersDone();}
// Virtual functions that SHOULD be reimplanted
void Modele::saveParameters(string file_name){
    ofstream fichier(file_name.c_str(), ios::out);
    if(fichier){
        fichier << nbParams << endl;
        for(int i = 0; i < nbParams; ++i){
            if(i > 0) fichier << "\t";
            fichier << getParam(i);
        }
        fichier << endl;
        fichier.close();
    } else {cerr << "Modele::saveParameters(), file not found : " << file_name << endl;}
}
// Virtual functions that SHOULD be reimplanted
void Modele::derivatives(const vector<double> &x, vector<double> &dxdt, const double t){
    cerr << "ERR : the function derivatives has not been implemented properly in the daughter class\n";}
void Modele::initialise(long long ){
    cerr << "WRN : Nothing specified for initialization\n";}
void Modele::setBaseParameters(){
    cerr << "ERR : the function setBaseParameters has been called but not implemented in the daughter class\n";}				// gives a correct set of parameters, if you don't load other set.
void Modele::initialiseDone(){
    if(! parametersLoaded) cerr << "ERR : Please provide a full parameter set to the modele before simulating. Details : you called initialise() without loading/setting parameters before initializing ! - you can not perform simulation ! Solutions : call 'loadParameters(file)', 'setParameters(full set)' or 'setBaseParameters()\n";
    deleteCinetique(); penalities = 0;stopCriterionReached=false;}
void Modele::setBaseParametersDone(){parametersLoaded = true;}
void Modele::needNewParameterSet(){parametersLoaded=false;}
void Modele::loadParametersDone(){parametersLoaded = true;}
double Modele::max(double a, double b){if(a > b) return a; else return b;}
string Modele::print(){
    stringstream res;
    // I preferred to remove any use depending on the namespaces N:: and Back:: so that the modele class is general and independent of them.
    res << "   Nb Params " << nbParams << endl;
    res << "   Nb Vars   " << nbVars << endl;
    res << "   Internal Variables : " << endl;
    if((int) names.size() == nbVars){
        for(int i = 0; i < nbVars; ++i){
            res << "\t" << i << "\t" << names[i] << "\t";
            if(extNames[i] >= 0) res << "Globally Known As Var ID=: (" << extNames[i] << ")"; //<< GlobalName(extNames[i]) <<
            else res << "Internal only ";
            res << endl;
        }
     } else res << "ERR: Modele::print, wrong size for names[] - there are not enough names found for species" << endl;
    res << "    Current parameter values\n";
    for(int i = 0; i < nbParams; ++i){res << "   " << i << "\t" << params[i] << endl;}
    return res.str();
}
string Modele::printVarNames(double _t){
    stringstream res;
    res << "\n" << _t/3600 <<" h\t";
    for(int i = 0; i < nbVars; ++i){
        res << names[i] << "\t";
    } res << endl;
    return res.str();
    }
string Modele::printVarValues(double _t){
    stringstream res;
    res << _t/3600 << "h" << (_t - 3600 * (t/3600))/60 << "'\t" << std::fixed;
    for(int i = 0; i < nbVars; ++i){
        res << setprecision(4) << val[i] << "\t";
    } res << "\n";
    return res.str();}
string Modele::printParValues(){
    stringstream res;
    res << "Parameter values (\t" << nbParams << " params)\n";
    for(int i = 0; i < nbParams; ++i){
        res << i << "\t" << params[i] << "\t";
    } res << endl;
    return res.str();}
string Modele::printParNames(){
    stringstream res;
    for(int i = 0; i < nbParams; ++i){
        res << i << "\t" << getParamName(i) << "\t";
    } res << endl;
    return res.str();}



// ============================ 3 - Managing the names of variables and I/O to variables ========================

// virtual functions that don't need to be overrided (provided extNames and backSimulated are filled).
void Modele::setValue(int idGlobalVariable, double value){
    int id = internValName(idGlobalVariable); if((id >= 0) && (id < nbVars)) val[id] = value; else cerr << "Modele::setValue(" << idGlobalVariable << "," << value << "), this species ID is not implanted !! \n";}
void Modele::addValue(int idGlobalVariable, double value){
    int id = internValName(idGlobalVariable); if((id >= 0) && (id < nbVars)) val[id] += value; else cerr << "Modele::addValue(" << idGlobalVariable << "," << value << "), this species ID is not implanted !! \n";}
double Modele::getValue(int idGlobalVariable){
    int id = internValName(idGlobalVariable); if((id >= 0) && (id < nbVars)) return val[id]; else {cerr << "Modele::getValue(" << idGlobalVariable << "), this species ID is not implanted !! \n"; return -1;}}
bool Modele::isSimuBack(long long idGlobalBack){
    long long nb = backSimulated.size();
    for(int i = 0; i < nb; ++i){if(backSimulated[i] == idGlobalBack) return true;}
    return false;}
bool Modele::isVarKnown(int idGlobalVariable){return (internValName(idGlobalVariable) >= 0);}
int Modele::internValName(int idGlobalVariable){
    int nb = extNames.size();
    for(int i = 0; i < nb; ++i){if(extNames[i] == idGlobalVariable) return i;}
    return -1;}
int Modele::getNbParams(){return nbParams;}
void Modele::setParam(int i, double v){
    if((i < 0) || (i >= nbParams)) {cerr << "Modele::setParam(" << i << "," << v << "), index out of bounds. NbParams = " << nbParams << endl; return;}
    params[i] = (v > 0? v : -v);}   // fuck the system !!
double Modele::getParam(int i){
    if((i < 0) || (i >= nbParams)) {cerr << "Modele::getParam(" << i << "), index out of bounds. NbParams = " << nbParams << endl; return 0.0;}
    return params[i];}              // fuck the system !!
string Modele::getName(int internalID){
    if((internalID < 0) || (internalID >= nbVars)) {return string("");}
    return names[internalID];}
int Modele::getGlobalID(int internalID){
    if((internalID < 0) || (internalID >= nbVars)) {return -1;}
    return extNames[internalID];}
int Modele::getNbVars(){
    return nbVars;}
vector<double> Modele::getInit(){
    return init;}
double Modele::getT(){return t;}
vector<double> Modele::getParameters(){
    return params;}
void Modele::setParameters(vector<double> &newParamSet){
    if((int) newParamSet.size() != nbParams) {cerr << "ERR- Modele::setParameters(vector), wrong size for the given vector (" <<  newParamSet.size() << ") instead of " << nbParams << " parameters\n"; return;}
    for(int i = 0; i < nbParams; ++i){params[i] = newParamSet[i];}
    parametersLoaded = true;
}

string Modele::getParamName(int i){ if((i < 0) || (i >= nbParams)) {cerr << "ERR- Modele::getParamName(" << i << "), out of bounds (" << nbParams << " params)" << endl; return "";} else return paramNames[i];}
double Modele::getLowerBound(int i){ if((i < 0) || (i >= nbParams)) {cerr << "ERR- Modele::getLowerBound(" << i << "), out of bounds (" << nbParams << " params)" << endl; return 0;} else return paramLowBounds[i];}
double Modele::getUpperBound(int i){ if((i < 0) || (i >= nbParams)) {cerr << "ERR- Modele::getUpperBound(" << i << "), out of bounds (" << nbParams << " params)" << endl; return 0;} else return paramUpBounds[i];}
void Modele::setBounds(int i, double vLow, double vHi){ if((i < 0) || (i >= nbParams)) {cerr << "ERR- Modele::setBound(" << i << "), out of bounds (" << nbParams << " params)" << endl; return;} else {paramLowBounds[i] = vLow; paramUpBounds[i] = vHi;}}
void Modele::setBounds(vector<double> lowVals, vector<double> upVals){
    if(((int) upVals.size() != nbParams) || ((int) lowVals.size() != nbParams)) {cerr << "ERR- Modele::setBounds(vector1, vector2), the sizes of vectors don't match nbParams = " << nbParams << endl; return;}
    paramLowBounds = lowVals;
    paramUpBounds = upVals;}

void Modele::action(string name, double parameter){cerr << "ERR- Modele::action(string, double), this function should and was not implemented in the modele subclass\n";}
void Modele::action(string name, vector<double> parameters){cerr << "ERR- Modele::action(string, vector<double>), this function should and was not implemented in the modele subclass\n";}
// ============================ 4 - Managing the kinetic mode : records every xx secs ==============================

void Modele::setPrintMode(bool z, double _print_all_secs){
    saveKinetics = z;
    if(_print_all_secs > 0) print_all_secs = _print_all_secs;}
void Modele::newCinetiqueIfRequired(){
    if(cinetique != NULL) return;
    cinetique = new TableCourse(nbVars);
    cinetique->setHeader(0, string("Time"));
    for(int i = 0; i < nbVars; ++i){
        cinetique->setHeader(i+1, names[i]);}}
void Modele::deleteCinetique(){
    if(cinetique){delete cinetique; cinetique = NULL;}}
TableCourse Modele::getCinetique(){
    if(cinetique) return *cinetique;
    else return TableCourse(nbVars);}
void Modele::save_state(double _t){
    if(saveKinetics && cinetique){cinetique->addSet(t, val);}}

// ============================= 5 - Managing overriding data ========================================================

#ifdef USE_OVERRIDERS
void Modele::applyOverride(vector<double> &x, double t){
    if(currentOverrider){
        // to be further improved !!
        //cout << "OverAtt=\t" << t;
        for(int i = 0; i < nbVars; ++i){
            int z = extNames[i];
            if(z > 0) if((*currentOverrider)(z)) {x[i] = (*currentOverrider)(z, t);}
            //cout << "\t" << x[i] << "=" << (*currentOverrider)(z, t);
        }
        //cout << endl;
    }
}
void Modele::clearOverride(vector<double> &x, vector<double> &dxdt){
    if(currentOverrider){
        // to be further improved !!
        for(int i = 0; i < nbVars; ++i){
            int z = extNames[i];
            if(z > 0) if((*currentOverrider)(z)) {x[i] = 0.0; dxdt[i] = 0.0;}
        }
    }
}
void Modele::setOverrider(overrider* newOverrider){
    currentOverrider = newOverrider;
    //cerr << "SET overrider " << (currentOverrider ? "EXISTS" : "NULL") << endl;
}
bool Modele::over(int indexLocal){
    if(!currentOverrider) return false;
    if((indexLocal < 0) || (indexLocal >= nbVars)) return false;
    if(extNames[indexLocal] == 0) return false;  ///Whyyyy ???
    return (*currentOverrider)(extNames[indexLocal]);
}

#else
bool Modele::over(int indexLocal){return false;}
#endif
