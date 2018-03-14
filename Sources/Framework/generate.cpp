#include "generate.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm> // for sorting

#define NEW












/// ================================= 1 - GRAPHES ========================================

void printGrVect(vector<Graphe *>);
void testeGraphManip(){
    enum {A,B,C,D,E};
    Graphe a(5);
    a.add(A,B,+1);
    a.add(A,D,-1);
    a.add(B,D,+1);
    a.add(D,B,+1);
    a.add(B,E,-1);
    a.add(D,C,+1);
    a.add(C,E,+1);
    a.add(E,C,+1);
    a.add(C,A,-1);
    a.add(E,D,-1);
    Graphe b(5);
    b.add(A,B,+1);
    b.add(A,D,-1);
    cerr << "---------- Starting Graph ------------\n";
    a.print();
    cerr << "---------- Subgraphes with size 4 ------------\n";
    printGrVect(between(&a, &b, 4));
    cerr << "---------- Subgraphes with all sizes ------------\n";
    printGrVect(between(&a, &b));
}



    Graphe::Graphe(int _nbNodes) : nbNodes(_nbNodes), nbVertices(0) {
         table.clear();
         table.resize(nbNodes);
         for(int i = 0; i < nbNodes; ++i){
            table[i].resize(nbNodes, (valeur) 0);
         }
    }

    Graphe::Graphe(Graphe* toCopy) : nbNodes(toCopy->size()), nbVertices(0) {
         table.clear();
         table.resize(nbNodes);
         for(int i = 0; i < nbNodes; ++i){
            table[i].resize(nbNodes, (valeur) 0);
            for(int j = 0; j < nbNodes; ++j){
                //table[i][j] = (*toCopy)[i][j];
                add(i,j,(*toCopy)(i,j));
            }
         }
    }

    void Graphe::resize(int newNbNodes) {
        nbNodes = newNbNodes;
        nbVertices = 0;
        table.clear();
        table.resize(nbNodes);
        for(int i = 0; i < nbNodes; ++i){
           table[i].resize(nbNodes, (valeur) 0);
        }
    }
    void Graphe::add(int i, int j, valeur value){
         if((i < 0) || (j < 0) || (i >= nbNodes) || (j >= nbNodes)) {cerr << "add : wrong indices"; return;}
         if((table[i][j] == 0) && (value != 0)) nbVertices++;
         if((table[i][j] != 0) && (value == 0)) nbVertices--;
         if(nbVertices < 0) cerr << "ERR: graphe::add, problem counting the number of non-zero vertices, get negative number" << endl;
         table[i][j] = value;
    }
    //    int nbNodes;
    int Graphe::size(){return nbNodes;}
    int Graphe::inters(){return nbVertices;}
    vector<int> Graphe::operator()(int i){  // successors
        vector<int> z;
        z.clear();
        if((i<0) || (i>nbNodes)) return z;
        for(int j = 0; j < nbNodes; ++j){
            if(table[i][j] != (valeur) 0) z.push_back(j);
        }
        return z;
    }
    valeur Graphe::operator()(int i, int j){
        if((i < 0) || (j < 0) || (i >= nbNodes) || (j >= nbNodes)) return 0;
        else return table[i][j];
    }
    vector<int> Graphe::pred(int i){    // predecessors
        vector<int> z;
        z.clear();
        if((i<0) || (i>nbNodes)) return z;
        for(int j = 0; j < nbNodes; ++j){
            if(table[j][i] != (valeur) 0) z.push_back(j);
        }
        return z;
    }

    //string generateGraph();
    void Graphe::print(){
        cerr << "--------- Graphe, size= " << nbNodes << " ; nbInteractions= " << nbVertices << " --------" << endl;
        for(int i = 0; i < nbNodes; ++i){
            for(int j = 0; j < nbNodes; ++j){
                cerr << table[i][j] << "\t";
            }
            cerr << endl;
        }
    }


    bool compInters(Graphe*a, Graphe*b) {return a->inters() < b->inters();}     // function to sort graphs based on the number of interactions.

    vector<Graphe*> between(Graphe Gbig, Graphe Gsmall, int new_size){ // very important that this is not a referenece or a ppinter, because will be modified !

        // removes Gsmall from Gbig and checks size
        vector<Graphe*> vres;
        vres.clear();

        int s = Gbig.size();
        int sS = Gsmall.size();
        if(s != sS) {cerr << "ERR:Graphe::between(), the two graphs don't share the same size ! " << s << " != " << sS << endl;
            return vres;}
        /*if(new_size < 0) return vres;
        if(new_size == 0) {
            Graphe* gg = new Graphe(G->size());
            vres.push_back(gg);
            return vres;
        }*/

        for(int i = 0; i < s; ++i){
            for(int j = 0; j < s; ++j){
                if((Gsmall(i,j) != 0) && (Gbig(i,j) != 0)){
                    if(Gsmall(i,j) != Gbig(i,j)) cerr << "WRN:Graphe::between, the small graph has vertices in common with the big one and they don't have the same value !! Gsmall value is taking over.";
                    Gbig.add(i,j,0);
                }
            }
        }

        Graphe* Start = new Graphe(&Gsmall);
        vres.push_back(Start);
        //for(int K = 0; K < new_size; ++K){

            for(int i = 0; i < s; ++i){
                for(int j = 0; j < s; ++j){
                    if(Gbig(i,j) != 0){
                        int currS = vres.size();
                        for(int m = 0; m < currS; ++m){
                            Graphe* extended = new Graphe(vres[m]);
                            extended->add(i,j,Gbig(i,j));
                            vres.push_back(extended);
                        }
                    }
                }
            }
        //}
        // manual sorting ...
        sort (vres.begin(), vres.end(), compInters);

        if(new_size != 0){
            vector<Graphe*> vtemp = vres;
            int st = vtemp.size();
            vres.clear();
            for(int i = 0; i < st; ++i){
                if(vtemp[i]->inters() == new_size) vres.push_back(vtemp[i]);
            }
        }
        return vres;
    }


    void printGrVect(vector<Graphe*> v){
        int s = v.size();
        for(int i = 0; i < s; ++i){
            v[i]->print();
        }
    }
















/// ================================= 2 - Basic definitions of Generate Class ========================================

Generate::Generate(string _MName) : MName(_MName), nbVar(0) {
    VarEnum.clear();
    GlobVarEnum.clear();
    Degradation.clear();
    Basal.clear();
    filesToInclude.clear();
    Types.clear();
}

