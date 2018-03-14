#ifndef modeleDevelopment_H
#define modeleDevelopment_H
#include "../../common.h"
#include "../../Framework/modele.h"
#include "../namesGradCourse.h"

/* This model has  unknown parameters, +  */
struct modeleDevelopment : public Modele {
    modeleDevelopment();
    enum{initP1, initP2, initP3, kinput, kd1, k1_2, kd2, k2_3, kd3, k3_out, kinhib3_1, maxinhib3_1, mutcoeffkd1, mutcoeffk1_2, usedkd1, usedk1_2, NbParameters};
    enum{P1, P2, P3, CumulOut, NbVariables};

    long long background; // for different models
    virtual void derivatives(const vector<double> &x, vector<double> &dxdt, const double t);
    void initialise(long long _background = Back::WT);
	void setBaseParameters();

    void action(string name, double parameter){
        if(!name.compare("wash")){
            if((parameter > 1.0) || (parameter < 0)) {cerr << "ERR: ModeleMinLatent::action(" << name << ", " << parameter << "), wrong parameter value\n"; return;}
            // val[IL2] =    (1 - parameter) * val[IL2]; // example of how the hell it works
            return;
        }
    }
};
#endif

