#include "Genetic.h" 

void GeneticAlgo::update(){
	int n = population.size();
	meanCost = 0;
	bestLocalCost = population[0].energy();
	worstLocalCost = population[0].energy();
	for(int i = 0; i < n; ++i){
		meanCost += population[i].energy();
		if(population[i].energy() < bestLocalCost){
			bestLocalIndividual = i;
			bestLocalCost = population[i].energy();
		}
		if(population[i].energy() > worstLocalCost){
			worstLocalCost = population[i].energy();
		}
		if((population[i].energy()) < globalOptima()){
			bestGlobalIndividual.copy(&population[i]);
			setToCurrent(&population[i]);
			setGlobal(population[i].energy());
		}
	};
	meanCost = meanCost / n;
	
	
}

double GeneticAlgo::variancePop(){
	int n = population.size();
	double sum = 0;
	double sumSquare = 0;
	for(int i = 0; i < n; ++i){
		sum += population[i].energy();
		sumSquare += (population[i].energy())*(population[i].energy());
	};
	return sqrt((sumSquare / n) - ((sum * sum) / (n*n)));
}

void GeneticAlgo::print_total_parameters(){
	std::cerr << "List of Params Best Global : ";
	for(size_t i = 0; i < numIndex(); ++i){
		std::cerr << (bestGlobalIndividual.myParam(i)) << " ";
	}
	std::cerr << "\n";
}

void GeneticAlgo::print_evolution(){
	std::cerr << "Evolution : " << 
		generation << "\t" << bestLocalCost << "\t" << worstLocalCost << "\t" <<
		globalOptima() << "\t" <<  meanCost << "\n";
}

void print(std::vector<double> energies){
	int n = energies.size();
	std::cout << "Population energies : ";
	for(int i = 0; i < n; ++i){
		std::cerr << energies[i] << " ";
	}
	std::cerr << "\n";
	
}

GeneticAlgo::GeneticAlgo(BaseOptimizationProblem *E, const std::string &file) :BaseOptimizer(E,file) {
	construct();
}
GeneticAlgo::GeneticAlgo(BaseOptimizationProblem *E, std::ifstream &F_IN) :BaseOptimizer(E,F_IN) {
	construct();
}

void GeneticAlgo::construct(){
	if (numArguments() !=  4) { std::cerr << "Bad argument number (" << numArguments() << "\n"; exit(-1);};
	if((int) argument(0) == -1){
		popSize = -1;
		maxCalls = (int) argument(1);
		coeff_fork = (double) argument(2);
		num_tries =  (int) argument(3);
		// popSize, numGen max, numMutations and numOffspring will variate
		std::cerr << "Old Genetic Algo - Teste different Population size and Mutation rates.\n" << 
			"\tParameters :\n" << 
			"\t   Coefficient of reproduction (nb Off + Mut = coeff * popSize) " << coeff_fork << "\n" <<
			"\t   Number of cost calls max                                     " << maxCalls << "\n" <<
			"\t   Number of replicates by set                                  " << num_tries << "\n";
	}
	else{
		popSize = (int) argument(0);
		maxCalls = (int) argument(1);
		numMutations = (int) argument(2);
		numOffspring = (int) argument(3);
		numGenMax = 1 + maxCalls / (numMutations + numOffspring);
		std::cerr << "Old Genetic Algo - 1 simulation \n" << 
			"\tParameters :\n" << 
			"\t   popSize = " << popSize << "\n" <<
			"\t   Number of cost calls max        " << maxCalls << "\t" << "equivalent to " << numGenMax << " gen.\n" <<
			"\t   Number of mutants / gen         " << numMutations << "\n" <<
			"\t   Number of cross-over / gen      " << numOffspring << "\n";
	}
		
	std::cerr << "Data we have from the optimizer :\n" << 
			"We want to optimize " << numIndex() << " parameters, of indices :\n";
	//for(size_t i = 0; i < numIndex() ; ++i){ std::cerr << " " << indexVector_[i];}
	std::cerr << "\n";
	//std::cerr << "Total number of parameters : " << E_->numPara() << "\n";
	generation = 0;
}

