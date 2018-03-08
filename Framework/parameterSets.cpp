#include "parameterSets.h"
#include <algorithm>        // for std::sort

/// ========================= Part 1 - Managing (ordered) sets of parameters ================================

oneSet::oneSet(vector<double>* pset, double _cost) : cost(_cost){
    if(pset) {
        int s = pset->size();
        v.resize(s);
        for(int i = 0; i < s; ++i){
            v[i] = (*pset)[i];
        }
    }
}
oneSet::oneSet(vector<double> & pset, double _cost) : cost(_cost){
    int s = pset.size();
    v.resize(s);
    for(int i = 0; i < s; ++i){
        v[i] = pset[i];
    }
}
oneSet::oneSet(const oneSet &toCopy){
    v = toCopy.v;
    cost = toCopy.cost;
}

string oneSet::print(){
    stringstream res;
    res << "Cost:\t" << cost;
    int s = v.size();
    for(int i = 0; i < s; ++i){
        res << "\t" << v[i];
    }
    res << "\n";
    return res.str();
}



pSets::pSets(int _MaxNb, int _nbParams) : MaxNb(_MaxNb), nbParams(_nbParams), threshold(-1) {}
bool pSets::addSet(vector<double>* v, double _cost){
    if((threshold < 0) || (_cost < threshold)){
        // then, add it to the list
        oneSet* s = new oneSet(v, _cost);
        store.push(s);
        if((int) s->v.size() != nbParams) cerr << "Err : giving not properly sized vectors to pSets !!!" << endl;
        // Now, remove if necessary
        if((int) store.size() > MaxNb){
            oneSet* o = store.top();
            store.pop();
            delete o;
            threshold = store.top()->cost;
        }
        return true;
    }
    return false;
}


vector<double> pSets::bestSet(){
    vector<oneSet*> liste = toVector();
    reverse(liste.begin(), liste.end());
    if(liste.size() == 0) return vector<double>();
    oneSet* best = liste[0];
    return best->v;
}

oneSet pSets::bestOneSet(){
    vector<oneSet*> liste = toVector();
    reverse(liste.begin(), liste.end());
    if(liste.size() == 0) return oneSet();
    oneSet* best = liste[0];
    oneSet res = oneSet(*best);
    return res;
}

void pSets::saveBestSet(string _name){
    vector<double> bset = bestSet();
    ofstream f(_name.c_str(), ios::out);
    if(!f) {cerr << "File not Found" << _name << endl; return;}
    f << bset.size() << endl;
    for(int i = 0; i < (int) bset.size(); ++i){
        if(i > 0) f << "\t";
        f << bset[i];
    }
    f << endl;
    f.close();
}


vector<oneSet*> pSets::toVector(){
    vector<oneSet*> local;
    int cpt = 0;
    oneSet* best;// it's the worst, actually
    while((cpt < 2*MaxNb) && (!store.empty())){
        best = store.top();
        store.pop();
        local.push_back(best);
        cpt++;
    }
    store = priority_queue<oneSet*, vector<oneSet*>, CompareSets> (local.begin(), local.end());
    return local;
}

oneSet* pSets::getSetNumber(int i){
    if((i < 0) || (i >= (int) store.size())) {cerr << "ERR: pSets::getSetNumber(" << i << "), wrong index, only " << store.size() << " elements\n"; return NULL;}
    return toVector()[store.size() - i - 1]; // returns in opposite order because the worst is in first position
}

void pSets::clear(){
    int cpt = 0;
    oneSet* best; // worst !!!
    while((cpt < 2*MaxNb) && (!store.empty())){
        best = store.top();
        store.pop();
        delete best;
        cpt++;
    }
    threshold = -1;
}

