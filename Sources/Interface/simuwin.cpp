#include "simuwin.h"
#include <sstream>
#include <fstream>

#ifndef WITHOUT_QT
#include "ui_simuwin.h"
#ifdef QT5
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#endif
#ifdef QT4
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>
#endif
#include <QThread>
#endif

#include <algorithm> // for reverse
#include "../Framework/parameterSets.h"

// Maximum number of columns (combinations) for the interface
#define NBCOLS 101



/// =============================== Part Class without graphical interface ========================

double manageSims::getCost(){
                                                        //if(dimension > currentModel->getNbParams()) {cerr << "Optimizer has more parameters than model\n"; return -1;}      // commented because would cause delay,
                                                        // this function is used during optimization, where the optimizer use manageSims as a 'generalImplementation' and gives parameters to the 'parameter' field and then call 'getCost'. Therefore, this function has to use the values in parameters[] to launch simulation and retrieve the cost.
    #ifdef ALLOW_OPTIMIZE                               // this requires the field 'parameters' inherited from the class generalImplementation --> need ALLOW_OPTIMIZE
    int NPs =  currentModel->getNbParams();
    for(int i = 0; i < NPs; ++i){                       // the parameters to optimize are real values, the other ones are NAN so they are not used to override the model's parameters.
        if(!std::isnan(parameters[i])) currentModel->setParam(i, parameters[i]);}
    #else
    cerr << "ERR: manageSims::getCost(), ALLOW_OPTIMIZE should be defined to use this function for optimization. Instead, the current parameter values from the model are taken for this time.\n";
    #endif

    simulate();
    double v1 = NAN, v2 = NAN;
    if(currentMode == MONO_EXPERIMENT){
        v1 = currentExperiment->costPart();
        v2 = currentExperiment->getPenalities();
    } else {
        v1 = listExperiments->costPart();
        v2 = listExperiments->getPenalities();
    }
    vector<double> set = currentModel->getParameters();
    history.addSet(&set, v1+v2);
    costRecords.newValue(v1+v2);
    return v1 + v2;
}

void manageSims::simulate(){
    currentExperiment->init();                      // This looks important, to clear the evaluators ...
    currentModel->setPrintMode(false, 25000);
    if(currentMode == MONO_EXPERIMENT){             // model initialization is done inside the function experiment->simulationsPart/simulate, no need to do it here
        currentExperiment->simulateAll(true);
    } else {
        listExperiments->simulateAll(true);
    }
}






/// ====================== Configuration <=> Modele (independent of the current experiment) ==========================

bool manageSims::isInConfig(int idParameter, int idConfig){
    if((idConfig < 0) || (idConfig >= nbCombs)) {cerr << "manageSims::isInConfig(idParameter=" << idParameter << ", idConfig=" << idConfig << "), wrong config index (only " << nbCombs << " combinations defined\n"; return false;};
    if((idParameter < 0) || (idParameter >= currentModel->getNbParams())) {cerr << "manageSims::isInConfig(idParameter=" << idParameter << ", idConfig=" << idConfig << "), wrong parameter index (only " << currentModel->getNbParams() << " parameters\n"; return false;};
    return(! currentConfig[idParameter][idConfig+4].compare("1"));
}

vector<int> manageSims::parametersInConfig(int idConfig){
    vector<int> result;
    for(int i = 0; i < currentModel->getNbParams(); ++i){
        if(isInConfig(i, idConfig)){
            result.push_back(i);
        }
    }
    return result;
}

string manageSims::loadConfig(string _name){
    if(recording) currentMacro << "macro" << macroID << ".loadConfig(" << _name << ")\n";
    if(recording) addFileToMacro(_name);

    cout << "   -> Reading configuration file " << _name;
    ifstream f(_name.c_str(), ios::in);
    if(!f) {return string("ERR: loadConfig, file couldn't be opened '") + _name + string("'\n");}
    stringstream listErrors;
    string moreInfo = string("   Expected file format :\n      NbParameters NbVariables NbCombinaisons, \n      followed by a table of NbParameters + NbVariables rows \n      and Nbcomnbinaisons + 4 columns : \n      Param_value ParamMin ParamMax Opt? Comb1 Comb2 ... (one line for each parameter)\n      .<tab>.<tab>.<tab> Simulate?  Comb1 Comb2 ... (one line for each variable) ");

    int NP = 0, NV = 0, NC = 0;
    f >> NP;
    f >> NV;
    f >> NC;
    if(NP != currentModel->getNbParams()) listErrors << "   -> Incorrect number of parameters  : current model has " << currentModel->getNbParams() << " while file says " << NP << "\n";
    if(NV != currentModel->getNbVars()) listErrors <<   "   -> Incorrect number of variables   : current model has " << currentModel->getNbVars() << " while file says " << NV << "\n";
    if((NC < 0) || (NC > NBCOLS - 4)) listErrors <<     "   -> Incorrect number of combinaisons: maximum number is " << NBCOLS - 4 << " while file has " << NC << "\n";
    if(listErrors.str().size() > 0) {
        return string("\n   ERR: Incorrect formating\n") + listErrors.str() + moreInfo;
    }
    cout << "\n      ... got : (" << NP << " params, " << NV << "vars, " << NC << " combs)" << endl;

    currentConfig.clear();
    currentConfig.resize(NP + NV);
    for(int i = 0; i < NP + NV; ++i){
        currentConfig[i].resize(NC+4);
        for(int j = 0; j < NC + 4; ++j){
            f >> currentConfig[i][j];
            if(i < NP){
                if(j == 0) currentModel->setParam(i, (double) atof((currentConfig[i][j]).c_str()));
                if(j == 2) currentModel->setBounds(i, (double) atof((currentConfig[i][1]).c_str()), (double) atof((currentConfig[i][2]).c_str()) );
            }
            //cout << (double) atof(currentConfig[i][j].c_str()) << "\t";
        }
        //cout << endl;
    }
    f.close();
    nbCombs = NC;
    return string("");
}

/// Phi : I have the impression this function could be merged with the previous one
string manageSims::resetParamSetFromConfig(string _name){
    if(recording) currentMacro << "macro" << macroID << ".resetParamSetFromConfig(" << _name << ")\n";

    vector<double> newParameterSet;
    ifstream f(_name.c_str(), ios::in);
    if(!f) {return string("ERR: loadConfig, incorrect file name\n");}
    stringstream listErrors;
    string moreInfo = string("Expected file format :\n   NbParameters NbVariables NbCombinaisons, \n   followed by a table of NbParameters + NbVariables rows \n   and Nbcomnbinaisons + 4 columns : \n   Param_value ParamMin ParamMax Opt? Comb1 Comb2 ...");

    int NP = 0, NV = 0, NC = 0;
    f >> NP;
    f >> NV;
    f >> NC;
    if(NP != currentModel->getNbParams()) listErrors << "-> Incorrect number of parameters  : current model has " << currentModel->getNbParams() << " while file says " << NP << "\n";
    if(NV != currentModel->getNbVars()) listErrors <<   "-> Incorrect number of variables   : current model has " << currentModel->getNbVars() << " while file says " << NV << "\n";
    if((NC < 0) || (NC > NBCOLS - 4)) listErrors <<     "-> Incorrect number of combinaisons: maximum number is " << NBCOLS - 4 << " while file has " << NC << "\n";
    if(listErrors.str().size() > 0) {return string("Incorrect formating\n") + listErrors.str() + moreInfo;}

    currentConfig.clear();
    currentConfig.resize(NP + NV);
    for(int i = 0; i < NP + NV; ++i){
        currentConfig[i].resize(NC+4);
        for(int j = 0; j < NC + 4; ++j){
            f >> currentConfig[i][j];
            if(i < NP){
                //(see comment below) if(j == 0) currentModel->setParam(i, (double) atof((currentConfig[i][j]).c_str()));
                if(j == 0) newParameterSet.push_back((double) atof((currentConfig[i][j]).c_str()));
            }
        }
    }

    f.close();
    currentModel->setParameters(newParameterSet);   // note : this function will also check that the newparameterset has the good size.
                                                    // it is important to use setParameters(full set) rather than giving parameters one by one,
                                                    // because the model doesn't allow to be simulated without an initial full set of parameters,
                                                    // and the boolean checkpoint 'parametersLoaded' inside the modele is only checked when a full parameter set is given.
    return string();
}

void manageSims::resetConfigFromModel(){
    if(recording) currentMacro << "macro" << macroID << ".resetConfigFromModel()\n";

    int NP = currentModel->getNbParams();
    int NV = currentModel->getNbVars();
    int NC = 0;

    currentConfig.clear();
    currentConfig.resize(NP + NV);

    for(int i = 0; i < NP + NV; ++i){
        currentConfig[i].resize(NC+4);
        for(int j = 0; j < NC + 4; ++j){
            if(i < NP){
                stringstream ss;
                if(j == 0) ss << currentModel->getParam(i);
                if(j == 1) ss << currentModel->getLowerBound(i);
                if(j == 2) ss << currentModel->getUpperBound(i);
                currentConfig[i][j] = ss.str();
            }
        }
    }
    nbCombs = NC;
}

// this function only takes the parameter values and boundaries
void manageSims::updateConfigParamsFromModel(){
    if(recording) currentMacro << "macro" << macroID << ".updateConfigParamsFromModel()\n";

    int NP = currentModel->getNbParams();
    int NV = currentModel->getNbVars();

    if(currentConfig.size() == 0) resetConfigFromModel();       // in case no config loaded yet.
    if((int) currentConfig.size() != NP + NV) {cerr << "ERR: manageSims::updateConfigParamsFromModel(), incompatible sizes between model (NParams = " << NP << ", NVars = " << NV << " and configuration of size NP+NV = " << currentConfig.size() << endl; return;}

    for(int i = 0; i < NP + NV; ++i){
        for(int j = 0; j <= 2; ++j){    // just for parameter value and bounds
            if(i < NP){
                stringstream ss;
                if(j == 0) ss << currentModel->getParam(i);
                if(j == 1) ss << currentModel->getLowerBound(i);
                if(j == 2) ss << currentModel->getUpperBound(i);
                currentConfig[i][j] = ss.str();
            }
        }
    }
}



//============== History management =================




vector<double> manageSims::bestSetFromHistory(){
    return history.bestSet();
}

void manageSims::saveBestSetFromHistory(string _name){
    history.saveBestSet(_name);
}

void manageSims::saveHistory(string _name){
    if(recording) currentMacro << "macro" << macroID << "." << "saveHistory(" << _name << ")" << "\n";

    int NP = currentModel->getNbParams();
    int storeMax = 1e9;
    cout << "      ... Writing into file " << _name << endl;
    ofstream f(_name.c_str(), ios::out);
    if(!f) {cerr << "File not Found" << _name << endl; return;}
    vector<oneSet*> liste = history.toVector();
    reverse(liste.begin(), liste.end());
    int ls = liste.size();

    f << NP << "\t";
    f << ls << "\n";

    for(int i = 0; (i < ls) && (i < storeMax); ++i){
        f << liste[i]->cost;
        int s = liste[i]->v.size();
        if(s != NP) cerr << "ERR : save History incorrect size for parameters\n";
        for(int j = 0; j < NP; ++j){
            f << "\t" << liste[i]->v[j];
        }
        f << "\n";
    }
    f.close();
    addFileToMacro( _name);
}

pSets readHistory(string _name){
    int storeMax = 1e9;
    cout << "      ... Reading history file " << _name << endl;
    ifstream f(_name.c_str(), ios::in);
    if(!f) {cerr << "ERR: pSets readHistory(" << _name << "), File not Found" << endl;}

    int NP = 0; // = currentModel->getNbParams();
    int ls = 0; // = liste.size();
    f >> NP;
    f >> ls;
    pSets history = pSets(10000, NP);
    history.clear();
    if((ls > storeMax) || (ls < 0)) {cerr << "manageSims::loadHistory(...) Incorrect number of lines (" << ls << ")\n"; return history;}

    for(int i = 0; (i < ls) && (i < storeMax); ++i){
        vector<double>* tempStore = new vector<double>(NP, 0.0);
        double tempCost = 0;
        f >> tempCost;
        for(int j = 0; j < NP; ++j){
            f >> (*tempStore)[j];
        }
        history.addSet(tempStore, tempCost);
        delete tempStore;
    }
    f.close();
    return history;
}


void manageSims::loadHistory(string _name){
    if(recording) currentMacro << "macro" << macroID << "." << "loadHistory(" << _name << ")" << "\n";
    int storeMax = 1e9;
    cout << "      ... Reading file " << _name << endl;
    ifstream f(_name.c_str(), ios::in);
    if(!f) {cerr << "File not Found" << endl; return;}

    history.clear();
    //vector<oneSet*> liste = history.toVector();
    //reverse(liste.begin(), liste.end());
    int NP = 0; // = currentModel->getNbParams();
    int ls = 0; // = liste.size();

    f >> NP;
    f >> ls;
    cerr << "      ... Expecting " << NP << " parameters and " << ls << " lines"<< endl;
    if(NP != currentModel->getNbParams()) {cerr << "manageSims::loadHistory(...) Incorrect file format : " << NP << " parameters provided while " << currentModel->getNbParams() << " in the model\n"; return;}
    if((ls > storeMax) || (ls < 0)) {cerr << "manageSims::loadHistory(...) Incorrect number of lines (" << ls << ")\n"; return;}

    for(int i = 0; (i < ls) && (i < storeMax); ++i){
        vector<double>* tempStore = new vector<double>(NP, 0.0);
        double tempCost = 0;
        f >> tempCost;
        cerr << tempCost << " - ";
        for(int j = 0; j < NP; ++j){
            f >> (*tempStore)[j];
            cerr << j << "=" << (*tempStore)[j] << "\t";
        }
        cerr << endl;
        history.addSet(tempStore, tempCost);
        delete tempStore;
    }
    f.close();
    addFileToMacro( _name);
}