void Generate::addVar(string nameVar, D degPolicy, B basalPolicy, string globalName, int typeVar){
    nbVar++;
    VarEnum.push_back(nameVar);
    GlobVarEnum.push_back(globalName);
    Degradation.push_back(degPolicy);
    Basal.push_back(basalPolicy);
    Types.push_back(typeVar);
    g.resize(nbVar);
}
void Generate::addReaction(string Var1, string Var2, int IDfunction){
    int ID1 = -1, ID2 = -1;
    for(int i = 0; i < nbVar; ++i){
        if(!Var1.compare(VarEnum[i])) ID1 = i;
        if(!Var2.compare(VarEnum[i])) ID2 = i;
    }
    g.add(ID1, ID2, IDfunction);
}

void Generate::addInclude(string fileName){
    filesToInclude.push_back(fileName);
}

void Generate::setBorderPolicy(vector<typeVar> policy){
    ParamBordersPolicy = policy;
}





//$ enum {HILL_ACTIV, HILL_INHIB, BLOCKING, EXTERNAL

// pour blocking,
// deux choix : mettre dt = 0, ou dégradation relative á block /
// pour external
//-> idée : addExternalFunction(Variable, where, string name, vector parameters, vectorType);
// et ajoutera les paramètres à la liste







/// ================================= Generate the LIST of PARAMETERS ========================================

vector<string> Generate::generateParameters(){
    vector<string> res;
    res.clear();
    // Degradation parameters;
    for(int i = 0; i < nbVar; ++i){
        if(Degradation[i] == PROP_DEG) res.push_back(string("KD") + VarEnum[i]);
    }
#ifdef OLD
    // Basal parameters, in case they are not automatically set
    for(int i = 0; i < nbVar; ++i){
        if((Basal[i] == CST)) res.push_back(string("B") + VarEnum[i]);
    }

    for(int i = 0; i < nbVar; ++i){
        if(Basal[i] == CST_FROM_EQ) res.push_back(VarEnum[i] + string("EQ"));
    }
#endif


    for(int i = 0; i < nbVar; ++i){
        if(Basal[i] == CST_FROM_EQ) res.push_back(VarEnum[i] + string("EQ"));
    }
    // for each receiving node, Cst translation * big formula
    if(g.size() != nbVar) cerr << "ERR::GenerateParameters, Graph size is incorrect (" << g.size() << ") instead of nbVars = " << nbVar << endl;
    for(int i = 0; i < nbVar; ++i){
        vector<int> vpred = g.pred(i);
        int nbAct = vpred.size();
        if(nbAct > 0){
            res.push_back(string("C") + VarEnum[i]);
            for(int j = 0; j < nbAct; ++j){
#ifdef NEW
                //for(int i = 0; i < nbVar; ++i){
                    if((Basal[i] == CST) || (Basal[i] == CST_FROM_EQ)) res.push_back(string("S") + VarEnum[vpred[j]] + string("_TO_") + VarEnum[i]);
                //}
#endif
                res.push_back(string("K") + VarEnum[vpred[j]] + string("_TO_") + VarEnum[i]);
                res.push_back(string("N") + VarEnum[vpred[j]] + string("_TO_") + VarEnum[i]);
            }
        }
        if((nbAct == 0) && (Basal[i] == CST_FROM_EQ)){
            res.push_back(string("P") + VarEnum[i]);
        }
    }

#ifdef OLD
    // Basal parameters, in case they are automatically set
    for(int i = 0; i < nbVar; ++i){
        if((Basal[i] == CST_FROM_EQ)) res.push_back(string("B") + VarEnum[i]);
    }
#endif
    return res;
}

















/// ================================= Generate PARAMETER BOUNDARIES ========================================

// function to access a vector index, but if out of range, returns the constructor,
// in case of border policy, if it doesn't exist, avoids a seg fault and returns the basal policy
template <typename T>
T fr(vector<T> v, int index){
    T a = T();  // carefull, because with numbers this will be weird ... don't use T=int ...
    if ((index < 0) || (index > (int) v.size())) return a; else return v[index];
}

string Generate::generateParamBorders(){
    stringstream f;

    // Degradation parameters;
    for(int i = 0; i < nbVar; ++i){
        if(Degradation[i] == PROP_DEG) {
            f << "\tparamLowBounds[" << "KD" << VarEnum[i] << "] \t= " << fr(ParamBordersPolicy, Types[i]).Deg_min << ";\t";
            f << "\tparamUpBounds[" << "KD" << VarEnum[i] << "] \t= " <<  fr(ParamBordersPolicy, Types[i]).Deg_max << ";\n";
        }
    }
#ifdef OLD
    for(int i = 0; i < nbVar; ++i){
        if(Basal[i] == CST) {    // careful to keep the same order than generate parameters !!
            f << "\tparamLowBounds[" << "B" << VarEnum[i] << "] \t= " << fr(ParamBordersPolicy, Types[i]).Basal_Cst_min << ";\t";
            f << "\tparamUpBounds[" << "B" << VarEnum[i] << "] \t= " <<  fr(ParamBordersPolicy, Types[i]).Basal_Cst_max << ";\n";
        }
    }
#endif
    for(int i = 0; i < nbVar; ++i){
        if(Basal[i] == CST_FROM_EQ) {
            f << "\tparamLowBounds[" << VarEnum[i] << "EQ" << "] \t= " << fr(ParamBordersPolicy, Types[i]).ValueEQ_min << ";\t";
            f << "\tparamUpBounds[" << VarEnum[i] << "EQ" << "] \t= " <<  fr(ParamBordersPolicy, Types[i]).ValueEQ_max << ";\n";
        }
    }

    // for each receiving node, Cst translation * big formula
    if(g.size() != nbVar) cerr << "ERR::GenerateParameters, Graph size is incorrect (" << g.size() << ") instead of nbVars = " << nbVar << endl;
    for(int i = 0; i < nbVar; ++i){
        vector<int> vpred = g.pred(i);
        int nbAct = vpred.size();
        if(nbAct > 0){
            f << "\tparamLowBounds[" << "C" << VarEnum[i] << "] \t= " << fr(ParamBordersPolicy, Types[i]).Trans_min << ";\t";
            f << "\tparamUpBounds[" << "C" << VarEnum[i] << "] \t= " <<  fr(ParamBordersPolicy, Types[i]).Trans_max << ";\n";
            for(int j = 0; j < nbAct; ++j){
#ifdef NEW
                if((Basal[i] == CST) || (Basal[i] == CST_FROM_EQ)) {    // careful to keep the same order than generate parameters !!
                    if(g(vpred[j],i) > 0){  // activation
                        f << "\tparamLowBounds[" << "S" << VarEnum[vpred[j]]  + string("_TO_") + VarEnum[i] << "] \t= " << 1.1 << ";\t";
                        f << "\tparamUpBounds["  << "S" << VarEnum[vpred[j]]  + string("_TO_") + VarEnum[i] << "] \t= " <<  fr(ParamBordersPolicy, Types[i]).Basal_Cst_max << ";\n";
                    } else if(g(vpred[j],i) < 0) {
                        f << "\tparamLowBounds[" << "S" << VarEnum[vpred[j]]  + string("_TO_") + VarEnum[i] << "] \t= " << fr(ParamBordersPolicy, Types[i]).Basal_Cst_min << ";\t";
                        f << "\tparamUpBounds["  << "S" << VarEnum[vpred[j]]  + string("_TO_") + VarEnum[i] << "] \t= " <<  0.9 << ";\n";
                    }
                }
#endif
                f << "\tparamLowBounds[" << string("K") + VarEnum[vpred[j]] + string("_TO_") + VarEnum[i] << "] \t= " << fr(ParamBordersPolicy, Types[i]).Khill_min << ";\t";
                f << "\tparamUpBounds["  << string("K") + VarEnum[vpred[j]] + string("_TO_") + VarEnum[i] << "] \t= " <<  fr(ParamBordersPolicy, Types[i]).Khill_max << ";\n";
                f << "\tparamLowBounds[" << string("N") + VarEnum[vpred[j]] + string("_TO_") + VarEnum[i] << "] \t= " << fr(ParamBordersPolicy, Types[i]).Nhill_min << ";\t";
                f << "\tparamUpBounds["  << string("N") + VarEnum[vpred[j]] + string("_TO_") + VarEnum[i] << "] \t= " <<  fr(ParamBordersPolicy, Types[i]).Nhill_max << ";\n";
            }
        }
    }

    // parameters BfromEq omitted because they are automatically computed.
    return f.str();
}