void pSets::resize(int newMaxNb){
    if((int) store.size() > newMaxNb){
        int cpt = 0;
        while((cpt < 2*MaxNb) && ((int) store.size() > newMaxNb)){
            oneSet* toDst = store.top();
            store.pop();
            delete toDst;
            cpt++;
        }
    } else {
        threshold = -1;
    }
    MaxNb = newMaxNb;
}
string pSets::print(){
    stringstream res;
     vector<oneSet*> v = toVector();
     int s = v.size();
     for(int i = 0; i < s; ++i){
        res << v[i]->print();
     }
     return res.str();
}


void testeSets(){
    vector<double> v1 = {1, 2};
    vector<double> v2 = {3, 4};
    vector<double> v3 = {5, 6};
    vector<double> v4 = {0, 1};
    pSets ps = pSets(2, 2);
    ps.addSet(&v1, 1);
    cout << ps.print(); cout << "\n";
    ps.addSet(&v2, 3);
    cout << ps.print(); cout << "\n";
    ps.addSet(&v3, 5);
    cout << ps.print(); cout << "\n";
    ps.addSet(&v4, 0);
    cout << ps.print(); cout << "\n";
}







/// ========================= Part 2 - Managing variations around a set of parameters (ex : identifiability) ================================

/// class to store the cost of changing a parameter from a set

pointVariation::pointVariation(int _indexParameter, double _val, double _cost, vector<double> _costPerVariable,vector<double> _costPerExperiment, vector<double> &_paramSet) :
    indexParameter(_indexParameter), val(_val), cost(_cost), paramSet( _paramSet, cost) {
    if(indexParameter < 0) cerr << "ERR : pointVariation(...) negative indexParameter" << endl;
    // copying costPervariable in a new vector*
    int cpvs = _costPerVariable.size();     if(cpvs == 0) cerr << "ERR: pointVariation::pointVariation, NO COST PER VARIABLE" << endl;
    costPerVariable = new vector<double>(cpvs, 0.0);                    // I got an error if using costPerVariable = new vector<double>(_costPerVariable); Why ???
    for(int i = 0; i < cpvs; ++i){
        (*costPerVariable)[i] = _costPerVariable[i];
    }
    if(costPerVariable == NULL) cerr << "ERR: !!!!! copy of costPerVariable Failed!" << endl;

    // copying costPerExperiment in a new vector*
    int cpes = _costPerExperiment.size();     if(cpes == 0) cerr << "ERR: pointVariation::pointVariation, NO COST PER EXPERIMENT" << endl;
    costPerExperiment = new vector<double>(cpes, 0.0);                    // I got an error if using costPerVariable = new vector<double>(_costPerVariable); Why ???
    for(int i = 0; i < cpes; ++i){
        (*costPerExperiment)[i] = _costPerExperiment[i];
    }
    if(costPerExperiment == NULL) cerr << "ERR: !!!!! copy of costPerExperiment Failed!" << endl;
}

string pointVariation::print(){
    stringstream ss;
    if(costPerVariable == NULL) cerr << "ERR: pointVariation, costPerVariable not instanciated !" << endl;
    int NV = costPerVariable->size(); // can not be NULL because of the constructor
    int NP = paramSet.v.size();
    int NE = costPerExperiment->size(); // can not be NULL because of the constructor
    ss << "param[" << indexParameter << "]=\t" << val << "\t" << cost;
    ss << "\tperExperiment(" << NE << "):";
    for(int i = 0; i < NE; ++i){
        ss << "\t" << (* costPerExperiment)[i];
    }
    ss << "\tperVariable(" << NV << "):";
    for(int i = 0; i < NV; ++i){
        ss << "\t" << (* costPerVariable)[i];
    }
    ss << "\tFullSet(" << NP << "):";
    for(int i = 0; i < NP; ++i){
        ss << "\t" << paramSet.v[i];
    }
    ss << "\n";
    return ss.str();
}

pointVariation::~pointVariation(){
    //delete costPerVariable; //NOOO don't do that because when you copy a costVariation, the new copy keeps the pointer to costPervariable and the older ones destroys it when dying ...
}