/// returns the new list of parameters by "replacing" the parameters from indexSet,
/// **** according to the configuration ****,
vector<double> manageSims::useParamSetFromHistory(int indexSet, int indexCombToOverride){

    vector<oneSet*> v = history.toVector();
    reverse(v.begin(), v.end());
    int s = v.size();
    if((indexSet < 0) || (indexSet >= s)) {cerr << "ERR: manageSims::useParamSetFromHistory(" << indexSet << "), wrong index (only " << s << " sets in the history) \n"; return vector<double>();}
    oneSet* theSet = v[indexSet];
    int NP = theSet->v.size();
    vector<double> result = vector<double>(NP, NAN);
    if(NP != (int) currentModel->getNbParams()) {cerr << "ERR: manageSims::useParamSetFromHistory(...), not the same parameter numbers in the history (" << NP << " than in the model " << currentModel->getNbParams() << endl; return result;}
    if(indexCombToOverride < 0){
        currentModel->setParameters(theSet->v); // to unlock simulations if no valid parameters set before
        /*for(int j = 0; j < NP; ++j){
           currentModel->setParam(j, theSet->v[j]);
        }*/
    } else if(indexCombToOverride < nbCombs){
        for(int j = 0; j < NP; ++j){    // note : in this case, a full valid set of parameters should have been given before
            if(!currentConfig[j][indexCombToOverride+4].compare(string("1"))){
                currentModel->setParam(j, theSet->v[j]);
                result[j] =  theSet->v[j];
            } else result[j] = currentModel->getParam(j);//to avoid NANs
        }
    } else cerr << "ERR: manageSims::useParamSetFromHistory(set nr:" << indexSet << ", comb nr:" << indexCombToOverride << "), invalid combination index, only " << nbCombs << " defined combinations\n";
    return result;
}










/// Optimizes according to the current settings of the model:
/// overriding variables and defining parameter boundaries should have been done before calling this function.
void manageSims::motherOptimize(string optFileName, int nbSetsToRecord){
    if(recording) currentMacro << "macro" << macroID << "." << "motherOptimize(" << optFileName << "," << nbSetsToRecord << ")" << "\n";
    addFileToMacro(optFileName);

    #ifdef ALLOW_OPTIMIZE
    nbCostCalls = 0;
    history.resize(nbSetsToRecord);
    history.clear();
    costRecords.clear();

    GeneralImplementation::initialize();                                    // is doing parameters.resize(currentModel->getNbParams(), NAN);
    myRandom::Randomize();
    myTimes::getDiffTime();
    BaseOptimizationProblem *E = this;                                      //instead of saying new simuWin(currentExperiment, this);
    BaseOptimizer *Opt = BaseOptimizer::createOptimizer(E, optFileName);
    //mySignal::addOptimizer(Opt);
    Opt->optimize();
    Opt->bestGlobal.print();
    currentModel->needNewParameterSet();
    //delete E; // program destructor first...
    delete Opt;
    cout << "      ... Optimization Complete, elapsed time : " << myTimes::getDiffTime() << " sec\n";
    #else
    cerr << "manageSims::motherOptimize(...), this function is only allowed when ALLOW_OPTIMIZE is defined, in order to include the optimizations files during compilation.\n";
    #endif
}

string manageSims::motherCreateOptimizerFile(int indexComb, string optMethodText, int parameterToExclude){
    if(parameterToExclude >= currentModel->getNbParams()) {cerr << "ERR: motherCreateOptimizerFile, incorrect parameter index (to exclude) " << parameterToExclude << endl; return string("");}
    if(parameterToExclude >= 0) cout << "      ... Excluding parameter from optimization : " << parameterToExclude << " (" << currentModel->getParamName(parameterToExclude) << ")\n";
    if(recording) {
        currentMacro << "string optMethod = string(\"" << optMethodText << "\"\n";
        currentMacro << "macro" << macroID << "." << "motherCreateOptimizerFile(" << indexComb << ", optMethod, " << parameterToExclude << ")" << "\n";    }
    if((indexComb < -1) || (indexComb >= nbCombs)) {cerr << "ERR:manageSims::createOptimizer, index comb is out of bounds. Note that indexComb=-1 means pooling parameters and variables from all combinations\n"; return string("");}
    int combStart = (indexComb < 0 ? 0 : indexComb);            // indexComb = -1 means pool all the parameters to optimize from each configuration
    int combEnd   = (indexComb < 0 ? nbCombs-1 : indexComb);

    int NP = currentModel->getNbParams();
    vector<bool> paramsToOptimize = vector<bool>(NP, false);

    stringstream f;
    f << optMethodText;
    f << "\n";
    int nbToOpt = 0;
    for(int i = 0; i < NP; ++i)
        for(int j = combStart; j <= combEnd; ++j){
            if((!currentConfig[i][4+j].compare("1")) && (i != parameterToExclude) && (!paramsToOptimize[i])) {nbToOpt++; paramsToOptimize[i] = true;}  }//cout << currentConfig[i][4+i] << ".";
    if(nbToOpt == 0){cerr << "ERR: manageSims::motherCreateOptimizerFile(index=" << indexComb << ",txt) - No parameter selected for optimization -> 0 parameter to optimize !!\n"; return f.str();}
    f << nbToOpt << "\n";
    for(int i = 0; i < NP; ++i){
        if((!paramsToOptimize[i]) || (i == parameterToExclude)) f << "#";   // comments the parameters that will not be optimized
        //f << "\t" << i << "\t" << currentModel->getLowerBound(i) << "\t" << currentModel->getUpperBound(i) << "\t#" << currentModel->getParamName(i) << endl;
        f << "\t" << i << "\t" << currentConfig[i][1] << "\t" << currentConfig[i][2] << "\t#" << currentModel->getParamName(i) << endl;
    }
    f << "\n\n0\nExponential\n0\n";
    return f.str();
}








// *** depends on the experiments ***

// Overrides a model according to the current combination
// this depends on the experiments (who are owning the overriders : a curve for each sub-experiment)
// for each variable which is requested to be overrided (by the configuration),
//    -> in MONO_EXPERIMENT mode, then it is overrided only if it is possible (if there is a data-curve for each sub-experiment)
//    -> in MULTI_EXPERIMENT mode, for each experiment, it is overrided according to the mono-exp separately.
void manageSims::motherOverrideUsingComb(int newIndex){
    cerr << "OVER using comb " << newIndex << endl;
    if((newIndex < -1) || (newIndex >= nbCombs)) {cerr << "Wrong comb index " << newIndex << "\n"; return;}
    if(recording) {currentMacro << "macro" << macroID << "." << "motherUseComb(" << newIndex << ")" << "\n";}
    int NP = currentModel->getNbParams();
    int NV = currentModel->getNbVars();
    if((int) currentConfig.size() != NP + NV) {cerr << "manageSims::motherOverrideUsingComb, Wrong size of saved configuration\n";return;}
    int combStart = (newIndex < 0 ? 0 : newIndex);
    int combEnd   = (newIndex < 0 ? nbCombs-1 : newIndex);

    for(int i = 0; i < NV; ++i){
        bool override = true;
        for(int j = combStart; j <= combEnd; ++j){              // override only if all selected combinations override (i.e. override the minimum, simulate the maximum)
            override = override && (!currentConfig[i+NP][4+j].compare("1"));
        }
        if(currentMode == MONO_EXPERIMENT){
            override = override && (currentExperiment->canOverride(currentModel->getGlobalID(i)));      // ... and if the variable has data to be overrided
            currentExperiment->overrideVariable(currentModel->getGlobalID(i), override);

        } else {
            for(int pp = 0; pp < listExperiments->nbBigExp(); ++pp){
                bool localOverride  = override && (listExperiments->getExperiment(pp)->canOverride(currentModel->getGlobalID(i)));      // ... and if the variable has data to be overrided
                /////§§§ toCHeck :: if(localOverride) cerr << "SUCCESS§§§" << endl; else cerr << "wHY§§§" << endl;
                listExperiments->getExperiment(pp)->overrideVariable(currentModel->getGlobalID(i), localOverride);
            }
        }
        if(override)    cout << "         is overrided by data :   ";
        else            cout << "         is simulated :           ";
        cout << "Variable " << i << "\tglobalId = " << currentModel->getGlobalID(i) << "\t(" << currentModel->getName(i) << ")";
        if(!override) cout << "*";
        cout << endl;
    }
}


// Independent : cuts a segment into n points (log or not)
vector<double> cutSpace(int nbPoints, bool logarithmic, double base, double vstart, double vending){
    if((nbPoints < 0) || (nbPoints > 1e9)) { cerr << "ERR: cutSpace, out of bounds number of points " << nbPoints << "\n"; return vector<double>();}
    vector<double> valuesToTest = vector<double>(nbPoints, 0.0);

    if(base < 0) base = 2.0;
    if(logarithmic){
        double lstart = log(vstart)/ log(base);
        double lending = log(vending) / log(base);
        double interval = (lending - lstart) / (nbPoints - 1);
        for(int i = 0; i < nbPoints; ++i){
            valuesToTest[i] = pow(base, lstart + interval * i);
            //cout << valuesToTest[i] << endl;
        }
    } else {
        double interval = (vending - vstart) / (nbPoints - 1);
        for(int i = 0; i < nbPoints; ++i){
            valuesToTest[i] = vstart + interval * i;
            //cout << valuesToTest[i] << endl;
        }
    }
    return valuesToTest;
}



// *** Analysis of a single experiment ***
void manageSims::motherSensitivity(vector<double>& initialSet, int parameterIndex){

    if(currentMode == MULTI_EXPERIMENT){cerr << "ERR: manageSims::motherSensitivity is not allowed in the MULTI_EXPERIMENTS mode\n"; return;}
    int NP = currentModel->getNbParams();
    if(parameterIndex >= NP) return;
    if((int) sensitivities.size() != NP) {cerr << "Internal ERR:  manageSims::motherSensitivity, sensitivities has wrong size (" << sensitivities.size() << ")compared to nb of parameters (" << NP << ")\n";}
    //vector<double> initialParamSet = currentModel->getParameters();
    for(int i = 0; i < NP; ++i){
        if((parameterIndex == i) || (parameterIndex < 0)){
            cerr << i << endl;
            delete sensitivities[i];
            sensitivities[i] = new oneParameterAnalysis(initialSet);
            cerr << "." << endl;


            // manually adds the initial parameter set. Note : it should not be sorted later.
            currentModel->setParameters(initialSet);
            double v = getCost();
            cout << "\t cost=" << v << endl;
            vector<double> costPerVariable = vector<double>(currentModel->getNbVars(), 0);
            for(int j = 0; j < currentModel->getNbVars(); ++j){
                costPerVariable[j] = currentExperiment->costPart(currentModel->getGlobalID(j));
            }
            vector<double> costPerExperiment = vector<double>(currentExperiment->nbCond(), 0);
            for(int j = 0; j < currentExperiment->nbCond(); ++j){
                costPerExperiment[j] = currentExperiment->costExp(j);
            }
            sensitivities[parameterIndex]->add(parameterIndex, initialSet[parameterIndex], v, costPerVariable, costPerExperiment, currentModel->getParameters());



            motherRecursiveSensitivity(initialSet, i, 20, true, 2, currentModel->getLowerBound(i), currentModel->getUpperBound(i), 5);
            cout << sensitivities[i]->print();
        }
    }
}

void manageSims::motherRecursiveSensitivity(vector<double>& initialSet, int parameterIndex, int nbPoints, bool logarithmic, double base, double vstart, double vending, int deepLevel){
    if(recording) currentMacro << "macro" << macroID << "." << ".motherRecursiveSensitivity(" << parameterIndex << "," << nbPoints << "," << (logarithmic ? "true": "false")  << "," << base << "," << vstart << "," << vending << "," << deepLevel << ")" << "\n";
    if(deepLevel == 0) return;
    if(deepLevel < 0) deepLevel = 0;
    if(deepLevel > 1000) { cerr << "ERR: manageSims::motherSensitivity, out of bounds number of levels " << deepLevel << endl; return;}
    if((parameterIndex < 0) || (parameterIndex > currentModel->getNbParams())){cerr << "ERR: manageSims::motherSensitivity, wrong parameter index "<< parameterIndex << ", only " << currentModel->getNbParams() << " parameters\n"; return;}
    if((nbPoints < 0) || (nbPoints > 1e9)) { cerr << "ERR: manageSims::motherSensitivity, out of bounds number of points " << nbPoints << "\n"; return;}
    if(vstart < 0) vstart = currentModel->getLowerBound(parameterIndex);
    if(vending < 0) vending = currentModel->getUpperBound(parameterIndex);

    vector<double> valuesToTest = cutSpace(nbPoints, logarithmic, base, vstart, vending);
    int NV = currentModel->getNbVars();
    for(int i = 0; i < nbPoints; ++i){
        cout << "sensitivity" << i << ", param" << parameterIndex << " = " << valuesToTest[i] ;
        currentModel->setParameters(initialSet);
        currentModel->setParam(parameterIndex, valuesToTest[i]);

        double v = getCost();
        cout << "\t cost=" << v << endl;
        vector<double> costPerVariable = vector<double>(NV, 0);
        for(int j = 0; j < NV; ++j){
            costPerVariable[j] = currentExperiment->costPart(currentModel->getGlobalID(j));
        }
        vector<double> costPerExperiment = vector<double>(currentExperiment->nbCond(), 0);
        for(int j = 0; j < currentExperiment->nbCond(); ++j){
            costPerExperiment[j] = currentExperiment->costExp(j);
        }
        sensitivities[parameterIndex]->add(parameterIndex, valuesToTest[i], v, costPerVariable, costPerExperiment, currentModel->getParameters());

    }
    //sensitivities[parameterIndex]->sort();

    // call the mother recursive
}


// *** Analysis of a single experiment ***

void manageSims::prepareOptFilesForIdentifibiality(string folder, int parameterIndex, int indexComb, string optMethodText){
    int NP = currentModel->getNbParams();
    if(parameterIndex >= NP) {cerr << "ERR: manageSims::prepareOptFilesForIdentifibiality, parameterIndex out of bounds (" << parameterIndex << ")\n"; return;}
    for(int i = 0; i < NP; ++i){
        if((i == parameterIndex) || (parameterIndex < 0)){
            stringstream fileName;
            fileName << folder << "/OptForParam" << i << "InComb" << indexComb << ".txt";
            optFileNamesIdentifiability[i] = fileName.str();
            string optFileWithoutTheParameter = motherCreateOptimizerFile(indexComb, optMethodText, i);
            ofstream f(fileName.str().c_str(), ios::out);
            if(f){f << optFileWithoutTheParameter; f.close();} else cerr << "ERR: manageSims::prepareOptFilesForIdentifibiality, could not create " << fileName.str() << endl;
        }
    }
}

