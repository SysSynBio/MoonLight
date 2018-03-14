#include "evaluator.h"

#define penaltyNotRecorded 0.0
Evaluator::Evaluator() : recording_before_simulation(true), nbPoints(0), nextTimePoint(1e8) {
        Species.clear();
        Times.clear();
        theData.clear();
        recorded.clear();
        ExpectedValues.clear();
        StdDeviations.clear();
}
string Evaluator::printState(){
    stringstream res;
    res << "Eva : " << nbPoints << " pts, nextTime=" << nextTimePoint << endl;
    for(int i = 0; i < nbPoints; ++i){
        res << "Sp="<< Species[i] << "\t=" << Times[i] << ", val=" << theData[i] << ", expected= " << ExpectedValues[i] << ", std-dev=" << StdDeviations[i] << ", rec=" << (recorded[i] == true ? "Oui" : "Non") << "\n";
    }
    return res.str();
}

bool Evaluator::getValsFromKinetics(TableCourse* _expectedValues, TableCourse* _StdDevs, vector<string> NamesVariablesInTheirIndex){
    /// BIG constraint : StdDev should have data at the exact same position than expectedValues
    if(! _expectedValues) {cerr << "ERR GetValFromKinetics(), empty table course for expected values\n"; return false;}
    vector<string> heads = _expectedValues->headers;
    vector<typeTime> timepts = _expectedValues->getTimePoints();
    vector<int> indicesHeads(heads.size(), -1);

    vector<string> heads2 = (_StdDevs ? _StdDevs->headers : vector<string> ());
    vector<typeTime> timepts2 = (_StdDevs ? _StdDevs->getTimePoints() : vector<typeTime> ());
    if(_StdDevs) if(heads.size() != heads2.size()){cerr << "ERR: Evaluator::getValsFromKinetics(), the two tables should have same sizes and headers"; return false;}
    vector<int> indicesHeads2(heads2.size(), -1);
    if(_StdDevs) if(timepts.size() != timepts2.size()){cerr << "ERR: Evaluator::getValsFromKinetics(), the two tables should have same sizes and headers"; return false;}

    int nbTimePoints = timepts.size();
    int nbHeaders = heads.size();
    int sizeNames = NamesVariablesInTheirIndex.size();

    for(int i = 1; i < nbHeaders; ++i){        // headers[0] is the title of the table
        if(_StdDevs) if(heads[i].compare(heads2[i])) {cerr << "ERR: Evaluator::getValsFromKinetics(), the two tables should have same sizes and headers"; return false;}
        for(int j = 0; j < sizeNames; ++j){
            if(! NamesVariablesInTheirIndex[j].compare(heads[i])) {
                if(indicesHeads[i] > 0) cerr << "ERR : Evaluator::getValsFromKinetics, a header is defined twice";
                indicesHeads[i] = j;
                if(_StdDevs) indicesHeads2[i] = j;
            }
        }
    }
    /*cout << "--> Loading Kinetics into evaluator\n";
    cout << "IDs found for each variable : \n";
    for(int i = 1; i < nbHeaders; ++i){
        cout << heads[i] << "\t" << indicesHeads[i] << endl;
    }*/

    for(int i = 0; i < nbTimePoints; ++i){
        if(_StdDevs) if(timepts[i] != timepts2[i]) {cerr << "ERR : getValsFromKinetics, the kinetics for expected values and std deviations have different time points\n"; return false;}
        for(int j = 1; j < nbHeaders; ++j){
            if((indicesHeads[j]) >= 0){  // to avoid the '-1'(=not found) raised by the name of the table (stored in header[0]),
                double currentExpValue = ((*_expectedValues)(j-1, i));
                if(!((std::isnan(currentExpValue) || std::isinf(currentExpValue)))){

            //Philippe replaced by the three last lines ((indicesHeads[j]) >= 0) // to avoid the '-1'(=not found) raised by the name of the table (stored in header[0]),
                getVal(indicesHeads[j], timepts[i], (*_expectedValues)(j-1, i), (_StdDevs ? (*_StdDevs)(j-1, i) : 0));

                } // added
            } // added
        }
    }
    return true;
}


