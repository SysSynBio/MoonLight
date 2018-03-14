// Philippe Robert, 19-09-2014 - philippe-robert.com
// Evaluator.h and Evaluator.cpp are independent files

#ifndef EVALUATOR_H
#define EVALUATOR_H


#define SQUARE_COST
// #define SQUARE_COST_STD
//#define LOG_COST
//#define LOG_COST_STD
//#define PROPORTION_COST

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>      // std::setprecision
#include <cmath>
#include <utility>      // std::pair
#include <sstream>
using namespace std;

#include "tableCourse.h"




///============================ WISH-COST-LIST========================
/// this structure stores a list of values (species-time point) to be compared with simulations later.
/// basically, you say first which couples of (time-point, variable) you want to record,
/// (you can give expected values, it'soptional). During the simulation, you provide the simulation values
/// and after the simulation, you can call to get the values - it avoids to record a whole simulation
///
/// Bref,
///     - before the simulation, call       'double getVal(species, time, expectedValue = 0, stdDev = 0)' to add this point in the wish list
///         (you don't need to provide an expected value nor a standard deviation)
///     - to complete the wish list, call   'void recordingCompleted();'
///     - during the simulation, call       'bool takeDataAtThisTame(int t_sec)' to see if
///         there are wished data at this time point (or earlier time-points that were missed)
///     - then, call                        'int speciesToUpdate();'  to know which vaiables are desired
///         for this time-point (continue until the function gives -1, meaning 'no more species desired'
///     - for each variable, call           'void setValNow(int _Species, double value); to give
///         the value to the evaluator
///     - then, each recorded data can be accessed by   'double getVal(species, time)'
///         note that this is the same function for asking and getting, allowing to
///         call the same cost function before simulation (to record wishes) and after simulation
///         (to compute a cost based on the simulated values) - and this doesn't require to save
///         the whole kinetics of simulation.
///     - to re-access the expected value, for each, call 'double getExpVal(species, time)'
///
///  --> it is not made to save time, but :
/// 	- to save memory : no time kinetics are stored excepted wish values
///     - to be easy programmed : if you use getVal, you use the same function to say you want a
///         data point before simulation, and to get the result after simulation, so you can
///         design only one cost function and to call it twice, before and after simulation
///
///     see example at the end,
///

typedef int iteratorForEvaluator;
double fitnessFunction(double measured, double expected, double StdDev_or_zero);
void testFitnessFunction();

struct Evaluator {
public:Evaluator();


public:
    double getVal(int _Species, typeTime _time_sec, double _expectedValue = 0, double _stdDev = 0);
    /// means : 'I want the value of this species at this time point'
    /// during recording phase, returns 0 but stores the (species, time-point) in the wish list
    /// when recording is completed, returns the recorded value.

    bool getValsFromKinetics(TableCourse* _expectedValues, TableCourse* _StdDevs, vector<string> NamesVariablesInTheirIndex);
    /// Note: the data points with NAN or inf are not taken into the list of "wish list to record" points.
private:
    bool recording_before_simulation;
public:
    void recordingCompleted();
    /// means : the wish list is complete, now I can use 'takeDataAtThisTime?' and 'setValNow'

private:
    int findIndex(int _Species, typeTime _time_sec);
    /// to see if a time is in the wish list. returns -1 if not found

private:
    // list of pairs<species, time> wanted
    /// nbPoints = nb couples (Species[i], Times[i])
    int nbPoints;
    vector<int>         Species;
    vector<typeTime>    Times;
    /// additionally, the experimental (expected) and their standard deviation can be given & stored,
    vector<double>      ExpectedValues;
    vector<double>      StdDeviations;
    /// table that have to be filled with data using setValNow (initially, recorded[..] = false
    vector<double>      theData;
    vector<bool>        recorded;
public:
    int size();

private:
    typeTime nextTimePoint;
private:
    typeTime updateNextPoint();
    /// gives the next wanted time-point (minimum of not-recorded-yet time points) - when finished, returns 1e8
public:
    typeTime nextPoint();

public:
    bool takeDataAtThisTime(typeTime t_sec);
    /// says if data is required at this time point (says yes the first time point that goes after)
    /// example : if t=10 is required, if you ask for 8,9,10,11, you get false, false, true, true
    ///         but if you ask for 9,11,13, you get false, true, false

    int speciesToUpdate();
    /// gives the next wanted species for the current 'nextTimePoint' ; gives -1 when time point is completed,
    /// you need to have updated all the values and then the last call of this function will update the next point automatically

    void setValNow(int _Species, double value);
    /// to fill the value for a couple (Species-Value)    

    double contains(int _Species);
    double getExpVal(int _Species, typeTime _time_sec);
    double getStdDev(int _Species, typeTime _time_sec);
    double getFitnessPoint(int _species, typeTime _time_sec);
    double getFitnessSpecies(int _species);
    double getTotalFitness();
    string reportTotalFitness();

    string printState();

    void resetDataKeepingExpected();

    iteratorForEvaluator begin(){return 0;}
    iteratorForEvaluator end(){return nbPoints;}
    pair<int, typeTime> operator()(iteratorForEvaluator i){return pair<int, typeTime>(Species[i], Times[i]);}
};


/// Example :
/*
 void testeEvaluator() {
     Evaluator E = Evaluator();
     E.getVal(1,15);
     E.getVal(3,20);
     E.getVal(1,20);
     E.getVal(5,50);

     E.recordingCompleted();

     //during simulation
     //E.printState();
     for(int i = 0; i < 100; ++i){
         if(E.takeDataAtThisTime(i)){
             int z = E.speciesToUpdate();
             while(z >= 0){
                 E.setValNow(z, (double) (i + z*100));
                 z = E.speciesToUpdate();
             }
             //E.printState();
         }
     }
     cerr << "\n";
     cerr << E.getVal(1,15) << "\t";
     cerr << E.getVal(3,20) << "\t";
     cerr << E.getVal(1,20) << "\t";
     cerr << E.getVal(5,50) << "\n";
     cerr << E.printState();
 }

*/
#endif // EVALUATOR_H