void manageSims::motherIdentifiability(vector<double>& initialSet, int parameterIndex){
    if(currentMode == MULTI_EXPERIMENT){cerr << "ERR: manageSims::motherIdentifiability is not allowed in the MULTI_EXPERIMENTS mode\n"; return;}
    int NP = currentModel->getNbParams();
    if(parameterIndex >= NP) return;
    if((int) identifiabilities.size() != NP) {cerr << "Internal ERR:  manageSims::motherIdentifiability, identifiabilities has wrong size (" << identifiabilities.size() << ")compared to nb of parameters (" << NP << ")\n";}
    //vector<double> initialParamSet = currentModel->getParameters();
    for(int i = 0; i < NP; ++i){
        if((parameterIndex == i) || (parameterIndex < 0)){

                if(identifiabilities[i]) delete identifiabilities[i];
                identifiabilities[i] = new oneParameterAnalysis(initialSet);

                ///// puts the initial set as the first line
                currentModel->setParameters(initialSet);
                double v = getCost();
                vector<double> costPerVariable = vector<double>(currentModel->getNbVars(), 0);
                for(int j = 0; j < currentModel->getNbVars(); ++j){
                    costPerVariable[j] = currentExperiment->costPart(currentModel->getGlobalID(j));
                }
                vector<double> costPerExperiment = vector<double>(currentExperiment->nbCond(), 0);
                for(int j = 0; j < currentExperiment->nbCond(); ++j){
                    costPerExperiment[j] = currentExperiment->costExp(j);
                }
                identifiabilities[parameterIndex]->add(parameterIndex, initialSet[parameterIndex], v, costPerVariable, costPerExperiment, currentModel->getParameters());



                motherRecursiveIdentifibiality(initialSet, i, 20, true, 2, currentModel->getLowerBound(i), currentModel->getUpperBound(i), 5);
                cout << "Parameter " << i << " finished" << endl;
                //cout << identifiabilities[i]->print();

        }
    }
    cout << "Identifiability finished" << endl;
}

void manageSims::motherRecursiveIdentifibiality(vector<double>& initialSet, int parameterIndex, int nbPoints, bool logarithmic, double base, double vstart, double vending, int deepLevel){
    if(recording) currentMacro << "macro" << macroID << "." << ".motherRecursiveIdentifibiality(" << parameterIndex << "," << nbPoints << "," << (logarithmic ? "true": "false")  << "," << base << "," << vstart << "," << vending << "," << deepLevel << ")" << "\n";
    #ifdef ALLOW_OPTIMIZE
    if(deepLevel == 0) return;
    if(deepLevel < 0) deepLevel = 0;
    if(deepLevel > 1000) { cerr << "ERR: manageSims::motherRecursiveIdentifibiality, out of bounds number of levels " << deepLevel << endl; return;}
    if((parameterIndex < 0) || (parameterIndex > currentModel->getNbParams())){cerr << "ERR: manageSims::motherRecursiveIdentifibiality, wrong parameter index "<< parameterIndex << ", only " << currentModel->getNbParams() << " parameters\n"; return;}
    if((nbPoints < 0) || (nbPoints > 1e9)) { cerr << "ERR: manageSims::motherRecursiveIdentifibiality, out of bounds number of points " << nbPoints << "\n"; return;}
    if(vstart < 0) vstart = currentModel->getLowerBound(parameterIndex);
    if(vending < 0) vending = currentModel->getUpperBound(parameterIndex);

    vector<double> valuesToTest = cutSpace(nbPoints, logarithmic, base, vstart, vending);
    int NV = currentModel->getNbVars();
    for(int i = 0; i < (int) valuesToTest.size(); ++i){
        for(int nRep = 0; nRep < 5; ++nRep ){
            history.resize(1);
            history.clear();
            currentModel->setParameters(initialSet);
            currentModel->setParam(parameterIndex, valuesToTest[i]);
            cout << "      ... Identifiability, point " << i+1 << "." << nRep + 1 << " / " << nbPoints << ", param " << parameterIndex << "(" << currentModel->getParamName(parameterIndex) << ") = " << valuesToTest[i] << " ";
            GeneralImplementation::initialize();
            myRandom::Randomize();
            myTimes::getTime();
            BaseOptimizationProblem *E = this; //new simuWin(currentExperiment, this);
            BaseOptimizer *Opt = BaseOptimizer::createOptimizer(E, optFileNamesIdentifiability[parameterIndex]);
            //mySignal::addOptimizer(Opt);
            Opt->optimize();
            //Opt->bestGlobal.print();
            //delete E; // Do not do that :-)
            currentModel->needNewParameterSet();
            delete Opt;
            currentModel->setParameters(history.toVector()[0]->v);

            double v = getCost();
            cout << " -> Cost : " << v << endl;
            vector<double> costPerVariable = vector<double>(NV, 0);
            for(int j = 0; j < NV; ++j){
                costPerVariable[j] = currentExperiment->costPart(currentModel->getGlobalID(j));
            }
            vector<double> costPerExperiment = vector<double>(currentExperiment->nbCond(), 0);
            for(int j = 0; j < currentExperiment->nbCond(); ++j){
                costPerExperiment[j] = currentExperiment->costExp(j);
            }
            identifiabilities[parameterIndex]->add(parameterIndex, valuesToTest[i], v, costPerVariable, costPerExperiment, currentModel->getParameters());

            currentModel->needNewParameterSet();
        }
    }
    cout << "Identifiability finished for this parameter\n";
    //identifiabilities[parameterIndex]->sort();
    #else
    cerr << "manageSims::motherRecursiveIdentifibiality(...), this function is only allowed when ALLOW_OPTIMIZE is defined, in order to include the optimizations files during compilation.\n";
    #endif

}

void manageSims::makeIdentifibialityReport(int parameterID, string existingBaseFolder, int currentConfigID){
    if(currentMode == MULTI_EXPERIMENT){cerr << "ERR: manageSims::makeIdentifiabilityReport is not allowed in the MULTI_EXPERIMENTS mode\n"; return;}
    if(parameterID >= currentModel->getNbParams()){cerr << "ERR: manageSims::makeIdentifibialityReport(paramID= " << parameterID << "), out of bounds, only " << currentModel->getNbParams() << " parameters\n"; return;}
    if(parameterID < 0) {
        for(int i = 0; i < currentModel->getNbParams(); ++i){
            makeIdentifibialityReport(i, existingBaseFolder, currentConfigID);
        }
        return;
    }

    int NP = currentModel->getNbParams();
    int NV = currentModel->getNbVars();
    int NE = currentExperiment->nbCond();
    oneParameterAnalysis* data = identifiabilities[parameterID];
    stringstream head;
    head << "PointNr\tIDparam\tValue\tCost\tNbExps\t";
    for(int i = 0; i < NE; ++i){
        head << "\"" << currentExperiment->expName(i) << "\"\t";
    }
    head << "NbVars\t";
    for(int i = 0; i < NV; ++i){
        head << "\"" << currentModel->getName(i) << "\"\t";
    }
    head << "NbParameters\t";
    for(int i = 0; i < NP; ++i){
        head << "\"" << currentModel->getParamName(i) << "\"\t";
    }
    head << endl;
    head << data->print();

    stringstream fname; fname << existingBaseFolder << "reportIdentifParam" << parameterID << ".txt";
    fstream fp(fname.str(), ios::out); if(fp) {fp << head.str(); fp.close();}




    stringstream Rscr;
    //Rscr << "install.packages(\"stringr\")\n";
    Rscr << "library(stringr) #for str_replace\n";
    Rscr << "setwd(\"" << existingBaseFolder << "\");\n";
    Rscr << "a <- read.table(\"" << fname.str() << "\", header=TRUE);";
    Rscr << "#changing . to space in the column names ; saved in nm\n";
    Rscr << "nm <- colnames(a)\n";
    Rscr << "nm <-str_replace_all(nm, fixed(\".\"), \" \")\n";
    Rscr << "# there are " << NP << " parameters, " << NV << " variables and " << NE << " conditions " << endl;

    // 1 ------------------- main plot : global cost
    Rscr << "png(filename=\"IdentifP" << parameterID << "A0.png\");\n";
    Rscr << "plot(a[,3], a[,4], type=\"o\", pch=1, cex = 1, col=\"mediumorchid4\", main=\"Cost profile for P" << parameterID << "(" << currentModel->getParamName(parameterID) << ")\", sub=\"Each point is an optimization around the best parameter set\", xlab=nm[3], ylab=nm[4], lwd=1);\n";
    Rscr << "dev.off();\n";





    vector<string> Rcolors = {string("red"), string("royalblue"), string("seagreen1"), string("darkorchid"), string("gold"), string("deeppink"), string("lightsalmon"), string("mediumpurple"), string("mediumspringgreen"), string("midnightblue"), string("orange1"), string("palegreen"), string("chartreuse3"), string("blue2"), string("firebrick1")};

    // 2 ------------------- cost per experiment
    for(int t = 0; t < 2; ++t)
    {
        Rscr << "png(filename=\"IdentifP" << parameterID << "A1-perExperiment" << ((t == 0) ? "-lin" : "-log") << ".png\");\n";
        Rscr << "axisMax = max(a[,4]/" << NE;
        for(int i = 0; i < NE; ++i){
            Rscr << ", a[," << 6+i << "]";
        }
        Rscr << ");\n";

        Rscr << "plot(a[,3], a[,4]/" << NE << "," << ((t == 1) ? "log=\"x\"," : "") << "ylim=c(0, axisMax * 1.2), type=\"o\", pch=1, cex = 1, col=\"mediumorchid4\", main=\"Cost profile for P" << parameterID << "(" << currentModel->getParamName(parameterID) << ")\", sub=\"Each point is an optimization around the best parameter set\", xlab=nm[3], ylab=nm[4], lwd=1);\n";
        for(int i = 0; i < NE; ++i){
            Rscr << "lines(a[,3], a[," << 6+i << "], type=\"o\", pch=1, cex = 1, col=\"" << Rcolors[i] << "\", lwd=1);\n";
        }

        Rscr << "legend(\"topright\", inset=0.05, legend=c(\"Average\",";
        for(int i = 0; i < NE; ++i){
            if(i > 0) Rscr << ",";
            Rscr << "\"" << currentExperiment->expName(i) << "\"";
        }
        Rscr << "), lwd=c(1,1), lty=c(1,1), pch=c(1,2), pt.cex=c(1,1),";
        Rscr << "col=c(\"mediumorchid4\"";
        for(int i = 0; i < NE; ++i){
            Rscr << ",\"" << Rcolors[i] << "\"";
        }
        Rscr << "), cex=1, bg=\"white\", box.lwd=0.5, box.lty=1, box.col=\"black\");\n";
        Rscr << "dev.off();\n";
    }

    // 3 ------------------- correlation between parameters

    for(int i = 0; i < NP; ++i){
        if((i != parameterID) && (isInConfig(i, currentConfigID))){
            Rscr << "axisMax = max(a[,3], a[," << 8+ NE + NV + i << "]);\n";
            Rscr << "costMin = min(a[," << 8+ NE + NV + i << "]);\n";
            for(int t = 0; t < 2; ++t){

                Rscr << "png(filename=\"IdentifP" << parameterID << "B" << i << "correl" << currentModel->getParamName(i) << ((t == 0) ? "-lin" : "-log") << ".png\");\n";
                Rscr << "plot(a[,3], a[," << 8+ NE + NV + i << "]," << ((t == 1) ? "log=\"x\"," : "") <<  "ylim=c(0, axisMax * 1.2), type=\"o\", pch=1, cex = 1, col=\"mediumorchid4\", main=\"Best fit for P " << i << " (" << currentModel->getParamName(i) << "), when identifying P" << parameterID << "(" << currentModel->getParamName(parameterID) << ")\", sub=\"Each point is an optimization around the best parameter set\", xlab=\"forced value P" << parameterID << " (" << currentModel->getParamName(parameterID) << ")\", ylab=\"Fitted value for P" << i << " (" << currentModel->getParamName(i) << ")\", lwd=1);\n";
                Rscr << "dev.off();\n";

                Rscr << "png(filename=\"IdentifP" << parameterID << "B" << i << "correl" << currentModel->getParamName(i) << "Zoom" << ((t == 0) ? "-lin" : "-log") << ".png\");\n";
                Rscr << "plot(a[,3], a[," << 8+ NE + NV + i << "]," << ((t == 1) ? "log=\"x\"," : "") <<  "ylim=c(costMin, costMin * 2.5), type=\"o\", pch=1, cex = 1, col=\"mediumorchid4\", main=\"Best fit for P " << i << " (" << currentModel->getParamName(i) << "), when identifying P" << parameterID << "(" << currentModel->getParamName(parameterID) << ")\", sub=\"Each point is an optimization around the best parameter set\", xlab=\"forced value P" << parameterID << " (" << currentModel->getParamName(parameterID) << ")\", ylab=\"Fitted value for P" << i << " (" << currentModel->getParamName(i) << ")\", lwd=1);\n";
                Rscr << "dev.off();\n";
            }
        }
    }

    stringstream Rname; Rname << existingBaseFolder << "RscriptP" << parameterID << ".R";
    fstream fR(Rname.str(), ios::out); if(fR) {fR << Rscr.str(); fR.close();}

    stringstream cmd; cmd << "R --vanilla < " << Rname.str() << endl;
    system(cmd.str().c_str());

}



/*  void test

double tryfunct(double x){
    return 0.02 * (x-2) * (x-3) * sin(x);
}

void tryrecursive(vector<double>& initialSet, int parameterIndex, int nbPoints, bool logarithmic, double base, double vstart, double vending, int deepLevel){
    if(deepLevel == 0) return;
    if(deepLevel < 0) deepLevel = 0;
    if(deepLevel > 1000) { cerr << "ERR: manageSims::motherRecursiveIdentifibiality, out of bounds number of levels " << deepLevel << endl; return;}
    if((nbPoints < 0) || (nbPoints > 1e9)) { cerr << "ERR: manageSims::motherRecursiveIdentifibiality, out of bounds number of points " << nbPoints << "\n"; return;}
    if(vstart < 0) vstart = 1.0;
    if(vending < 0) vending = 100.0;

    vector<double> valuesToTest = cutSpace(nbPoints, logarithmic, base, vstart, vending);

        identifiabilities[parameterIndex]->add(parameterIndex, valuesToTest[i], v, costPerVariable, costPerExperiment, currentModel->getParameters());

    //identifiabilities[parameterIndex]->sort();
    #else
    cerr << "manageSims::motherRecursiveIdentifibiality(...), this function is only allowed when ALLOW_OPTIMIZE is defined, in order to include the optimizations files during compilation.\n";
    #endif

}
*/















/*
void manageSims::orderSensitivityPoints(int parameterIndex){
    // stupid n² sort
    int nbPts = sensitivityXs[parameterIndex]->size();
    for(int i = 0; i < nbPts-1; ++i){
        for(int j = 0; j < nbPts - i; ++i){
            if((* sensitivityXs[parameterIndex])[i] > (* sensitivityXs[parameterIndex])[i+1]){
                double buf1 =           (* sensitivityXs[parameterIndex])[i];
                double buf2 =           (* sensitivityCosts[parameterIndex])[i];
                vector<double> buf3 =   (* sensitivityCostsPerVariable[parameterIndex])[i];
                (* sensitivityXs[parameterIndex])[i] =                  (* sensitivityXs[parameterIndex])[i+1];
                (* sensitivityCosts[parameterIndex])[i] =               (* sensitivityCosts[parameterIndex])[i+1];
                (* sensitivityCostsPerVariable[parameterIndex])[i] =    (* sensitivityCostsPerVariable[parameterIndex])[i+1];
                (* sensitivityXs[parameterIndex])[i+1] = buf1;
                (* sensitivityCosts[parameterIndex])[i+1] = buf2;
                (* sensitivityCostsPerVariable[parameterIndex])[i+1] = buf3;
            }
        }
    }
}

void manageSims::nextValues(int parameterIndex){
    // Look at the median dCost (for each variable)
    orderSensitivityPoints(parameterIndex);
    int currentNb = sensitivityXs[parameterIndex]->size();
    vector< vector<double> > slopesPerVariable;
    // for each interval that is more than the average
    for(int i = 0; i < currentNb-1; ++i){
        double slope = (* sensitivityCostsPerVariable[parameterIndex])[i];
    }
    vector<double>


}
*/



