//
// Filename     : baseOptimizer.h
// Description  : Base class defining optimization
// Author(s)    : Pontus Melke (pontus@thep.lu.se) - Philippe Robert (philippe.robert@ens-lyon.org)
// Created      : August 2006
// Revision     : $Id: baseOptimizer.h 474 2010-08-30 15:23:20Z henrik $
//

#ifndef BASEOPTIMIZER_H
#define BASEOPTIMIZER_H

// to give full information on the beginning.
#define VERB 1

#include <iostream>
#include <fstream>

#include "../common/myFiles.h"
#include "../common/myRandom.h"
#include "../common/statistiques.h"
//#include "../solvers/baseSolver.h"
//#include "../cost/baseEstimator.h"
#include "../baseProblem.h"
#include "individual.h"
#include "population.h"
#include "scalingFunctions.h"
using namespace OrganismLib;

///
/// @brief A base class using a factory method to create optimizers. 
///
/// The baseOptimizer class is a general mother class for optimizer, providing an interface to the cost function.
/// Optimizers are a subclass of baseOptimizer.
/// The class baseOptimizer monitors :
/// 1- the best individual (optimizers have to update)
/// 2- the index of parameters to optimize. (optimizers only see the parameters to optimize and work on this reduced
///    dimension space only)
/// 3- the scaling of parameters (optimizers work on an 'intern scale' defined by the user, and the conversion is
///    done automatically when asking the cost)
///
/// To create a new instance of an optimizer, you must call the function 
/// BaseOptimizer::createOptimizer(baseEstimator* E, const std::string &file), 
///
/// This function will call in turn the constructor of BaseOptimizer. DO NOT CALL the
/// constructor yourself. 
///
/// This function takes a pointer to an Estimator E (containing the cost function to optimize) and a text file which contains
/// optimization parameters (name of the optimizer type, numeric arguments 
/// for the optimizer, bounds for the parameters, and initial set of parameters 
/// (for seed-based optimizers). 
///
/// Each derived class must implement a constructor that checks if the
/// optimization parameters are correct and a funtion called optimize which
/// defines the desired optimization procedure.
///
/// @note Example :
///
/// For optimization only, you can use the subclass 'GeneralImplementation(dimension)' which stores a set of parameters (double),
/// and implement your 'getCost()' function inside :
///
/// \code
///
/// #include "optimizers/baseOptimizer.h"
/// #include "baseProblem.h"
///
/// class Essai : public GeneralImplementation {
///     Essai() : GeneralImplementation(3) {}
///     double getCost(){
///         /* the parameters are stored in parameters[i] */
///         return (parameters[0] + parameters[1] - 2.0) * (parameters[2] + parameters[1] - 1.0);
///     }
/// };
///
/// int main(void){
///     std::string optimizer = "optimizer file"
///     BaseOptimizationProblem *E = new Essai();
///     BaseOptimizer *Opt = BaseOptimizer::createOptimizer(E, optimizer);
///     Opt->optimize();
///     Opt->bestGlobal.print();
///     delete E;
///     delete Opt;
/// }
///  \endcode
///
/// And for optimising a network simulated with organism,
///
/// \code 
///
///  std::string simPara="simulator file"
///  std::string analyzerFile = "solver file"
///  std::string estimatorFile = "estimator file"
///  std::string optimizer = "optimizer file"
///  
///  Organism O(modelFile);
///  BaseSolver *S = BaseSolver::getSolver(&O, simPara);
///  BaseEstimator *E = new BaseEstimator(S, analyzerFile);
///  BaseOptimizer *Opt = BaseOptimizer::createOptimizer(E, optimizerFile);
///
///  Opt->optimize();
/// 
///  delete S;
///  delete E;
///  delete Opt;
///
///  \endcode
///
///
///	In the original (and real) scaling, as they will be given to the solver
///	In a scaled way, which is the only way the optimization algorithms will access them. 
/// 
/// I will speak of "Non Scaled" for the solver type of parameters, and of "Intern Scaled" 
///     for the scaled values accessible by the sub classes of baseOptimizer
///

