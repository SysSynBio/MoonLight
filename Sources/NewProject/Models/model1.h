#ifndef  modele1_H
#define  modele1_H
#include "../../common.h"
#include "../../Framework/modele.h"
#include "../namesNewProject.h"

/* This model has  unknown parameters, +  */
struct model1 : public Modele {
    model1();
    enum{InitA, InitB, initC, rateAB, rateAC, NbParameters};
    enum{A, B, C, NbVariables}; // internal variables, can have more than the global ones
    long long background; // for different models
    virtual void derivatives(const vector<double> &x, vector<double> &dxdt, const double t);
    void initialise(long long _background = Back::WT);
    void setBaseParameters();

    /* optional: this allows to perform customed actions from a 'name' and a parameter, from an experiment. Example:
    void action(string name, double parameter){
        if(!name.compare("IncreaseB")){
            if((parameter > 1000.0) || (parameter < 0)) {cerr << "ERR: Modele1::action(" << name << ", " << parameter << "), wrong parameter value\n"; return;}
            val[B] =    parameter * val[B]; // example of how the hell it works
            return;
        }
    }*/
};

#endif