/// ================================= Generate ACTIVATION FUNCTIONS ========================================

#ifdef OLD
string Generate::generateActivFunction(int nbAct, int nbInh){
        std::stringstream ss;
        if((nbAct < 0) || (nbInh < 0) || (nbAct > 100) || (nbInh > 100)) {
            cerr << "ERR: generateActiv, Out of bounds" << endl;
            return ss.str();
        }
        if(nbAct + nbInh == 0) return ss.str(); // no point in Activ0Inhib0 ...
        // Implicitely, the first ones are activating, the last ones inhibiting

        ss << "\tdouble Activ" << nbAct << "Inhib" << nbInh << "(";
        int i = nbAct + nbInh;
        for(int j = 1; j <= i-1; ++j ){
            ss << "double Val" << j << ", double K" << j << ", double N" << j << ",";
        }
        ss << "double Val" << i << ", double K" << i << ", double N" << i << "){\n";
        ss << "\t\tif(";
        for(int j = 1; j <= i-1; ++j){
            ss << "(Val" << j <<" <= 0.0) || ";
        }
        ss << "(Val" << i <<" <= 0.0)) return 0.0; \n\t\telse return (";

#ifdef USE_MULTI_HILL
        for(int j = 1; j <= nbAct; ++j){
            ss << "pow(Val" << j << ", N" << j <<") / pow(K" << j << ", N" << j << ")";
            if(j != nbAct) ss << " + ";
        }
        if(nbAct == 0) ss << "1";
        ss << ") / ( 1 + ";
        for(int j = 1; j <= i-1; ++j){
            ss << "pow(Val" << j << ", N" << j <<") / pow(K" << j << ", N" << j << ") + ";
        }
        ss << "pow(Val" << i << ", N" << i <<") / pow(K" << i << ", N" << i << ")";
#else
        for(int j = 1; j <= nbAct; ++j){
            ss << "pow(Val" << j << ", N" << j <<") / ( pow(K" << j << ", N" << j << ") + pow(Val" << j << ", N" << j <<") ) *";
        }
        if(nbInh == 0) ss << "1";
        for(int j = nbAct+1; j <= i; ++j){
            ss << "pow(K" << j << ", N" << j << ") / ( pow(K" << j << ", N" << j << ") + pow(Val" << j << ", N" << j <<") )";
            if(j != i) ss << " * ";
        }

#endif
        ss << "); }\n";
        return ss.str();
}
#endif

#ifdef NEW
string Generate::generateActivFunction(int nbAct, int nbInh){
        std::stringstream ss;
        if((nbAct < 0) || (nbInh < 0) || (nbAct > 100) || (nbInh > 100)) {
            cerr << "ERR: generateActiv, Out of bounds" << endl;
            return ss.str();
        }
        if(nbAct + nbInh == 0) return ss.str(); // no point in Activ0Inhib0 ...
        // Implicitely, the first ones are activating, the last ones inhibiting

        ss << "\tdouble Activ" << nbAct << "Inhib" << nbInh << "(";
        int i = nbAct + nbInh;
        for(int j = 1; j <= i-1; ++j ){
            ss << "double Val" << j << ", double K" << j << ", double N" << j << ", double S" << j <<",";
        }
        ss << "double Val" << i << ", double K" << i << ", double N" << i << ", double S" << i << "){\n";
        ss << "\t\tif(";
        for(int j = 1; j <= i-1; ++j){
            ss << "(Val" << j <<" <= 0.0) || ";
        }
        ss << "(Val" << i <<" <= 0.0)) return 0.0; \n\t\telse return (";

        for(int j = 1; j <= nbAct; ++j){
            ss << "(1.0 + (S" << j << " - 1.0) * (pow(Val" << j << ", N" << j <<") / ( pow(K" << j << ", N" << j << ") + pow(Val" << j << ", N" << j <<") ) ) )*";
        }
        if(nbInh == 0) ss << "1";
        for(int j = nbAct+1; j <= i; ++j){
            ss << "(S" << j << " + (1.0 - S" << j << ") * ( pow(K" << j << ", N" << j << ") / ( pow(K" << j << ", N" << j << ") + pow(Val" << j << ", N" << j <<") ) ) )";
            if(j != i) ss << " * ";
        }

        ss << "); }\n";
        return ss.str();
}
#endif

