// ------- Automatically generated model -------- //
#include "../../common.h"

#include "1-modeleLogistic.h"


modeleLogistic::modeleLogistic() : Modele(NbVariables, NbParameters), background(Back::WT) {
    name = string("Logistic Growth");
    dt = 0.001; // initial time step -> then it is adaptive
    print_all_secs = 1200; //every how many seconds it is plotting
    // Name of variables
    names[NPop] = string("Fish Population");

    // the names of variables that can be accessed by outside (global name-space)
    extNames[NPop] = N::Logistic_N;

    // Name of parameters
    paramNames[InitPop] = "Initial value";
    paramNames[R_growth] = "r (population growth rate)";
    paramNames[K_carrying_capacity] 	= "K (carrying capacity)";
    //----------------------------------------------------------------------
    paramLowBounds[InitPop] = 0.001;
    paramLowBounds[R_growth] = 1e-8;
    paramLowBounds[K_carrying_capacity] = 0.01;
    //-------------------------------------------------------------------
    paramUpBounds[InitPop]              = 1e6;
    paramUpBounds[R_growth]             = 1e5;
    paramUpBounds[K_carrying_capacity] 	= 1e8;

    backSimulated.clear();
    backSimulated.push_back(Back::WT);
}

void modeleLogistic::setBaseParameters(){
    background = Back::WT;
    params.clear();     // to make sure they are all put to zero
    params.resize(NbParameters, 0.0);

    params[InitPop] = 10;
    params[R_growth] = 0.001;
    params[K_carrying_capacity]   = 1000;

    setBaseParametersDone();
}

void modeleLogistic::initialise(long long _background){ // don't touch to parameters !
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
    init[NPop]   = params[InitPop];

    for(int i = 0; i < NbVariables; ++i){
        val[i] = init[i];}
    t = 0;
    initialiseDone();
}

void modeleLogistic::derivatives(const vector<double> &x, vector<double> &dxdt, const double t){

    if(!over(NPop)) {// if it is not overrided by data then make the computaiton below
        dxdt[NPop] 	= params[R_growth] * x[NPop] * (1 - (x[NPop] / (params[K_carrying_capacity]) + 1e-8));
    }
}
