// ------- Automatically generated model -------- //
#include "../../common.h"

#include "4-modeleDevelopmentNoFeedback.h"

//     enum{initP1, initP2, initP3, kinput, kd1, k1_2, kd2, k2_3, kd3, k3_out, kinhib3_1, maxinhib3_1, NbParameters};
//     enum{P1, P2, P3, CumulOut, NbVariables};

modeleDevelopmentNoFeedback::modeleDevelopmentNoFeedback() : Modele(NbVariables, NbParameters), background(Back::WT) {
    name = string("modeleDvpNoFeedback");
    dt = 0.001; // initial time step -> then it is adaptive
    print_all_secs = 1200; //every how many seconds it is plotting
    // Name of variables
    names[P1] = string("Progenitor 1 (P1)");
    names[P2] = string("Progenitor 2 (P2)");
    names[P3] = string("Mature cells (P3)");
    names[CumulOut] = string("CumulatedOutput");

    // the names of variables that can be accessed by outside (global name-space)
    extNames[P1] = N::Dvp_P1;
    extNames[P2] = N::Dvp_P2;
    extNames[P3] = N::Dvp_P3;
    extNames[CumulOut] = N::Dvp_Mout;

    // Name of parameters
    paramNames[initP1] = "Initial Number of P1";
    paramNames[initP2] = "Initial Number of P2";
    paramNames[initP3] = "Initial Number of P3 (mature)";
    paramNames[kinput] = "Inflow rate to P1";
    paramNames[kd1] = "Death rate P1";
    paramNames[k1_2] = "Conversion P1 -> P2";
    paramNames[kd2] = "Death rate P2";
    paramNames[k2_3] = "Conversion P2 -> P3";
    paramNames[kd3] = "Death rate P3";
    paramNames[k3_out] = "Outflow P3 -> out";
   // paramNames[kinhib3_1] = "K inhibition P3 to inflow";
   // paramNames[maxinhib3_1] = "Max inhibition P3 to inflow";
    paramNames[mutcoeffkd1] = "Mutant kd1";
    paramNames[mutcoeffk1_2] = "Mutant k1_2";
    paramLowBounds[initP1] = 1e-3;
    paramLowBounds[initP2] = 1e-3;
    paramLowBounds[initP3] = 1e-3;
    paramLowBounds[kinput] = 1e-6;
    paramLowBounds[kd1] = 1e-5;
    paramLowBounds[k1_2] = 1e-5;
    paramLowBounds[kd2] = 1e-5;
    paramLowBounds[k2_3] = 1e-5;
    paramLowBounds[kd3] = 1e-5;
    paramLowBounds[k3_out] = 1e-5;
    //paramLowBounds[kinhib3_1] = 0.01;
    //paramLowBounds[maxinhib3_1] = 1.0;
    paramLowBounds[mutcoeffkd1] = 0.0001;
    paramLowBounds[mutcoeffk1_2] = 0.0001;

    paramUpBounds[initP1] = 1e6;
    paramUpBounds[initP2] = 1e6;
    paramUpBounds[initP3] = 1e6;
    paramUpBounds[kinput] = 10;
    paramUpBounds[kd1] = 10;
    paramUpBounds[k1_2] = 10;
    paramUpBounds[kd2] = 10;
    paramUpBounds[k2_3] = 10;
    paramUpBounds[kd3] = 10;
    paramUpBounds[k3_out] = 10;
  //  paramUpBounds[kinhib3_1] = 1e6;
  //  paramUpBounds[maxinhib3_1] = 1000;
    paramUpBounds[mutcoeffkd1] = 1000;
    paramUpBounds[mutcoeffk1_2] = 1000;

    backSimulated.clear();
    backSimulated.push_back(Back::WT);
}

void modeleDevelopmentNoFeedback::setBaseParameters(){
    background = Back::WT;
    params.clear();     // to make sure they are all put to zero
    params.resize(NbParameters, 0.0);

    params[initP1] = 10;
    params[initP2] = 2;
    params[initP3] = 1;
    params[kinput] = 0.005;
    params[kd1] = 0.02;
    params[k1_2] = 0.05;
    params[kd2] = 0.01;
    params[k2_3] = 0.03;
    params[kd3] = 0.002;
    params[k3_out] = 0.1;
 //   params[kinhib3_1] = 20;
 //   params[maxinhib3_1] = 8;
    params[mutcoeffkd1] = 1.0;
    params[mutcoeffk1_2] = 1.0;


    setBaseParametersDone();
}

void modeleDevelopmentNoFeedback::initialise(long long _background){ // don't touch to parameters !
    val.clear();
    val.resize(NbVariables, 0.0);
    init.clear();
    init.resize(NbVariables, 0.0);

    // test if each possible background is in the current combination of backgrounds (stored in the field background)
    if((background != Back::WT) && (background != _background)){
        //cerr << "WRN : modeleLeishmania::initialize, when the background has been changed from WT, you should not change it again, because parameters can not be retrieved. Please load parameters again" << endl;
    }
    background = _background;
    if(background == Back::WT){
        params[usedkd1] = params[kd1];
        params[usedk1_2] = params[k1_2];

    }
    if(background == Back::MUT){
        params[usedkd1] = params[mutcoeffkd1];
        params[usedk1_2] = params[mutcoeffk1_2];
    }

    init[P1]   = params[initP1];
    init[P2]    = params[initP2];
    init[P3]   = params[initP3];

    for(int i = 0; i < NbVariables; ++i){
        val[i] = init[i];}
    t = 0;
    initialiseDone();
}

void modeleDevelopmentNoFeedback::derivatives(const vector<double> &x, vector<double> &dxdt, const double t){
    if(!over(P1))
        dxdt[P1] 	= params[kinput]/*  * (1.0 / (params[maxinhib3_1] * (x[P3] + params[kinhib3_1])))*/ + x[P1] * (- params[usedkd1] - params[usedk1_2]);
    if(!over(P2))
        dxdt[P2] 	= x[P1] * params[usedk1_2] +  x[P2] * (- params[kd2] - params[k2_3]);
    if(!over(P3))
        dxdt[P3] 	= x[P2] * params[k2_3] - x[P3] * params[k3_out]- params[kd3] * x[P3];
    if(!over(CumulOut))
        dxdt[CumulOut] 	= x[P3] * params[k3_out];
}