/* Analysis of a single experiment */
string manageSims::makeTextReportParamSet(string _folder, int configuration, double simDt, double displayDt){
    //if(currentMode == MULTI_EXPERIMENT){cerr << "WRN: Be careful that the report is done only for the current experiment (not for all of them) !!\n";}

    //if(currentConfig.size() == 0) { "ERR: manageSims::makeTextReportParamSet, you have to load a configuration first, before doing a parameter set report. Sorry !\n"; return string("");}
    if(_folder.size() == 0) {cerr << "ERR: manageSims::makeTextReportParamSet, empty folder name\n"; return string("");}
    if((configuration < 0) || (configuration >= nbCombs)) {cerr << "ERR: manageSims::makeTextReportParamSet, bad configuration index(" << configuration << " out of " << nbCombs << " possible configurations\n"; return string("");}
    if((simDt <= 1e-9) || (simDt >= 1e12)) {cerr << "ERR: manageSims::makeTextReportParamSet, too huge/small dt for simulation(" << simDt << ")\n"; return string("");}
    if(( displayDt <= 1e-6) || ( displayDt >= 1e12)) {cerr << "ERR: manageSims::makeTextReportParamSet, too huge/small dt for fsaving the kinetics(" << displayDt << ")\n"; return string("");}

    // - the folder has to be created before
    // - Recapitulates the list of simulated variables and optimized parameters with their names and boundary,
    // - for each simulated variable, table of values for simulations versus data (if there is data), and cost of this curve
    // ?? - makes a fitness report : for each data point, simulated value versus data


    currentModel->setPrintMode(true, displayDt);    // ui->doubleSpinBoxGraphDT->value();
    currentModel->dt = simDt;                       //  ui->doubleSpinBoxSimDT->value();

    stringstream f;
    f << "================== Parameter set report =================== \n\n" ;


    int NV = currentModel->getNbVars();
    int NP = currentModel->getNbParams();


    f << "Fitted parameter values, inside their boundaries : \n";

    //cerr << currentConfig.size() << "\t" << nbCombs << "\t" << configuration << endl;
    for(int i = 0; i < NP; ++i){
        if(!currentConfig[i][4+configuration].compare("1")) f << i << "\t" << currentModel->getParam(i) << "\t" << currentModel->getLowerBound(i) << "\t" << currentModel->getUpperBound(i) << "\t" << currentModel->getParamName(i) << "\n";
    }

    f << "Simulated variables : \n";
    for(int i = 0; i < NV; ++i){
        if(! currentModel->over(i)) f << i << "\t" << currentModel->getName(i) << "\n";
    }
    //cerr << "Step 1" << endl;
    //ofstream f1((_folder + string("")).c_str(), ios::out);
    //if(!f1) {cerr << "File not Found" << endl; return;}


    int nbExp = currentExperiment->nbCond();

    // simulate once for the kinetics, per experiment, and store everything in currentData
    for(int i = 0; i < nbExp; ++i){
        f << "Simulated kinetics for experiment " << currentExperiment->names_exp[i] << "\n";
        currentExperiment->simulate(i, NULL, true);
        if(currentData[i]) {delete currentData[i]; currentData[i] = NULL;}
        currentData[i] = new TableCourse(currentModel->getCinetique());
        if(currentModel->penalities > 0){
            f << "Note : simulation diverged for experiment (" << currentExperiment->expName(i) << ") with penalty " << currentExperiment->m->penalities << "\n";
        }
        f << currentModel->getCinetique().print();
    }
    //cerr << "Step 2" << endl;
    f << "Total cost of parameter set : " << currentExperiment->costPart() << endl;
    for(int j = 0; j < NV; ++j){
        if(! currentModel->over(j)){
            f << "Simulated kinetics for variable " << j << " : " << currentModel->getName(j) << "\n";
            if(currentExperiment->canOverride(currentModel->getGlobalID(j))) {f << "Cost : " << currentExperiment->costPart(currentModel->getGlobalID(j)) << "\n";}
            if(nbExp < 1) return string("No Experiment !");
            vector<double> Xs = currentData[0]->getTimePoints();
            vector< vector<double>* > packData;
            for(int i = 0; i < currentExperiment->nbCond(); ++i){
                packData.push_back(new vector<double>(currentData[i]->getTimeCourse(j)));
                //// if(packData[i]->size() != Xs.size()) cerr << "ERR: manageSims::makeTextReportParamSet, exp " << currentExperiment->expName(i) << ", kinetics from different variables have different number of time points\n";
            }
            int nbTp = Xs.size();


            for(int k = 0; k < nbTp; ++k){
                f << Xs[k];
                for(int i = 0; i < currentExperiment->nbCond(); ++i){
                    f << "\t" << (*packData[i])[k];
                }
                f << "\n";
            }
        }
    }
    //cerr << "Step 3" << endl;
    f << "Detailed costs \n";
    f << currentExperiment->costReport();




    return f.str();
}




void manageSims::addMacro(string text){
    currentMacro << text << "\n";
}
void manageSims::addFileTextToMacro(string text, string fileName){
    currentMacro << "/* ---------- Informations about a text file : -------- \n";
    if(fileName.size() > 0) currentMacro << fileName << "\n";
    currentMacro << "// ------------------ Content of file : --------------- \n";
    currentMacro << text;
    currentMacro << "// ---------------------------------------------------- */\n";
}
string manageSims::getMacro(){
    return currentMacro.str();
}
void manageSims::startMacro(string experimentName){
    recording = true;
    currentMacro.clear();
    currentMacro << "// ================= New automatically generated macro ============ \n";
    currentMacro << "manageSims* macro" << macroID << " = new manageSims(experimentName)\n";
}
void manageSims::addFileToMacro(string fileName){
    ifstream inFile;
    inFile.open(fileName);//open the input file
    stringstream strStream;
    strStream << inFile.rdbuf();
    addFileTextToMacro(strStream.str(), fileName);
}


















/// =============================== GRAPHICAL INTERFACE ===================================

























#ifndef WITHOUT_QT

#define TABLE currentTable
simuWin::simuWin(Experiment* _Exp, QWidget *parent): manageSims(_Exp),
    ui(new Ui::simuWin), paramSetID(0) // QWidget(parent),
{
    ui->setupUi(this);
    reset();
    #ifdef ALLOW_OPTIMIZE
    optChoice = new optSelect(ui->widgetOpt);
    #endif
}

simuWin::simuWin(MultiExperiments* _Exp, QWidget *parent): manageSims(_Exp),
    ui(new Ui::simuWin), paramSetID(0) // QWidget(parent),
{
    ui->setupUi(this);
    reset();
    #ifdef ALLOW_OPTIMIZE
    optChoice = new optSelect(ui->widgetOpt);
    #endif
}

