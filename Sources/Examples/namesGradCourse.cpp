#include "namesGradCourse.h"
#include <iostream>
#include <cmath>
#include <sstream>
using namespace std;

static vector<string> GNames;
static vector<string> codingNames;
static vector<string> expCodingNames;
static vector<long long> backgrounds;

long long getBackgroundNr(int i){
    static bool loaded = false;
    if(!loaded){
        backgrounds.push_back(Back::WT);
        backgrounds.push_back(Back::MUT);
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
    if(background & Back::MUT)	res << "-MUTANT";

    return res.str();
}


/*void testBackgroundSystem(){
    long a1 = pow(2,10);
    long b1 = pow(2,5);
    long c1 = a1 | b1;  // adding backgrounds
    cerr << a1 << "," << b1 << "," << (a1 | b1) << endl;
    // recovering backgrounds
    cerr << (c1 & ((long) pow(2,1))) << "\t";
    cerr << (c1 & ((long) pow(2,2))) << "\t";
    cerr << (c1 & ((long) pow(2,3))) << "\t";
    cerr << (c1 & ((long) pow(2,4))) << "\t";
    cerr << (c1 & ((long) pow(2,5))) << "\t";
    cerr << (c1 & ((long) pow(2,6))) << "\t";
    cerr << (c1 & ((long) pow(2,7))) << "\t";
    cerr << (c1 & ((long) pow(2,8))) << "\t";
    cerr << (c1 & ((long) pow(2,9))) << "\t";
    cerr << (c1 & ((long) pow(2,10))) << "\t";
}*/


string CodingNameExp(int id){
    static bool loaded = false;
    if(!loaded){
        expCodingNames.push_back(string("WT"));
        expCodingNames.push_back(string("Mutant"));
    }
    if((id < 0) || (id >= (int)  expCodingNames.size())) {
          return string("Not Found");
    }
    else return expCodingNames[id];
}


string GlobalName(int id){
    static bool loaded = false;

    if(!loaded){
        GNames.push_back(string("NoName"));
        GNames.push_back(string("Logistic_N"));
        GNames.push_back(string("Volterra_Prey"));
        GNames.push_back(string("Volterra_Predator"));
        GNames.push_back(string("Progenitor_1"));
        GNames.push_back(string("Progenitor_2"));
        GNames.push_back(string("MatureCells_P3"));
        GNames.push_back(string("CumulatedOutput"));
        GNames.push_back(string("Flu_UninfectedCells"));
        GNames.push_back(string("Flu_InfectedCells"));
        GNames.push_back(string("Flu_VirusLoad"));
        GNames.push_back(string("Flu_C8_Titers"));

    }
    if((id < 0) || (id >= (int) GNames.size())) {
          return string("Not Found");
    }
    else return GNames[id];
}

/*string CodingName(int id){
    static bool loaded = false;

    if(!loaded){
        codingNames.push_back(string("N::NONAME"));
        codingNames.push_back(string("N::IL12"));
        codingNames.push_back(string("N::STAT4"));
        codingNames.push_back(string("N::STAT4P"));
        codingNames.push_back(string("N::TBET"));
        codingNames.push_back(string("N::IFNG"));
        codingNames.push_back(string("N::IL12RB2"));
        codingNames.push_back(string("N::RL12"));
        codingNames.push_back(string("N::RIFN"));
        codingNames.push_back(string("N::IFNGR"));
        codingNames.push_back(string("N::STAT1"));
        codingNames.push_back(string("N::STAT1P"));
        codingNames.push_back(string("N::IL4"));
        codingNames.push_back(string("N::IL4R"));
        codingNames.push_back(string("N::RL4"));
        codingNames.push_back(string("N::IL13"));
        codingNames.push_back(string("N::IL13R"));
        codingNames.push_back(string("N::RL13"));
        codingNames.push_back(string("N::IL5"));
        codingNames.push_back(string("N::STAT6"));
        codingNames.push_back(string("N::STAT6P"));
        codingNames.push_back(string("N::GATA3"));
        codingNames.push_back(string("N::GATA3TBET"));
        codingNames.push_back(string("N::IL2"));
        codingNames.push_back(string("N::IL2RA"));
        codingNames.push_back(string("N::RL2"));
        codingNames.push_back(string("N::STAT5"));
        codingNames.push_back(string("N::STAT5P"));
        codingNames.push_back(string("N::TGFB"));
        codingNames.push_back(string("N::TGFBR"));
        codingNames.push_back(string("N::RTGFB"));
        codingNames.push_back(string("N::SMAD23"));
        codingNames.push_back(string("N::SMAD23P"));
        codingNames.push_back(string("N::FOXP3"));
        codingNames.push_back(string("N::FOXP3GATA3"));
        codingNames.push_back(string("N::FOXP3RORGT"));
        codingNames.push_back(string("N::IL6"));
        codingNames.push_back(string("N::IL6R"));
        codingNames.push_back(string("N::RL6"));
        codingNames.push_back(string("N::IL21"));
        codingNames.push_back(string("N::IL21R"));
        codingNames.push_back(string("N::RL21"));
        codingNames.push_back(string("N::IL10"));
        codingNames.push_back(string("N::IL10R"));
        codingNames.push_back(string("N::RL10"));
        codingNames.push_back(string("N::STAT3"));
        codingNames.push_back(string("N::STAT3P"));
        codingNames.push_back(string("N::STAT3SERP"));
        codingNames.push_back(string("N::RORGT"));
        codingNames.push_back(string("N::IL17"));
        codingNames.push_back(string("N::IL22"));
        codingNames.push_back(string("N::IL17F"));
        codingNames.push_back(string("N::SOCS1"));
        codingNames.push_back(string("N::SOCS2"));
        codingNames.push_back(string("N::SOCS3"));
        codingNames.push_back(string("N::SMAD7"));
        codingNames.push_back(string("N::TCR"));
        codingNames.push_back(string("N::STAT4mRNA"));
        codingNames.push_back(string("N::TBETmRNA"));
        codingNames.push_back(string("N::IFNGmRNA"));
        codingNames.push_back(string("N::IL12RB2mRNA"));
        codingNames.push_back(string("N::IFNGRmRNA"));
        codingNames.push_back(string("N::STAT1mRNA"));
        codingNames.push_back(string("N::IL4mRNA"));
        codingNames.push_back(string("N::IL4RmRNA"));
        codingNames.push_back(string("N::IL13mRNA"));
        codingNames.push_back(string("N::IL13RmRNA"));
        codingNames.push_back(string("N::IL5mRNA"));
        codingNames.push_back(string("N::STAT6mRNA"));
        codingNames.push_back(string("N::GATA3mRNA"));
        codingNames.push_back(string("N::IL2mRNA"));
        codingNames.push_back(string("N::IL2RAmRNA"));
        codingNames.push_back(string("N::STAT5mRNA"));
        codingNames.push_back(string("N::TGFBmRNA"));
        codingNames.push_back(string("N::TGFBRmRNA"));
        codingNames.push_back(string("N::SMAD23mRNA"));
        codingNames.push_back(string("N::FOXP3mRNA"));
        codingNames.push_back(string("N::IL6mRNA"));
        codingNames.push_back(string("N::IL6RmRNA"));
        codingNames.push_back(string("N::IL21mRNA"));
        codingNames.push_back(string("N::IL21RmRNA"));
        codingNames.push_back(string("N::IL10mRNA"));
        codingNames.push_back(string("N::IL10RmRNA"));
        codingNames.push_back(string("N::STAT3mRNA"));
        codingNames.push_back(string("N::RORGTmRNA"));
        codingNames.push_back(string("N::IL17mRNA"));
        codingNames.push_back(string("N::IL22mRNA"));
        codingNames.push_back(string("N::IL17FmRNA"));
        codingNames.push_back(string("N::antiIL4"));
        codingNames.push_back(string("N::antiIFNg"));
        codingNames.push_back(string("N::antiIL2"));
        codingNames.push_back(string("N::openIL2"));
        codingNames.push_back(string("N::openIL21"));
        codingNames.push_back(string("N::openFOXP3"));
        codingNames.push_back(string("N::openRORGT"));
        codingNames.push_back(string("N::openTBET"));
        codingNames.push_back(string("N::transl"));
        codingNames.push_back(string("N::secret"));
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

vector<string> getGlobalNames(){
    GlobalName(0); // to make sure it's loaded
    return GNames;
}