class BaseOptimizer {
	protected:
        int debug_live;
		double worstGlobal;
	private:
		double (*scaling_function_intern_to_solver)(double, vector<double>*);
		double (*scaling_function_solver_to_intern)(double, vector<double>*);
		vector<double>* scaling_parameters;
		int CostCalls;				// Number of cost function evaluation. All calls to a cost must be called through computeCost functions, and the number of cost calls will be upgraded automatically
	public:	//protected
		individual bestGlobal;			// The best set is stored into an individual	

		
	protected:
		/// Main constructor, each derived class must call this constructor!
		/// Should not be called by the user directly. Use createOptimizer instead.
		BaseOptimizer(BaseOptimizationProblem *E, const std::string &file);
		BaseOptimizer(BaseOptimizationProblem *E, std::ifstream &IN); // The reading point should be just after IDVALUE
	
	public:
		///
		/// @brief Main factory creator, all creation should be mapped onto this one
		///
		/// Given the idValue an optimizer of the defined type is returned (using
		/// the constructor of the corresponding sub Class). 		
		///
		/// @param E is a pointer to a cost Estimator
		///
		/// @param file is a file containing which optimizer that should be used,
		/// arguments for the optimizer, and initial set of parameters if needed
		///
		/// @return Returns a pointer to an instance of an optimizer class defined
		/// in file
		///
		static BaseOptimizer* createOptimizer(BaseOptimizationProblem* E, const std::string &file);
		static BaseOptimizer* createOptimizer(BaseOptimizationProblem* E, std::ifstream &IN); // The reading point should be just before IDVAlue
		

		///
		/// @brief Function that reads the optimizer file
		///		
		virtual void optimize();

		virtual ~BaseOptimizer();
	
	protected:
		///
		/// @brief Function that reads the optimizer file
		///
		/// Is called by createOptimizer. Don't call it yourself, because the type of the 
		/// optimizer can change. Call createOptimizer instead. 
		///
		/// The general structure of the optimizer file should be :
		///
		/// \verbatim 
		/// 	IdOptimizer		Number_of_Arguments
		///	Argument1	Argument2	...	# comments
		///	Argument3	...
		///
		///	Number of parameters to optimize
		///	indice_first_parameter	lower_boundary	upper_boundary
		///	indice_second_parameter	lower_boundary	upper_boundary
		/// 	...
		///	
		///	Number_of_initial_parameter_sets
		///	set1_param1	set1param1	...
		///	set2_param1	set2param1	...
		///
		/// \endverbatim
		///
		void readOptimizer(const std::string &file);
		void readOptimizer(std::ifstream &IN); // the reading point of IN should be JUST AFTER THE IDVALUE
		
		
		
    public:
    //private:
		// -------- Setting Up Informations  (that will not change) -------------
		BaseOptimizationProblem *E_;				// Estimator to get a cost value. An estimator stores Init, Cost and Parameters.
		size_t numIndex_;				// Number of parameters to optimize
		std::vector<size_t> indexVector_;		// Indices of the parameters to opimize (in the total amount of parameters)
		size_t numArg_;					// Number of arguments given in the optimizer file
		std::vector<double> argument_;			// List of arguments given in optimizer file
		std::vector<double> paraLowVector_;		// Lower bound for each parameter to optimize
		std::vector<double> paraHighVector_;		// Upper bound for each parameter to optimize
		size_t numParaSet_;				// Number of initial set of parameters suggested by optimizer file (for seed-based optimizers)
		std::vector< std::vector<double> > parameters_;	// list of these sets of parameters
		inline size_t indexVector(size_t i) const;

		std::vector< std::vector<double> > NotScaledParameters_;	// list of these sets of parameters
		std::vector<double> paraNotScaledLowVector_;		// Lower bound for each parameter to optimize
		std::vector<double> paraNotScaledHighVector_;		// Upper bound for each parameter to optimize

