#ifndef INDIV_H
#define INDIV_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm> // for sorting in selection

///
/// @brief A general class implementing simple genetic algorithm with cross-over ans mutation
///

#include <iostream>
#include <fstream>
#include "../common/myFiles.h"
#include "../common/myRandom.h"
#include <vector>
using namespace std;

int genID();

class individual{
	private:
		int ID;
        int nGenes_;
		std::vector <double> Genome_;
		std::vector <double> StepSize_;
		int age_;
		vector<vector<double> > OtherValues_;
		double modified_;			// true if the cost is valid, false if we have changed something after last cost call

    private:                //that we cam trust only when modified_ = false
							// Be aware that only Genome_/gene() is always up to date. EquivParams is 
							// Computed before the fitness calculation
							// However, if modified = false, then we are sure that EquivParam and Genes are updated one from another
		std::vector <double> EquivParam_;	// Non scaled version of the genes (scaled).
        double cost_;       // to not compute again each time
		
	public:
        int myID(){return ID;}
		
		individual();
		void copy(individual* i2);
		individual(individual* i2);
		individual(vector<double>* newGenome);
		individual(vector<double>* newGenome, vector<double>* newStepSizes);

		int size();
		void resize(int n);
				
		double gene(int i);
		double step(int i);
		double equivparam(int i);
		void setGene(int i, double value);
		void setStepSize(int i, double value);
		void setGenome(vector<double>* newGenome);
		void setStepSizes(vector<double>* newStepSizes);
		void setEquivParam(int i, double value);
		void setEquivParams(vector<double>* newEquivParams);
		
		void ComputeEquivParamsFromGenes(double (*scale_from_intern_to_solver)(double, vector<double>*), vector<double>*);
		void ComputeGenesFromEquivParams(double (*scale_from_intern_to_solver)(double, vector<double>*), vector<double>*);
		
		vector<double>* genome();
		vector<double>* stepSizes();
		vector<double>* equivParams();
		
		bool modified();
		double cost();
		void setCost(double E);
		
		int age();
		void setAge(int newAge);
		void ageing();
		
		bool improve(individual* contestant);

		void print();

		vector<vector<double> >* OtherValues();
		void setOtherValues(vector<vector<double> >*);
		
		vector<int> parents;
		int num_parents();
		void add_parent(int p);
		int parent(int i);
		void clearParents();
};


#endif