// not : didn't test whether reset() can be called multiple times ...
void simuWin::reset(){
    if(!currentModel) {cerr << "simuWin::reset() was called from a non-existing model"; return;}
    if((currentMode == MONO_EXPERIMENT) && (!currentExperiment)) {cerr << "simuWin()::reset() was called from a non-existing experiment (MONO_exp mode)\n"; return;}
    if((currentMode == MULTI_EXPERIMENT) && (!listExperiments)) {cerr << "simuWin()::reset() was called from a non-existing group of experiment (MULTI_exp mode)\n"; return;}

    nbCombs = 0;                    // empty configuration to start ...
    removeMode = false;             // the buttons of the configuration panel are in 'Add' or 'Del' mode
    stopOpt = false;                // signal to stop an optimization at any time
    lastParamID = -1;               // ID of the last parameter that was modified from the table --> will have its own spin bow under the main graph
    nbCostCalls = 0;
    ui->groupBoxFuture->hide();     // buttons not yet implemente
    ui->labelModelName->setText(currentModel->name.c_str());
    ui->checkBoxAll->setChecked(true);

    // Creates the main graph and plots an example graphe to check that the plotting is working. This example should be erased just afterwards.
#ifdef USE_QWT
    currentGraphe = new Graphe2(ui->widgetGraphe);
    costGraphe = new Graphe2(ui->widgetCostEvolution);
#endif
#ifdef USE_QCP
    currentGraphe = new grapheCustom(ui->widgetGraphe);
    costGraphe = new grapheCustom(ui->widgetCostEvolution);
#endif
    currentGraphe->Example();
    costGraphe->Example();
    costGraphe->setNbCurves(4);
    costGraphe->logarithmic(true);

#ifdef USE_BOOST_SOLVER
    ui->labelSolver->setText(QString("Solver: Adap-RK4"));
#else
    ui->labelSolver->setText(QString("Solver: Adap-Euler"));
#endif


#ifdef LOG_COST
    ui->labelCostFn->setText(QString("Cost: LOG"));
#endif
#ifdef SQUARE_COST
    ui->labelCostFn->setText(QString("Cost: SumSquares"));
#endif


    ui->doubleSpinBoxCostMultiExp->setDisabled(true);
    ui->doubleSpinBoxCostMultiExp->setMaximum(1e8);
    ui->doubleSpinBoxCostMultiExp->setDecimals(5);
    ui->doubleSpinBoxCostExp->setDisabled(true);
    ui->doubleSpinBoxCostExp->setMaximum(1e8);
    ui->doubleSpinBoxCostExp->setDecimals(5);
    ui->doubleSpinBoxCostVar->setDisabled(true);
    ui->doubleSpinBoxCostVar->setMaximum(1e8);
    ui->doubleSpinBoxCostVar->setDecimals(5);
    ui->doubleSpinBoxLastParam->setDecimals(7);
    ui->lcdNumberCostCall->setDigitCount(10);
    ui->spinBoxDisplay->setValue(100);
    ui->spinBoxDisplay->setMinimum(10);
    ui->spinBoxDisplay->setMaximum(10000);
    ui->spinBoxStore->setValue(10000);
    ui->spinBoxStore->setMinimum(10);
    ui->spinBoxStore->setMaximum(1e9);
    ui->doubleSpinBoxSimDT->setMinimum(0.000001);
    ui->doubleSpinBoxSimDT->setMaximum(1000);
    ui->doubleSpinBoxGraphDT->setMinimum(0.01);
    ui->doubleSpinBoxGraphDT->setMaximum(100000);

    ui->doubleSpinBoxLastParam->setMinimum(1e-8);
    ui->doubleSpinBoxLastParam->setMaximum(1e8);

/*    // Values for Leishmania
    ui->doubleSpinBoxSimDT->setValue(0.0010);
    ui->doubleSpinBoxGraphDT->setValue(0.5);
    // Values for THDiff
    ui->doubleSpinBoxSimDT->setValue(10);
    ui->doubleSpinBoxGraphDT->setValue(5000); */

    ui->doubleSpinBoxSimDT->setValue(currentModel->dt);
    ui->doubleSpinBoxGraphDT->setValue(currentModel->dt * 50);

    ui->checkBoxDisplayCurves->setChecked(true);
    ui->tabWidgetRight->setTabText(0, QString("Opt Method"));
    ui->tabWidgetRight->setTabText(1, QString("Cost in time"));
    ui->tabWidgetRight->setTabText(2, QString("Macro"));
    ui->checkBoxLinkToCombs->setChecked(false);

    ui->doubleSpinBoxCostExp->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->doubleSpinBoxCostVar->setButtonSymbols(QAbstractSpinBox::NoButtons);



    // Table for the configuration
    currentModel->setBaseParameters();
    currentModel->initialise();
    int Start = currentModel->getNbParams();
    int NV = currentModel->getNbVars();

    // In multi-experiment mode, fills a combo to chose which experiment. By default, the first one will be taken as currentExperiment (because it is the default selected one of the combo)
    ui->comboBoxMultiExperiment->clear();
    if(currentMode == MULTI_EXPERIMENT){
        // Fills combo
        int NE = listExperiments->nbBigExp();
        for(int i = 0; i < NE; ++i){
            ui->comboBoxMultiExperiment->addItem(QString(listExperiments->getExperiment(i)->Identification.c_str()));
        }

        // Creates the table to compare the cost of each variable in each experiment
        tableMultiExpCosts = new QStandardItemModel(NE, NV+2, ui->tableViewMultiExp);
        for(int i = 0; i < NE; ++i){
            tableMultiExpCosts->setVerticalHeaderItem(i, new QStandardItem(QString(listExperiments->getExperiment(i)->Identification.c_str())));
        }
        tableMultiExpCosts->setHorizontalHeaderItem(0, new QStandardItem(QString("Coefficient")));
        tableMultiExpCosts->setHorizontalHeaderItem(1, new QStandardItem(QString("Total")));
        for(int j = 0; j < NV; ++j){
            tableMultiExpCosts->setHorizontalHeaderItem(j+2, new QStandardItem(QString(currentModel->getName(j).c_str())));
        }
        for(int i = 0; i < NE; ++i){    // the QStandardItems need to be created even if they are empty ...
            tableMultiExpCosts->setItem(i,0, new QStandardItem(QString::number((double) listExperiments->getCoefficient(i))));
            for(int j = 1; j < NV + 2; ++j){
                tableMultiExpCosts->setItem(i,j, new QStandardItem(QString("")));
                //tableHistory->item(j,i)->setBackground(QBrush(QColor(Qt::lightGray)));
            }
        }
    } else {
        // No combo
        ui->labelMultiExp->hide();
        ui->comboBoxMultiExperiment->hide();
        ui->doubleSpinBoxCostMultiExp->hide();
        ui->checkBoxCostMultiExp->hide();

        // the table will just show the cost of each variable for the current experiment.
        tableMultiExpCosts = new QStandardItemModel(1, NV+2, ui->tableViewMultiExp);
        tableMultiExpCosts->setHorizontalHeaderItem(0, new QStandardItem(QString("Coefficient")));
        tableMultiExpCosts->setHorizontalHeaderItem(1, new QStandardItem(QString("Total")));
        for(int j = 0; j < NV; ++j){
            tableMultiExpCosts->setHorizontalHeaderItem(j+2, new QStandardItem(QString(currentModel->getName(j).c_str())));
        }
        tableMultiExpCosts->setItem(0,0, new QStandardItem(QString("1")));          // coefficient
        tableMultiExpCosts->item(0,0)->setBackground(QBrush(QColor(Qt::lightGray)));
        for(int j = 1; j < NV + 2; ++j){
            tableMultiExpCosts->setItem(0,j, new QStandardItem(QString("")));
        }
    }
    ui->tableViewMultiExp->resizeColumnsToContents();
    ui->tableViewMultiExp->setModel(tableMultiExpCosts);

    //QObject::connect(ui->tableViewMultiExp, SIGNAL(entered(QModelIndex)), this, SLOT(coefficientsChanged(QModelIndex)));
    QObject::connect(tableMultiExpCosts, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(coefficientsChanged(QModelIndex, QModelIndex)));

    TABLE = new QTableWidget(ui->widgetForTable);           // note : TABLE is 'currentTable'
    TABLE = new QTableWidget(currentModel->getNbParams() + currentModel->getNbVars()+1, NBCOLS, ui->widgetForTable);
    TABLE->setMinimumHeight(660);
    TABLE->setMinimumWidth(590);
    TABLE->setColumnWidth(0, 100);
    TABLE->setColumnWidth(1, 70);
    TABLE->setColumnWidth(2, 70);
    TABLE->setColumnWidth(3, 35);
    for(int i = 4; i < NBCOLS; ++i){
        TABLE->setColumnWidth(i,18);
    }
    // ui->textEditParamSet->clear();
    // ui->tableWidget->resizeColumnsToContents();
    for(int i = 0; i < Start; ++i){
        TABLE->setVerticalHeaderItem(i, new QTableWidgetItem(QString(currentModel->getParamName(i).c_str())));
        TABLE->setItem(i,0, new QTableWidgetItem(true));
        TABLE->setItem(i,1, new QTableWidgetItem(true));
        TABLE->setItem(i,2, new QTableWidgetItem(true)); // need to give item to every cell, if not, segfault if clicked ...
        TABLE->setItem(i,3, new QTableWidgetItem(true));
        QCheckBox* qcb = new QCheckBox();
        qcb->setChecked(true);
        TABLE->setCellWidget(i,3,qcb);
    }
    for(int i = 0; i < NV+1; ++i){
        for(int j = 0; j < 3; ++j){
            TABLE->setItem(Start+i,j, new QTableWidgetItem(QString("")));
            TABLE->item(Start+i,j)->setBackground(QBrush(QColor(Qt::lightGray)));
            TABLE->item(Start+i,j)->setFlags(Qt::ItemIsEnabled);
        }
    }
    for(int i = 0; i < NV; ++i){
        ///HEREHERE
        TABLE->setItem(Start + i,3, new QTableWidgetItem(true));
        TABLE->setVerticalHeaderItem(Start + i, new QTableWidgetItem(QString(currentModel->getName(i).c_str())));
        QCheckBox* qcb = new QCheckBox();
        qcb->setProperty("VarIndex", i);        // a trick to know which check box is sending signals : each check box gets an additional ID
        TABLE->setCellWidget(Start + i,3,qcb);
        // => the state of the check boxes will be updated in 'currentExperimentChanged'
        QObject::connect(qcb, SIGNAL(stateChanged(int)), this, SLOT(changeOverride(int)));
    }

    // A cell without widget, once clicked, returns seg fault
    for(int i = 0; i < NV + Start + 1; ++i){
        for(int j = 4; j < NBCOLS; ++j){
            TABLE->setItem(i,j, new QTableWidgetItem(QString(".")));
            TABLE->item(i,j)->setBackground(QBrush(QColor(Qt::lightGray)));
            TABLE->item(i,j)->setFlags(Qt::ItemIsEnabled);
        }
    }

    for(int i = 4; i < NBCOLS; ++i){
        QPushButton* btn = new QPushButton(QString("Use"));
        btn->setProperty("IndComb", i-4);
        QObject::connect(btn,SIGNAL(released()), this, SLOT(useComb()));
        if(i-3 > nbCombs) btn->hide();  //doesn't work, snif
        TABLE->setCellWidget(NV+Start,i,btn);
    }
    QPushButton* cre = new QPushButton(QString("Create"));
    QObject::connect(cre, SIGNAL(released()), this, SLOT(createComb()));
    TABLE->setCellWidget(NV+Start,3,cre);
    //cre->hide();
    QPushButton* rem = new QPushButton(QString("Del. Mode"));
    QObject::connect(rem, SIGNAL(released()), this, SLOT(removeCombs()));
    TABLE->setCellWidget(NV+Start,2,rem);
    //cre->hide();


    TABLE->setHorizontalHeaderItem(1, new QTableWidgetItem( QString("Low. Bound")));
    TABLE->setHorizontalHeaderItem(2, new QTableWidgetItem( QString("Up. Bound")));
    TABLE->setHorizontalHeaderItem(0, new QTableWidgetItem( QString("Param.")));
    TABLE->setHorizontalHeaderItem(3, new QTableWidgetItem( QString("Opt?")));
    for(int i = 4; i < NBCOLS; ++i){
        TABLE->setHorizontalHeaderItem(i, new QTableWidgetItem( QString::number(i-4)));
    }

    QObject::connect(TABLE, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));
    QObject::connect(ui->tableViewHistory, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(paramSetDoubleClickedFromHistory(const QModelIndex &)));

    int show = ui->spinBoxDisplay->value();
    tableHistory = new QStandardItemModel(ui->spinBoxDisplay->value(), Start + 1, ui->tableViewHistory);
    for(int i = 0; i < Start+1; ++i){
        for(int j = 0; j < show; ++j){
            tableHistory->setItem(j,i, new QStandardItem(QString("")));
            //tableHistory->item(j,i)->setBackground(QBrush(QColor(Qt::lightGray)));
        }
    }


    QObject::connect(ui->pushButtonResetParams, SIGNAL(released()), this, SLOT(resetParams()));
    QObject::connect(ui->pushButtonGo, SIGNAL(released()), this, SLOT(simulate()));
    QObject::connect(ui->pushButtonSensitivity, SIGNAL(released()), this, SLOT(sensitivity()));
    QObject::connect(ui->pushButtonIdentifiability, SIGNAL(released()), this, SLOT(identifiability()));
    QObject::connect(ui->pushButtonLoadOpt, SIGNAL(released()), this, SLOT(loadOptim()));
    QObject::connect(ui->pushButtonLoadSet, SIGNAL(released()), this, SLOT(loadSet()));
    QObject::connect(ui->pushButtonLoadConfig, SIGNAL(released()), this, SLOT(loadConfig()));
    QObject::connect(ui->pushButtonSaveConfig, SIGNAL(released()), this, SLOT(saveConfig()));
    QObject::connect(ui->pushButtonLoadSet, SIGNAL(released()), this, SLOT(loadSet()));
    QObject::connect(ui->pushButtonStop, SIGNAL(released()), this, SLOT(stopOptimization()));
    QObject::connect(ui->pushButtonExpandPlot, SIGNAL(released()), this, SLOT(expandPlot()));
    QObject::connect(ui->comboBoxVariable, SIGNAL(activated(int)), this, SLOT(varChanged(int)));
    QObject::connect(ui->pushButtonOptimize, SIGNAL(released()), this, SLOT(optimize()));
    QObject::connect(ui->comboBoxSubExperiment, SIGNAL(activated(int)), this, SLOT(simulate()));
    QObject::connect(ui->checkBoxAll, SIGNAL(stateChanged(int)), this, SLOT(simulate())); /// mistake! why ???
    QObject::connect(ui->checkBoxCostMultiExp, SIGNAL(stateChanged(int)), this, SLOT(simulate()));
    QObject::connect(ui->doubleSpinBoxLastParam, SIGNAL(valueChanged(double)), this, SLOT(paramChangedFromBox(double)));
    QObject::connect(ui->pushButtonRefresh, SIGNAL(released()), this, SLOT(refreshHistory()));
    QObject::connect(ui->pushButtonRefreshMacro, SIGNAL(released()), this, SLOT(refreshMacro()));
    QObject::connect(ui->pushButtonResetMacro, SIGNAL(released()), this, SLOT(resetMacro()));
    QObject::connect(ui->checkBoxRecord, SIGNAL(stateChanged(int)), this, SLOT(changeRecord(int)));
    QObject::connect(ui->pushButtonSaveHistory, SIGNAL(released()), this, SLOT(saveHistory()));
    QObject::connect(ui->pushButtonLoadHistory, SIGNAL(released()), this, SLOT(loadHistory()));
    QObject::connect(ui->pushButtonCost, SIGNAL(released()), this, SLOT(costReport()));
    QObject::connect(ui->pushButtonReport, SIGNAL(released()), this, SLOT(makeFigReportParamSet()));
    QObject::connect(ui->checkBoxOnlyComb, SIGNAL(stateChanged(int)), this, SLOT(showOnlyCombChecked(int)));
    QObject::connect(ui->comboBoxMultiExperiment, SIGNAL(activated(int)), this, SLOT(currentExperimentChanged()));


    updateParmsFromModel();         // to fill the values from the model.
    currentExperimentChanged();     // to finish the set-up according to the current configuration

    /*simulate();  // why do I have to call it ???
    ui->checkBoxAll->setChecked(true);      // now, can request to simulate all !
    simulate();  // why do I have to call it ???*/
}



void simuWin::coefficientsChanged(QModelIndex left, QModelIndex right){
    int row = left.row();
    int column = left.column();
    if((right.row() != row) || (right.column() != column)) cerr << "ERR: simuWin::coefficientsChanged, you are not supposed to change more than one cell at a time" << endl;
    if(currentMode == MONO_EXPERIMENT) return;
    if((column == 0) && (row >= 0) && (row < listExperiments->nbBigExp())) {
        double paramValue = this->tableMultiExpCosts->item(row, column)->text().toDouble();
        listExperiments->setCoefficient(row, paramValue);
        cout << "      changing coefficient to " << paramValue << " for experiment: " << listExperiments->getExperiment(row)->Identification << endl;
    }
    simulate();         // does currentExperiment->init() inside
}






void simuWin::currentExperimentChanged(){

    QObject::disconnect(TABLE, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));

    // selects the current Experiment
    if(currentMode == MULTI_EXPERIMENT){
        int selected = ui->comboBoxMultiExperiment->currentIndex();
        currentExperiment = listExperiments->getExperiment(selected);
    }
    if(!currentExperiment) {cerr << "ERR::simuWin::currentExperimentChanged, the new experiment is NULL"; return;}
    int NC = currentExperiment->nbCond();

    for(int i = 0; i < currentData.size(); ++i){
        delete currentData[i];
        currentData[i] = NULL;
    }
    currentData.clear();
    // Philippe 2018:  be careful, because everything is continued to be plotted, so will look for currentData etc ...
    currentData.resize(NC, NULL);

    // Update the combo-box to chose the different conditions of the current experiment
    QObject::disconnect(ui->comboBoxSubExperiment, SIGNAL(activated(int)), this, SLOT(simulate()));
    listSubExps = new QListWidget(ui->comboBoxSubExperiment);    // the combo box for the experiments is made using ListWidgets (instead of usual combo), because they can be enabled / disabled
    listSubExps->hide();
    ui->comboBoxSubExperiment->setModel(listSubExps->model());
    cout << " -> New exp. selected : " << ui->comboBoxMultiExperiment->currentText().toStdString() << endl;
    cout << "       Nb Exps : " << NC << endl;
    ui->comboBoxSubExperiment->clear();
    for(int i = 0; i < NC; ++i){
        listSubExps->addItem(currentExperiment->expName(i).c_str());
        if(currentExperiment->isDoable(i)){
            cout << "       - " << currentExperiment->expName(i).c_str() << endl;
        } else {
            QListWidgetItem *item = listSubExps->item(i);
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            cout << "       X " << currentExperiment->expName(i).c_str() << endl;
        }
    }

    // Update the check boxes. They just represent the state of the table override[] of the currentExperiment.
    int NP = currentModel->getNbParams();
    int NV = currentModel->getNbVars();
    for(int i = 0; i < NV; ++i){
        QCheckBox* qcb = (QCheckBox*) TABLE->cellWidget(i+NP,3);
        QObject::disconnect(qcb, SIGNAL(stateChanged(int)), this, SLOT(changeOverride(int)));
        if(currentExperiment->canOverride(currentModel->getGlobalID(i))) {
            int nbExpsOvers = 0;
            for(int j = 0; j < currentExperiment->nbCond();++j){
                nbExpsOvers += currentExperiment->Overs[j]->override[currentModel->getGlobalID(i)];
            }
            qcb->setEnabled(true);
            qcb->setChecked(nbExpsOvers == currentExperiment->nbCond());
            if((nbExpsOvers > 0) && (nbExpsOvers < currentExperiment->nbCond())){
                cerr << "ERR: Simuwin::currentExperimentChanged() for exp name " << currentExperiment->Identification << ", the variable " << currentModel->getName(i) << " is overriden for some conditions but not all. This is not allowed." << endl;
                TABLE->item(i+NP,2)->setText(QString("Partially(") + QString::number(nbExpsOvers) + QString("/") + QString(currentExperiment->nbCond()) + QString(")"));
            }
        } else {
            TABLE->item(i+NP,2)->setText(QString("No Data ->"));
            qcb->setEnabled(false);
            qcb->setChecked(false);
        }
        QObject::connect(qcb, SIGNAL(stateChanged(int)), this, SLOT(changeOverride(int)));
    }
    QObject::connect(ui->comboBoxSubExperiment, SIGNAL(activated(int)), this, SLOT(simulate()));

    simulate();
    QObject::connect(TABLE, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));
}





void simuWin::updateParmsFromModel(){
    updateConfigParamsFromModel();          // inherited from manageSims
    QObject::disconnect(TABLE, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));
    if(currentModel){
        int NP = currentModel->getNbParams();
        int NV = currentModel->getNbVars();
        for(int i = 0; i < NP; ++i){
            //ui->textEditParamSet->append(QString::number(currentModel->getParam(i)) + QString("\t"));
            TABLE->item(i,0)->setText(QString::number(currentModel->getParam(i)));
            TABLE->item(i,1)->setText(QString::number(currentModel->getLowerBound(i)));
            TABLE->item(i,2)->setText(QString::number(currentModel->getUpperBound(i)));
        }
        ui->comboBoxVariable->clear();
        for(int i = 0; i < NV; ++i){
            ui->comboBoxVariable->addItem(QString(currentModel->getName(i).c_str()));
        }
    }
    QObject::connect(TABLE, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));
}


