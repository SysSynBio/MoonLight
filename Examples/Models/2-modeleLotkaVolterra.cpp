// ------- Automatically generated model -------- //
#include "../../common.h"

#include "2-modeleLotkaVolterra.h"
//modeleLotkaVolterra();
//enum{InitPrey, InitPredator, alpha_repro_prey, beta_prey_capture_rate, gamma_dead_predator, delta_food_repro_predator, NbParameters};
//enum{Prey, Predator, NbVariables};

modeleLotkaVolterra::modeleLotkaVolterra() : Modele(NbVariables, NbParameters), background(Back::WT) {
    name = string("Modele Lotka Volterra");
    dt = 0.001; // initial time step -> then it is adaptive
    print_all_secs = 1200; //every how many seconds it is plotting
    // Name of variables
    names[Prey] = string("Prey");
     names[Predator] = string("Predator");

    // the names of variables that can be accessed by outside (global name-space)
    extNames[Prey] = N::Volterra_Prey;
    extNames[Predator] = N::Volterra_Predator;

    // Name of parameters

    paramNames[InitPrey] = "Prey Initial Number";
    paramNames[InitPredator] = "Predator Initial Value";
    paramNames[alpha_repro_prey] = "Repro prey (alpha)";
    paramNames[beta_prey_capture_rate] = "Capture prey (beta)";
    paramNames[gamma_dead_predator] = "Dead predator (gamma)";
    paramNames[delta_food_repro_predator] = "Food-dependent Repro Predator (delta)";
    //----------------------------------------------------------------------
    paramLowBounds[InitPrey] = 10;
    paramLowBounds[InitPredator] = 10;
    paramLowBounds[alpha_repro_prey] = 1e-8;
    paramLowBounds[beta_prey_capture_rate] = 1e-8;
    paramLowBounds[gamma_dead_predator] = 1e-6;
    paramLowBounds[delta_food_repro_predator]= 1e-8;
    //-------------------------------------------------------------------
    paramUpBounds[InitPrey] = 1e6;
    paramUpBounds[InitPredator] = 1e6;
    paramUpBounds[alpha_repro_prey] = 1e6;
    paramUpBounds[beta_prey_capture_rate] = 1e6;
    paramUpBounds[gamma_dead_predator] = 1e6;
    paramUpBounds[delta_food_repro_predator] = 1e6;

    backSimulated.clear();
    backSimulated.push_back(Back::WT);
}

void modeleLotkaVolterra::setBaseParameters(){
    background = Back::WT;
    params.clear();     // to make sure they are all put to zero
    params.resize(NbParameters, 0.0);

    params[InitPrey] = 200;
    params[InitPredator] = 20;
    params[alpha_repro_prey] = 0.001;
    params[beta_prey_capture_rate] = 0.01;
    params[gamma_dead_predator] = 0.1;
    params[delta_food_repro_predator] = 0.1;

    setBaseParametersDone();
}

void modeleLotkaVolterra::initialise(long long _background){ // don't touch to parameters !
    val.clear();
    val.resize(NbVariables, 0.0);
    init.clear();
    init.resize(NbVariables, 0.0);

    // test if each possible background is in the current combination of backgrounds (stored in the field background)
    if((background != Back::WT) && (background != _background)){
        cerr << "WRN :modeleLogistic::initialize, when the background has been changed from WT, you should not change it again, because parameters can not be retrieved. Please load parameters again" << endl;
    }
    background = _background;
    if(background == Back::WT){}
    init[Prey]   = params[InitPrey];
    init[Predator]   = params[InitPredator];

    for(int i = 0; i < NbVariables; ++i){
        val[i] = init[i];}
    t = 0;
    initialiseDone();
}

void modeleLotkaVolterra::derivatives(const vector<double> &x, vector<double> &dxdt, const double t){

    if(!over(Prey)) {// if it is not overrided by data then make the computaiton below
        dxdt[Prey] 	= x[Prey] *  ( params[alpha_repro_prey] - params[beta_prey_capture_rate] * x[Predator]);
    }
    if(!over(Predator)) {// if it is not overrided by data then make the computaiton below
        dxdt[Predator] 	= x[Predator] *  ( -1.0 * params[gamma_dead_predator] + params[delta_food_repro_predator] * x[Prey]);
    }

}
