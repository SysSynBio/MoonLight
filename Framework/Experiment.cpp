#include "Experiment.h"

#include <iostream>
#include <fstream>
#include <sstream>

void    Experiment::simulate(int /*IdExp*/, Evaluator* /*E*/,  bool force){cerr << "ERR: Experiment::simulate() should be reimplemented in daughter class\n";}
bool    Experiment::isDoable(int i){if((i >= 0) && (i < (int) doable.size())) return doable[i]; else {cerr << "ERR: Experiment::isDoable(" << i << "), index out of scope\n"; return false;}}
int     Experiment::nbCond(){return names_exp.size();}
string  Experiment::expName(int i){if((i >= 0) && (i < nbCond())) return names_exp[i]; else {cerr << "ERR: Experiment::expName(" << i << "), index out of scope\n"; return string("");}}

Experiment::Experiment(Modele* _m, int _nbConditions) : m(_m), nbConditions(_nbConditions), totalPenalities(0.0) {
    if((_nbConditions < 0) || (_nbConditions > 100000)) {cerr << "ERR : Experiment::Experiment(m, nbConditions=" << _nbConditions << "), invalid nb of Experiment\n"; return;}
    names_exp.resize(nbConditions, string("NoName"));
    doable.resize(nbConditions, false);
    Overs.resize(nbConditions, NULL);
    VTG.resize(nbConditions, NULL);
    ExpData.resize(nbConditions, NULL);       // to avoid segfault at reset
    ExpStdDevs.resize(nbConditions, NULL);
    reset();    // to fill the other vectors
}

void Experiment::motherReset(){
    for(int i = 0; i < nbConditions; ++i){
        if(VTG[i]) delete VTG[i];
        VTG[i] = new Evaluator();
        if(ExpData[i]) delete ExpData[i];
        if(ExpStdDevs[i]) delete ExpStdDevs[i];
    }
    ExpData.clear();
    ExpData.resize(nbConditions, NULL);
    ExpStdDevs.clear();
    ExpStdDevs.resize(nbConditions, NULL);
}

void Experiment::motherInit(){
    for(int i = 0; i < nbConditions; ++i){
        VTG[i]->resetDataKeepingExpected();
    }
}

void Experiment::motherSimulateAll( bool force){
    totalPenalities = 0;
    for(int i = 0; i < nbConditions; ++i){
        if(isDoable(i)) simulate(i, NULL, force);
        totalPenalities += m->penalities;
    }
}

double Experiment::motherCostExp(int expID){
    if(expID >= nbConditions) cerr << "Experiment::motherCostExp(" << expID << "), exp ID incorrect (only " << nbConditions << " Experiment)" << endl;
    int nbIntVars = this->m->getNbVars();
    double res = 0;
    if(expID < 0){ // this case is to check that motherCostPart(-1) and mlotherCostExp(-1) give the same result.
        for(int i = 0; i < nbIntVars; ++i){
            for(int j = 0; j < nbConditions; ++j){
                if((VTG[j])->contains(i)){
                    double subFit = VTG[j]->getFitnessSpecies(i);
                    if(! std::isnan(subFit)) res += subFit;
                }
            }
        }
    } else {
        for(int i = 0; i < nbIntVars; ++i){
            if((VTG[expID])->contains(i)){
                double subFit = VTG[expID]->getFitnessSpecies(i);;
                if(! std::isnan(subFit)) res += subFit;
            }
        }
    }
    return res;
}

double Experiment::motherCostPart(int species){
    double res = 0;
    if(species < 0){
        for(int i = 0; i < nbConditions; ++i){
            double subFitness = VTG[i]->getTotalFitness();
            if(! std::isnan(subFitness)) res += subFitness;
            ///else cerr << "Note : Exp " << i << " Has no data" << endl;
            //cout << " =========0 Cost Report for exp " << expName(i) << " 0========\n";
            //cout << VTG[i]->reportTotalFitness();
        }
        return res;
    } else {
        for(int i = 0; i < nbConditions; ++i){
            //Evaluator v;
            if((VTG[i])->contains(m->internValName(species))){
                double subFit = VTG[i]->getFitnessSpecies(m->internValName(species));
                if(!std::isnan(subFit)) res += subFit;
            }
        }
        return res;
    }
}




