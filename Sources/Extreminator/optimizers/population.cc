#include "population.h"
using namespace OrganismLib;


Population::Population(){
	pop.clear();
}
		
Population::Population(int size_){
	pop.resize(size_);
	for(int i = 0; i < size_; ++i){
		individual* indi = new individual();
		pop[i] = indi;
	}
}

void Population::resize(int new_size){
	int old_size = pop.size();
	if(old_size < new_size){
		pop.resize(new_size);
		for(int i = old_size; i < new_size; ++i){
			individual* indi = new individual();
			pop[i] = indi;
		} 
	}
	if(old_size > new_size){
		for(int i = new_size; i < old_size; ++i){
			if(pop[i]) delete pop[i];
		}
		pop.resize(new_size);
	}
}
int Population::size(){
	return pop.size();
}
// It is therefore possible to change the content of this individual directly with the pointer,
// but the class here keep the way to storing pointers, in particular for sorting the population, etc.
individual* Population::operator ()(int i){
	return pop[i];
}

void Population::sort(){
	// magical, isn't it ?
	std::sort(pop.begin(), pop.end(), Compare);
}


void Population::doStatistics(){
	int n = pop.size();
	meanCost = 0;
	bestLocalCost = pop[0]->cost();
	worstLocalCost = pop[0]->cost();
	for(int i = 0; i < n; ++i){
		meanCost += pop[i]->cost();
		if(pop[i]->cost() < bestLocalCost){
			bestLocalCost = pop[i]->cost();
		}
		if(pop[i]->cost() > worstLocalCost){
			worstLocalCost = pop[i]->cost();
		}
	};
	meanCost = meanCost / n;
}

void Population::print(){
	std::cerr << " Pop (Cost -- Parameters) \n";
	int CurrentSize = pop.size(); // it can change during each step but should be of popSize at the end of each step.
	for(int i = 0; i < CurrentSize ; ++i){
	std::cerr << "elem " << i << "\tID = " << pop[i]->myID() << "\tE= " << pop[i]->cost() << "  \tAge= " << pop[i]->age() << "\tParams:";
		for(int j = 0; j < (int) pop[i]->size(); ++j){
			std::cerr << " " << pop[i]->gene(j);
		}
		std::cerr << "\n";
	}
	std::cerr << " --- End (" << CurrentSize << ") Elements --- \n";
}

void Population::printEnergies(){
	int n = pop.size();
	std::cout << "Population energies : \n";
	sort();
	for(int i = 0; i < n; ++i){
		std::cerr << i << "\t" << pop[i]->cost() << "\tID=" << pop[i]->myID() << "\n";
	}
	std::cerr << "\n";
}

void Population::replace(int i, int by_j)
{
	delete pop[i];
	if(i != by_j) pop[i]->copy(pop[by_j]);
	//pop[by_j] = NULL; NOOOOO ! Because if ze use multiple copies in selection
}

double Population::variance(){
	int n = pop.size();
	double sum = 0;
	double sumSquare = 0;
	for(int i = 0; i < n; ++i){
		sum += pop[i]->cost();
		sumSquare += (pop[i]->cost())*(pop[i]->cost());
	};
	return sqrt((sumSquare / n) - ((sum * sum) / (n*n)));
}