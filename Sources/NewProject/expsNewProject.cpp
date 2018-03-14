#include "../Framework/Experiment.h"
#include "../Framework/evaluator.h"
#include "../Framework/modele.h"
//#include "namesNewProject.h"
#include "expsNewProject.h"

#include <iostream>
#include <fstream>
#include <sstream>

expOne::expOne(Modele* _m) : Experiment(_m, 1){
    Identification = string("Kinetics");
    //names_exp[EXP_XXX] = string("XXX");
    doable.clear();
    doable.resize(1, true);
    m->setBaseParameters();
}


void expOne::simulate(int IdExp, Evaluator* E, bool force){
    if(motherPrepareSimulate(IdExp, E, force)){
        switch(IdExp){
            /*case EXP_XXX:  {
                m->initialise(Back::XXX); break;}*/
            default: {m->initialise(Back::WT); break;}
        }
        switch(IdExp){
        case EXP_KIN1:{
            m->setValue(N::GlobVAR3, 5000.0);
            m->simulate(40, E); break; // 130 days -> might change later
        }
        case EXP_KIN2:{
            m->setValue(N::GlobVAR2, 5000.0);
            m->simulate(40, E); break; // 130 days -> might change later
        }
        }
        m->setOverrider(NULL);
    }
}
