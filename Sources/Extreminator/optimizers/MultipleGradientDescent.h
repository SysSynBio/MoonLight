#ifndef OPT_MGD_H
#define OPT_MGD_H

// independant genetic alforithm
// parameters stored in the class 

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm> // for sorting in selection
#include "../optimizers/baseOptimizer.h"
#include "../solvers/baseSolver.h"
#include "../common/myRandom.h"
using namespace std;
using namespace OrganismLib;



///
/// @brief A general class implementing simple genetic algorithm with cross-over ans mutation
///

// Rem - In the class baseOptimizer, we have access to :
/*
	numIndex_ 				number of parameters to optimize : 
	indexVector_ (size = num_index)	indices of parameters we want to optimize
	numPara_				number of total parameters (total)
	paraLowVector_ and paraHighVector_ 
		(size = num_index)  		range for this parameters FOR INITIATION ONLY (after, we can diverge)
	

	The arguments(parameters) chosen for the oprimizer are in 
		argument_	arguments
		numArg_		size

	A table of INITIAL parameters proposed in the optimizer file are in :
		parameters_ [i][j]	size : numParaSet * num_index 
		numParaSet_		number of initial set of parameters given
	
	We should update regularly :
		globalOptima_	value of the best energy seen
		optimalParameter_	table of the best parameters seen
	
	function for doing that : 
		setGlobal(Energy)	that copy the current parameters in the organism to optimal parameter, and update globalOptima

	Functions available :
		getCost() : launch simulation, compute the total cost for these parameters, and return it.
	
	To change the parameters of the current/next simulation :
		setToCurrent(&population[whichInd]);
		Then, we can change only on the organism (not on the indovodual) by :
			setParameter(indexVector(position), newValue)

	To get the parameters of the current simulation
		double E = getCost();
		catchCurrentTo(E, &population[n+i]);
		
	We should implement the optimize() function, in our subclass of optimizer.
	
	
	
	*/

class individual4{
	private: 
		std::vector <double> parameters;
		double cost;
		
	public :
		individual4(){parameters.resize(0);};
		void resize(int newSize){parameters.resize(newSize);};
		double energy(){return cost;};
		double myParam(int i){ return parameters[i];};
		int mySize(){return parameters.size();};
		void setmyParam(int i, double val){ parameters[i] = val;}
		void setCost(double E) {cost = E;};
		void copy(individual4* j){
			int n = j->mySize(); 
			parameters.resize(n);
			for(int i = 0; i < n; ++i){
				setmyParam(i, j->myParam(i));
			}
			setCost(j->energy());
		}
};

class MultipleGradientDescent : public BaseOptimizer {

	private:
		int numGenMax;
		double myEpsilon;
		double myEta;
		double initialEta;
		double mydX;
		int maxAge;
		double myCoeffEta;
		int ageGradient;
		std::vector<double> gradient;
		
		void catchCurrentTo(double E, individual4* ind){
			ind->resize(numIndex_);
			for(int i = 0; i < (int) numIndex_; ++i){
				ind->setmyParam(i, parameter(indexVector_[i]));
			}
			ind->setCost(E);
		}
		
		void setToCurrent(individual4* ind){
			for(int i = 0; i < (int) numIndex_; ++i){
				setParameter(indexVector_[i], ind->myParam(i));
			}
		}
		
	public: 
		MultipleGradientDescent(BaseSolver *S, const std::string &file);

		void optimize();
		individual4 bestEver;
		individual4 current;
		double bestPrevious;
		void print_total_parameters();
		
		~MultipleGradientDescent(){};
		
};

MultipleGradientDescent::MultipleGradientDescent(BaseSolver *S, const std::string &file) :BaseOptimizer(S,file) {

	if (argument_.size() !=  6) { std::cerr << "Bad argument number (" << argument_.size() << ")\n"; exit(-1);};
	
	std::cerr << "Launch a Multiple Gradient Descent Optimizer." <<  argument_.size() << "\n";
	numGenMax = (int) argument_[0];
	myEpsilon = (double) argument_[1];
	mydX = (double) argument_[2];	
	myEta = (double) argument_[3];
	initialEta = myEta;
	maxAge = (int) argument_[4];
	myCoeffEta = (double) argument_[5];
	std::cerr << "\tParameters :\n\t   Nb Generations : " << numGenMax << "\n";
		
	std::cerr << "Data we have from the optimizer :\n" << 
			"We want to optimize " << numIndex_ << " parameters, of indices :\n";
	for(int i = 0; i < (int) numIndex_ ; ++i){ std::cerr << " " << indexVector_[i];}
	std::cerr << "\n";

	gradient.resize(numIndex_);
	
	std::cerr << "Total number of parameters : " << numPara_ << "\n";

}