string generateActivFunction(int nbAct, int nbInh){
        std::stringstream ss;
        if((nbAct < 0) || (nbInh < 0) || (nbAct > 100) || (nbInh > 100)) {
            cerr << "ERR: generateActiv, Out of bounds" << endl;
            return ss.str();
        }
        if(nbAct + nbInh == 0) return ss.str(); // no point in Activ0Inhib0 ...
        // Implicitely, the first ones are activating, the last ones inhibiting

        ss << "\tdouble Activ" << nbAct << "Inhib" << nbInh << "(";
        int i = nbAct + nbInh;
        for(int j = 1; j <= i-1; ++j ){
            ss << "double Val" << j << ", double K" << j << ", double N" << j << ", double S" << j <<",";
        }
        ss << "double Val" << i << ", double K" << i << ", double N" << i << ", double S" << i << "){\n";
        ss << "\t\tif(";
        for(int j = 1; j <= i-1; ++j){
            ss << "(Val" << j <<" <= 0.0) || ";
        }
        ss << "(Val" << i <<" <= 0.0)) return 0.0; \n\t\telse return (";

        for(int j = 1; j <= nbAct; ++j){
            ss << "(1.0 + (S" << j << " - 1.0) * (pow(Val" << j << ", N" << j <<") / ( pow(K" << j << ", N" << j << ") + pow(Val" << j << ", N" << j <<") ) ) )*";
        }
        if(nbInh == 0) ss << "1";
        for(int j = nbAct+1; j <= i; ++j){
            ss << "(S" << j << " + (1.0 - S" << j << ") * ( pow(K" << j << ", N" << j << ") / ( pow(K" << j << ", N" << j << ") + pow(Val" << j << ", N" << j <<") ) ) )";
            if(j != i) ss << " * ";
        }

        ss << "); }\n";
        return ss.str();
}



#ifdef OLD
string latexHillAct(string X, string K, string N){
    stringstream ss;
    ss << "\\left(\\frac{" << X << "^{" << N << "}}{" << K << "^{" << N << "} + " << X << "^{" << N << "}} \\right)";
    return ss.str();
}

string latexHillInh(string X, string K, string N){
    stringstream ss;
    ss << "\\left(\\frac{" << K << "^{" << N << "}}{" << K << "^{" << N << "} + " << X << "^{" << N << "}} \\right)";
    return ss.str();
}
#endif

#ifdef NEW
string latexHillAct(string X, string K, string N, string S){
    stringstream ss;
    ss << "\\left(1 + (1 - " << S << ").\\frac{" << X << "^{" << N << "} }{" << K << "^{" << N << "} + " << X << "^{" << N << "} } \\right)";
    return ss.str();
}

string latexHillInh(string X, string K, string N, string S){
    stringstream ss;
    ss << "\\left(" << S << " + (1 - " << S << "). \\frac{" << K << "^{" << N << "} }{" << K << "^{" << N << "} + " << X << "^{" << N << "} } \\right)";
    return ss.str();
}
#endif

















/// ================================= Generate MODEL HEADER ========================================

string Generate::generateCodeHeader(){

    if((int) filesToInclude.size() == 0) {cerr << "WRN: Generate::generateCodeHeader(), you didn't give any files to include. You might probably want to provide a file with namespaces ... \n";}
    stringstream f;
    f << "// ------- Automatically generated model -------- //\n";
    f << "#ifndef MODELE" << MName <<"_H\n";
    f << "#define MODELE" << MName <<"_H\n";
    f << "#include \"../common.h\"\n\n";
    f << "#include \"../Framework/modele.h\"\n\n";
    f << "#ifdef COMPILE_AUTOGEN\n";
    for(int i = 0; i < (int) filesToInclude.size(); ++i){
        if((int) filesToInclude[i].size() > 0){
            if(filesToInclude[i][0] == '<')
                f << "#include " << filesToInclude[i] << "\n\n";
            else
                f << "#include \"" << filesToInclude[i] << "\"\n\n";
        }
    }

    f << "/* Automatically generated modele */ \n";
    f << "struct Modele" << MName << " : public Modele {\n";
    f << "\tModele" << MName << "();\n";
    f << "\tenum {";
        if(nbVar < 1) cerr << "ErR:Generate::nVar should be >= 1\n";
        for(int i = 0; i < nbVar-1; ++i){
            f << VarEnum[i] << ", ";
        }
        f << VarEnum[nbVar-1] << ", NBVAR};\n";
    f << "\tenum {";
        vector<string> listPar = generateParameters();
        int nbPar = listPar.size();
        for(int i = 0; i < nbPar-1; ++i){
            f << listPar[i] << ", ";
        }
        f << listPar[nbPar-1] << ", NBPARAM};\n\n";
    f << "\tlong long background;\n";
    f << "\tvoid derivatives(const vector<double> &x, vector<double> &dxdt, const double t);\n";
    f << "\tvoid initialise(long long _background = Back::WT);\n";
    f << "\tvoid setBaseParameters();\n";

    vector <vector <bool> > needed;
    needed.clear();
    needed.resize(MAX_DEGREE+1);
    for(int i = 0; i < MAX_DEGREE+1; ++i){
        needed[i].resize(MAX_DEGREE+1,false);
    }
    for(int i = 0; i < nbVar; ++i){
        vector<int> vpred = g.pred(i);
        int nbPred = vpred.size();
        int nbAct = 0;
        int nbInh = 0;
        for(int m = 0; m < nbPred; ++m){
            if(g(vpred[m],i) > 0) nbAct++; else nbInh++;
        }
        if((nbAct > MAX_DEGREE) || (nbInh > MAX_DEGREE)) {
            cerr << "ERR : GenerateHeader, The number of Activators (or Inhibitors) of a node are exceeding MAX_DEGREE = " << MAX_DEGREE << endl;
        }
        else needed[nbAct][nbInh] = true;
    }


    for(int i = 0 ; i < MAX_DEGREE; ++i){
        for(int j = 0; j <= MAX_DEGREE /*- i*/ ; ++j){
            if(needed[i][j]) f << generateActivFunction(i, j);
        }
    }
    f << "};\n";
    f << "#endif\n";
    f << "#endif\n";

    return f.str();
}












