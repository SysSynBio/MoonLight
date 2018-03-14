// ------- Automatically generated model -------- //
#include "../../common.h"

#include "5-modeleInfluenza.h"

//enum{initU, initI, initV, initT, beta, delta, p_replicV, c_clearV, ST_basalCD8, rho_activ, deathT, NbParameters};
//enum{U, I, V, T, NbVariables};

modeleInfluenza::modeleInfluenza() : Modele(NbVariables, NbParameters), background(Back::WT) {
    name = string("modeleInfluenza");
    dt = 0.001; // initial time step -> then it is adaptive
    print_all_secs = 1200; //every how many seconds it is plotting
	// Name of variables
    names[U] = string("Uninfected");
    names[I] = string("Infected");
    names[V] = string("Virus Load");
    names[T] = string("CD8 T cells");
	
	// the names of variables that can be accessed by outside (global name-space)
    extNames[U] = N::Flu_U;
    extNames[I] = N::Flu_I;
    extNames[V] = N::Flu_V;
    extNames[T] = N::Flu_T;

    // Name of parameters
    paramNames[initU] = "Init Uninfected cells";
    paramNames[initI] = "Init Infected cells";
    paramNames[initV] = "Init Virus";
    paramNames[initT] = "Init CD8 T cells";
    paramNames[beta] = "Infection rate (beta)";
    paramNames[delta] = "Infected cells clearance (delta)";
    paramNames[p_replicV] = "Viral replication rate (p)";
    paramNames[c_clearV] = "Viral clearance (c)";
    paramNames[ST_basalCD8] = "Basal CD8 production";
    paramNames[rho_activ] = "Activation of CD8 (rho)";
    paramNames[deathT] = "Death of CD8";

  //----------------------------------------------------------------------
    paramLowBounds[initU] = 0.01;
    paramLowBounds[initI]   = 0.01;
    paramLowBounds[initV]     = 0.01;
    paramLowBounds[initT] 	= 0.01;
    paramLowBounds[beta] 	= 1e-5;
    paramLowBounds[delta] 	= 1e-5;
    paramLowBounds[p_replicV] 	= 1e-5;
    paramLowBounds[c_clearV] 	= 1e-5;
    paramLowBounds[ST_basalCD8] 	= 1e-5;
    paramLowBounds[rho_activ] 	= 1e-5;
    paramLowBounds[deathT] 	= 1e-5;
//-------------------------------------------------------------------
    paramUpBounds[initU] = 1e6;
    paramUpBounds[initI]   = 1e6;
    paramUpBounds[initV]     = 1e6;
    paramUpBounds[initT] 	= 1e6;
    paramUpBounds[beta] 	= 1e4;
    paramUpBounds[delta] 	= 1e4;
    paramUpBounds[p_replicV] 	= 1e4;
    paramUpBounds[c_clearV] 	= 1e4;
    paramUpBounds[ST_basalCD8] 	= 1e4;
    paramUpBounds[rho_activ] 	= 1e4;
    paramUpBounds[deathT] 	= 1e4;

    backSimulated.clear();
	backSimulated.push_back(Back::WT);
}

void modeleInfluenza::setBaseParameters(){
    background = Back::WT;
	params.clear();     // to make sure they are all put to zero
    params.resize(NbParameters, 0.0);

    params[initU] = 1e6;
    params[initI]   = 0;
    params[initV]     = 25;
    params[initT] 	= 0;
    params[beta] 	= 1e-3;
    params[delta] 	= 1e-3;
    params[p_replicV] 	= 1e-3;
    params[c_clearV] 	= 1e-3;
    params[ST_basalCD8] 	= 1e-3;
    params[rho_activ] 	= 1e-3;
    params[deathT] 	= 1e-3;

	setBaseParametersDone();
}

void modeleInfluenza::initialise(long long _background){ // don't touch to parameters !
	val.clear();
    val.resize(NbVariables, 0.0);
	init.clear();
    init.resize(NbVariables, 0.0);

    // test if each possible background is in the current combination of backgrounds (stored in the field background)
    if((background != Back::WT) && (background != _background)){
        cerr << "WRN : modeleLeishmania::initialize, when the background has been changed from WT, you should not change it again, because parameters can not be retrieved. Please load parameters again" << endl;
    }
    background = _background;
    if(background == Back::WT){}

    init[U]   = params[initU];
    init[I]   = params[initI];
    init[V]   = params[initV];
    init[T]   = params[initT];


    for(int i = 0; i < NbVariables; ++i){
		val[i] = init[i];}
	t = 0;
	initialiseDone();
}


void modeleInfluenza::derivatives(const vector<double> &x, vector<double> &dxdt, const double t){

    if(!over(U))
        dxdt[U] 	= - params[beta] * x[U] * x[V];

    if(!over(I))
        dxdt[I] 	= params[beta] * x[U] * x[V] - params[delta] * x[I] * x[T];

    if(!over(V))
        dxdt[V] 	= params[p_replicV]*x[I] - params[c_clearV]*x[V];

    if(!over(T))
        dxdt[T] 	= params[ST_basalCD8] + params[rho_activ]*x[T]*x[V] - params[delta]*x[T];
}