	public: //protected
		///	To be used by subclasses : number of parameters to optimize
		inline size_t numIndex() const;
		///	To be used by subclasses : number of initial paremeters sets
		inline size_t numParaSet() const;
		///	To be used by subclasses : to get the ith set of initial parameters
		inline vector<double>* parameterSet(int i);
		///	To be used by subclasses : number of arguments for the optimizer		
		inline size_t numArguments() const;
		///	To be used by subclasses : value of argument number i 
		inline double argument(size_t i) const;
		///	To be used by subclasses : lower boundary for parameter i
		inline double paraLowVector(size_t i) const;
		///	To be used by subclasses : upper boundary for parameter i
		inline double paraHighVector(size_t i) const;
		///	To be used by subclasses : lower boundary for parameter i
		inline double paraNotScaledLowVector(size_t i) const;
		///	To be used by subclasses : upper boundary for parameter i
		inline double paraNotScaledHighVector(size_t i) const;
		
		///
		/// @brief Resets the number of total cost calls
		///		
		void resetCostCalls();
		
		///
		/// @brief return the number of cost calculation done
		///
		/// The number of cost calls is autoöatically updated each time a cost is called.
		int nbCostCalls();
		
		///
		/// @brief Prints the optimal cost and corresponding parameter values
		///
		void printOptima(std::ostream &os = std::cout);

		///
		/// @brief Value of the current best cost
		///		
		double globalOptima();
		
		///
		/// @brief Value of the ith optimal parameter (all parameters, not only those to optimize)
		///		
		double optimalParameter(int i);

		///
		/// @brief Value of the current best parameters (ONLY THE SUBSET OF PARAMETERS TO OPTIMIZE, NOT ALL OF THEM !)
		///		
		void getOptimalParameters(vector<double>* vector_to_put_optimal_optimized_parameters);

		///
		/// @brief Value of the current best parameters (ALL PARAMETERS, INCLUDING THOSE NOT TO OPTIMIZE)
		///		
		void getAllOptimalParameters(vector<double>* vector_to_put_optimal_total_set_of_parameters);
	
	public:	//protected
		/// --------  Multiple possible interfaces are possible with subclasses (and in order to maintain all optimizers)  -------
		
		/// 1 : recommanded interface : 
		///	- The optimizer (subClass) works only in scaled mode and has no idea of how the scaling is done.
		///	- The optimizer works on its own memory and dialogs with the cost function via the vector of arguments each time (instead of working inside the estimator memory)
		///
		/// 1a : ------- With a dialogue by individual type -------------
		///
		///	To be used by subclasses : Gets the cost of the individual's parameters and sets its cost value. Note that, if the cost of 
		///	this individual is already known, no cost call will be done.
		///	(information stored in the field individual::modified checks if the parameters have been
		///	changed since last cost update.)
		void updateCost(individual* ind);
			
		///	To be used by subclasses : Creates a new individual with random parameters in the correct boundaries
		individual* newRandomIndividual();

		///	Takes an individual and randomize each parameter according to the correct boundaries
		void randomize(individual* i2);

		///	subfunction that calls Estimator::getCost() and returns the cost of an individual, but
		///	doesn't update the field cost of individual.
		///	Obs : Constraint handling done by adding max(1e5, -1 + t * exp(distance to boundaries in scaled space))
		double computeCost(individual* ind);

		
		/// 1b : -------- With a dialogue via vector of parameters. Warning : these vectors should have size of numIndex() (i.e. the number of parameters to optimize).
		///
		///	To be used by subclasses : Gives a new global optima and its associated cost. 
		///	 No check is done : the previous bestGlobal will be erased even if better.
		void setGlobal(double cost, vector<double>* new_parameters);
		
		///	To be used by subclasses : Gives a new global optima and its associated cost.
		///	bestGlobal is improved only if the proposed set of parameters is better
		void improveGlobal(double cost, vector<double>* new_parameters);

		/// 	To be used by subclasses : Returns the cost of a list of parameters.
		///	Obs : Constraint handling done by adding max(1e5, -1 + t * exp(distance to boundaries in scaled space))
		double computeCost(vector<double>* parameters);
		
