#include "../Framework/Experiment.h"
#include "expsGradCourse.h"
#include "../Framework/evaluator.h"
#include "../Framework/modele.h"
#include "namesGradCourse.h"

#include <iostream>
#include <fstream>
#include <sstream>

expOneKin::expOneKin(Modele* _m) : Experiment(_m, 1){
    Identification = string("Kinetics");
    names_exp[WT_EXP] = string("Kinetics"); // activation (TCR Only)");
    doable.clear();
    doable.resize(1, true);
    m->setBaseParameters();
}


////// gné force is missing in simulate
void expOneKin::simulate(int IdExp, Evaluator* E, bool force){
    if(motherPrepareSimulate(IdExp, E, force)){
        switch(IdExp){
            /*case TH1_IFNGRKO: case IFNG_IFNGRKO: {
                m->initialise(Back::IFNGRKO); break;}*/
            default: {m->initialise(Back::WT); break;}
        }
        switch(IdExp){
        case WT_EXP:{
            //m->setValue(N::P, 5000.0);
            m->simulate(40, E); break;} // 130 days -> might change later
        }
        m->setOverrider(NULL);
    }
}
expWithMutant::expWithMutant(Modele* _m) : Experiment(_m, NB_EXP){
    Identification = string("Kinetics");
    names_exp[WT_EXP] = string("WT"); // activation (TCR Only)");
    names_exp[MUTANT_EXP] = string("Mutant"); // activation (TCR Only)");
    doable.clear();
    doable.resize(NB_EXP, true);
    m->setBaseParameters();
}


////// gné force is missing in simulate
void expWithMutant::simulate(int IdExp, Evaluator* E, bool force){
    if(motherPrepareSimulate(IdExp, E, force)){
        switch(IdExp){
            case WT_EXP: {m->initialise(Back::WT); break;}
            case MUTANT_EXP: {m->initialise(Back::MUT); break;}
            default: {m->initialise(Back::WT); break;}
        }
        switch(IdExp){
        case WT_EXP:{
            //m->setValue(N::P, 5000.0);
            m->simulate(40, E); break;} // 130 days -> might change later

        case MUTANT_EXP:{
             m->simulate(40, E);
            break;
        }
        }
        m->setOverrider(NULL);
    }
}
