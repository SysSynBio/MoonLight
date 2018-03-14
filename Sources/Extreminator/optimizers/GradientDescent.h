#ifndef OPT_GD_H
#define OPT_GD_H

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

class GradientDescent : public BaseOptimizer {
	public:
	GradientDescent(BaseOptimizationProblem *E, const std::string &file) : BaseOptimizer(E, file){};
    GradientDescent(BaseOptimizationProblem *E, std::ifstream &F_IN) : BaseOptimizer(E, F_IN){};
	~GradientDescent(){};
	void optimize(){};
};


// Rem - In the class baseOptimizer, we have access to :
/*
	numIndex_ 				number of parameters to optimize : 
	indexVector_ (size = num_index)	indices of parameters we want to optimize
	numPara()				number of total parameters (total)
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

/*
class individual3{
	private: 
		std::vector <double> parameters;
		double cost;
		
	public :
		individual3(){parameters.resize(0);};
		void resize(int newSize){parameters.resize(newSize);};
		double energy(){return cost;};
		double myParam(int i){ return parameters[i];};
		int mySize(){return parameters.size();};
		void setmyParam(int i, double val){ parameters[i] = val;}
		void setCost(double E) {cost = E;};
		void copy(individual3* j){
			int n = j->mySize(); 
			parameters.resize(n);
			for(int i = 0; i < n; ++i){
				setmyParam(i, j->myParam(i));
			}
			setCost(j->energy());
		}
};

class GradientDescent : public BaseOptimizer {

	private:
		int numGenMax;
		double myEpsilon;
		double myEta;
		double mydX;
		std::vector<double> gradient;
		
		void catchCurrentTo(double E, individual3* ind){
			ind->resize(numIndex());
			for(size_t i = 0; i < numIndex(); ++i){
				ind->setmyParam(i, parameter(i));
			}
			ind->setCost(E);
		}
		
		void setToCurrent(individual3* ind){
			for(size_t i = 0; i < numIndex(); ++i){
				setParameter(i, ind->myParam(i));
			}
		}
		
	public: 
		GradientDescent(BaseOptimizationProblem *E, const std::string &file);

		void optimize();
		individual3 bestEver;
		individual3 current;
		void print_total_parameters();
		
		~GradientDescent(){};
		
};

GradientDescent::GradientDescent(BaseOptimizationProblem *E, const std::string &file) :BaseOptimizer(E,file) {

	if (numArguments() !=  4) { std::cerr << "Bad argument number (" << numArguments() << ")\n"; exit(-1);};
	
	std::cerr << "Launch a Simple Gradient Descent Optimizer." << numArguments() << "\n";
	numGenMax = (int) argument(0);
	myEpsilon = (double) argument(1);
	mydX = (double) argument(2);	
	myEta = (double) argument(3);
	std::cerr << "\tParameters :\n\t   Nb Generations : " << numGenMax << "\n";
		
	std::cerr << "Data we have from the optimizer :\n" << 
			"We want to optimize " << numIndex() << " parameters\n";
	//for(int i = 0; i < (int) numIndex() ; ++i){ std::cerr << " " << indexVector_[i];}
	std::cerr << "\n";

	gradient.resize(numIndex());
	
	std::cerr << "Total number of parameters : " << E_->numPara() << "\n";

}

void GradientDescent::print_total_parameters(){
	std::cerr << "List of Parameters : ";
	for(int i = 0; i < (int) E_->numPara(); ++i){
		std::cerr << E_->parameter(i) << " ";
	}
	std::cerr << "\n";
}

void GradientDescent::optimize(){
	randomizeParameter();
	
	double E = E_->getCost();
	catchCurrentTo(E, &bestEver); 
	catchCurrentTo(E, &current); 

	
	for(int i = 0; i < numGenMax; ++i){
		for(int j = 0; j < (int) numIndex(); ++j){
			//double Edep = current.energy();
			double initialparam = E_->parameter(indexVector(j));
			E_->setParameter(indexVector(j),  initialparam * (1 + mydX));
			cerr << ":";
			double E2 = E_->getCost();
			E_->setParameter(indexVector(j),  initialparam * (1 - mydX));
			double E3 = E_->getCost();
			cerr << "|";
			gradient[j] = (E2 - E3)/(2 * mydX * initialparam);
			E_->setParameter(indexVector(j),  initialparam);
		}
		for(int j = 0; j < (int) numIndex(); ++j){
			//setParameter(indexVector(j), parameter(indexVector(j)) - gradient[j] * myEta);
			if(isnan(gradient[j]) || (abs(gradient[j]) < myEpsilon)){}
			else E_->setParameter(indexVector(j), E_->parameter(indexVector(j)) - gradient[j] * myEta);

		}
		double E = E_->getCost();			
		if(E < bestEver.energy()){
			catchCurrentTo(E, &bestEver);
			std::cerr << "Success" << "\n";
		} else cerr << "\n";			
		std::cerr << "Etape " << i << "\t" << bestEver.energy() << "\tGrad:\t";
		for(int j = 0; j < (int) numIndex(); ++j){
			std::cerr << gradient[j] << "\t";
		}
		std::cerr << "\n";		
		print_total_parameters();
	}
}*/
	
#endif
