#ifndef EXPERIMENTSTHALL_H
#define EXPERIMENTSTHALL_H



#include "namesGradCourse.h"
#include "../Framework/Experiment.h"
#include "../Framework/evaluator.h"
#include "../Framework/modele.h"
#include "../Framework/overrider.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;

struct expOneKin : public Experiment {
    expOneKin(Modele* _m);

    //void init();
    void simulate(int IdExp, Evaluator* E = NULL, bool force = false); // if no E is given, VTG[i] is used
};


struct expWithMutant : public Experiment {
    expWithMutant(Modele* _m);

    //void init();
    void simulate(int IdExp, Evaluator* E = NULL, bool force = false); // if no E is given, VTG[i] is used
};



#endif // EXPERIMENTSTHALL_H