bool Experiment::motherPrepareSimulate(int IdExp, Evaluator* &E, bool force){
    if((IdExp < 0) || (IdExp >= nbCond()) || (!isDoable(IdExp))) {   // to avoid simulation if not doable with this model\n";
        m->initialise(); return false;}     // do initialize to be sure there is no track of a previous simulation
    if(!E) E = VTG[IdExp]; if(! E){
        cerr << "ERR : ExpTHs::simulate(), internal VTG[] Evaluators are not defined, seg faults on approach ...\n"; return false;};
    if(E) E->recordingCompleted();                                  // in case it has not been done
    if((force == false) && E && (E->size() == 0))                   // to avoid simulation if not required for cost
        return false;
    else {
        //if(m->saveKinetics) cout << "   -> -------------- " << names_exp[IdExp].c_str() << " -----------\n";
        m->setOverrider(Overs[IdExp]);                              // Caution, this erases the previous overrider from the model.
        return true;
    }
}


double Experiment::getPenalities(){return totalPenalities;}



void Experiment::giveData(TableCourse* kineticData, int IdExp){
    if((IdExp < 0) || (IdExp >= nbCond())) {cerr << "ERR: ExperimentThAll::giveData, out of bounds ExpID(" << IdExp << ")\n"; return;}
    ExpData[IdExp] = new TableCourse(kineticData);
}
void Experiment::giveStdDevs(TableCourse* kineticStds, int IdExp){
    if((IdExp < 0) || (IdExp >= nbCond())) {cerr << "ERR: ExperimentThAll::giveData, out of bounds ExpID(" << IdExp << ")\n"; return;}
    ExpStdDevs[IdExp] = new TableCourse(kineticStds);
}
void Experiment::giveHowToReadNamesInKinetics(vector<string> _NamesVariablesInTheirIndex){
    NamesVariablesInTheirIndex.clear();
    NamesVariablesInTheirIndex = _NamesVariablesInTheirIndex;
}

void Experiment::loadEvaluators(){
    static bool loaded = false;
    // the problem is that an evaluator should have the local indices of the model
    // but here the data is provided with global names,
    // so we need to create a vector with the global names of variable with the model's indices
    vector<string> NamesVariablesInModel;
    //cout << "      ... Note : Expreriments::loadEvaluators() uses the indices of variables inside the model instead of global indices. Kinetics read with names:\n";
    NamesVariablesInModel.resize(m->getNbVars(), string(""));
    for(int i = 0; i < m->getNbVars();++i){
        NamesVariablesInModel[i] =  NamesVariablesInTheirIndex[m->getGlobalID(i)];
        if(!loaded) cout << "         Var In Model : " << i << "\t" << NamesVariablesInModel[i] << endl;
    }
    for(int i = 0; i < nbCond(); ++i){
        if(VTG[i]) {
            if(ExpData[i]){ /// Phi 11/07/16, not 100% sure this is correct
                VTG[i]->getValsFromKinetics(ExpData[i],ExpStdDevs[i],NamesVariablesInModel);
            }
            VTG[i]->recordingCompleted();
        }
    }
    if(!loaded) loaded = true;
}







void Experiment::setOverrider(int IdExp, overrider* _ov){
    if((IdExp < 0) || (IdExp >= nbCond())) {cerr << "ERR: ExperimentThAll::setOverrider, out of bounds ExpID(" << IdExp << ")\n"; return;}
    Overs[IdExp] = _ov;
}

/// CAREFULL, the overriders will erase those from the model ...
void Experiment::overrideVariable(int IdGlobalName, bool override, int IdExp){
    if((IdExp >= nbCond())) {cerr << "ERR: ExperimentThAll::overrideVariable, out of bounds ExpID(" << IdExp << ")\n"; return;}
    if(IdExp >= 0){
        if(Overs[IdExp]){
            (* Overs[IdExp]).setOver(IdGlobalName, override);
        }
    } else {
        for(int i = 0; i < nbCond();++i){
            if(Overs[i]){
                //cerr << "Override " << ((override) ? "TRUE" : "FALSE") << endl;
                (* Overs[i]).setOver(IdGlobalName, override);
            }
        }
    }
}

