#include "namesNewProject.h"
#include <iostream>
#include <cmath>
#include <sstream>
using namespace std;

// an example of implementation, using static vectors filled the first time the functions are called.

static vector<string> GNames;

string GlobalName(int id){
    static bool loaded = false;

    GNames.resize(N::NB_GLOB_VARS);
    if(!loaded){
        GNames[N::NoName] = string("NotFound");
        GNames[N::GlobVAR1] = string("Var1");
        GNames[N::GlobVAR2] = string("Var2");
        GNames[N::GlobVAR3] = string("Var2");
    }
    if((id < 0) || (id >= (int) GNames.size())) {
          return string("Not Found");
    }
    else return GNames[id];
}

vector<string> getGlobalNames(){
    GlobalName(0); // to make sure it's loaded
    return GNames;
}

/* can be implemented ...

static vector<long long> backgrounds;

long long getBackgroundNr(int i){
    static bool loaded = false;
    if(!loaded){
        backgrounds.push_back(Back::WT);
        backgrounds.push_back(Back::DEFICIENTA);
        backgrounds.push_back(Back::DEFICIENTB);
        backgrounds.push_back(Back::AGED);
        backgrounds.push_back(Back::DEFICIENTA | Back::AGED);
        backgrounds.push_back(Back::DEFICIENTB | Back::AGED); // you can combine
    }
    if(i < 0)  return backgrounds.size();
    if((i < 0) || (i >= (int) backgrounds.size())){
        cerr << "ERR:  backgrounds(" << i << ") is not defined, only " << backgrounds.size() << " backgrounds" << endl;
        return -1;
    }
    return backgrounds[i];
}

string getBackgroundName(long long background){
    stringstream res;
    if(background & Back::WT)	res << "-WT";
    if(background & Back::DEFICIENTA)	res << "-A-/-";
    if(background & Back::DEFICIENTB)	res << "-B-/-";
    if(background & Back::AGED)	res << "-Aged";
    return res.str();
}*/



/* can also be implemented
 static vector<string> codingNames;

 string CodingName(int id){
    static bool loaded = false;

    if(!loaded){
        codingNames.push_back(string("N::NoName"));
        codingNames.push_back(string("N::GlobVAR1"));
        codingNames.push_back(string("N::GlobVAR2"));
        codingNames.push_back(string("N::GlobVAR3"));
        //...
        loaded = true;
        }
    if((id < 0) || (id >= (int) codingNames.size())) {
          return string("Not Found");
    }
    else return codingNames[id];
}*/

/*vector<string> getCodingNames(){
    CodingName(0); // to make sure it's loaded
    return codingNames;
}*/