void simuWin::paramChangedFromBox(double newValue){
    cout << "Parameter changed from Box, new value " << newValue << endl;
    //QObject::disconnect(ui->doubleSpinBoxLastParam, SIGNAL(valueChanged(double)), this, SLOT(paramChangedFromBox(double)));
    if(lastParamID > -1){
        currentModel->setParam(lastParamID, newValue);
        int varSel = ui->comboBoxVariable->currentIndex();
        updateParmsFromModel();
        ui->comboBoxVariable->setCurrentIndex(varSel);
        simulate();
    }
    //QObject::connect(ui->doubleSpinBoxLastParam, SIGNAL(valueChanged(double)), this, SLOT(paramChangedFromBox(double)));
}




void simuWin::cellClicked(int row, int column){
    cerr << "cell clicked, ligne=" << row << ", col=" << column << ", newValue = " << TABLE->item(row, column)->text().toStdString() << endl;}
void simuWin::cellPressed(int row, int column){
    cerr << "cell clicked, ligne=" << row << ", col=" << column << ", newValue = " << TABLE->item(row, column)->text().toStdString() << endl;}

void simuWin::createComb(){
    QObject::disconnect(TABLE, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));
    if(nbCombs + 4 >= NBCOLS) return;
    nbCombs++;
    for(int i = 0; i < (int) currentConfig.size(); ++i){
        currentConfig[i].resize(nbCombs+4);
    }
    int NP = currentModel->getNbParams();
    int NV = currentModel->getNbVars();
    for(int i = 0; i < NP + NV; ++i){
        QWidget* ww = TABLE->cellWidget(i,3);
        if(ww){
            //cout << i << " is " << ww->metaObject()->className() << endl;
            if(((QCheckBox*) TABLE->cellWidget(i,3))->isChecked()){
                TABLE->item(i,3+nbCombs)->setText(QString("1"));
                currentConfig[i][nbCombs+3] = "1";
                TABLE->item(i,3+nbCombs)->setBackground(QBrush(QColor(Qt::green)));}
            else {
                TABLE->item(i,3+nbCombs)->setText(QString("0"));
                currentConfig[i][nbCombs+3] = "0";
                TABLE->item(i,3+nbCombs)->setBackground(QBrush(QColor(Qt::white)));
            }
            TABLE->item(i,3+nbCombs)->setFlags(TABLE->item(i,3+nbCombs)->flags() | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable );

        }
    }
    //cout << "Added" << endl;
    QObject::connect(TABLE, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));

}

void simuWin::removeCombs(){
    removeMode = !removeMode;
    int MaxRows = currentModel->getNbParams() + currentModel->getNbVars() + 1;
    for(int i = 4; i < NBCOLS; ++i){
        QPushButton* ww =  (QPushButton*) TABLE->cellWidget(MaxRows-1,i);
        if(ww){
            if(removeMode) ww->setText(QString("DEL"));
            else ww->setText(QString("Use"));
        }
    }
}

void simuWin::useComb(int IDComb){
    int z = (IDComb < 0) ? sender()->property("IndComb").toInt() : IDComb;
    if(recording) currentMacro << "macro" << macroID << ".motherUseComb(" << z << ")\n";
    if(!removeMode){

        if(z >= nbCombs) return;
        cout << "      Note : Use comb nr(0..) " << z <<  endl;
        int NP = currentModel->getNbParams();
        int NV = currentModel->getNbVars();
        for(int i = 0; i < NP + NV; ++i){
            int check = TABLE->item(i, 4+z)->text().toInt();
            //cout << check << endl;
            if(((QCheckBox*) TABLE->cellWidget(i,3))) ((QCheckBox*) TABLE->cellWidget(i,3))->setChecked((check > 0));
            //if((i >= NP)){
            //    currentExperiment->overrideVariable(currentModel->getGlobalID(i-NP), check && currentExperiment->canOverride(currentModel->getGlobalID(i-NP)));
            //}
        }
    }
    else {
        if(IDComb >= 0) cerr << "ERR : simuWin::useComb(int IDComb = " << IDComb << ") is not supposed to be used for deleting" << endl;
        if(z >= nbCombs) return;    // and ensures that nbCombs will not get negative at the end
        cerr << "Delete comb nr(0..) " << z <<  endl;
        int res = QMessageBox::question(this, QString("Confirm?"), QString("Confirm the deletion ?"), QMessageBox::Yes | QMessageBox::No);
        if(res == QMessageBox::No) return;
        QObject::disconnect(TABLE, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));
        int NP = currentModel->getNbParams();
        int NV = currentModel->getNbVars();
        // Move all the configs afterwards of one index on the left
        for(int k = 4 + z +1; k < 4 + nbCombs; ++k){
            for(int i = 0; i < NP + NV; ++i){
                TABLE->item(i, k-1)->setText(TABLE->item(i,k)->text());
                TABLE->item(i, k-1)->setBackgroundColor(TABLE->item(i,k)->backgroundColor());
            }
        }
        for(int i = 0; i < NP + NV; ++i){
            TABLE->item(i, 4 + nbCombs - 1)->setText(".");
            TABLE->item(i, 4 + nbCombs - 1)->setBackgroundColor(QColor(Qt::gray));
        }
        nbCombs--;
        QObject::connect(TABLE, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));
    }
}



void simuWin::changeOverride(int newState){
    //if(currentMode == MULTI_EXPERIMENT) {cerr << "ERR : change Override called in the Multi-obj mode, return.\n"; return;}
    int z = sender()->property("VarIndex").toInt();
    if(recording) currentMacro << "macro" << macroID << ".currentModel->applyOverride(macro" << macroID << ".currentModel->getGlobalID(" << z << "), " << (newState > 0 ? "true" : "false") << ")\n";
    cout << "Change override of var " << z << " to " << newState << endl;
    if(currentMode == MONO_EXPERIMENT){
        if(currentExperiment->canOverride(currentModel->getGlobalID(z))){
            currentExperiment->overrideVariable(currentModel->getGlobalID(z), (newState > 0));
        }
    } else {
        //////§§§§ Philippe HEREHERE
        listExperiments->overrideVariable(currentModel->getGlobalID(z), (newState > 0));
    }
    simulate();
}

void simuWin::simulate(){

    int NV = currentModel->getNbVars();


    ui->doubleSpinBoxCostExp->setEnabled(ui->checkBoxAll->isChecked());
    ui->doubleSpinBoxCostVar->setEnabled(ui->checkBoxAll->isChecked());
    ui->doubleSpinBoxCostExp->setValue(0.0);
    ui->doubleSpinBoxCostVar->setValue(0.0);

    currentExperiment->init();      // to clear the evaluators, for the cost (to not keep values from the previous simulation ??)
    // keep track of the variable selectzed to display
    int varSel = ui->comboBoxVariable->currentIndex();
    // note that currentModel and currentExperiment->m are exactly the same thing (same pointer)
    ui->textBrowserStatus->clear();
    if(currentExperiment){
        // says the time step for recording a kinetics
        currentModel->setPrintMode(true, ui->doubleSpinBoxGraphDT->value());
        // gives the starting dt (it will be tuned by the solver)
        currentModel->dt = ui->doubleSpinBoxSimDT->value();
        // model initialization is done inside the function experiment->simulationsPart/simulate, no need to do it here
        int nbExp = currentExperiment->nbCond();
        if(ui->checkBoxAll->isChecked()){
            // disables the selection of experiments
            if(ui->checkBoxDisplayCurves->isChecked()){
                ui->comboBoxSubExperiment->setDisabled(true);
                ui->progressBar->show();
                ui->progressBar->setValue(0.0);
            }
            for(int i = 0; i < nbExp; ++i){
                // SIMULATE !!
                currentExperiment->simulate(i, NULL, true);
                if(ui->checkBoxDisplayCurves->isChecked()){
                    if( currentData[i]) {delete currentData[i]; currentData[i] = NULL;}
                    if(i > currentData.size()) cerr << "Something happened to currentData" << endl;
                    currentData[i] = new TableCourse(currentExperiment->m->getCinetique());
                    ui->progressBar->setValue((100 * (i+1)) / nbExp);
                    if(currentExperiment->m->penalities > 0){
                        ui->textBrowserStatus->append(QString("ERR : simulation diverged for ") + QString(currentExperiment->expName(i).c_str()) + QString(", penality : ") + QString::number(currentExperiment->m->penalities));
                    }
                }
            }
            if(ui->checkBoxDisplayCurves->isChecked()){
                ui->progressBar->hide();
            }
        } else {
            ui->comboBoxSubExperiment->setEnabled(true);
            ui->progressBar->hide();
            currentExperiment->simulate(ui->comboBoxSubExperiment->currentIndex(), NULL, true); //dangereux le 'currentIndex' !!
             if(currentData[ui->comboBoxSubExperiment->currentIndex()]) delete currentData[ui->comboBoxSubExperiment->currentIndex()];
            currentData[ui->comboBoxSubExperiment->currentIndex()] = new TableCourse(currentExperiment->m->getCinetique());
            if(currentModel->penalities > 0){
                ui->textBrowserStatus->append(QString("ERR : simulation diverged for ") + ui->comboBoxSubExperiment->currentText() + QString(", penality : ") + QString::number(currentExperiment->m->penalities));
            }
        }
    }

    // updates the plots
    varChanged(varSel);

    QObject::disconnect(tableMultiExpCosts, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(coefficientsChanged(QModelIndex, QModelIndex)));
    // Note : the cost has to be takken AFTER the simulation !!!
    if((currentMode == MULTI_EXPERIMENT) && (ui->checkBoxCostMultiExp->isChecked())) {
        manageSims::simulate();
        double v1 = NAN, v2 = NAN;
        v1 = listExperiments->costPart();
        v2 = listExperiments->getPenalities();
        double costToSimAll = v1 + v2;
        ui->doubleSpinBoxCostMultiExp->setEnabled(true);
        ui->doubleSpinBoxCostMultiExp->setValue(costToSimAll);

        int NE = listExperiments->nbBigExp();
        for(int i = 0; i < NE; ++i){
            for(int j = 0; j < NV; ++j){
                tableMultiExpCosts->item(i,j+2)->setText(QString::number(listExperiments->getExperiment(i)->costPart(currentModel->getGlobalID(j))));
            }
            tableMultiExpCosts->item(i,1)->setText(QString::number(listExperiments->getExperiment(i)->costExp())); // note: this is different than putting listExperiment(i)->costBigExp, which is multiplied by coefficient
            tableMultiExpCosts->item(i,0)->setText(QString::number(listExperiments->getCoefficient(i)));
        }


    } else {
        ui->doubleSpinBoxCostMultiExp->setEnabled(false);
        ui->doubleSpinBoxCostMultiExp->setValue(0.0);

        for(int j = 0; j < NV; ++j){
            tableMultiExpCosts->item(0,j+2)->setText(QString::number((double) currentExperiment->costPart(currentModel->getGlobalID(j))));
        }
        tableMultiExpCosts->item(0,1)->setText(QString::number(currentExperiment->costExp()));
        tableMultiExpCosts->item(0,0)->setText(QString("1"));

        if(currentMode == MULTI_EXPERIMENT){
            // erase the rest
            int NE = listExperiments->nbBigExp();
            for(int i = 1; i < NE; ++i){ // i = row
                for(int j = 0; j < NV; ++j){
                    tableMultiExpCosts->item(i,j+2)->setText(QString(""));
                }
                tableMultiExpCosts->item(i,1)->setText(QString(""));
                tableMultiExpCosts->item(i,0)->setText(QString(""));
            }
        }

    }
    QObject::connect(tableMultiExpCosts, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(coefficientsChanged(QModelIndex, QModelIndex)));


    /*if(currentExperiment && (currentExperiment->m->getCinetique())){
        QString title = QString("Modele: ") + ui->comboBoxModele->currentText() + QString("Modele: ") + ui->comboBoxDataSet->currentText() + QString("Modele: ") + ui->comboBoxExperiment->currentText();
        currentExperiment->m->getCinetique()->save(string("C:/Users/parobert/Desktop/Optimisation/ThModeles/Modeles/SimulOutput.txt"), title.toStdString());
    }*/
    if(ui->checkBoxDisplayCurves->isChecked()){
        updateParmsFromModel(); // to update the parameters that depend on other ones upon currentModel->init()
    }
    ui->comboBoxVariable->setCurrentIndex(varSel);

}

void simuWin::varChanged(int j){
    //cerr << "var changed " << endl;
    if(currentExperiment){
        if(ui->checkBoxAll->isChecked()){
            if(ui->checkBoxDisplayCurves->isChecked()){
                ////§§§ to check ui->textBrowserStatus->clear();
                currentGraphe->setNbCurves(2*currentExperiment->nbCond());
                currentGraphe->setTitle(ui->comboBoxVariable->currentText());
                for(int i = 0; i < currentExperiment->nbCond(); ++i){
                    vector<double> Xs = currentData[i]->getTimePoints();
                    vector<double> Ys = currentData[i]->getTimeCourse(j);
                    // needs to find the index of the variable
                    //cout << "looks for ID " << currentModel->getGlobalID(j) << "\tglobalName=" << currentExperiment->NamesVariablesInTheirIndex[currentModel->getGlobalID(j)] << "\t";
                    int nbTp = Xs.size();
                    for(int l = 0; l < nbTp; ++l){
                        Xs[l] = Xs[l] / 3600.0;
                    }
                    currentGraphe->Plot(2*i, Ys, Xs, QString(currentExperiment->expName(i).c_str()), currentGraphe->baseList(i));
                    //cerr << currentGraphe->nbCurves << " Curves and nbexp" << currentExperiment->nbCond() << endl;
                    if(currentExperiment->ExpData[i]){
                        int k = getPosInKinetics(currentExperiment->NamesVariablesInTheirIndex[currentModel->getGlobalID(j)], currentExperiment->ExpData[i]);
                        vector<double> Ys2 = (k >= 0) ? currentExperiment->ExpData[i]->getTimeCourse(k) : vector<double>();
                        vector<double> Xs2 = currentExperiment->ExpData[i]->getTimePoints();
                        if(k < 0) Xs2.clear();
                        /// maybe should return an error if k < 0, but it naturally happens when there is no data for this variable
                        //ui->textBrowserStatus->append();
                        if(((int) Xs2.size() != (int) Ys2.size())) cerr << "Problems reading exp kinetics, size Xs" << Xs2.size() << ", Ys " << Ys2.size() << "\n";
                            //cout << "Data for Exp: " << currentExperiment->expName(i) << ", variable " << currentExperiment->NamesVariablesInTheirIndex[currentModel->getGlobalID(j)] << endl;
                            //for(int l = 0; l < (int) Xs2.size();++l){ cout << "x=" << Xs2[l] << ", y=" << Ys2[l] << endl;}
                        int nbTp2 = Xs2.size();
                        for(int l = 0; l < nbTp2; ++l){
                            Xs2[l] = Xs2[l] / 3600.0;
                        }
                        //cout << Xs2.size() << " experimental points added to the curve" << endl;
                        currentGraphe->Plot(2*i+1, Ys2, Xs2, QString("") /*QString("Data ") + QString(currentExperiment->expName(i).c_str()) */, currentGraphe->baseList(i), Qt::DashDotDotLine);
                    } else {
                        currentGraphe->Plot(2*i+1, vector<double>(), vector<double>(), QString(""));
                    }
//                    cout << "Finished" << endl;
                }
            }
            //cout << endl;
            double costExp = currentExperiment->costPart();
            ui->doubleSpinBoxCostExp->setValue(costExp);
            // only display variable cost if there is data for it
            double costVar = /*currentExperiment->canOverride(currentModel->getGlobalID(j)) ? */ currentExperiment->costPart(currentModel->getGlobalID(j)); // : 0;
            ui->doubleSpinBoxCostVar->setValue(costVar);
            ui->labelCostVar->setText(ui->comboBoxVariable->currentText());
        } else {
            if(ui->checkBoxDisplayCurves->isChecked()){
                currentGraphe->setNbCurves(1);
                currentGraphe->Plot(0, currentExperiment->m->getCinetique().getTimeCourse(j), currentExperiment->m->getCinetique().getTimePoints(), QString(""), currentGraphe->baseList(ui->comboBoxSubExperiment->currentIndex()));
            }
        }
    }
}