void GeneticAlgo::initializeRandom(){
	//std::cerr << "Create the initial population.\n\tNote that Element are created : \n\t  randomly in the given range for the parameters to optimize,\n\t  and conform to initial set of parameters for other ones\n";
	// already implemented in baseOptimizer : 
	for(int i = 0; i < popSize; ++i){
		randomizeParameter();
		double E = computeCost();
		catchCurrentTo(E, &population[i]);	
	}
	bestGlobalIndividual.copy(&population[popSize-1]);
	setGlobal(population[popSize-1].energy()); // still the current one
	update();
}

void GeneticAlgo::selection(){
	//std::cerr << "Selection step\n";
	// for the moment, keeps the popSize better elements
	std::vector<double> energies;
	int n = population.size();
	energies.resize(n,0);
	for(int i = 0; i < n; ++i){
		energies[i] = population[i].energy(); //+ random ??
	}
	sort(energies.begin(), energies.end());

	if(popSize > n) {std::cerr << "problem : popSize = " << popSize << " and population size observed : " << n << "\n";};
	if(popSize < 1) {std::cerr << "problem : popSize = " << popSize << "\n";}
	double cutoff = energies[popSize-1];
	//print(energies);
	//std::cerr << "	cutoff = " << cutoff << "\n";
	int ind_fill = 0;
	for(int i = 0; i < n; ++i){	//Assumes that energies are continuous and so are not equals between individualOlds
		if((population[i].energy() <= cutoff) && (ind_fill < popSize)){ //keep this one
			if(i != ind_fill){
				population[ind_fill].copy(&population[i]);
			}
			ind_fill++;
		}		
	}
	population.resize(popSize);
	
	//print_total_parameters();
}



void GeneticAlgo::optimize(){
	if(popSize > 0){
		std::cerr << "Launch Optimization\n";
		population.resize(popSize);
		initializeRandom();
		print_total_parameters();
		/*print_total_parameters();
		printPop();
		Mutate();
		Fork();
		printPop();
		selection();
		printPop();*/
		std::cerr << "Evolution : Gen\tbestLocal\tGlob\tmean\n";
	
		for(generation = 0; generation < numGenMax; ++generation){
			Mutate();
			Fork();
			selection();
			update();
			print_total_parameters();
			print_evolution();
			//if(variancePop() < 0.00001 * population[0].energy())
			//	{ std::cerr << "LIMITED VARIANCE : " << variancePop() << " comp to " << population[0].energy() << " STOP\n"; exit(0);};
		}
		print_total_parameters();
		//printPop();
	} else {

		/* inp :
		int maxCalls;
		int num_tries;
		double coeff_fork;
		out :
		int popSize;
		int numGenMax;
		int numMutations;
		int numOffspring;
		int generation;
		*/
		
		int numTotalStep = 0;
		individualOld MegaSuperBest;
		double TimeTotal = 0;
		
		for(popSize = 7 ; popSize < 2000 ; popSize = (int) (popSize * 1.2)){
			if(popSize < 50) popSize *= 1.5;
			numGenMax = maxCalls / popSize;
			float n_mutations_f = 0;
			float n_offspring_f = 0;
						
			for(int pourcent = 4; pourcent <= 104; pourcent = pourcent + 5){ 
				if(pourcent < 10) pourcent = pourcent - 4;
				if(pourcent > 95) pourcent = pourcent - 4;
				if((pourcent < 70) && (pourcent > 30) && ((pourcent % 5) == 0)) pourcent += 5;
				
				n_mutations_f = ((float) pourcent * (float) popSize) / 100.0;
				n_offspring_f = (float) popSize - n_mutations_f;
						
				numTotalStep++;
				myTimes::getDiffTime();
				std::cerr << popSize << "\t" << pourcent << "\t" << numGenMax << "\t" << n_mutations_f << "\t" << n_offspring_f << "\t";		
				struct statistique a;
				
				vector<statistique> intermediaires;
				intermediaires.resize(21); //21 in case, should be 20
				vector<int> liste_generations(21,0);
				vector<int> liste_calls(21,0);
				
				for(int j = 0; j < num_tries; ++j){
					//myTimes::getDiffTime();
					resetCostCalls();
					population.resize(popSize);		
					initializeRandom();		// initializes also the best global
					for(generation = 0; generation < numGenMax; ++generation){
						int plusOne = (myRandom::Rnd() <= (n_mutations_f - (float) ((int) n_mutations_f)) ? 1 : 0);
						numMutations = plusOne + (int) n_mutations_f; 
						numOffspring = 1 - plusOne + (int) n_offspring_f;
						// cerr << "M " << numMutations << "   O " << numOffspring << "\n";
		
						Mutate();
						Fork();
						selection();
						update();
						if((generation% ((int) (max(numGenMax,20)) / 20)) == 0){
							int indice = (20*generation)/numGenMax;
							liste_generations[indice] = generation;
							liste_calls[indice] = nbCostCalls();
							intermediaires[indice].add(bestGlobalIndividual.energy());
							//print_evolution();
						}
						//if(variancePop() < 0.00001 * (population(0))->cost()){ std::cerr << "LIM_VARIANCE";}// : " << variancePop() << " comp to " << (population(0))->cost() << " STOP";};// break;};
					}		
					a.add(bestGlobalIndividual.energy());
					if(numTotalStep == 1) MegaSuperBest.copy(&bestGlobalIndividual);
					else if(bestGlobalIndividual.energy() < MegaSuperBest.energy()){
						MegaSuperBest.copy(&bestGlobalIndividual);}
					//std::cerr << "Best Individual for this iteration : E = " << bestGlobalIndividual.energy() << "\t";
					/*for(int k = 0; k < (int) numIndex(); k++){
						cerr << bestGlobalIndividual.myParam(k) << "\t";
					}
					cerr << "\n";*/
					//std::cerr << "TimeEnd :" << myTimes::getDiffTime() << "\tnbCostCalls:" << nbCostCalls() << "\tBest:\t" << bestGlobal.cost() << "\n";
				}
				double elapsed = myTimes::getDiffTime();
				cerr << "TimeSet= " << elapsed << "\t";
				TimeTotal += elapsed;
				a.sumUp();
				for(int i = 0; i < 20; ++i){
					cerr << "\t\t" << liste_calls[i] << "\t" << liste_generations[i] << "\t";
						intermediaires[i].sumUp();
				}
			}
		}
		
		std::cerr << "Best Individual at all : E = " << MegaSuperBest.energy() << "\t";
		for(int k = 0; k < (int) numIndex(); k++){
			cerr << MegaSuperBest.myParam(k) << "\t";}
		std::cerr << "\n";		
	}
}