bool Experiment::canOverride(int IdGlobalVariable, int IdExp){
    bool canDo = true;
    if((IdExp < -1) << (IdExp >= nbCond())) return false;
    if(IdExp == -1){
        for(int i = 0; i < nbCond(); ++i){
            canDo = canDo && (Overs[i] != NULL) && (Overs[i]->hasData(IdGlobalVariable));
        }
    } else canDo = canDo && (Overs[IdExp] != NULL) && (Overs[IdExp]->hasData(IdGlobalVariable));
    /*cerr << "Exp " << IdExp << ", " << IdGlobalVariable << "Can override ?";
    if(Overs[IdExp] != NULL) cerr << "Overrides defined ";
    if(( nbCond() > 0) && (Overs[0]->hasData(IdGlobalVariable))) cerr << "At least one condition ok";
    cerr << endl;*/
    return canDo;
}









string Experiment::extractData(vector<int> timePoints, vector<int> idGlobalVariables, vector<string> namesAllGlobVars, vector<int> IDexps){

    int nbIdExps = IDexps.size();
    int nbTP = timePoints.size();
    int nbGlVars = idGlobalVariables.size();

    if((nbGlVars == 0) || (nbTP == 0)) {cerr << "ERR: Experiment::generateDataFile, no variable or time point given " << endl; return string("");}
    init();
    // puts data to the evaluators

    if(nbIdExps == 0){
        IDexps.resize(nbCond());
        nbIdExps = nbCond();
        for(int i = 0; i < nbIdExps; ++i){
            IDexps[i] = i;
        }
    }

    vector<Evaluator*> EVs;
    EVs.resize(nbIdExps);
    for(int i = 0; i < nbIdExps; ++i){
        EVs[i] = new Evaluator();
    }

    for(int i = 0; i < nbIdExps; ++i){
        Evaluator* locE = EVs[i];
        for(int j = 0; j < nbTP; ++j){
            for(int k = 0; k < nbGlVars; ++k){
                locE->getVal(m->internValName(idGlobalVariables[k]), timePoints[j]);
                /*if(m->saveKinetics) {
                    cout << "I want : " << m->getName(m->internValName(idGlobalVariables[k])) << ", t=" << timePoints[j] << endl;
                }*/
            }
        }
        locE->recordingCompleted();
    }
    for(int i = 0; i < nbIdExps; ++i){
        simulate(IDexps[i], EVs[i]);
    }

    //   writes a string with data as tables :
    //  NB_EXPS
    //  ID_EXP  NameEXP NbLg    NbVars
    //  Time    NameV1  NameV2  ...
    //  data    ...     ...
    //
    //  ID_EXP  NameExp ...

    stringstream fi;
    //fi << "# Automatically generated Data\n";
    fi << IDexps.size() << "\n";
    for(int i = 0; i < nbIdExps; ++i){
        fi << "\n" << IDexps[i];
        if(isDoable(IDexps[i])){
            fi << "\t" << nbTP << "\t" << nbGlVars << "\t#" << expName(IDexps[i]) << "\n";

            Evaluator* locE = EVs[i];
            if(! locE) {cerr << "ERR: GenerateDataFile Part, Evaluators are not defined, seg faults on approach ...\n";};


            fi << "time(s)";
            for(int k = 0; k < nbGlVars; ++k){
                if((idGlobalVariables[k] < 0) || (idGlobalVariables[k] >= (int) namesAllGlobVars.size())) {fi << "\tNOTGIVEN"; cerr << "ERR : Experiment::ExtractData, the name of global variable " << idGlobalVariables[k] << " was not found in the table of names given (that was of size " << namesAllGlobVars.size() << endl;}
                fi << "\t" << namesAllGlobVars[idGlobalVariables[k]]; //m->getName(m->internValName(idGlobalVariables[k]));
            }
            /* fi << " #";
            for(int k = 0; k < nbGlVars; ++k){
                fi << "\t" << m->getName(m->internValName(idGlobalVariables[k])); //m->getName(m->internValName(idGlobalVariables[k]));
            }*/
            fi << "\n";

            for(int j = 0; j < nbTP; ++j){
                fi << timePoints[j];
                for(int k = 0; k < nbGlVars; ++k){
                    double v = locE->getVal(m->internValName(idGlobalVariables[k]), timePoints[j]);
                    fi << "\t" << v;
                }
                fi << "\n";
            }

        } else {fi << "\t0\t0\t#" << expName(IDexps[i]) << "\n";;}
    }
    return fi.str();
}