		/// 	To be used by subclasses : Returns a new vector of randomized values for each parameter, according
		///	the lower and upper bounds
		void randomizeSet(vector<double>* toRandomize);
		
		/// 2 : The same interface but in non scaled values
		///
		///	Gives a new global set of parameters, in the solver real range (not scaled)
		void setGlobalNotScaled(double cost, vector<double>* v);

		///	Idem but in real range solver scaled
		void improveGlobalNotScaled(double cost, vector<double>* v);

		///	Idem for a non scaled (real solverr range) parameters
		///	OBS : WARNING : No constraint handling penality is done.
		double computeCostNotScaled(vector<double>* parameters);

		///	Warning ! Here, the uniform values are taken in the whole solver range, which is different from doing a uniform in the intern scaled range and convert after.
		void randomizeSetNotScaledUniform(vector<double> * toRandomize); // Intern Scaled Version WARNING ! This is uniform on the non scaled range, which is different from randomized in intern scale plus converted to non scaled solver type
		
		/// 3 : Interface working directly on the cost function memory and assuming he is the only one to change it. Older functions that work on the memory of the estimator / solver
		///
		///	To be used by subclasses : Implicitely says that the current list of parameters of the estimator is 
		///	a global optima and gives its cost. No check is done : the previous bestGlobal will be erased even if better.
		void setGlobal(double cost);

		///	To be used by subclasses : replaces bestGlobal by the current set of parameters of the estimator with the given 
		/// 	cost, only if this cost is better.
		void improveGlobal(double cost);

		/// 	To be used by subclasses : Directly change the parameters inside the estimator
		void setParameter(vector<double>* v);

		///	Not Scaled Version
		void setParameterNotScaled(vector<double>* v);

		///	To be used by subclasses : Directly change the parameter number indexVector(i) inside
		/// 	the estimator/solver.
		void setParameter(int i, double value);

		///	Not Scaled Version
		void setParameterNotScaled(int i, double value);
		
		///	To be used by subclasses : Randomize the values of each parameter to optimize inside the estimator
		///	itself, and according to lower and upper bounds.
		void randomizeParameter();
	
		///	Not scaled version Warning : uniform in the whole real range
		void randomizeParameterUsingNotScaledUniform();

		///	To be used by subclasses : Returns the cost of the current set of parameters inside the estimator
		///	WARNING : No Constraint Handling penalty done
		double computeCost();
		
		///	To be used by subclasses : Returns the parameter at position IndexVector(i)
		double parameter(int i);

		double parameterNotScaled(int i);

		/// 	Prints the current total list of parameter values
		void printState(double E, std::ostream &os = std::cout);

        bool InsideBoundaries(vector<double>* intern_parameters);
        bool InsideBoundariesNotScaled(vector<double>* parameters);
        bool InsideBoundaries(individual* ind);

};


inline size_t BaseOptimizer::indexVector(size_t i) const
{
	return indexVector_[i];
}    

inline size_t BaseOptimizer::numIndex() const
{
	return numIndex_;
}

inline size_t BaseOptimizer::numParaSet() const
{
	return numParaSet_;
}

inline vector<double>* BaseOptimizer::parameterSet(int i)
{
	return &parameters_[i];
}

inline size_t BaseOptimizer::numArguments() const
{
	return numArg_;
}

inline double BaseOptimizer::argument(size_t i) const
{
	return argument_[i];
}    

inline double BaseOptimizer::paraLowVector(size_t i) const
{
	return paraLowVector_[i];
}    

inline double BaseOptimizer::paraHighVector(size_t i) const
{
	return paraHighVector_[i];
}    

inline double BaseOptimizer::paraNotScaledLowVector(size_t i) const
{
	return paraNotScaledLowVector_[i];
}	

inline double BaseOptimizer::paraNotScaledHighVector(size_t i) const
{
	return paraNotScaledHighVector_[i];
}


#endif 
