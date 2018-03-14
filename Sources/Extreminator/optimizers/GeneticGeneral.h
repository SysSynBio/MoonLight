#ifndef GENETICGENERAL_H
#define GENETICGENERAL_H 

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm> // for sorting
#include "../optimizers/baseOptimizer.h"
//#include "../solvers/baseSolver.h"
#include "../common/myRandom.h"
#include "../common/myTimes.h"
#include "population.h"
#include "Orthogonalise.h"
using namespace OrganismLib;

#include "Config.h"

#define DBG 0
///
/// @brief A general class implementing all the functions a genetic algorithm will need
///

class GeneticGeneral : public BaseOptimizer {

	private:
		Population population;
		//individual bestGlobal; 	They are carried by light Optimizer, and updated in each updateCost function
		// double worstGlobal;		Idem				// necessary for the cost corrections

#ifdef USE_BOOST
        BoostRandoms BR;
#else
        stdRandoms BR;
#endif
		void initRandoms();
		Configuration Config; 

	public: 
		GeneticGeneral(BaseOptimizationProblem *E, const std::string &file);
		GeneticGeneral(BaseOptimizationProblem *E, std::ifstream & IN);
		void optimize();
		~GeneticGeneral(){};

	private:
		// ========================== Base functions, where all parameters are explicit ==================================================
		void initializeRandomPop(int p_size);				// Creates population of popSize Randoms calling the mother class random function for an individual
		int killAgeing(int maxAgeAuthorized);				// Returns the number of deads
		void AgePopulation();						// Add one year old to all the individuals of the population
		void addRandom(int nb_to_add);

		// --------------------- Selection Variants Implementations ---------------------------
		void random_selection       (vector<int>* indices_selected, 	int num_groups_to_select, 	int num_parents_by_group, 													bool retake_allowed_between_groups,	bool same_parent_allowed_in_one_group);
		void proportional_selection (vector<int>* indices_selected, 	int num_groups_to_select, 	int num_parents_by_group, 	COST_NORMALISATION_T cost_normalization_type, 	SAMPLING_METHOD_T sampling_method, 	bool retake_allowed_between_groups,	bool same_parent_allowed_in_one_group,	double temperature = 0);
		void tournament_selection   (vector<int>* indices_selected, 	int num_groups_to_select, 	int num_parents_by_group,	int size_subpop, 										bool retake_allowed_between_groups,	bool same_parent_allowed_in_one_group);
		void rank_based_selection   (vector<int>* indices_selected, 	int num_groups_to_select, 	int num_parents_by_group,	 												bool retake_allowed_between_groups,	bool same_parent_allowed_in_one_group);		
		void selection_keeping_indices(vector<int>* indices_selected);	

		// --------------------- Cross-Over Variants Implementations --------------------------
		void cross_over_one_point(individual* fils, individual* parent1, individual* parent2);
		void cross_over_two_point(individual* fils, individual* parent1, individual* parent2);
		void cross_over_wright1(individual* fils1, individual* fils2, individual* fils3, individual* parent1, individual* parent2);
		void cross_over_wright2(individual* fils, individual* parent1, individual* parent2);
		void cross_over_arithmetic(individual* fils, individual* parent1, individual* parent2, double gamma = 0.5);
		void cross_over_arithmetic(individual* fils, vector<individual*>* parents, vector<double>* gamma = NULL);
		void cross_over_BLXAlpha(individual* fils, individual* parent1, individual* parent2, double alpha = 0.5);
		void cross_over_geometric(individual* fils, individual* parent1, individual* parent2, double gamma = 0.5);
		void cross_over_geometric(individual* fils, vector<individual*>* parents, vector<double>* gamma = NULL);
		void cross_over_SBX(individual* fils1, individual* fils2, individual* parent1, individual* parent2, double eta = 1);
		void cross_over_UNDX(individual* fils1, individual* parent1, individual* parent2, individual* parent3);
		void cross_over_UNDX(individual* fils1, individual* parent1, individual* parent2, individual* parent3, individual* parent4);
		void simple_random(individual* fils);

		// --------------------- Mutation Variants Implementations ----------------------------
		void mutate_one_point(individual* ind, int type_distrib, int position = -1);		// Additive mutations
		void mutate_all_points(individual* ind, int type_distrib); 				// Additive mutations



		// ==================== Upper Level functions that use the Config values to call the appropriate Base Function ======================
		void AddMutants(int n_to_add);
		bool AddMutants(double n_to_add);
        void AddOffspring(int nb, Population* pop_destination = NULL, bool dont_compute_cost = false, bool save_parents = false);
        void AddMutatedOffspring(int n_to_add, Population* pop_destination = NULL, bool dont_compute_cost = false, bool save_parents = false);
        void AddMutatedOffspringAndReplace(int n_iterations); // let's take n_it = p_c * size_pop    is the p_c necessary????, int proba_cross_over);
        void selectionParents(vector<int>* indices_selected, int offspring);
		void selection(int finalPopSize); 

		void updateSigmas(int generation, int genMax);

		// =========================================== Accessory functions ================================
		double temperature;
		bool metropolis(double E1, double E2);
		void MegaTest();
		
        double GGA(int popSize, int maxCalls, double prcOffspring, int speak = 0);
        double CEP(int popSize, int maxCalls, double prcOffspring, double forkCoeff, int speak = 0);
        double SSGA(int popSize, int maxCalls, double prcOffspring, int speak = 0);
		
		void testOperators(int popSize, double propCross, double forkCoeff, int num_tries, int maxCalls);
		
		int type_of_algo;

        // for statistics
        void Initialize();
        int crossOverSuccess;
        int crossOverFail;
        int mutationSuccess;
        int mutationFail;
        individual* bestOverall;
};

#endif