// would be better as a map between ID and name.
int getIdNameFromTable(string s, vector<string> & _NamesVariablesInTheirIndex){
    int n =  _NamesVariablesInTheirIndex.size();
    for(int i = 0; i < n ; ++i){
        if(! _NamesVariablesInTheirIndex[i].compare(s)) return i;
    }
    return -1;
}

int getPosInKinetics(string sname, TableCourse* Tc){
    //cout << "look for " << sname;
    int n = Tc->nbVar;
    for(int i = 1; i < n+1; ++i){
        if(! (Tc->headers[i]).compare(sname)) return i-1;
    }
    return -1;
}

string Experiment::print(){
    stringstream res;
    res << "========== Sum up of experiment content ========\n";
    res << "\n---------- Model used \n\n";
    res << m->print();
    res << "\n---------- " << nbConditions << " Experiments declared\n\n";
    for(int i = 0; i < nbConditions; ++i){
        res << "\t" << i << "\t" << expName(i) << (isDoable(i) ? "\tCan Be Simulated\n" : "\tNot doable with this model\n");
    }
    for(int i = 0; i < nbConditions; ++i){
        res << "\n---------- Additional information for " << expName(i) << "\n\n";
        if((! VTG[i]) || (VTG[i]->size() == 0)) res << "No Evaluator / Empty evaluator for this experiment\n";
        else{ res << VTG[i]->printState();}
        if(ExpData[i]) {res << "Experimental data :\n"; res << ExpData[i]->print(false);} else res << "No Experimental data\n";
        res << endl;
        if(ExpStdDevs[i]) {res << "Experimental Std Deviations :\n"; res << ExpStdDevs[i]->print(false);} else res << "No Standard Deviations\n";
    }
    res << "Table of names for reading the kinetics :\n";
    for(int i = 0; i < (int) NamesVariablesInTheirIndex.size(); ++i){
        res << i << "\t" << NamesVariablesInTheirIndex[i] << endl;
    }
    return res.str();
}

string Experiment::costReport(){
    stringstream res;
    for(int i = 0; i < nbConditions; ++i){
        res << names_exp[i] << "\n";
        res << VTG[i]->reportTotalFitness();
    }
    res << "\n\nCost per condition : \nExpNr\tCost\tName\n";
    for(int i = 0; i < nbConditions; ++i){
        res << i << "\t" << this->costExp(i) << "\t" << names_exp[i] << endl;
    }
    res << "\n\nCost per Variable : \nVarLocalNr\tVarGlobID\nCost\tName\n";
    for(int i = 0; i < m->getNbVars(); ++i){
        res << i << "\t" << m->getGlobalID(i) << "\t" << this->costPart(m->getGlobalID(i)) << "\t" << m->getName(i) << endl;
    }
    return res.str();
}




















































/// TODO : remove evaluator from here
void    MultiExperiments::simulate(int /*IdExp*/, Evaluator* /*E*/,  bool force){cerr << "ERR: Experiments::simulate() should be reimplemented in daughter class, or call simulateAll !\n";}
int     MultiExperiments::nbBigExp(){return NbBigExps;}
string  MultiExperiments::BigExpName(int i){if((i >= 0) && (i < nbBigExp())) return ListBigExperiments[i]->Identification; else {cerr << "ERR: Experiments::BigExpName(" << i << "), index out of scope\n"; return string("");}}
MultiExperiments::MultiExperiments(Modele* _m) : m(_m), NbBigExps(0) {reset();}
void MultiExperiments::AddExperiment(Experiment* Ex){
    coefficients.push_back(1.0);
    ListBigExperiments.push_back(Ex);
    NbBigExps++;}
Experiment* MultiExperiments::getExperiment(int BigExpID){
    if((BigExpID < 0) || (BigExpID > NbBigExps)) {cerr << "ERR: MultiExperiments::getExperiment(" << BigExpID << "), only " << NbBigExps << " experiments\n"; return NULL;}
    return ListBigExperiments[BigExpID];}