string Generate::generateCodeSource(){
    stringstream f;
    f << "// ------- Automatically generated model -------- //\n";
    f << "#include \"../common.h\"\n\n";
    f << "#include \"modele" << MName << ".h\"\n\n";
    f << "#ifdef COMPILE_AUTOGEN\n";


    f << "Modele" << MName << "::Modele" << MName << "() : Modele(NBVAR, NBPARAM), background(Back::WT) {\n";
    f << "\tdt = 0.2;\n";
    f << "\tprint_all_secs = 1200;\n";
    //f << "\tparametersLoaded = false;\n";
    //f << "\tnames.clear();\n";
    //f << "\tnames.resize(NBVAR);\n";
    for(int i = 0; i < nbVar; ++i){
        f << "\tnames[" << VarEnum[i] << "] = string(\"" << VarEnum[i] << "\");\n";
    }
    f << "\t// the names of variables that can be accessed by outside (by setValue and getValue)\n";
    //f << "\textNames.clear();\n";
    //f << "\textNames.resize(NBVAR);\n";
    for(int i = 0; i < nbVar; ++i){
        f << "\textNames[" << VarEnum[i] << "] = " << GlobVarEnum[i] << ";\n";
    }
    vector<string> listPar = generateParameters();
    int nbPar = listPar.size();
    for(int i = 0; i < nbPar; ++i){
        f << "\tparamNames[" << listPar[i] << "] = \"" << listPar[i] << "\";\n";
    }
    f << generateParamBorders();
    f << "\tbackSimulated.clear();\n";
    f << "\tbackSimulated.push_back(Back::WT);\n";
    f << "}\n\n";

    f << "void Modele" << MName << "::setBaseParameters(){\n";
    f << "\tparams.clear();     // to make sure they are all put to zero\n";
    f << "\tparams.resize(NBPARAM, 0.0);\n";

    for(int i = 0; i < nbPar; ++i){
        f << "\tparams[" << listPar[i] << "] = 1e-1;\n";
    }
    //f << "\tparametersLoaded = true;\n";
    f << "\tsetBaseParametersDone();\n";
    f << "}\n\n";

    f << "void Modele" << MName << "::initialise(long long _background){ // don't touch to parameters ! \n";
    //f << "\tif(! parametersLoaded) cerr << \"ERR : M110, you need to load/set parameters before initializing !\\n\";\n";
    //f << "\tdeleteCinetique();\n";
    f << "\tbackground = _background;\n";
    f << "\tval.clear();\n";
    f << "\tval.resize(NBVAR, 0.0);\n";
    //f << "\tvalTemp.clear();\n";
    //f << "\tvalTemp.resize(NBVAR, 0.0);\n";
    f << "\tinit.clear();\n";
    f << "\tinit.resize(NBVAR, 0.0);\n";
    f << generateBasalParameters();
#ifdef OLD
    for(int i = 0; i < nbVar; ++i){
        if(Basal[i] == CST_FROM_EQ){
            f << "\tif(params[B" <<  VarEnum[i] << "] < 0) params[B" <<  VarEnum[i] << "] = 0;\n";}}
#endif
    for(int i = 0; i < nbVar; ++i){
        if(Basal[i] == CST_FROM_EQ){
            f << "\tinit[" <<  VarEnum[i] << "] = params[" <<  VarEnum[i] << "EQ];\n";}}
    //params[BS4P]= params[KDS4P] * params[S4PEQ] - params[CS4P] * Activ2(0.0, params[K12_TO_S4P], params[N12_TO_S4P], params[S4PEQ], params[KS4P_AMPLI], params[NS4P_AMPLI])   ;
    //params[BTBET] = params[KDTBET] * params[TBETEQ] - params[CTBET] * Activ2(params[S4PEQ], params[KS4P_TO_TBET], params[NS4P_TO_TBET], params[TBETEQ] * (1 + params[CTCR]), KTBET_TO_TBET, NTBET_TO_TBET);
    //if(params[BS4P] < 0) params[BS4P] = 0;
    //if(params[BTBET] < 0) params[BTBET] = 0;
    // init[TBET] = params[TBETEQ];
    // init[STAT4P] = params[S4PEQ];

/// TODO
    // backSimulated.push_back(Back::STAT4KO);
    // backSimulated.push_back(Back::TBETKO);
    // backSimulated.push_back(Back::IFNGRKO);
    // if(background == Back::STAT4KO) init[STAT4P] = 0.0;
    // if(background == Back::TBETKO) init[TBET] = 0.0;
    // if(background == Back::IFNGRKO) params[KTBET_TO_TBET] = 1e6;
    f << "\tfor(int i = 0; i < NBVAR; ++i){\n";
    f << "\t\tval[i] = init[i];}\n";
    f << "\tt = 0;\n";
    //f << "\tpenalities = 0;\n";
    f << "\tinitialiseDone();\n";
    f << "}\n\n";
    f << "void Modele" << MName << "::derivatives(const vector<double> &x, vector<double> &dxdt, const double t){\n";
    f << generateEquations();
    f << "}\n";
    f << "#endif\n";

    cerr << "Source Code Generated" << endl;
    return f.str();

}





