// class, to avoid compiling error because code in the .h file
//class ComparePointVars {
//    public: bool operator()(pointVariation t1, pointVariation t2){return (t1.val < t2.val);}
//} ComparePointVars;

bool ComparePointVars(pointVariation t1, pointVariation t2) {return (t1.val < t2.val);}

/*string printVector(vector<double> &v){
    stringstream ss;
    int s = v.size();
    ss << "V(" << s << ") :";
    for(int i = 0; i < s; ++i){
        ss << "\t" << v[i];
    }
    return ss.str();
}*/

oneParameterAnalysis::oneParameterAnalysis(vector<double> _initialParameterSet) : nbPoints(0) {
    initialParameterSet =  _initialParameterSet; nbParams = initialParameterSet.size();}


void oneParameterAnalysis::add(int _indexParameter, double _val, double _cost, vector<double> _costPerVariable, vector<double> _costPerExperiment, vector<double> _paramSet){
    points.push_back(pointVariation(_indexParameter, _val, _cost, _costPerVariable,_costPerExperiment, _paramSet));
    nbPoints++;
}

void oneParameterAnalysis::add(pointVariation pv) {
    points.push_back(pv);
    nbPoints++;
}

void oneParameterAnalysis::sort(){
    std::sort(points.begin(), points.end(), ComparePointVars);
}

string oneParameterAnalysis::print(){
    if(nbPoints < 1) {cerr << "ERR: oneParameterAnalysis::print(), empty analysis, can not print anything\n"; return string("");}
    stringstream cumul;
    /*cumul << "#------ OneParameterAnalysis: ------\nnbPoints=\t" << nbPoints << "\tnbParams=\t" << nbParams << endl;
    cumul << "#Initial Parameter Set : " << printVector(initialParameterSet) << endl;
    cumul << "#Variations around this parameter set, with associated costs " << endl;
    cumul << "#PointNr\tParamChanged\tval\tcost\tNbExps\t\n";*/
    for(int i = 0; i < nbPoints; ++i){
        cumul << "point(" << i+1 << ")\t" << points[i].print();}
    //cumul << "------ _____________________ ------" << endl;
    return cumul.str();
}


oneParameterAnalysis::~oneParameterAnalysis(){
    points.clear(); // should call the destructor of pointVariation
}



void testPointVariations(){
    //int NP = 5; // nb parameters
    //int NV = 3; // nb variables
    vector<double> costPerVar1 = {50.0, 1.0, 51.5};
    vector<double> costPerVar2 = {25.0, 75.5, 5.0};
    vector<double> costPerExp1 = {40, 62.5};
    vector<double> costPerExp2 = {65.0, 35.5};
    vector<double> initialParamSet = {1.0, 2.5, 3.0, 4.0, 5.0};
    vector<double> paramSet1 = {1.0, 2.0, 3.0, 4.0, 5.0};
    vector<double> paramSet2 = {1.0, 3.0, 3.0, 4.0, 5.0};

    pointVariation a = pointVariation(2, 2.0, 102.5, costPerVar1, costPerExp1, paramSet1);
    cout << "Test with printing a pointVariation:\n" << a.print() << endl;
    cout << "adress costPerVar1 : " << &costPerVar1 << endl;
    cout << "adress costPerVar inside a : " << a.costPerVariable << endl;


    oneParameterAnalysis b = oneParameterAnalysis(initialParamSet);
    cout << "Printing pointAnalysis before adding:\n" << b.print()  << endl;
    b.add(2, 3.0, 105.5, costPerVar2, costPerExp2, paramSet2);
    b.add(a);
    cout << "After adding:\n" << b.print()  << endl;
    cout << "still, first set = " << a.print() << endl;
    cout << "adress costPerVar inside a : " << a.costPerVariable << endl;

    b.sort();
    cout << "... and sorting:\n" << b.print()  << endl;
}
