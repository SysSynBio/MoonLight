

#include "../Framework/modele.h"

enum{ Dvp_P1, Dvp_P2, Dvp_P3, CumOut, NB_GLOB_VARS};   // ID of variables
vector<string> GlobalNamesVariables = {"Progenitor_1", "Progenitor_2", "MatureCells_P3", "Cumulated Output"};

namespace Back { enum : long long {WT= 1, MUT=2}; }// the different backgrounds

struct modeleDevelopment : public Modele {
    enum {A, B, C, NBVAR};                             // List of variables
    enum {KF, KD1, KD2, KD3, K12, K23, KD, NBPARAM};   // List of parameters
    long long background;                              // to define different options of simulations (Wt and Mut backgrounds)

modeleDevelopment() : Modele(NbVariables, NbParameters), background(Back::WT) {

    name = string("modeleDvp");
    dt = 0.001; 			// initial guess of time step -> then it is adaptive
    print_all_secs = 1200; 		// every how many seconds the simulation will be plotted (resolution)

    names[A] = string("PopA");
    names[B] = string("PopB");
    names[C] = string("PopC");
    paramNames[KF]      = string("production"); 	paramNames[KD1]   = string("Death of A");
    paramNames[KD2]  	= string("Death of B");	paramNames[KD3]   = string("Death of C");
    paramNames[K12]  	= string("Conversion A->B");	paramNames[K23]   = string("Conversion B->C");
    paramNames[KD]  	= string("Maximal space");	paramNames[mutKD1]= string("Mutant death of A");
    paramNames[mutK12]	= string("Mutant conversion A->B");

    paramLowBounds[KF] = 1e-6;			paramUpBounds[KF] = 10;
    paramLowBounds[KD1] = 1e-5; 		paramUpBounds[KD1] = 10;
    paramLowBounds[K12] = 1e-5;		paramUpBounds[K12] = 10;
    paramLowBounds[KD2] = 1e-5;		paramUpBounds[KD2] = 10;
    paramLowBounds[K23] = 1e-5;		paramUpBounds[K23] = 10;
    paramLowBounds[KD3] = 1e-5;		paramUpBounds[KD3] = 10;
    paramLowBounds[KD] = 0.01;			paramUpBounds[KD] = 1e6;
    paramLowBounds[mutKD1] = 0.0001;		paramUpBounds[mutKD1] = 1000;
    paramLowBounds[mutK12] = 0.0001;		paramUpBounds[mutK12] = 1000;
}

void setBaseParameters(){
    background = Back::WT;
    params.clear(); 	         		params.resize(NBPARAM, 0.);     // clear previous values
    params[KF]   = 1e-3;	    		params[KD1]  = 1e-4;
    params[KD2]  = 1e-6;	    		params[KD3]  = 1e-6;
    params[K12]  = 1e-6;	    		params[K23]  = 1e-5;
    params[KD]  = 0.1; 	    		params[mutK1D]  = 1.0;
    params[mutK12]  = 1.0;
    setBaseParametersDone();
}

void initialise(long long _background){
    init.clear();			init.resize(NbVariables, 0.0);
    background = _background;
    if(background == Back::WT){
        init[A]   = 12.;	init[B]   = 2.;		init[C]   = 1.;
    }
    if(background == Back::MUT){
        init[A]   = 12.;	init[B]   = 4.;        init[C]   = 1.;
    }
    for(int i = 0; i < NBVAR; ++i){
    val[i] = init[i];
    }
    t = 0;
    initialiseDone();
}

void derivatives(const vector<double> &x, vector<double> &dxdt, const double t){
    double C1 = (background == Back::WT) ? 1 : mutKD1;	// coefficient to put to KD1 in case of WT or Mut
    double C2 = (background == Back::WT) ? 1 : mutK12;	// coefficient to put to K12 in case of WT or Mut
    dxdt[A] = params[KF] * (1- x[C] / params[KD]) - (params[KD1] * C1 + params[K12] * C2) * x[A];
    dxdt[B] = params[K12] * C2 * x[A] - (params[KD2] + params[K23]) * x[B];
    dxdt[C] = params[K23] * x[B] - params[KD3] * x[C];
}
};