string Generate::generateLatex(){
    stringstream f;
    vector<string> listPar = generateParameters();
    int nbPar = listPar.size();
    f << "\\documentclass[a3paper]{article}\n";
    f << "\\usepackage[utf8]{inputenc}   % LaTeX, comprends les accents !\n";
    f << "\\usepackage[T1]{fontenc}      % Police contenant les caracteres francais\n";
    f << "\\usepackage[francais]{babel}  % Placez ici une liste de langues (la derniere est la principale)\n";
    f << "\\usepackage{geometry}\n";
    f << "\\usepackage{graphicx}\n";
    f << "\\geometry{hmargin=1cm, vmargin=1cm}\n";
    f << "\\begin{document}\n";

    f << "\\subsection{" << MName << "(" << nbVar << " Variables, " << nbPar << " Parameters)}\n";

    f << "List of variables : \n";
    f << "\\begin{verbatim}\n";
    for(int i = 0; i < nbVar; ++i){
        if((i % 10) == (9 % 10)) f << "\n";
        f << VarEnum[i] << ", ";
    }
    f << "\\end{verbatim}\n";

    /*f << "List of parameters : \n";
    f << "\\begin{verbatim}\n";
    for(int i = 0; i < nbPar; ++i){
        if((i % 7) == (6 % 7)) f << "\n";
        f << listPar[i] << ", ";
    }
    f << "\\end{verbatim}\n";*/

    /*f << generateBasalParameters();
    for(int i = 0; i < nbVar; ++i){
        if(Basal[i] == CST_FROM_EQ){
            f << "\tif(params[B" <<  VarEnum[i] << "] < 0) params[B" <<  VarEnum[i] << "] = 0;\n";}}
    for(int i = 0; i < nbVar; ++i){
        if(Basal[i] == CST_FROM_EQ){
            f << "\tinit[" <<  VarEnum[i] << "] = params[" <<  VarEnum[i] << "EQ];\n";}}
    */
    //params[BS4P]= params[KDS4P] * params[S4PEQ] - params[CS4P] * Activ2(0.0, params[K12_TO_S4P], params[N12_TO_S4P], params[S4PEQ], params[KS4P_AMPLI], params[NS4P_AMPLI])   ;
    //params[BTBET] = params[KDTBET] * params[TBETEQ] - params[CTBET] * Activ2(params[S4PEQ], params[KS4P_TO_TBET], params[NS4P_TO_TBET], params[TBETEQ] * (1 + params[CTCR]), KTBET_TO_TBET, NTBET_TO_TBET);
    //if(params[BS4P] < 0) params[BS4P] = 0;
    //if(params[BTBET] < 0) params[BTBET] = 0;
    // init[TBET] = params[TBETEQ];
    // init[STAT4P] = params[S4PEQ];


    for(int i = 0; i < nbVar; ++i){
        f << "\\begin{eqnarray}\n";
        f << "\\frac{d" << VarEnum[i] << "}{dt} &=& ";
        if(Degradation[i] == PROP_DEG) f << "- K_{D," << VarEnum[i] << "} . [" << VarEnum[i] << "] ";
#ifdef OLD
        if((Basal[i] == CST) || (Basal[i] == CST_FROM_EQ)) f << "+ B_{" << VarEnum[i] << "}";
#endif
        vector<int> vpred = g.pred(i);
        int nbPred = vpred.size();
        int nbAct = 0;
        int nbInh = 0;
        for(int m = 0; m < nbPred; ++m){
            if(g(vpred[m],i) > 0) nbAct++; else nbInh++;
        }
        //if(VERBOSE) cerr << "" << endl;
        if(nbAct + nbInh > 0){
            f << " + C_{" << VarEnum[i] << "} . ("; //\\left(";
            // activators
            int nbAdded = 0;
            for(int m = 0; m < nbPred; ++m){
                if(g(vpred[m],i) > 0) {
                    f << " \\nonumber \\\\\n & &";
                    stringstream X, K, N, S;
                    X << "[" << VarEnum[vpred[m]] << "]";
                    K << "K_{" << VarEnum[vpred[m]] << " \\rightarrow " << VarEnum[i] << "}";
                    N << "N_{" << VarEnum[vpred[m]] << " \\rightarrow " << VarEnum[i] << "}";
                    #ifdef NEW
                    S << "S_{" << VarEnum[vpred[m]] << " \\rightarrow " << VarEnum[i] << "}";
                    f << latexHillAct(X.str(), K.str(), N.str(), S.str());
                    #endif
                    #ifdef OLD
                    f << latexHillAct(X.str(), K.str(), N.str());
                    #endif
                    nbAdded++;
                    if(nbAdded < nbPred) f << ".";
                }
            }
            for(int m = 0; m < nbPred; ++m){
                if(g(vpred[m],i) < 0) {
                    f << " \\nonumber \\\\\n & &";
                    stringstream X, K, N, S;
                    X << "[" << VarEnum[vpred[m]] << "]";
                    K << "K_{" << VarEnum[vpred[m]] << " \\rightarrow " << VarEnum[i] << "}";
                    N << "N_{" << VarEnum[vpred[m]] << " \\rightarrow " << VarEnum[i] << "}";
                    #ifdef NEW
                    S << "S_{" << VarEnum[vpred[m]] << " \\rightarrow " << VarEnum[i] << "}";
                    f << latexHillInh(X.str(), K.str(), N.str(), S.str());
                    #endif
                    #ifdef OLD
                    f << latexHillInh(X.str(), K.str(), N.str());
                    #endif
                    nbAdded++;
                    if(nbAdded < nbPred) f << ".";
                }
            }
            f << ")"; //"\\right)";
        } else {
            if((Basal[i] == CST_FROM_EQ)){
                f << " + P_{" << VarEnum[i] << "}";
            }
        }

        f << "\n\\end{eqnarray}\n\n";
    }
    f << "\\end{document}\n";

    cerr << "Latex Code Generated" << endl;
    return f.str();

}








#ifdef OLD
string Generate::generateBasalParameters(){
    stringstream f;

    for(int i = 0; i < nbVar; ++i){
        if(Basal[i] == CST_FROM_EQ){
            f << "\tparams[B" << VarEnum[i] << "] = ";
            if(Degradation[i] == PROP_DEG) f << "params[KD" << VarEnum[i] << "] * params[" << VarEnum[i] + string("EQ") << "] ";

            vector<int> vpred = g.pred(i);
            int nbPred = vpred.size();
            int nbAct = 0;
            int nbInh = 0;
            for(int m = 0; m < nbPred; ++m){
                if(g(vpred[m],i) > 0) nbAct++; else nbInh++;
            }

            //if(VERBOSE) cerr << "" << endl;
            if(nbAct + nbInh > 0){
                f << " - params[C" << VarEnum[i] << "] * Activ" << nbAct << "Inhib" << nbInh << "(";
                // activators
                int nbAdded = 0;
                for(int m = 0; m < nbPred; ++m){
                    if(g(vpred[m],i) > 0) {
                        if(Basal[vpred[m]] == CST_FROM_EQ){
                            f << "params[" << VarEnum[vpred[m]] + string("EQ") << "],";
                        } else {
                            cerr << "WRN: Generate::generateBasalParameters : You ask a (" << VarEnum[i] << ") to have a basal production term (params[B" << VarEnum[i] << "]) from equilibrium desired values (params[...EQ]). However, it requires the equilibrium values of other variables (" << VarEnum[vpred[m]] << ", for which the option 'Degradation = CST_FROM_EQ' should be chosen, in order to create the parameter : params[" << VarEnum[vpred[m]] + string("EQ") << "]. The initial value is chosen instead as equilibrium **potential** value. Bref, it is advised to choose this option for every variable. Note that, if the equilibrium is known, then this option will give a basal rate of zero, which you can check later.\n\n";
                            f << "init[" << VarEnum[vpred[m]] << "], /* ERR : no equilibrium parameter for it */ ";
                        }
                        f << "params[K" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "],";
                        f << "params[N" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "]";
                        nbAdded++;
                        if(nbAdded < nbPred) f << ",";
                    }
                }
                for(int m = 0; m < nbPred; ++m){
                    if(g(vpred[m],i) < 0) {
                        if(Basal[vpred[m]] == CST_FROM_EQ){
                            f << "params[" << VarEnum[vpred[m]] + string("EQ") << "],";
                        } else {
                            cerr << "generate::generateBasalParameters : You ask a variable (" << VarEnum[i] << ") to have a basal production term (params[B" << VarEnum[i] << "]) from equilibrium desired values (params[...EQ]). However, it requires the equilibrium values of other variables (" << VarEnum[vpred[m]] << ", for which the option 'Degradation = CST_FROM_EQ' should be chosen, in order to create the parameter : params[" << VarEnum[vpred[m]] + string("EQ") << "]. The initial value is chosen instead as equilibrium **potential** value. Bref, it is advised to choose this option for every variable. Note that, if the equilibrium is known, then this option will give a basal rate of zero, which you can check later.\n";
                            f << "init[" << VarEnum[vpred[m]] << "], /* ERR : no equilibrium parameter for it */ ";
                        }
                        f << "params[K" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "],";
                        f << "params[N" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "]";
                        nbAdded++;
                        if(nbAdded < nbPred) f << ",";
                    }
                }
                f << ")";
            }
            f << ";\n";
        }
    }
    return f.str();
}
#endif