void simuWin::cellChanged(int row, int column){
    //if(currentMode == MULTI_EXPERIMENT) {cerr << "ERR : cellChanged called in the Multi-obj mode, return."; return;}

    //cerr << "cell changed, ligne=" << row << ", col=" << column << ", newValue = " << TABLE->item(row, column)->text().toStdString() << endl;
    QObject::disconnect(ui->doubleSpinBoxLastParam, SIGNAL(valueChanged(double)), this, SLOT(paramChangedFromBox(double)));
    if((column == 0) && (row < currentModel->getNbParams())) {
        double paramValue = TABLE->item(row, column)->text().toDouble();
        currentExperiment->m->setParam(row, paramValue);
        ui->doubleSpinBoxLastParam->setValue(paramValue);
        ui->labelLastParam->setText(QString(currentExperiment->m->getParamName(row).c_str()));
        ui->doubleSpinBoxLastParam->setSingleStep(paramValue / 10.0);
        lastParamID = row;
    }
    currentExperiment->m->initialise();

    simulate();         // does currentExperiment->init() inside
    QObject::connect(ui->doubleSpinBoxLastParam, SIGNAL(valueChanged(double)), this, SLOT(paramChangedFromBox(double)));
}

void simuWin::paramSetDoubleClickedFromHistory(const QModelIndex & QMI){
    int row = QMI.row();
    // not allowed in optimize mode
    //if(!stopOpt){QMessageBox::information(this, QString("Error"), QString("You are not allowed to simulate a new parameter set while an optimization is running")); return;}
    int indexComb = -1;
    if(ui->checkBoxLinkToCombs->isChecked()) indexComb = QInputDialog::getInt(this, QString("Give the associated combination index (starts at 0)"), QString("?"), 0, -1 /* min */, nbCombs - 1, 1 /* step */);
    if(indexComb >= 0) cout << "Overriding only parameters from combination nr :" << indexComb << endl;
    //if(indexComb >= 0) ui->tableViewHistory->setHorizontalHeader();
    useParamSetFromHistory(row, indexComb);
    updateParmsFromModel();

    simulate();         // does currentExperiment->init() inside
}

void simuWin::expandPlot(){

    static bool expanded = true;
    ui->groupBoxHistory->setHidden(expanded);
    ui->tabWidgetRight->setHidden(expanded);
    ui->groupBoxOptim->setHidden(expanded);
    ui->pushButtonExpandPlot->setText((expanded) ? QString("Small") : QString("Big"));
    if(expanded){
        ui->groupBoxPlot->resize(1000,700);
        ui->widgetGraphe->resize(980, 670);
        currentGraphe->resize(980, 670);
        currentGraphe->bigPlot->resize(980, 670);
    } else {
        ui->groupBoxPlot->resize(611,431);
        ui->widgetGraphe->resize(591, 361);
        currentGraphe->resize(591, 361);
        currentGraphe->bigPlot->resize(591, 361);
    }
    expanded = !expanded;
}





void simuWin::optimize(){
    #ifdef ALLOW_OPTIMIZE
    stopOpt = false;
    nbCostCalls = 0;
    if(ui->lineEditWorkingFolder->text().size() == 0)
        ui->lineEditWorkingFolder->setText(QFileDialog::getExistingDirectory(this, tr("Open Directory"), "",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
    string optText = createOptimizerFile();
    if(ui->lineEditWorkingFolder->text().size() == 0) {cerr << "No Directory\n"; return;}
    string optFileName = (ui->lineEditWorkingFolder->text() + QString("Opt.txt")).toStdString();

//double costVar = currentExperiment->costPart(currentModel->getGlobalID(j));

    ofstream fichier((const char*) optFileName.c_str(), ios::out);
    if(fichier){
        fichier << optText << "\n0\n" << endl;
        fichier.close();}
    else {QMessageBox::information(this, QString("Error"), QString("The optimizer file could not be written in the provided file location : ") + (ui->lineEditWorkingFolder->text()));
        return;}

    if(recording) {
        currentMacro << "string optTxt = string(\"" << optText << "\");\n";
        currentMacro << "macro" << macroID << ".motherOptimize(optTxt, 50)\n";
    }
    history.resize(ui->spinBoxStore->value());
    history.clear();
    costRecords.clear();
    refreshHistory();
    myRandom::Randomize();
            myTimes::getTime();
            ui->checkBoxAll->setChecked(true);
            GeneralImplementation::initialize();
            BaseOptimizationProblem *E = this; //new simuWin(currentExperiment, this);
            BaseOptimizer *Opt = BaseOptimizer::createOptimizer(E, optFileName);
                //mySignal::addOptimizer(Opt);
                Opt->optimize();
                Opt->bestGlobal.print();
                //delete E; // program destructor first...
                delete Opt;
                cout << "Optimization Complete\n";

    #else
    QMessageBox::information(this, QString("Error"), QString("ERR : SimuWin::optimize(), function not available, you should do #define ALLOW_OPTIMIZATION in common.h and compile again ! \n"));
    cerr << "ERR : SimuWin::optimize(), function not available, you should do #define ALLOW_OPTIMIZATION\n";
    #endif
}


#ifdef ALLOW_OPTIMIZE
double simuWin::getCost(){
    nbCostCalls++;
    if(stopOpt == true) {               // to stop the optimization, replace a simulation by a huge value ...
        if((nbCostCalls % 100000) == 0) QCoreApplication::processEvents();
        return 1e15;
    }
    if((nbCostCalls % 50) == 1) {       // plots the evolution of costs every 50 simulations
        costGraphe->Plot(MINS, costRecords.mins, costRecords.Xs, QString("Min"), costGraphe->baseList(MINS));
        costGraphe->Plot(MAXS, costRecords.maxs, costRecords.Xs, QString("Max"), costGraphe->baseList(MAXS));
        costGraphe->Plot(MEANS, costRecords.means, costRecords.Xs, QString("Average"), costGraphe->baseList(MEANS));
        costGraphe->Plot(STDDEVS, costRecords.stddevs, costRecords.Xs, QString("Variances"), costGraphe->baseList(STDDEVS));
    }
    if((nbCostCalls % 10) == 0){        // displays the number of simulations
        ui->lcdNumberCostCall->display(nbCostCalls);
    }
    if((nbCostCalls % 15) == 0) {       // refreshes the interface regularly to avoid freezing during all the optimization
        QCoreApplication::processEvents();
    }

    //if(dimension > currentModel->getNbParams) {cerr << "Optimizer has more parameters than model\n"; return -1;}

    // applies the values from the optimizer (stored in parameters[i]) into the model
    int NPs =  currentModel->getNbParams();
    for(int i = 0; i < NPs; ++i){
        if(!std::isnan(parameters[i])) currentModel->setParam(i, parameters[i]);
    }

    // simulates ... (calling currentExperiment->simulate)

    simulate();             // does currentExperiment->init() inside
    double v1 = NAN, v2 = NAN;
    if(currentMode == MONO_EXPERIMENT){
        v1 = currentExperiment->costPart();
        v2 = currentExperiment->getPenalities();         // penalty in case of divergence
    } else {
        v1 = listExperiments->costPart();
        v2 = listExperiments->getPenalities();
    }
    // stores the parameter set with its cost in the history
    vector<double> set = currentModel->getParameters();
    history.addSet(&set, v1+v2);
    costRecords.newValue(v1+v2);

    return v1 + v2;
}
#else
double simuWin::getCost(){
    cerr << "ERR : SimuWin::getCost(), function not available, you should do #define ALLOW_OPTIMIZATION\n";
    return 0;
}
#endif

void simuWin::stopOptimization(){
    stopOpt = true;
    ui->checkBoxDisplayCurves->setChecked(true);
    this->useParamSetFromHistory(0);
    simulate();             // does currentExperiment->init() inside
    //updateParmsFromModel();
    //history.print();
}

void simuWin::refreshHistory(){
    int NP = currentModel->getNbParams();
    int show = ui->spinBoxDisplay->value();
    if(tableHistory->rowCount() != show){
        cout << "Re-sizing history table \n";
        tableHistory->clear();
        tableHistory = new QStandardItemModel(show, NP + 1, ui->tableViewHistory);
        for(int i = 0; i < NP+1; ++i){
            for(int j = 0; j < show; ++j){
                tableHistory->setItem(j,i, new QStandardItem(QString("")));
                //tableHistory->item(j,i)->setBackground(QBrush(QColor(Qt::lightGray)));
            }
        }
        // should I delete all the previous items ?
    }

    ui->tableViewHistory->setColumnWidth(0, 80);
    tableHistory->setHorizontalHeaderItem(0, new QStandardItem(QString("Cost")));
    for(int i = 0; i < currentModel->getNbParams(); ++i){
        //ui->textEditParamSet->append(QString::number(currentModel->getParam(i)) + QString("\t"));
        tableHistory->setHorizontalHeaderItem(i+1, new QStandardItem(QString(currentModel->getParamName(i).c_str())));
    }
    vector<oneSet*> liste = history.toVector();
    reverse(liste.begin(), liste.end());
    int ls = liste.size();
    for(int i = 0; (i < ls) && (i < show); ++i){
        tableHistory->item(i,0)->setText(QString::number(liste[i]->cost));
        int s = liste[i]->v.size();
        if(s != NP) cerr << "ERR : refresh History incorrect size for parameters\n";
        for(int j = 0; j < NP; ++j){
            tableHistory->item(i, j+1)->setText(QString::number(liste[i]->v[j]));
        }
    }
    for(int i = ls; i < show; ++i){
        tableHistory->item(i,0)->setText(QString(""));
        for(int j = 0; j < NP; ++j){
            tableHistory->item(i, j+1)->setText(QString(""));
        }
    }
    ui->tableViewHistory->setModel(tableHistory);
}

void simuWin::saveHistory(QString _name){
    if(!_name.compare(QString(""))){
         _name = QFileDialog::getSaveFileName(this, QString(""), ui->lineEditWorkingFolder->text());
    }
    if(recording) {currentMacro << "macro" << macroID << ".saveHistory(" << _name.toStdString() << ")\n";}
    int NP = currentModel->getNbParams();
    int store = ui->spinBoxStore->value();
    cout << "      ... Writing into file " << _name.toStdString() << endl;
    ofstream f(_name.toStdString().c_str(), ios::out);
    if(!f) {QMessageBox::information(this, QString("Error"), QString("File not Found")); return;}
    f << NP << "\t";
    f << store << "\t";

    vector<oneSet*> liste = history.toVector();
    reverse(liste.begin(), liste.end());
    int ls = liste.size();
    for(int i = 0; (i < ls) && (i < store); ++i){
        f << liste[i]->cost;
        int s = liste[i]->v.size();
        if(s != NP) cerr << "ERR : save History incorrect size for parameters\n";
        for(int j = 0; j < NP; ++j){
            f << "\t" << liste[i]->v[j];
        }
        f << "\n";
    }
    f.close();
}

void simuWin::loadHistory(QString _name){
    if(!_name.compare(QString(""))){
         _name = QFileDialog::getOpenFileName(this, QString(""), ui->lineEditWorkingFolder->text());
    }
    if(recording) {currentMacro << "macro" << macroID << ".loadHistory(" << _name.toStdString() << ")\n";}
    manageSims::loadHistory(_name.toStdString());

    refreshHistory();
}



//#ifdef ALLOW_OPTIMIZE
string simuWin::createOptimizerFile(){
    // generates the text file for the optimizer and the text file for
    stringstream f;
    f << optChoice->generate();
    f << "\n";
    int NV = currentModel->getNbParams();
    int nbToOpt = 0;
    for(int i = 0; i < NV; ++i){
        if(((QCheckBox*) TABLE->cellWidget(i,3))->isChecked()) nbToOpt++;
    }
    f << nbToOpt << "\n";
    if(nbToOpt == 0){
        QMessageBox::information(this, QString("Error"), QString("No parameter selected for optimization -> 0 parameter to optimize !!"));
    }
    for(int i = 0; i < NV; ++i){
        if(! ((QCheckBox*) TABLE->cellWidget(i,3))->isChecked()) f << "#";
        f << "\t" << i << "\t";
        f << currentModel->getLowerBound(i) << "\t" << currentModel->getUpperBound(i) << "\t#" << currentModel->getParamName(i) << endl;
    }
    f << "\n\n0\nExponential\n0\n";
    ui->plainTextEditOptFile->setPlainText(QString(f.str().c_str()));
    return f.str();
}

//#endif

// nOte : by calling the loadConfig function of hte mother class, the model is updated according the config's parameters, be careful !
string simuWin::loadConfig(string _name){
    ////////int erase = QMessageBox::question(this, QString("Erase"), QString("This will erase all the table, do you confirm ?"), QMessageBox::Yes|QMessageBox::No);
    //////if(erase != QMessageBox::Yes) return string("Loading cancelled.");
    if(!_name.compare("")){_name = QFileDialog::getOpenFileName(this, QString(""), ui->lineEditWorkingFolder->text()).toStdString();}
    //QFile file(_name);
    //file.open(QFile::ReadOnly | QFile::Text);
    //QTextStream ReadFile(&file);
    //ui->textEditOptimizerFile->setText(ReadFile.readAll());

    string raisedErrors = manageSims::loadConfig(_name);

    if(raisedErrors.size() > 0) {
         QMessageBox::information(this, QString("Error"), QString("Problems with file format :\n") + QString(raisedErrors.c_str()));
         return raisedErrors;
    }
    int NP = currentModel->getNbParams();
    int NV = currentModel->getNbVars();
    int NC = nbCombs;

    QObject::disconnect(TABLE, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));
    for(int i = 0; i < NP + NV; ++i){
        for(int j = 0; j < NC + 4; ++j){
            if(((i < NP) || (j > 2)) && (j != 3))
                TABLE->item(i,j)->setText(QString(currentConfig[i][j].c_str()));
            if((j > 3)){
                if(!currentConfig[i][j].compare("1"))
                    TABLE->item(i,j)->setBackground(QBrush(QColor(Qt::green)));
                else if(!currentConfig[i][j].compare("."))
                    TABLE->item(i,j)->setBackground(QBrush(QColor(Qt::lightGray)));
                else
                    TABLE->item(i,j)->setBackground(QBrush(QColor(Qt::white)));
            }
        }
    }
    if(NC < nbCombs){
        for(int i = 0; i < NP + NV; ++i){
            for(int j = 4+NC; j < 4+nbCombs;++j){
                 TABLE->item(i,j)->setBackground(QBrush(QColor(Qt::gray)));
                 TABLE->item(i,j)->setText(QString(""));
            }
        }
    }
    QObject::connect(TABLE, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));
    return string("");
}