void MultipleGradientDescent::print_total_parameters(){
	std::cerr << "List of Parameters : ";
	for(int i = 0; i < (int) numPara_; ++i){
		std::cerr << parameter(i) << " ";
	}
	std::cerr << "\n";
}

void MultipleGradientDescent::optimize(){
	randomizeParameter();
	ageGradient = 0;
	
	double E = getCost();
	bestPrevious = E;
	catchCurrentTo(E, &bestEver); 
	catchCurrentTo(E, &current); 

	
	for(int i = 0; i < numGenMax; ++i){
		
		
		// computes the gradient at this point (numIndex_ calls to getCost);
		for(int j = 0; j < (int) numIndex_; ++j){
			//double Edep = current.energy();
			double initialparam = parameter(indexVector(j));
			if(initialparam > 0){
				setParameter(indexVector(j),  initialparam * (1 + mydX));
				double E2 = getCost();
				setParameter(indexVector(j),  initialparam * (1 - mydX)); // if 0, then doesn't move anymore :-8
				double E3 = getCost();
				gradient[j] = (E2 - E3)/(2 * mydX * initialparam);
			} else gradient[j] = 0;
			setParameter(indexVector(j),  initialparam);
		}
		
		// Change the parameters accoding to the gradient
		// move = eta * gradient
		// but remains positive
		for(int j = 0; j < (int) numIndex_; ++j){
			if(isnan(gradient[j]) || (abs(gradient[j]) < 1e-12)){} //we must change all in order to keep the gradient direction || (abs((gradient[j] * myEta)/parameter(indexVector(j))) < myEpsilon)){}
			else {
				double newValue = parameter(indexVector(j)) - gradient[j] * myEta;
				setParameter(indexVector(j), max(0.0,newValue));
			}
		}

		// criterion of termination : if the moves are too small
		double maxGrad = abs((gradient[0] * myEta)/parameter(indexVector(0)));
		for(int j = 1; j < (int) numIndex_; ++j){
			maxGrad = max(maxGrad, abs((gradient[j] * myEta)/parameter(indexVector(j))));
		}
		
		// If too old or doesn't move anyöore, start random again
		if((maxGrad < myEpsilon) || (ageGradient > maxAge)){
			randomizeParameter();
			myEta = initialEta;
			std::cerr << "StartAgain" << (maxGrad < myEpsilon ? "(NullGradient)" : "") << (ageGradient > maxAge ? "(TooOld)" : "") << "";
		
			double E = getCost();
			if(E < bestEver.energy()){
				catchCurrentTo(E, &bestEver);	
			};
			bestPrevious = E;
			catchCurrentTo(E, &current); 
			ageGradient = 0;
		} else {
			double E = getCost();
			if(E < bestEver.energy()){
				catchCurrentTo(E, &bestEver);	
			};
			if(E < bestPrevious){
				if(E < (0.99 * bestPrevious)){ageGradient--;} else {myEta *= myCoeffEta;}
				bestPrevious = E;
				//std::cerr << "Success" << "\n"; 
			}
			else {
				// if it doesn't move so much, then go 
				myEta *= myCoeffEta;
				//std::cerr << "NO IMPROVEMENT \n";
				// ageGradient = maxAge + 1;
			}
		};
		std::cerr << "Etape " << i << "\t" << bestPrevious << "\t" << bestEver.energy() << "\t" << ageGradient << "\t" << myEta << "\t" << "Grad:\t";
		for(int j = 0; j < (int) numIndex_; ++j){
			std::cerr << gradient[j] << "\t";
		}
		std::cerr << "\n";		
		print_total_parameters();
		ageGradient++;
	}
}
	
#endif