#ifdef NEW
string Generate::generateBasalParameters(){
    stringstream f;

    for(int i = 0; i < nbVar; ++i){
        if(Basal[i] == CST_FROM_EQ){
            vector<int> vpred = g.pred(i);
            int nbPred = vpred.size();
            int nbAct = 0;
            int nbInh = 0;
            for(int m = 0; m < nbPred; ++m){
                if(g(vpred[m],i) > 0) nbAct++; else nbInh++;
            }

            #ifdef OLD
            f << "\tparams[B" << VarEnum[i] << "] = ";
            #endif
            #ifdef NEW
            if(nbPred == 0){
                f << "\tparams[P" << VarEnum[i] << "] = ";
            } else {
                f << "\tparams[C" << VarEnum[i] << "] = ";
            }
            #endif
            if(Degradation[i] == PROP_DEG) f << "params[KD" << VarEnum[i] << "] * params[" << VarEnum[i] + string("EQ") << "] / (";



            //if(VERBOSE) cerr << "" << endl;
            if(nbAct + nbInh > 0){
                #ifdef OLD
                f << "params[C" << VarEnum[i] << "] * Activ" << nbAct << "Inhib" << nbInh << "(";
                #endif
                #ifdef NEW
                f << "Activ" << nbAct << "Inhib" << nbInh << "(";
                #endif
                // activators
                int nbAdded = 0;
                for(int m = 0; m < nbPred; ++m){
                    if(g(vpred[m],i) > 0) {
                        if(Basal[vpred[m]] == CST_FROM_EQ){
                            f << "params[" << VarEnum[vpred[m]] + string("EQ") << "],";
                        } else {
                            cerr << "WRN: Generate::generateBasalParameters : You ask a (" << VarEnum[i] << ") to have a basal production term (params[B" << VarEnum[i] << "]) from equilibrium desired values (params[...EQ]). However, it requires the equilibrium values of other variables (" << VarEnum[vpred[m]] << ", for which the option 'Degradation = CST_FROM_EQ' should be chosen, in order to create the parameter : params[" << VarEnum[vpred[m]] + string("EQ") << "]. The initial value is chosen instead as equilibrium **potential** value. Bref, it is advised to choose this option for every variable. Note that, if the equilibrium is known, then this option will give a basal rate of zero, which you can check later.\n\n";
                            f << "init[" << VarEnum[vpred[m]] << "], /* ERR : no equilibrium parameter for it */ ";
                        }
                        f << "params[K" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "],";
                        f << "params[N" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "],";
                        f << "params[S" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "]";
                        nbAdded++;
                        if(nbAdded < nbPred) f << ",";
                    }
                }
                for(int m = 0; m < nbPred; ++m){
                    if(g(vpred[m],i) < 0) {
                        if(Basal[vpred[m]] == CST_FROM_EQ){
                            f << "params[" << VarEnum[vpred[m]] + string("EQ") << "],";
                        } else {
                            cerr << "generate::generateBasalParameters : You ask a variable (" << VarEnum[i] << ") to have a basal production term (params[B" << VarEnum[i] << "]) from equilibrium desired values (params[...EQ]). However, it requires the equilibrium values of other variables (" << VarEnum[vpred[m]] << ", for which the option 'Degradation = CST_FROM_EQ' should be chosen, in order to create the parameter : params[" << VarEnum[vpred[m]] + string("EQ") << "]. The initial value is chosen instead as equilibrium **potential** value. Bref, it is advised to choose this option for every variable. Note that, if the equilibrium is known, then this option will give a basal rate of zero, which you can check later.\n";
                            f << "init[" << VarEnum[vpred[m]] << "], /* ERR : no equilibrium parameter for it */ ";
                        }
                        f << "params[K" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "],";
                        f << "params[N" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "],";
                        f << "params[S" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "]";
                        nbAdded++;
                        if(nbAdded < nbPred) f << ",";
                    }
                }

                f << ")";
            }
            if((nbPred == 0) && (Basal[i] == CST_FROM_EQ)){
                f << 1.0; //"params[P" << VarEnum[i] << "]";
            }
            f << ");\n";
        }
    }
    return f.str();
}
#endif

string Generate::generateEquations(){

    stringstream f;

    for(int i = 0; i < nbVar; ++i){
        f << "\tif(!over(" << VarEnum[i] << "))"; /// NEW trick !!
        f << "\tdxdt[" << VarEnum[i] << "] = (";
        if(Degradation[i] == PROP_DEG) f << "- params[KD" << VarEnum[i] << "] * x[" << VarEnum[i] << "] ";
#ifdef OLD
        if((Basal[i] == CST) || (Basal[i] == CST_FROM_EQ)) f << "+ params[B" << VarEnum[i] << "]";
#endif

        vector<int> vpred = g.pred(i);
        int nbPred = vpred.size();
        int nbAct = 0;
        int nbInh = 0;
        for(int m = 0; m < nbPred; ++m){
            if(g(vpred[m],i) > 0) nbAct++; else nbInh++;
        }
        //if(VERBOSE) cerr << "" << endl;
        if(nbAct + nbInh > 0){
            f << " + params[C" << VarEnum[i] << "] * Activ" << nbAct << "Inhib" << nbInh << "(";
            // activators
            int nbAdded = 0;
            for(int m = 0; m < nbPred; ++m){
                if(g(vpred[m],i) > 0) {
                    f << "x[" << VarEnum[vpred[m]] << "],";
                    f << "params[K" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "],";
                    f << "params[N" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "]";
                    #ifdef NEW
                    f << ", params[S" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "]";
                    #endif
                    nbAdded++;
                    if(nbAdded < nbPred) f << ",";
                }
            }
            for(int m = 0; m < nbPred; ++m){
                if(g(vpred[m],i) < 0) {
                    f << "x[" << VarEnum[vpred[m]] << "],";
                    f << "params[K" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "],";
                    f << "params[N" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "]";
                    #ifdef NEW
                    f << ", params[S" << VarEnum[vpred[m]] << "_TO_" << VarEnum[i] << "]";
                    #endif
                    nbAdded++;
                    if(nbAdded < nbPred) f << ",";
                }
            }
            f << ")";
        } else {
            if(Basal[i] == CST_FROM_EQ){
                f << "+ params[P" << VarEnum[i] << "]";
            }
            if((Degradation[i] == NO_D) && (Basal[i] == NO_B)) f << "0.0"; // to avoid "delta_t * ()";
        }
        f << ");\n";
    }

    return f.str();
}

   // erases the current graph
