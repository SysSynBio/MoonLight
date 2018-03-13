

#include "model1.h"

model1::model1() : Modele(NbVariables, NbParameters), background(Back::WT) {
    name = string("Model for new project");
    dt = 0.001; // initial time step -> then it is adaptive
    print_all_secs = 1200; //every how many seconds it is plotting
    // Name of variables
    names[A] = string("A");
    // ...

    // the names of variables that can be accessed by outside (global name-space)
    extNames[A] = N::GlobVAR3;
    extNames[C] = N::GlobVAR2;
    // ...

    // Name of parameters

    paramNames[InitA] = "A Initial Number";
    // ...
    //----------------------------------------------------------------------
    paramLowBounds[InitA] = 0;
    // ...
    //-------------------------------------------------------------------
    paramUpBounds[InitA] = 100;
    // ...
}

void model1::setBaseParameters(){
    background = Back::WT;
    params.clear();     // to make sure they are all put to zero
    params.resize(NbParameters, 0.0);

    params[InitA] = 200;
    // ...

    setBaseParametersDone();
}

void model1::initialise(long long _background){ // don't touch to parameters !
    val.clear();
    val.resize(NbVariables, 0.0);
    init.clear();
    init.resize(NbVariables, 0.0);

    background = _background;
    // if(background == Back::XXX){
    //       init[A]   = ...
    //   }

    for(int i = 0; i < NbVariables; ++i){
        val[i] = init[i];}
    t = 0;
    initialiseDone();
}

void model1::derivatives(const vector<double> &x, vector<double> &dxdt, const double t){

    if(!over(A)) {// if it is not overrided by data then make the computation below
        dxdt[A] 	= x[A] * params[rateAB];
    }
    if(!over(B)) {// if it is not overrided by data then make the computation below
        dxdt[B] 	= 0; // etc.
    }
}

