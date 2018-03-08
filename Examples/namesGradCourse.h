#ifndef NAMES_H
#define NAMES_H

#include <vector>
#include <string>
using namespace std;

enum EXP{WT_EXP, MUTANT_EXP, NB_EXP};// experiments

namespace N{
     enum{NoName, Logistic_N, Volterra_Prey, Volterra_Predator, Dvp_P1, Dvp_P2, Dvp_P3, Dvp_Mout, Flu_U, Flu_I,Flu_V,Flu_T, NB_GLOB_VARS};
}

namespace Back {
    enum : long long {WT= 1, MUT=2
//          TCRGATA3NEGKO=2, // different backround than the WT

    };
}

/*long long getBackgroundNr(int i = -1); // if -1, returns the number of backgrounds
string getBackgroundName(long long background);
void testBackgroundSystem();*/


string GlobalName(int id);
//string CodingName(int id);
//string CodingNameExp(int id);
//vector<string> getCodingNames();
vector<string> getGlobalNames();


#endif // NAMES_H
