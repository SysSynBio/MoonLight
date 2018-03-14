#ifndef NAMESNEWPROJ_H
#define NAMESNEWPROJ_H

#include <vector>
#include <string>
using namespace std;

// you can define an enum for each experiment class.
enum EXP_KINETICS {EXP_KIN1, EXP_KIN2, NB_EXP_KIN};
enum EXP_PERTURBATION {EXP_PER1, EXP_PER2, NB_EXP_PER};

// An enum of the 'global variables' to communicate between data and models
namespace N{
     enum{NoName, GlobVAR1, GlobVAR2, GlobVAR3, NB_GLOB_VARS};
     // If possible keep NoName as first, it has interest to mean a variable was not found.
}


// an enum for the backgrounds / options of simulations, in case wanted
namespace Back {
    enum : long long {
        WT= 1,
        DEFICIENTA=2,
        DEFICIENTB=4,
        AGED=8
    };
}

// To read experimental kinetics, good to provide a list of names of global variables (with their position being their ID).
vector<string> getGlobalNames();

// Can be useful: get the name of a global variable from its ID
string GlobalName(int id);

// functions that can be useful to implement
/*long long getBackgroundNr(int i = -1);    // returns the ith background from a predefined list of interest if -1, returns the number of backgrounds
string getBackgroundName(long long background);
void testBackgroundSystem();*/

// if you use the functions from generate.cpp, useful to define these functions,
// so that the generated
//string CodingName(int id);
//string CodingNameExp(int id);
//vector<string> getCodingNames();



#endif // NAMES_H
