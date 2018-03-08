#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

// independant genetic alforithm
// parameters stored in the class 

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm> // for sorting in selection
#include "../optimizers/baseOptimizer.h"
//#include "../solvers/baseSolver.h"
#include "../common/myRandom.h"
using namespace OrganismLib;


///
/// @brief A general class implementing simple genetic algorithm with cross-over ans mutation
///

// Rem - In the class baseOptimizer, we have access to :
/*
	numIndex() 				number of parameters to optimize : 
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


class individualOld{
	private: 
		std::vector <double> parameters;
		double cost;
		
	public :
		individualOld(){parameters.resize(0);};
		void resize(int newSize){parameters.resize(newSize);};
		double energy(){return cost;};
		double myParam(int i){ return parameters[i];};
		int mySize(){return parameters.size();};
		void setmyParam(int i, double val){ parameters[i] = val;}
		void setCost(double E) {cost = E;};
		void copy(individualOld* j){
			int n = j->mySize(); 
			parameters.resize(n);
			for(int i = 0; i < n; ++i){
				setmyParam(i, j->myParam(i));
			}
			setCost(j->energy());
		}
};

class GeneticAlgo : public BaseOptimizer {

	private:
		int popSize;
		int numGenMax;
		int maxCalls;
		int numOffspring;
		int numMutations;
		int generation;
		int num_tries;
		double coeff_fork;
		
		std::vector<individualOld> population;

		void catchCurrentTo(double E, individualOld* ind){
			ind->resize(numIndex());
			for(size_t i = 0; i < numIndex(); ++i){
				ind->setmyParam(i, parameter(i));
			}
			ind->setCost(E);
		}
		
		void setToCurrent(individualOld* ind){
			for(size_t i = 0; i < numIndex(); ++i){
				setParameter(i, ind->myParam(i));
			}
		}

		double meanCost;		// to know the mean at any time. Sum and not mean because later the population size would change
		double bestLocalCost;
		double worstLocalCost;
		size_t bestLocalIndividual;
		individualOld bestGlobalIndividual;
		
	public: 
		GeneticAlgo(BaseOptimizationProblem *E, const std::string &file);
		GeneticAlgo(BaseOptimizationProblem *E, std::ifstream &IN);
		void construct();
		void initializeRandom();
		void optimize();
		
		// individualOld crossover(individualOld ind1, individualOld ind2);
		void selection(); 
		~GeneticAlgo(){};
		void printPop();
		void Mutate();
		void Fork();
		void cross_over(int ind_parent1, int ind_parent2);
		void  print_total_parameters();
		void print_evolution();
		double variancePop();
		void update(); // update : best global, mean, best ever

};

#endif