void MultiExperiments::reset(){
    cptNbSimus = 0;
    coefficients.clear();
    for(int i = 0; i < NbBigExps; ++i){
        ListBigExperiments[i]->reset();}}
void MultiExperiments::init(){
    for(int i = 0; i < NbBigExps; ++i){
        ListBigExperiments[i]->init();}}
void MultiExperiments::simulateAll(bool force){
    politics();     // to change the coefficients if necessary !
    for(int i = 0; i < NbBigExps; ++i){
        if(coefficients[i] > 0){
            ListBigExperiments[i]->simulateAll(force);
        } else { ///§§§ Danger ! be sure that the cost from previous experiment does not appear anywhere then
            ListBigExperiments[i]->init();
        }
    }
    postPolitics();
    cptNbSimus++;
}
double MultiExperiments::getCoefficient(int BigExpID){
    if((BigExpID < 0) || (BigExpID > NbBigExps)) {cerr << "ERR: MultiExperiments::getCoefficient(" << BigExpID << "), only " << NbBigExps << " experiments\n"; return 0.0;}
    return coefficients[BigExpID];}
void MultiExperiments::setCoefficient(int BigExpID, double newValue){
    if((BigExpID < 0) || (BigExpID > NbBigExps)) {cerr << "ERR: MultiExperiments::setCoefficient(" << BigExpID << "), only " << NbBigExps << " experiments\n"; return ;}
    if(newValue < -1e12) cerr << "WRN!!! : MultiExperiments::setCoefficient, are you serious ? you are giving a negative coefficient (" << newValue << ") to an experiment (" << getExperiment(BigExpID)->Identification << ")" << endl;
    coefficients[BigExpID] = newValue;}

/*double MultiExperiments::costBigExp(int BigExpID){
    double res = 0;
    if(BigExpID < 0){
        for(int i = 0; i < NbBigExps; ++i){
            res += ListBigExperiments[i]->costPart();
        }
    } else {
        if(BigExpID >= NbBigExps) cerr << "MultiExperiments::motherCostExp(" << BigExpID << "), BigExp ID incorrect (only " << NbBigExps << " experiments)" << endl;
        res = ListBigExperiments[BigExpID]->costPart();
    }
    return res;
}*/
/*double MultiExperiments::costPart(int species){
    double res = 0;
    for(int i = 0; i < NbBigExps; ++i){
        res += ListBigExperiments[i]->costPart(species);
    }
    return res;
}*/




double MultiExperiments::costBigExp(int BigExpID){
    double res = 0;
    cptNbSimus++;
    if(BigExpID < 0){
        for(int i = 0; i < NbBigExps; ++i){
            res += coefficients[i] * ListBigExperiments[i]->costPart();        // careful, mixing between indices and enum in
        }
    } else {
        if(BigExpID >= NbBigExps) cerr << "MultiExperiments::motherCostExp(" << BigExpID << "), BigExp ID incorrect (only " << NbBigExps << " experiments)" << endl;
        res = coefficients[BigExpID] * ListBigExperiments[BigExpID]->costPart();
    }
    return res;
}

double MultiExperiments::costPart(int species){
    double res = 0;
    for(int i = 0; i < NbBigExps; ++i){
        res += coefficients[i] * ListBigExperiments[i]->costPart(species);
    }
    return res;
}

void MultiExperiments::loadEvaluators(){
    for(int i = 0; i < NbBigExps; ++i){
        ListBigExperiments[i]->loadEvaluators();}}




string MultiExperiments::print(){
    stringstream res;
    res << "********** Multi-Experiments sum up ********\n";
    for(int i = 0; i < NbBigExps; ++i){
        res << ListBigExperiments[i]->print();
    }
    return res.str();
}

double MultiExperiments::getPenalities(){
    double res = 0;
    for(int i = 0; i < NbBigExps; ++i){
        res += ListBigExperiments[i]->getPenalities();
    }
    return res;
}

string MultiExperiments::costReport(){
    stringstream res;
    for(int i = 0; i < NbBigExps; ++i){
        res << ListBigExperiments[i]->costReport();
    }
    return res.str();
}

void MultiExperiments::overrideVariable(int IdGlobalName, bool override){
    for(int i = 0; i < NbBigExps; ++i){
        ListBigExperiments[i]->overrideVariable( IdGlobalName, override, -1);
    }
}