double Evaluator::getVal(int _Species, typeTime _time_sec, double _expectedValue, double _stdDev){
    if(recording_before_simulation && (_expectedValue >= 0) && (! std::isnan(_expectedValue))){    // negative values forbidden for kinetics --> excluded for cost
        //cerr << "Requires species " << _Species << " at time " << _time_sec << endl;
        if(findIndex(_Species, _time_sec) >= 0) return 0;
        else {
            nbPoints++;
            Species.push_back(_Species);
            Times.push_back(_time_sec);
            ExpectedValues.push_back(_expectedValue);
            StdDeviations.push_back(_stdDev);
            return 0;
        }
    } else {
        int a = findIndex(_Species, _time_sec);
        if(a < 0) {
            cout << "   WRN : Species = " << _Species << ", t = " << _time_sec << " sec, not found in evaluator record !!\n";
            return 0;}
        else {
            if(! recorded[a]) cout << "WRN: data not recorded/n";
            return theData[a];
        }
    }
}

int Evaluator::findIndex(int _Species, typeTime _time_sec){
    for(int i = 0; i < nbPoints; ++i){
        if((Species[i] == _Species) && (Times[i] == _time_sec)) return i;}
    return -1;
}
void Evaluator::recordingCompleted(){
    theData.clear();
    theData.resize(nbPoints, 0.0);
    recording_before_simulation = false;
    recorded.clear();
    recorded.resize(nbPoints, false);
    updateNextPoint();
}
typeTime Evaluator::updateNextPoint(){
    for(int i = 0; i < nbPoints; ++i){
        if((! recorded[i]) && (Times[i] < nextTimePoint))
            nextTimePoint = Times[i];}
    return nextTimePoint;
}

bool Evaluator::takeDataAtThisTime(typeTime t_sec){
    if(t_sec < nextTimePoint) return false;
    return true;
}

int Evaluator::speciesToUpdate(){
    for(int i = 0; i < nbPoints; ++i){
        if((Times[i] == nextTimePoint) && (recorded[i] == false)) return Species[i];
    }
    nextTimePoint = 1e8;
    updateNextPoint();
    return -1;
}

void Evaluator::setValNow(int _Species, double value){
    int a = findIndex(_Species, nextTimePoint);
    if(a < 0) {cerr << "ERR : SetValNow(" << _Species << ", ) for t=" << nextTimePoint << " not found in wish list\n"; return;}
    theData[a] = value;
    //cerr << "   ... data added : Sp=" << _Species << ", t=" << nextTimePoint << ", val=" << value << endl;
    recorded[a] = true;
}

int Evaluator::size(){
    return nbPoints;
}

typeTime Evaluator::nextPoint(){
    return nextTimePoint;
}

void Evaluator::resetDataKeepingExpected(){
    theData.clear();
    theData.resize(nbPoints, 0.0);
    recording_before_simulation = false;
    nextTimePoint = 1e8;
    recorded.clear();
    recorded.resize(nbPoints, false);
    updateNextPoint();
}

/* vector<int>         Species;
vector<typeTime>    Times;
/// additionally, the experimental (expected) and their standard deviation can be given & stored,
vector<double>      ExpectedValues;
vector<double>      StdDeviations;
/// table that have to be filled with data using setValNow (initially, recorded[..] = false
vector<double>      theData;
vector<bool>        recorded;
*/

double Evaluator::getExpVal(int _Species, typeTime _time_sec){
    if(recording_before_simulation){
        cerr << "ERR: Evaluator::getExpVal, you are still in 'recording phase' and you are not supposed to access any data at this point./n";
        return 0;
    }
    int a = findIndex(_Species, _time_sec);
    if(a < 0) {
        cerr << "ERR: Species = " << _Species << ", t = " << _time_sec << " sec, not found in evaluator record !!\n";
        return 0;}
    else return ExpectedValues[a];
}

double Evaluator::getStdDev(int _Species, typeTime _time_sec){
    if(recording_before_simulation){
        cerr << "ERR: Evaluator::getExpVal, you are still in 'recording phase' and you are not supposed to access any data at this point./n";
        return 0;
    }
    int a = findIndex(_Species, _time_sec);
    if(a < 0) {
        cerr << "ERR: Species = " << _Species << ", t = " << _time_sec << " sec, not found in evaluator record !!\n";
        return 0;}
    else return StdDeviations[a];
}

double boundedLog(double x){
    return 6.90776 + max(log(max(1e-8,x)), -6.90776);
}

