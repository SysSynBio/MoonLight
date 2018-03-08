#ifndef PARAMETER_SET_H
#define PARAMETER_SET_H

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <queue>
#include <cmath> // for NAN
using namespace std;

/*string printVector(vector<double> &v);*/

/// 1 - *A set of parameters* is a table of parameter values and possibly a cost.
struct oneSet {
    vector<double> v;
    double cost;
    oneSet() : cost(NAN){}
    oneSet(vector<double>* pset, double _cost);
    oneSet(vector<double>& pset, double _cost);
    oneSet(const oneSet &toCopy);
    string print();
};

/// 2 - When parameter sets need to be sorted based on their cost, a comparator should be defined (based on their cost)
class CompareSets {
    public: bool operator()(oneSet* t1, oneSet* t2){return (t1 && t2 && ((*t1).cost < (*t2).cost));}
};

/// 3 - *A list of parameter sets* : store them in a queue (fast sorting), but can convert them in vector<parameter set> when wanted
///     note : this data structure manages pointers to oneSet, which are already in memory somewhere. It doesn't change their
///     localization nor content.
struct pSets {
    /// 3a : when creating a param set list, needs to say how many will be recorded (maximum), and how many parameters per set.
    pSets(int _MaxNb, int _nbParams);
    int MaxNb;
    int nbParams;

    /// 3b : Data storage
    priority_queue<oneSet*, vector<oneSet*>, CompareSets> store;
    bool addSet(vector<double>* v, double _cost);   /// Adds the param set if there is still space (MaxNb), or if it is better than an existing set in the list (that will be wasted)
    double threshold;                               /// stores the cost of the worst set, so if the list is full, and a new set is not better than threshold, it will be rejected

    /// 3c : Getting parameter sets (because in a queue, only the top one can be read)
    ///      DANGER : these functions give the pointers to the parameter set(s). Therefore, this is not a copy, this is the original
    ///      location in memory of these parameter sets. Do not modify them !!
    vector<oneSet*> toVector();                     /// to get the whole list as vector
    oneSet* getSetNumber(int i);                    /// or to get a set directly.

    oneSet bestOneSet();
    vector<double> bestSet();
    void saveBestSet(string _name);

    /// 3d : additional functions
    void clear();
    void resize(int newMaxNb);                      /// to dynamically resize the number of stored parameter sets
    string print();
};

/// 3e : example function on how to use parameter sets.
void testeSets();



/// 4 - In case of identifiability or sensitivity, point modifications of a parameter set have to be done (i.e. changing one parameter value only.)

/// Here is a data structure to store the cost of varying a parameter around an initial parameter set.
/// It can be used during sensitivity analysis (the cost of just changing one parameter around the original one),
/// or during identifiability analysis (storing the best set after optimization if this parameter is fixed to this value)
/// It can also stores the statistics of the cost for each variable.

/// 4a - one parameter set (with additional information)
struct pointVariation {
public:
    pointVariation(int _indexParameter, double _val, double _cost, vector<double> _costPerVariable, vector<double> _costPerExperiment, vector<double> &_paramSet);
    int               indexParameter;           /// Index of the parameter that was changed compared to an initial parameter set
    double            val;                      /// Value of this particular parameter

    /// Note : this class could also extend the class oneSet and then these two fields would be inherited
    double            cost;                     // is this necessary ? there is paramSet.cost as well ...
    oneSet            paramSet;                 /// The parameter set itself (and its cost inside)

    vector<double>*   costPerVariable;          /// To additionally store the cost of each simualted/evaluated variable with this param. set.
    vector<double>*   costPerExperiment;
    string print();
    ~pointVariation();
};

/// 4b - a comparator between pointVariations (based on the cost), in order to sort them
bool ComparePointVars(pointVariation t1, pointVariation t2);

/// 4c - to store the result of an analysis (identifiability or sensitivity), by playing around one parameter.
struct oneParameterAnalysis {

    /// 4ci : Initial data : the initial parameter set.
    oneParameterAnalysis(vector<double> _initialParameterSet);
    int nbParams;
    vector<double> initialParameterSet;         /// could be a oneSet here

    /// 4cii : dynamic storage of new point variations
    vector<pointVariation> points;
    int nbPoints;
    int size() {return nbPoints;}

    void add(int _indexParameter, double _val, double _cost, vector<double> _costPerVariable, vector<double> _costPerExperiment, vector<double> _paramSet);
    void add(pointVariation pv); // do not use &pv, because wants to copy everything, not the reference/pointer
    void sort();
    string print();
    ~oneParameterAnalysis();
};


void testPointVariations();

#endif
