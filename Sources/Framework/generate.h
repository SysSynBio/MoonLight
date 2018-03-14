    // Philippe Robert, 19-09-2014 - philippe-robert.com
// generate.h and generate.cpp are independent files
#ifndef GENERATE_H
#define GENERATE_H

// cout << generateActivFunction(1, 3);

#include <vector>
#include <string>
using namespace std;

//Value to use graphes (or I should do a template)
typedef double valeur;

#define NO_INTER 0
#define HILL_ACT +1
#define HILL_INH -1

#define BASAL_FOR_ACT_HILL

//#define USE_MULTI_HILL

string generateActivFunction(int nbAct, int nbInh);

/// 1 - Simple structure for a network :
struct Graphe{
    // Storage :
    int nbNodes;
    int nbVertices;
    vector< vector<valeur> > table;         /// table[i][j] <=> type/weight of interaction for i-->j

    // functions to create and modify
    Graphe(int _nbNodes = 0);
    Graphe(Graphe* toCopy);                 /// to duplicate an existing graph
    void add(int i, int j, valeur value);   /// to add 'i-->j' with type/weight of interaction value.
                                            /// Returns a warning if interaction already existing
    void resize(int newNbNodes);            /// add new nodes. Clears all the interactions !!

    // functions to get information about the network
    valeur operator()(int i, int j);        /// reads the table : me(i,j) = table[i][j]
    vector<int> operator()(int i);          /// gets the 'sons' of i : me(i) = table[i]
    vector<int> pred(int i);                /// gets the 'parents' of i : pred(i) = {j | j --> i}
    void print();
    int size();                             /// returns Nb of nodes
    int inters();                           /// returns Nb of edges
};
void testeGraphManip();

/// 2 - Very cool function : generate all subgraphs of Gbig containing the edges of Gsmall, (with new_size interactions)
///     Gsmall has to be already a sub-graph (same nodes and subset of edges), or an error is thrown.
///     if new_size = 0, generate all the corresponding graphs with all possible sizes.
vector<Graphe*> between(Graphe Gbig, Graphe Gsmall, int new_size = 0);



/// 3 - To define boundary parameters for different kinds of variables : gene expression, proteins, receptors, cytokines, RNAs etc ...

struct typeVar {
    typeVar(string nm = string("")) : name(nm),
        Deg_min         (1e-12),             Deg_max(1e+12),
        Basal_Cst_min   (1e-12),       Basal_Cst_max(1e+12),
        Basal_prop_min  (1e-12),      Basal_prop_max(1e+12),
        Trans_min       (1e-12),           Trans_max(1e+12),
        ValueEQ_min     (1e-12),         ValueEQ_max(1e+12),
        Khill_min       (1e-12),           Khill_max(1e+12),
        Nhill_min       (1e-12),           Nhill_max(1e+12),
        KfKv_min        (1e-12),            KfKv_max(1e+12) {}
    string name;
    double Deg_min;             double Deg_max;
    double Basal_Cst_min;       double Basal_Cst_max;
    double Basal_prop_min;      double Basal_prop_max;
    double Trans_min;           double Trans_max;
    double ValueEQ_min;         double ValueEQ_max;
    double Khill_min;           double Khill_max;
    double Nhill_min;           double Nhill_max;
    double KfKv_min;            double KfKv_max;
};




/// 4 - Generating the code for simulating a network (according to the Modele/Experiments framework)

enum D {NO_D, PROP_DEG, NB_DEG_POL};
enum B {NO_B, CST, CST_FROM_EQ, NB_BAS_POL};
#define MAX_DEGREE 10

class Generate
{
public:
    Generate(string _MName);

    string MName;
    int nbVar;

    void addVar(string nameVar, D degPolicy = PROP_DEG, B basalPolicy = CST_FROM_EQ, string globalName = string(""), int typeVar = -1);
    /// to declare a new variable. Warning : Erase existing reactions ! Please define variables first and then reactions

    void addReaction(string Var1, string Var2, int IDfunction);
    /// to declare a new interaction (reaction).

    void useReactionsFromGraph(Graphe G);
    /// Keep the same variables (nodes), but use the reactions from another graph (G) - it erases the current graph and replaces it by G

    void clearReactions();
    /// remove all existing reactions

    void setBorderPolicy(vector<typeVar> policy); // Caution, will be read as policy[typeVariable], i.e. type is a vector index
    /// to give parameter boundaries depending on their type

    Graphe g;
    vector<string> VarEnum; // should be at least 1
    vector<string> GlobVarEnum; // should be at least 1, "" if not global
    vector<D> Degradation;
    vector<B> Basal;
    vector<int> Types;
    vector<typeVar> ParamBordersPolicy;


    /// I/O parts :

    vector<string> filesToInclude;
    void addInclude(string fileName);
    /// to include headers, addInclude("fileToInclude.h") or addInclude("<libraryToInclude.h>")

    vector<string> generateParameters();
    string generateActivFunction(int nbAct, int nbInh);
    string generateBasalParameters();
    string generateEquations();
    string generateLatex();

    string generateCodeHeader();
    string generateCodeSource();
    string generateParamBorders();
};

/// 5 - Independent function to transform a kinetics in text file into a C++ cost function
///     that can be used in a sub-class of Experiments

string generateCostCodeFromData(string, bool CostsPerCurves = true, string timeUnit = string(" DAY"));

#endif // GENERATE_H