void Generate::clearReactions(){
    g = Graphe(nbVar);
}

void Generate::useReactionsFromGraph(Graphe G){
    if(G.size() != nbVar) {cerr << "ERR : Generate::addReactionsFromGraph, incorrect graph size (" << G.size() << "), versus nb of variables already defined (" << nbVar << ")\n";  return;}
    if((int) VarEnum.size() != nbVar) {cerr << "ERR : Generate::addReactionsFromGraph, incorrect varEnum size\n";}
    for(int i = 0; i < nbVar; ++i){
        vector<int> succ = G(i);
        for(int j = 0; j < (int) succ.size(); ++j){
            if((succ[j] >= nbVar) || (succ[j] < 0)) {cerr << "ERR : Generate::addReactionsFromGraph, out of bounds values in the graph !!\n";}
            addReaction(VarEnum[i], VarEnum[succ[j]], G(i,succ[j]));
        }
    }
    g = Graphe(&G);
}

// the ultimate function !!!
//    void generateModelFromGraph(Graphe& G){


string generateCostCodeFromData(string dataTxt, bool CostsPerCurves, string timeUnit){
    stringstream f(dataTxt);

    stringstream of; // output code
    stringstream overrider;


    int nbConditions, IDexp;
    f >> nbConditions;
    cout << "Nb of experiences : " << nbConditions << endl;

    //of << "# Automatically generated Data\n";
    of << "double costPart(){\n";
    of << "\tdouble res = 0;\n";

    string strash;
    //int itrash;


    for(int i = 0; i < nbConditions; ++i){
        stringstream recapTransv;
        int nbL, nbV;
        f >> IDexp;
        char essai;

        stringstream toGetComment;
        while ((f.peek()!='\n') && (f>>essai)) toGetComment << essai;
        string toGetComment2 = toGetComment.str();
        f >> nbL >> nbV;

        // version transversale (coût par time-point), suppose that each variable is simulated (bad guess ...)
        if(nbL > 0){
            vector<string> varNames;
            vector<int> varGlobalIDs;
            f >> strash;    // for 'time'

            // this is not allowed in C++ vector<stringstream>
            vector<stringstream*> of2;
            vector<stringstream*> of2Data;
            vector<stringstream*> foroverrider;
            of2.resize(nbV);
            of2Data.resize(nbV);
            foroverrider.resize(nbV);
            for(int i = 0; i < nbV; ++i) {
                of2[i] = new stringstream();
                of2Data[i] = new stringstream();
                foroverrider[i] = new stringstream();
            }

            for(int j = 0; j < nbV; ++j){
                f >> strash;
                //f >> itrash;
                varNames.push_back(strash);
                //varGlobalIDs.push_back(itrash);
            }

            overrider << "\\Experiment " << IDexp << ")){     // " << toGetComment.str() << "\n";
            if(!CostsPerCurves)    of << "\tif(isDoable(" << toGetComment2 << ")){     // " << IDexp << "\n";
            int cptForf = 0;
            for(int k = 0; k < nbL; ++k){
                double tvalue, yvalue;
                if(!CostsPerCurves)  of << "\t\tdouble c" << cptForf << "a[] = {";
                f >> tvalue;
                for(int j = 0; j < nbV; ++j){
                    if(!CostsPerCurves){
                        of << "V(" << toGetComment2 << ", " << varNames[j] << ", " << tvalue << timeUnit << " )";
                        if(j < nbV-1) of << ",\t"; else of << "};\n";
                    } else {
                        *(of2[j]) << "V(" << toGetComment2 << ", " << varNames[j] << ", " << tvalue << timeUnit << ")";
                        if(k < nbL-1) *(of2[j]) << ",\t"; else *(of2[j]) << "};\n";
                    }
                    (*foroverrider[j]) << "set(" << IDexp << ", " << varNames[j] << ", " << tvalue << timeUnit << ",";
                }
                if(!CostsPerCurves)  of << "\t\tdouble c" << cptForf << "b[] = {";
                for(int j = 0; j < nbV; ++j){
                    f >> yvalue;
                    if(!CostsPerCurves)  {
                        of << yvalue;// << " DAY";
                        if(j < nbV-1) of << ",\t"; else of << "};\n";
                    } else {
                        *(of2Data[j]) << yvalue;// << " DAY";
                        if(k < nbL-1) *(of2Data[j]) << ",\t"; else *(of2Data[j]) << "};\n";
                    }
                    (*foroverrider[j]) << yvalue << "); ";
                }
                if(!CostsPerCurves) of << "\t\tres += functCost(c" << cptForf << "a, c" << cptForf << "b, " << nbV << ");\n";
                cptForf++;

            }


            if(CostsPerCurves){
                int cptForf2 = 0;
                recapTransv << "\tif(isDoable(" << toGetComment2 << ")){     // " << IDexp << "\n";
                for(int j = 0; j < nbV; ++j){
                     recapTransv <<"\t\tif(m->isVarKnown(" << varNames[j] << ")){\n";
                     recapTransv << "\t\t\tdouble c" << cptForf2 << "a[] = {";
                     recapTransv << (of2[j])->str();
                     recapTransv << "\t\t\tdouble c" << cptForf2 << "b[] = {";
                     recapTransv << (of2Data[j])->str();
                     recapTransv << "\t\t\tres += functCost(c" << cptForf2 << "a, c" << cptForf2 << "b, " << nbL << ");\n";
                     recapTransv <<"\t\t}\n";
                     (*foroverrider[j]) << "\n";
                     overrider << (foroverrider[j])->str();
                     cptForf2++;
                }
                //recapTransv << "\t}\n";
                of << recapTransv.str();
            }
            of << "\t}\n";

        }
    }


    of << "\treturn res;\n}\n";
    // cout << overrider.str() << endl;
    return of.str();

}











