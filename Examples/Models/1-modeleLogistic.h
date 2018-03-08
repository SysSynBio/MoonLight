#ifndef  modeleLogistic_H
#define  modeleLogistic_H
#include "../../common.h"
#include "../../Framework/modele.h"
#include "../namesGradCourse.h"

/* This model has  unknown parameters, +  */
struct modeleLogistic : public Modele {
    modeleLogistic();
    enum{InitPop, R_growth, K_carrying_capacity, NbParameters};
    enum{NPop, NbVariables};
    long long background; // for different models
    virtual void derivatives(const vector<double> &x, vector<double> &dxdt, const double t);
    void initialise(long long _background = Back::WT);
	void setBaseParameters();
    void action(string name, double parameter){
        if(!name.compare("FoldIncrease")){
            if((parameter > 1000.0) || (parameter < 0)) {cerr << "ERR: ModeleMinLatent::action(" << name << ", " << parameter << "), wrong parameter value\n"; return;}
            val[NPop] =    parameter * val[NPop]; // example of how the hell it works
            return;
        }
    }
};

#endif

