#ifndef POPULATION_H
#define POPULATION_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm> // for sorting in selection
#include "individual.h"
#include "../common/myRandom.h"
#include "../common/myTimes.h"
using namespace std;

///
/// @brief A general class implementing simple genetic algorithm with cross-over ans mutation
///

namespace OrganismLib{
class Population {
	private:
		vector<individual*> pop;
	
	public:
		Population();	
		Population(int size_);
		void resize(int new_size);
		int size();
		individual* operator ()(int i);	
		static bool Compare(individual* a,  individual* b){
			return a->cost() < b->cost();}
		void sort();

	public : 
		double meanCost;		// to know the mean at any time. Sum and not mean because later the population size would change
		double bestLocalCost;
		double worstLocalCost;

		void doStatistics();
		void print();
		void printEnergies();
		
		void replace(int i, int by_j);
		void copy(Population* pop2){
			if(size() > 0) resize(0);
			int newSize = pop2->size();
			pop.resize(newSize);
			for(int i = 0; i < newSize; ++i){
				pop[i] = (*pop2)(i);
			}
		}
		double variance();
};
}

#endif