double fitnessFunction(double valueToCompare, double refExpectedValue, double StdDev){
#ifdef SQUARE_COST
    return (refExpectedValue- valueToCompare) * (refExpectedValue - valueToCompare);
#endif
#ifdef SQUARE_COST_STD
    if(fabs(StdDev < 1e-8)) return (refExpectedValue - valueToCompare) * (refExpectedValue - valueToCompare);
    return (refExpectedValue- valueToCompare) * (refExpectedValue - valueToCompare) / StdDev;
#endif
#ifdef LOG_COST
    return fabs(boundedLog(fabs(refExpectedValue)) - boundedLog(fabs(valueToCompare)));
#endif
#ifdef LOG_COST_STD
    //if(fabs(StdDev < 1e-8)) return boundedLog(fabs(refExpectedValue- valueToCompare));
    return fabs(boundedLog(fabs(refExpectedValue)) - boundedLog(fabs(valueToCompare)));
#endif
#ifdef PROPORTION_COST
    if(fabs(fabs(refExpectedValue) < 1e-3)) return min(2.0, fabs(valueToCompare) * 1000);
    return min(2.0, fabs((refExpectedValue- valueToCompare) / refExpectedValue));
#endif
    cerr << "No cost function policy defined. Please put #define SQUARE_COST, SQUARE_COST_STD, LOG_COST, LOG_COST_STD, PROPORTION_COST in a file included by evaluator.cpp\n";
    return -1;
}

void testFitnessFunction(){
    vector<double> exp = {0, 0, 0.1, 0.1, 0.1, 1, 1, 1,      1.5,   1.5, 2,     5, 5, 5, 5,     10, 0.00001, 0.00001, 0.001};
    vector<double> mes = {0, 1, 0.1, 0.3, 1  , 1, 2, 0.0001, 1000,  0,   1.5,   3, 4, 5, 10,    100,0.0001,  0.001,    1};
    int L = exp.size();
    for(int i = 0; i < L; ++i){
        cout << std::setprecision(8) << "Exp= " << exp[i] << ",   \tmes= " << mes[i] << "   \tcost= " << fitnessFunction(mes[i], exp[i], 0) << "\t\t" << boundedLog(exp[i]) << "," << boundedLog(mes[i]) << endl;
    }
}




double Evaluator::getFitnessPoint(int _Species, typeTime _time_sec){
    int a = findIndex(_Species, _time_sec);
    if(a < 0) {
        cerr << "ERR: Species = " << _Species << ", t = " << _time_sec << " sec, not found in evaluator record !!\n";
        return 0;}
    if(! recorded[a]) return penaltyNotRecorded;
    return fitnessFunction(theData[a], ExpectedValues[a], StdDeviations[a]);
}

double Evaluator::getFitnessSpecies(int _Species){
    int nbVals = 0; double addCosts = 0;
    for(int i = 0; i < nbPoints; ++i){
        if(Species[i] == _Species){
            nbVals++;
            addCosts += fitnessFunction(theData[i], ExpectedValues[i], StdDeviations[i]);
        }
    }
    if(nbVals == 0) {
        cerr << "ERR: Evaluator::getFitnessSpecies(Species = " << _Species << "), species not found in evaluator record !!\n";
        return NAN;}
    return addCosts;// / nbVals;
}

double Evaluator::contains(int _Species){
    for(int i = 0; i < nbPoints; ++i){
        if(Species[i] == _Species){
            return true;
        }
    }
    return false;
}


double Evaluator::getTotalFitness(){
    int nbVals = 0; double addCosts = 0;
    for(int i = 0; i < nbPoints; ++i){
            nbVals++;
            addCosts += fitnessFunction(theData[i], ExpectedValues[i], StdDeviations[i]);
    }
    if(nbVals == 0) {
        //cerr << "ERR: Evaluator::getTotalFitness(), the evaluator has no time point !!\n";
        return NAN;}
    return addCosts;// / nbVals;
}



string Evaluator::reportTotalFitness(){
    int nbVals = 0; double addCosts = 0;
    stringstream ss;
    ss << "time\tvariable\tsimulation\tdata\tstddev\tlocalCost\n";
    for(int i = 0; i < nbPoints; ++i){
            nbVals++;
            addCosts = fitnessFunction(theData[i], ExpectedValues[i], StdDeviations[i]);
            ss << Times[i] << "\t" << Species[i] << "\t" << theData[i] << "\t" << ExpectedValues[i] << "\t" << StdDeviations[i] << "\t" << addCosts << endl;
    }
    if(nbVals == 0) {
        //cerr << "ERR: Evaluator::getTotalFitness(), the evaluator has no time point !!\n";
        return string("No Data for this evaluator.");}
    return ss.str();
}

