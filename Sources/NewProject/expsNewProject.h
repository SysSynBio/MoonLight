#ifndef EXPERIMENTSNEWPROJ_H
#define EXPERIMENTSNEWPROJ_H


#include "../Framework/Experiment.h"
#include "../Framework/evaluator.h"
#include "../Framework/modele.h"
#include "../Framework/overrider.h"
#include "namesNewProject.h"

#include <vector>
#include <string>
#include <iostream>

// Important: here, you don't need to include your modele subclass. An experiment is designed to take a model, whatever it is !

using namespace std;

struct expOne : public Experiment {
    expOne(Modele* _m);

    //void init();
    void simulate(int IdExp, Evaluator* E = NULL, bool force = false); // if no E is given, VTG[i] is used
};

/* etc ...
struct expTwo : public Experiment {
    expTwo(Modele* _m);

    //void init();
    void simulate(int IdExp, Evaluator* E = NULL, bool force = false); // if no E is given, VTG[i] is used
};*/



#endif
