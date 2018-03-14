// ------- Automatically generated model -------- //
#ifndef modeleFlu_H
#define modeleFlu_H
#include "../../common.h"
#include "../../Framework/modele.h"
#include "../namesGradCourse.h"


/* This model has  unknown parameters, +  */
struct modeleInfluenza : public Modele {
    modeleInfluenza();
    enum{initU, initI, initV, initT, beta, delta, p_replicV, c_clearV, ST_basalCD8, rho_activ, deathT, NbParameters};
    enum{U, I, V, T, NbVariables};

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

