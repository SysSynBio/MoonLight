#ifndef FITTE_H
#define FITTE_H

#include "../Framework/Experiment.h"
//#include "../Extreminator/optimizers/baseOptimizer.h"
//#include "../Extreminator/common/myRandom.h"
#include "../Extreminator/baseProblem.h"
//#include "../Extreminator/optimizers/baseOptimizer.h"
#include "../Interface/simuwin.h"

class Fitte : public GeneralImplementation {
public:
    Experiment* ExpToOpt;
    Fitte(Experiment* _Exp) ;
    double getCost();

    #ifndef WITHOUT_QT
    simuWin* window;
    #endif
};

void Optimize(string optimizerFile, Experiment* _Exp);

#endif // FITTE_H