double bCoeff(){
	double res = 2*(myRandom::Rnd());
	if(res < 0.5){res = res + 0.5;};
	return res;
}

void GeneticAlgo::Mutate(){
	int n = population.size();
	population.resize(n + numMutations);
	for(int i = 0; i < numMutations; ++i){
		
		int position = size_t(numIndex() * myRandom::Rnd());
		int whichInd = popSize * myRandom::Rnd();
		setToCurrent(&population[whichInd]);
		setParameter(position, (population[whichInd].myParam(position))*bCoeff()); //mutate inside the organism
		double E = computeCost();
		catchCurrentTo(E, &population[n+i]);
	}
}

void GeneticAlgo::cross_over(int ind_parent1, int ind_parent2){
	int position = size_t(numIndex() * myRandom::Rnd()); // position to cross
	
	setToCurrent(&population[ind_parent2]);
	for(int i = 0; i < position; ++i){
		setParameter(position, (population[ind_parent1].myParam(i)));
	}
}

void GeneticAlgo::Fork(){
	// selection of individualOlds for the cross-over
	int n = population.size();
	population.resize(n + numOffspring);
	for(int i = 0; i < numOffspring; ++i)
	{	
		int whichInd1 = popSize * myRandom::Rnd();
		int whichInd2 = popSize * myRandom::Rnd();

		cross_over(whichInd1, whichInd2);
		double E = computeCost();
		catchCurrentTo(E, &population[n+i]);
	}
	
	
}

void GeneticAlgo::printPop(){
	std::cerr << " --- State in generation " << generation << " --- \n";
	std::cerr << " Pop (Cost -- Parameters) \n";
	int CurrentSize = population.size(); // it can change during each step but should be of popSize at the end of each step.
	for(int i = 0; i < CurrentSize ; ++i){
		std::cerr << "elem " << i << "\tE=" << population[i].energy() << "\tParams:";
		for(int j = 0; j < (int) numIndex(); ++j){
			std::cerr << " " << population[i].myParam(j);
		}
		std::cerr << "\n";
	}
	std::cerr << " --- End (" << CurrentSize << " Elements --- \n";
}