void simuWin::saveConfig(string _name){
    if(!_name.compare("")){
         _name = QFileDialog::getSaveFileName(this, QString(""), ui->lineEditWorkingFolder->text()).toStdString();
    }
    //QFile file(_name);
    //file.open(QFile::ReadOnly | QFile::Text);
    //QTextStream ReadFile(&file);
    //ui->textEditOptimizerFile->setText(ReadFile.readAll());

    int NP = currentModel->getNbParams();
    int NV = currentModel->getNbVars();
    cout << "      ... Writing into file " << _name << endl;
    ofstream f(_name.c_str(), ios::out);
    if(!f) {QMessageBox::information(this, QString("Error"), QString("File not Found")); return;}
    f << NP << "\t";
    f << NV << "\t";
    f << nbCombs << "\n";
    for(int i = 0; i < NP + NV; ++i){
        for(int j = 0; j < nbCombs + 4; ++j){
            if(((i < NP) || (j > 2)) && (j != 3))
                f << TABLE->item(i,j)->text().toStdString();    // to avoid to save the "NoData" or any comments in the grey left bottom area
            else f << ".";
            if(j < nbCombs + 3) f << "\t";
        }
        f << "\n";
    }
    f.close();
}

void simuWin::refreshMacro(){
    ui->textBrowserMacro->setPlainText(QString(currentMacro.str().c_str()));
}
void simuWin::resetMacro(){
    startMacro(string("modeleMinNoIL10"));
    ui->textBrowserMacro->setPlainText(QString(currentMacro.str().c_str()));
}
void simuWin::changeRecord(int z){
    recording = (z > 0);
}

void simuWin::costReport(){
    if(currentMode == MULTI_EXPERIMENT) {cerr << "ERR : cost Report called in the Multi-obj mode, return."; return;}

    cout << "Comparison between experiment and simulation" << endl;
    currentExperiment->costReport();
}




// problem : this config is created from nothing, so it takes the current parameter values from the model
vector<string> simuWin::makeFigReportParamSet(string _folder){
    //cerr << "WRN : simuWin::makeFigReportParamSet(), not too sure about this function" << endl;
    if(_folder.size() == 0) _folder = (QFileDialog::getExistingDirectory(this, tr("Open Directory (Create it yourself if necessary)"), ui->lineEditWorkingFolder->text(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks)).toStdString() + string("/");

    ui->checkBoxAll->setChecked(true);
    // creates combination, uses it and destroys it !
    createComb();
    saveConfig(_folder + string("ConfigPlusCurrent.txt"));

    vector<string> res;
    res.push_back(_folder);
    {
        useComb(nbCombs-1); // starts at 0
        //loadConfig(_folder + string("ConfigPlusCurrent.txt"));
        string res = makeTextReportParamSet(_folder, nbCombs-1, ui->doubleSpinBoxSimDT->value(), ui->doubleSpinBoxGraphDT->value());
        ofstream f1((_folder + string("SimResult.txt")).c_str(), ios::out);
        cout << "      ... writing in " << _folder + string("SimResult.txt") << endl;
        if(f1) {f1 << res; f1.close();}

        int NV = currentModel->getNbVars();
        //int NP = currentModel->getNbParams();
        // removes the last combination
        /*for(int i = 0; i < NP + NV; ++i){
            TABLE->item(i, 4 + nbCombs - 1)->setText(".");
            TABLE->item(i, 4 + nbCombs - 1)->setBackgroundColor(QColor(Qt::gray));
        }
        nbCombs--;
        currentConfig.resize(nbCombs); // might leave/leak a non deleted vector*/

        //simulate(); is already done by makeTextReportParamSet

        cout << "      ... Saving plots in " << _folder << endl;
        for(int i = 0; i < NV; ++i){
                ui->comboBoxVariable->setCurrentIndex(i);
                varChanged(i);// and on top have the good title !
                string add = string("Unsim-"); ////// DOES NOT WORK !!! string(((TABLE->cellWidget(NP+i,3)) && (!((QCheckBox*) TABLE->cellWidget(NP+i,3))->isChecked())) ? "Sim-" : "Unsim-" );
                QString outFile = QString((_folder + add + currentModel->getName(i) + ".png").c_str());
                //cout << "Var " << i << ", Figure saved in " << outFile.toStdString() << endl;
                //currentGraphe->bigPlot
                currentGraphe->exportToPng(outFile);

            //}
        }
    }
    if(currentMode == MULTI_EXPERIMENT){
        for(int i = 0; i < listExperiments->nbBigExp(); ++i){
            ui->comboBoxMultiExperiment->setCurrentIndex(i);
            currentExperimentChanged();
            //currentExperiment = listExperiments->getExperiment(i); // is done inside currentExperimentChanged
            useComb(nbCombs-1); // starts at 0
            //loadConfig(_folder + string("ConfigPlusCurrent.txt"));
            string subFolder = _folder + string("ForExp") + QString::number(i).toStdString() + listExperiments->getExperiment(i)->Identification + string("/");
            createFolder(subFolder);
            res.push_back(subFolder);
            string res = makeTextReportParamSet(subFolder, nbCombs-1, ui->doubleSpinBoxSimDT->value(), ui->doubleSpinBoxGraphDT->value());
            ofstream f1((subFolder + string("SimResult.txt")).c_str(), ios::out);
            cout << "      ... writing in " << subFolder + string("SimResult.txt") << endl;
            if(f1) {f1 << res; f1.close();}

            int NV = currentModel->getNbVars();
            //int NP = currentModel->getNbParams();
            // removes the last combination
            /*for(int i = 0; i < NP + NV; ++i){
                TABLE->item(i, 4 + nbCombs - 1)->setText(".");
                TABLE->item(i, 4 + nbCombs - 1)->setBackgroundColor(QColor(Qt::gray));
            }
            nbCombs--;
            currentConfig.resize(nbCombs); // might leave/leak a non deleted vector*/

            //simulate(); is already done by makeTextReportParamSet

            cout << "      ... Saving plots in " << subFolder << endl;
            for(int i = 0; i < NV; ++i){
                    ui->comboBoxVariable->setCurrentIndex(i);
                    varChanged(i);// and on top have the good title !
                    string add = string("Unsim-"); ////// DOES NOT WORK !!! string(((TABLE->cellWidget(NP+i,3)) && (!((QCheckBox*) TABLE->cellWidget(NP+i,3))->isChecked())) ? "Sim-" : "Unsim-" );
                    QString outFile = QString((subFolder + add + currentModel->getName(i) + ".png").c_str());
                    //cout << "Var " << i << ", Figure saved in " << outFile.toStdString() << endl;
                    //currentGraphe->bigPlot
                    currentGraphe->exportToPng(outFile);

                //}
            }

        }


    }
    return res;

}

void simuWin::setColorScale(int newScale){
    //if(nbGraphes == 0) cerr << "WRN: simuWin::setColorScale(), no graph yet to " << endl;
    //for(int i = 0; i < this->nbGraphes; ++i){
        if(!currentGraphe) {cerr << "ERR: simuWin::setColorScale(), graphes not initialized yet " << endl; return;}
        currentGraphe->setColorScale(newScale);
    //}
}














void simuWin::loadOptim(string _name){
     if(!_name.compare("")){
             _name = QFileDialog::getOpenFileName(this).toStdString();
     }
     QFile file(_name.c_str());
     file.open(QFile::ReadOnly | QFile::Text);

     QTextStream ReadFile(&file);
     //ui->textEditOptimizerFile->setText(ReadFile.readAll());

     cout << "      ... Reading file " << _name << endl;
     ifstream fichier(_name.c_str(), ios::in);
     if(!fichier) cerr << "file not found\n" << endl;
     fichier.close();
}

void simuWin::loadSet(string dest){
    if(currentModel) {
         if(!dest.compare("")) dest = QFileDialog::getOpenFileName(this,"Open ...",QDir::currentPath().toStdString().c_str(), tr("Text Files (*.txt)")).toStdString();
         currentModel->loadParameters(dest);
         //ui->textEditParamSet->clear();
         QString res = QString("");
         for(int i = 0; i < currentModel->getNbParams(); ++i){
             res.append(QString::number(currentModel->getParam(i)) + QString("\t"));
         }
         //ui->textEditParamSet->append(res);
         // Experiments fera le initialize
    }
}


void simuWin::sensitivity(){
    vector<double> currentParamSet = currentModel->getParameters();
    motherSensitivity(currentParamSet, -1);
}
void simuWin::identifiability(){
    vector<double> currentParamSet = currentModel->getParameters();
    motherIdentifiability(currentParamSet, -1);
}

void simuWin::saveSet(string _name){
    if(!_name.compare("")){
         _name = QFileDialog::getSaveFileName(this, QString(""), ui->lineEditWorkingFolder->text()).toStdString();
    }
    currentModel->saveParameters(_name);
}


void simuWin::resetParams(){
    cout << "Reset Parameters\n";
    currentModel->setBaseParameters();
    //for(int i = 0; i < currentModel->getNbParams(); ++i){
        //currentModel->setParam(i, currentModel->getLowerBound(i));
        //cout << i << " , " << currentModel->getParam(i) << endl;
    //}
    //currentExperiment->m->initialise();
    updateParmsFromModel();
/*    QModelIndex startOfRow = tableContents->index(0, 0);
    tableContents->item(0,0)->setData(1); //>setData(QString("1"));
    emit tableContents->dataChanged(startOfRow, startOfRow);
    ui->tableView->repaint();
*/
    simulate();
}

simuWin::~simuWin()
{
    delete ui;
}

Optimizer::Optimizer(simuWin* _currentWindow) : currentWindow(_currentWindow){}
Optimizer::~Optimizer(){}
void Optimizer::process(){
    currentWindow->optimize();
    emit finished();
}


void simuWin::optimizeThread(){
    thread = new QThread;
    opt = new Optimizer(this);
    opt->moveToThread(thread);
    connect(opt, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(thread, SIGNAL(started()), opt, SLOT(process()));
    connect(opt, SIGNAL(finished()), thread, SLOT(quit()));
    connect(opt, SIGNAL(finished()), opt, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

#endif

evolution::evolution(int _sizeGroups) : sizeGroups(_sizeGroups), currentGroup(0), nbInCurrentGroup(0) {
    if((sizeGroups < 1) || (sizeGroups > 1e6)) {cerr << "ERR evolution : size group (" << sizeGroups << " is out of bounds. 1e6 taken\n"; sizeGroups = 1e6;}
    clear();
}

void evolution::clear(){
    int NL = tables.size();
    for(int i = 0; i < NL; ++i)
        if(tables[i]) delete tables[i];
    tables.clear();
    tables.push_back(new vector<double>(sizeGroups, 1e12));
    currentGroup = 0;   // is also the number of finished groups
    nbInCurrentGroup = 0;
    stddevs.clear(); // size : currentGroup, attention !
    mins.clear();
    maxs.clear();
    means.clear();
    Xs.clear();
}

void evolution::newValue(double _v){
    if(nbInCurrentGroup == sizeGroups) {
        static int cptNew = 0;
        cptNew++;
        if(cptNew > 10000) cerr << "WRN: Would need to free memory inside evolution::newValue" << endl;
        tables.push_back(new vector<double>(sizeGroups, 1e12));
        nbInCurrentGroup = 0;
        double sum = 0;
        double ecarts = 0;
        double locmin = (* tables[currentGroup])[0];
        double locmax = (* tables[currentGroup])[0];
        for(int i = 0; i < sizeGroups; ++i){
            double val = (* tables[currentGroup])[i];
            sum += val;
            ecarts += val * val;
            locmin = min(locmin, val);
            locmax = max(locmax, val);
        }
        if(currentGroup > 0) {
            locmin = min(locmin, mins[currentGroup-1]); // indice to check
        }
        mins.push_back(locmin);
        maxs.push_back(locmax);
        means.push_back(sum / (double) sizeGroups);
        stddevs.push_back(sqrt((ecarts / (double) sizeGroups) - means[currentGroup] * means[currentGroup]));
        currentGroup++;
        Xs.push_back(currentGroup * sizeGroups);
    }
    (*(tables[currentGroup]))[nbInCurrentGroup] = _v;
    nbInCurrentGroup++;
}

string evolution::print(){
    stringstream f;
    f << "Evolution of cost over time, evaluated for every group of (" << sizeGroups << ") new simulations\n";
    f << currentGroup << "\t" << 5 << "\n";
    f << "nb\tMin\tMax\tAverage\tStdDev\n";
    for(int i = 0; i < currentGroup; ++i){
        f << std::setprecision(5) << i << "\t" << mins[i] << "\t" << maxs[i] << "\t" << means[i] << "\t" << stddevs[i] << "\n";
    }
    return f.str();
}

void simuWin::showOnlyCombChecked(int newState){
    int NP = currentModel->getNbParams();
    int NV = currentModel->getNbVars();
    ui->spinBoxOnlyComb->setMinimum(0);
    ui->spinBoxOnlyComb->setMaximum(nbCombs - 1);
    int selectedConf = ui->spinBoxOnlyComb->value();
    for(int i = 0; i < NP; ++i){
        if(newState > 0){
            currentTable->setRowHidden(i, currentConfig[i][selectedConf + 4].compare("1"));
        }
        else currentTable->setRowHidden(i, false);
    }
    for(int i = NP; i < NP + NV; ++i){
        currentTable->setRowHidden(i, (newState > 0));
    }

}





