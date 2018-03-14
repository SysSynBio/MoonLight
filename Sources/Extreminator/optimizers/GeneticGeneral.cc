#include "GeneticGeneral.h" 

#define TRY_MUT  10
#define TRY_CROSS 30
// sqrt(12)
#define MUNIFORM sig*3.46410*(-0.5+BR.Uniform01())
#define MNORMAL sig*(BR.Normal())
#define MCAUCHY sig*(BR.Cauchy())
#define MEXPONENTIAL 1.4142*(0.5*((BR.Uniform01() > 0.5) ? 1 : -1)*BR.Exponential())
#define MCOMBINED 0.627*sig*(BR.Normal() + BR.Cauchy())

// =================================== I Constructor and initialization ========================================
// Note that the file / stream is read by the mother class (baseOptimizer) that loads Arguments, paraLowVector, etc ...
GeneticGeneral::GeneticGeneral(BaseOptimizationProblem *E, std::ifstream &F_IN) :BaseOptimizer(E,F_IN) {
    Initialize();}
GeneticGeneral::GeneticGeneral(BaseOptimizationProblem *E, const std::string &file) :BaseOptimizer(E,file) {
    Initialize();}
void GeneticGeneral::Initialize(){
    debug_live = 0;
    if (numArguments() !=  14) { std::cerr << "Bad argument number (" << numArguments() << "\n"; exit(-1);};
    type_of_algo = (int) argument(0);
    Config.setParentsPolicy((int) argument(1));
    Config.setCrossOverPolicy((int) argument(2), (double) argument(3));
    Config.setMutationPolicy((int) argument(4));
    Config.setReplacementPolicy((int) argument(5));
    Config.setSelectPolicy((int) argument(6));
    Config.setStrategyPolicy((int) argument(7), (double) argument(8));
    crossOverSuccess = 0;
    crossOverFail = 0;
    mutationSuccess = 0;
    mutationFail = 0;
    initRandoms();
}






// =========================== II The (virtual) function called for optimizing =========================

void GeneticGeneral::optimize(){
	//cerr << setiosflags(ios::fixed);	
	resetCostCalls();
	int num_tries = (int) argument(9);
	int maxCalls = (int) argument(10);
	int popSize = (int) argument(11);
	double propCross = (double) argument(12);
	double forkCoeff = (double) argument(13);

    bestOverall = new individual();
    bestOverall->setCost(1e50);

    // Basal values if '-1'
    if((popSize < 1) || (popSize > 1e9)) popSize = 80;
	if(forkCoeff < 0) forkCoeff = 1;
    if(propCross < 0) {if(type_of_algo == 0) propCross = 0.1; else propCross = 1.0;}

    //0 = CEP   1 = SGGA   2 = GGA
	if(type_of_algo == 0){
		for(int i = 0; i < num_tries; ++i){
            CEP(popSize, maxCalls, propCross, forkCoeff, i==0 ? 2:1);
            bestOverall->improve(&bestGlobal);
		}
	}
	if(type_of_algo == 1){
		for(int i = 0; i < num_tries; ++i){
            SSGA(popSize, maxCalls, propCross, i==0 ? 2:1);
            bestOverall->improve(&bestGlobal);
		}
	}
	if(type_of_algo == 2){
		for(int i = 0; i < num_tries; ++i){
            GGA(popSize, maxCalls, propCross, i==0 ? 2:1);
            bestOverall->improve(&bestGlobal);
		}
	}
    //10 = CEP   11 = SGGA   12 = GGA, testing all combination Mutation/Cross-Over
	if((type_of_algo / 10) == 1){
		type_of_algo -= 10;
		testOperators(popSize, propCross, forkCoeff, num_tries, maxCalls);
	}
    bestGlobal.copy(bestOverall);
	return;
}

void GeneticGeneral::testOperators(int popSize, double propCross, double forkCoeff, int num_tries, int maxCalls){
    cout << "\t|========= Teste les combinaisons Mutations / Cross-over avec : ================| \n";
    if(type_of_algo == 0) cout << "\t     - CEP\n";
    if(type_of_algo == 1) cout << "\t     - SSGA\n";
    if(type_of_algo == 2) cout << "\t     - GGA\n";
    cout << "\t     - popSize   = " << popSize << "\n";
    cout << "\t     - propCross = " << propCross << "\n";
    cout << "\t     - num_tries = " << num_tries << "\n";
    cout << "\t     - maxCalls  = " << maxCalls << "\n";
    if(type_of_algo == 0) cout << "\t     - forkCoeff = " << forkCoeff << "\n";
    cout << "\t|===============================================================================|\n";

	BR.CreateCauchy(0,0.0155);
	double TotalResBest[12][13];
	double TotalResMean[12][13];
	double TotalResWorst[12][13];
	double TotalResVariances[12][13];

    // if you test only a subset ...
    for(int co = 0; co < 11; ++co){
        for(int muttyp = 0; muttyp < 12; ++muttyp){
            TotalResBest[co][muttyp] = 0.0;
            TotalResWorst[co][muttyp] = 0.0;
            TotalResMean[co][muttyp] = 0.0;
            TotalResVariances[co][muttyp] = 0.0;
        }
    }
    for(int co = 0; co < 11; ++co){
		for(int muttyp = 0; muttyp < 12; ++muttyp){
			Config.setCrossOverPolicy(co, -1);
			Config.setMutationPolicy(muttyp);
			struct statistique a;
			for(int i = 0; i < num_tries; ++i){
                double res = (type_of_algo == 0 ? CEP(popSize, maxCalls, propCross, forkCoeff, i==0 ? 2:1) :
                        (type_of_algo == 1 ? SSGA(popSize, maxCalls, propCross, i==0 ? 2:1) :
                        GGA(popSize, maxCalls, propCross, i==0 ? 2:1)));
				a.add(res);
                bestOverall->improve(&bestGlobal);
			}
			a.sumUp();
			
			TotalResBest[co][muttyp] = a.bestSeen();
			TotalResWorst[co][muttyp] = a.worseSeen();
			TotalResMean[co][muttyp] = a.mean();
			TotalResVariances[co][muttyp] = a.variance;
		}
	}
	
	cout << "SUM UP Best Element. Ligne = cross-over; colonne = mutation\n";
    for(int co = 0; co < 11; ++co){
		for(int muttyp = 0; muttyp < 12; ++muttyp){
			cout << TotalResBest[co][muttyp] << "\t";
		}
		cout << "\n";
	}

	cout << "SUM UP Mean. Ligne = cross-over; colonne = mutation\n";
    for(int co = 0; co < 11; ++co){
		for(int muttyp = 0; muttyp < 12; ++muttyp){
			cout << TotalResMean[co][muttyp] << "\t";
		}
		cout << "\n";
	}

	cout << "SUM UP Worst Element. Ligne = cross-over; colonne = mutation\n";
    for(int co = 0; co < 11; ++co){
		for(int muttyp = 0; muttyp < 12; ++muttyp){
			cout << TotalResWorst[co][muttyp] << "\t";
		}
		cout << "\n";
	}

	cout << "SUM UP Variance. Ligne = cross-over; colonne = mutation\n";
    for(int co = 0; co < 11; ++co){
		for(int muttyp = 0; muttyp < 12; ++muttyp){
			cout << TotalResVariances[co][muttyp] << "\t";
		}
		cout << "\n";
	}
	return;
}















// =============================== III.1 The main loop for Steady State Genetic Algorithm ====================================

double GeneticGeneral::SSGA(int popSize, int maxCalls, double prcOffspring, int speak){
    if(speak>=2){
    std::cout << "\t-------------------Steady-State Genetic Algorithm-----------------------." <<  numArguments() <<  "\n" <<
                 "\t   type of cross-over         " << Config.type_cross_over << " - " << Config.type_of_cross_over() << "\t" << Config.nb_parents_of_cross_over() << " Par." << Config.nb_offsprings_of_cross_over() << " Off. p=" << (Config.param_cross_over < 0 ? "Default" : "") << Config.param_cross_over << "\n"
	//"\t   Parameter for cross-over   " << Config.param_cross_over << "\n" <<
	"\t   Type of replacement        " << Config.type_of_replacement() << "\n" <<
	//"\t   Type of selection(selec)   " << Config.type_of_selection_selec() << "\n" <<
	//"\t   Type of selection(parents) " << Config.type_of_selection_parents() << "\n" <<
    "\t   Strategy (step sizes)      " << Config.type_of_strategy() << "\tp=" << Config.strategy_parameter << "\n" <<
	"\t   Mutation Distribution      " << Config.type_of_mutation() << "\n" <<
	"\t   Maxcalls for an optimisat. " << maxCalls << "\n" <<
    "\t   prcOffspring               " << prcOffspring << "\ti.e. " << prcOffspring * (double) popSize << " off/gen (cross-over then mut.) repl. par." << "\n" <<
	"\t   popSize                    " << popSize << "\n" << 
	//"\t   each line : PopSize     %Mutations    NumGenerations    NumMutations     NumOffspring \n" << 
    "\t--------------------------------------------------------------------------------\n" <<
    "gen\tBestCost\tbestPopCost\tmeanPopCost\tworstPopCost\tnbCostCalls\n";
	}
    crossOverSuccess = 0;
    crossOverFail = 0;
    mutationSuccess = 0;
    mutationFail = 0;

	double numOffspring = prcOffspring * (double) popSize;
	initializeRandomPop(popSize);
	resetCostCalls();
    bestGlobal.copy(population(0));

    int generation;
	for(generation = 0; (generation < (maxCalls / (popSize*prcOffspring))) && (nbCostCalls() < maxCalls); ++generation){
		int nb_this_time =(myRandom::Rnd()  < (numOffspring - (double)((int) numOffspring)) ? 1 : 0) + (int) numOffspring;
		updateSigmas(generation, (maxCalls / (popSize*prcOffspring)));
		AddMutatedOffspringAndReplace(nb_this_time);
		int n = population.size(); 
		if(n < popSize){
			population.resize(popSize);
			for(int i = n; i < popSize; ++i){
				randomize(population(i));
				updateCost(population(i));
			}
		}
        if(speak>=1){ //if((generation% ((int) (max((int) (maxCalls / ((int) popSize*prcOffspring)),50)) / 50)) == 0){
            population.doStatistics();
            cout << generation +1<< "\t" << bestGlobal.cost() << "  \t" << population.bestLocalCost << "  \t" <<  population.meanCost << "  \t" << population.worstLocalCost << "    \t" << nbCostCalls() << "\n";
        }
	}

    if(speak>=1){
        population.doStatistics();  //population.print();
        cout << "\t\tCalls= " << nbCostCalls() << "\tBest= " << bestGlobal.cost() << "  \tPopBest= " << population.bestLocalCost << "\tPopMean= " <<  population.meanCost << "\tPopWorst" << population.worstLocalCost << "\tgen= " << generation << "\n";
        cout << "\t\tSuccessfull Mutations   : " << mutationSuccess << " / " << mutationSuccess + mutationFail << endl;
        cout << "\t\tSuccessfull cross-Overs : " << crossOverSuccess << " / " << crossOverSuccess + crossOverFail << endl;
    } else cout << bestGlobal.cost() << endl;
    return bestGlobal.cost();
}













// =============================== III.2 The main loop for Generational Gap Genetic Algorithm ====================================

double GeneticGeneral::GGA(int popSize, int maxCalls, double prcOffspring, int speak){
    speak = 2;
    if(speak>=2){
	std::cout << "-------------------Generation-Gap Genetic Algorithm-----------------------." <<  numArguments() <<  "\n" <<
                 "\t   type of cross-over         " << Config.type_cross_over << " - " << Config.type_of_cross_over() << "\t" << Config.nb_parents_of_cross_over() << " Par." << Config.nb_offsprings_of_cross_over() << " Off. p=" << (Config.param_cross_over < 0 ? "Default" : "") << Config.param_cross_over << "\n"
	//"\t   Parameter for cross-over   " << Config.param_cross_over << "\n" <<
	//"\t   Type of replacement        " << Config.type_of_replacement() << "\n" <<
	//"\t   Type of selection(selec)   " << Config.type_of_selection_selec() << "\n" <<
	//"\t   Type of selection(parents) " << Config.type_of_selection_parents() << "\n" <<
    "\t   Strategy (step sizes)      " << Config.type_of_strategy() << "\tp=" << Config.strategy_parameter << "\n" <<
    "\t   Mutation Distribution      " << Config.type_of_mutation() << "\n" <<
	"\t   Maxcalls for an optimisat. " << maxCalls << "\n" <<
    "\t   prcOffspring               " << prcOffspring << "\ti.e. " << prcOffspring * (double) popSize << " off/gen" << "\n" <<
    "\t   popSize                    " << popSize << "\n" <<
	//"\t   each line : PopSize     %Mutations    NumGenerations    NumMutations     NumOffspring \n" << 
    "--------------------------------------------------------------------------------\n" <<
    "gen\tBestCost\tbestPopCost\tmeanPopCost\tworstPopCost\tnbCostCalls\n";
    }
    crossOverSuccess = 0;
    crossOverFail = 0;
    mutationSuccess = 0;
    mutationFail = 0;

	if(prcOffspring < 1.0) cerr << "Warning ! You call a GGA with a cross-over rate that doesn't allow to create stable population size. Random elements will be added instead !!!\n";
	double numOffspring = prcOffspring * (double) popSize;
	initializeRandomPop(popSize);
	resetCostCalls();
    bestGlobal.copy(population(0));


	int generation;
	for(generation = 0; (generation < (maxCalls / (popSize*prcOffspring))) && (nbCostCalls() < maxCalls); ++generation){
		AgePopulation();
		int nb_this_time =(myRandom::Rnd()  < (numOffspring - (double)((int) numOffspring)) ? 1 : 0) + (int) numOffspring;
		updateSigmas(generation, (maxCalls / (popSize*prcOffspring)));
		AddMutatedOffspring(nb_this_time);
		killAgeing(0);
		int n = population.size(); 
		if(n < popSize){
			population.resize(popSize);
			for(int i = n; i < popSize; ++i){
				randomize(population(i));
				updateCost(population(i));
			}
		}
        if(speak>=1){ //if((generation% ((int) (max((int) (maxCalls / ((int) popSize*prcOffspring)),50)) / 50)) == 0){
            population.doStatistics();
            cout << generation+1 << "\t" << bestGlobal.cost() << "  \t" << population.bestLocalCost << "  \t" <<  population.meanCost << "  \t" << population.worstLocalCost << "    \t" << nbCostCalls() << "\n";
        }
	}

    if(speak>=1){
        population.doStatistics();  //population.print();
        cout << "\t\tCalls= " << nbCostCalls() << "\tBest= " << bestGlobal.cost() << "  \tPopBest= " << population.bestLocalCost << "\tPopMean= " <<  population.meanCost << "\tPopWorst" << population.worstLocalCost << "\tgen= " << generation << "\n";
        cout << "\t\tSuccessfull Mutations   : " << mutationSuccess << " / " << mutationSuccess + mutationFail << endl;
        cout << "\t\tSuccessfull cross-Overs : " << crossOverSuccess << " / " << crossOverSuccess + crossOverFail << endl;
    } else cout << bestGlobal.cost() << endl;
    return bestGlobal.cost();
}




















// =============================== III.3 The main loop for Classical Evolutionnary Programming ====================================

double GeneticGeneral::CEP(int popSize, int maxCalls, double prcOffspring, double forkCoeff, int speak){
    speak = 0;///§§§§ to remove !!
    if(speak>=2){
    std::cout << "\t-------------------Classical Evolutionnary Program-----------------------." <<  numArguments() <<  "\n" <<
    "\t   type of cross-over         " << Config.type_cross_over << " - " << Config.type_of_cross_over() << "\t" << Config.nb_parents_of_cross_over() << " Par." << Config.nb_offsprings_of_cross_over() << " Off. p=" << (Config.param_cross_over < 0 ? "Default" : "") << Config.param_cross_over << "\n"
	//"\t   Parameter for cross-over   " << Config.param_cross_over << "\n" <<
	//"\t   Type of replacement        " << Config.type_of_replacement() << "\n" <<
	"\t   Type of selection(selec)   " << Config.type_of_selection_selec() << "\n" <<
	"\t   Type of selection(parents) " << Config.type_of_selection_parents() << "\n" <<
    "\t   Strategy (step sizes)      " << Config.type_of_strategy() << "\tp=" << Config.strategy_parameter << "\n" <<
    "\t   Mutation Distribution      " << Config.type_of_mutation() << "\n" <<
	"\t   Maxcalls for an optimisat. " << maxCalls << "\n" <<
    "\t   forkCoeff :                " << forkCoeff << "\ti.e. " << (int) ((double) popSize * forkCoeff) << " new ind/gen\n" <<
    "\t   prcOffspring               " << prcOffspring << "\t=fraction from cross-over (rest from mutation)\n" <<
    "\t   popSize                    " << popSize << "\n" <<
	//"\t   each line : PopSize     %Mutations    NumGenerations    NumMutations     NumOffspring \n" << 
    "\t--------------------------------------------------------------------------------\n" <<
    "gen\tBestCost\tbestPopCost\tmeanPopCost\tworstPopCost\tnbCostCalls\n";
    }

    crossOverSuccess = 0;
    crossOverFail = 0;
    mutationSuccess = 0;
    mutationFail = 0;

	int nb_to_create = (int) ((double) popSize * forkCoeff);
	double numOffspring = prcOffspring * (double) nb_to_create;
	initializeRandomPop(popSize);
	resetCostCalls();
    bestGlobal.copy(population(0));


	int generation;
	for(generation = 0; (generation < 1+(maxCalls / (nb_to_create))) && (nbCostCalls() < maxCalls); ++generation){
		int nb_this_time =(myRandom::Rnd()  < (numOffspring - (double)((int) numOffspring)) ? 1 : 0) + (int) numOffspring;
		updateSigmas(generation, 1+(maxCalls / (nb_to_create)));
		AddOffspring(nb_this_time);
		AddMutants(nb_to_create - nb_this_time);
		selection(popSize);
        if(speak>=1){ // if((generation% ((int) (max(maxCalls / popSize,50)) /50)) == 0){
            population.doStatistics();
            cout << generation+1 << "\t" << bestGlobal.cost() << "  \t" << population.bestLocalCost << "  \t" <<  population.meanCost << "  \t" << population.worstLocalCost << "    \t" << nbCostCalls() << "\n";
        }
	}

    if(speak>=1){
        population.doStatistics();  //population.print();
        cout << "\t\tCalls= " << nbCostCalls() << "\tBest= " << bestGlobal.cost() << "  \tPopBest= " << population.bestLocalCost << "\tPopMean= " <<  population.meanCost << "\tPopWorst" << population.worstLocalCost << "\tgen= " << generation << "\n";
        cout << "\t\tSuccessfull Mutations   : " << mutationSuccess << " / " << mutationSuccess + mutationFail << endl;
        cout << "\t\tSuccessfull cross-Overs : " << crossOverSuccess << " / " << crossOverSuccess + crossOverFail << endl;
    } //else cout << bestGlobal.cost() << endl;
    return bestGlobal.cost();
}



























// ====================================== IV General manipulations on the population =======================================

void GeneticGeneral::initializeRandomPop(int newSize){
    population.resize(newSize);
    for(int i = 0; i < newSize; ++i){
        randomize(population(i)); 	// do the resizing of the individual by himself
        updateCost(population(i));
    }
}

void GeneticGeneral::addRandom(int nb_to_add){
    int currentSize = population.size();
    population.resize(currentSize + nb_to_add);
    for(int i = currentSize; i <  currentSize + nb_to_add; ++i){
        population(i)->resize(numIndex());
        randomize(population(i));
        updateCost(population(i));
    }
}

// Ageing (for GGA, kill olders !)
void GeneticGeneral::AgePopulation(){
    int n = population.size();
    for(int i = 0; i < n; ++i){
        population(i)->ageing();
    }
}

int GeneticGeneral::killAgeing(int maxAgeAuthorized){
    vector<int> survivors;
    survivors.clear();
    int n = population.size();
    for(int i = 0; i < n; ++i){
        if(population(i)->age() <= maxAgeAuthorized) survivors.push_back(i);
    }
    selection_keeping_indices(&survivors);
    return (n - survivors.size());
}

void GeneticGeneral::AddMutants(int n_to_add){
    int n = population.size();
    population.resize(n + n_to_add);
    if(Config.mutation_policy == MUTATE_ONE_POINT){
        for(int i = 0; i < n_to_add; ++i){
            int whichInd = n * myRandom::Rnd();
            population(n+i)->copy(population(whichInd));
            mutate_one_point(population(n+i), Config.mutation_distribution);
            updateCost(population(n+i));
        }
    }
    else if(Config.mutation_policy == MUTATE_ALL_POINTS){
        for(int i = 0; i < n_to_add; ++i){
            int whichInd = n * myRandom::Rnd();
            population(n+i)->copy(population(whichInd));
            mutate_all_points(population(n+i), Config.mutation_distribution);
            updateCost(population(n+i));
        }
    } else cerr << "AddMutants : Undefined Mutation Policy\n";
}


bool GeneticGeneral::AddMutants(double n_to_add){
    double chance = n_to_add - (double) ((int) n_to_add);
    if(myRandom::Rnd() <= chance){
        AddMutants(1 + (int) n_to_add);
        return true;	// to know if there was one more mutation or not and then do one less offspring for instance
    }
    AddMutants((int) n_to_add);
    return false;
}































// ------------------------------------------------------------ CrossOvers ---------------------------------------------------------------



//TODO finish the replacement policies
void GeneticGeneral::AddMutatedOffspringAndReplace(int n_to_add){
	int n = population.size();
	int remains = n_to_add;
	while(remains > 0){
		int nadd = min(Config.nb_offsprings_of_cross_over(), remains);
		remains = remains - Config.nb_offsprings_of_cross_over();
		switch (Config.replacement_policy) {
			case REPLACE_WORST_PARENT:{ //(only if better)
                AddMutatedOffspring(nadd, NULL, false, true);
				for(int i = n; i < n + nadd; ++i){
					int np = population(i)->num_parents();
					int worse = i;
					for(int j = 0; j < np; ++j){
						if(population(population(i)->parent(j))->cost() > population(worse)->cost()){
							worse = population(i)->parent(j);
						}
					}
					if(worse != i) population(worse)->copy(population(i)); // if the worse is the son, he is replaced by the parent, but poor him, who cares !
				}
				population.resize(n);
				break;}
			case REPLACE_WORST_PARENT_METROPOLIS:{
                AddMutatedOffspring(nadd, NULL, false, true);
				for(int i = n; i < n + nadd; ++i){
					int np = population(i)->num_parents();
					int worse = population(i)->parent(0);
					for(int j = 1; j < np; ++j){
						if(population(population(i)->parent(j))->cost() > population(worse)->cost()){
							worse = population(i)->parent(j);
						}
					}
					if(metropolis(population(worse)->cost(), population(i)->cost())) population(worse)->copy(population(i));
				}
				population.resize(n);
				break;}
			case REPLACE_WORST:{ // Only if better
                AddMutatedOffspring(nadd, NULL, false, true);
				// We don't care who is the parentm so we can sort the population
				population.sort();
				population.resize(n);
				break;}
			case REPLACE_RANDOM:{
                AddMutatedOffspring(nadd, NULL, false, true);
				for(int i = n; i < n + nadd; ++i){
					int target = n * myRandom::Rnd();
					population(target)->copy(population(i));
				}
				population.resize(n);
				break;}
			case REPLACE_RANDOM_METROPOLIS:{
                AddMutatedOffspring(nadd, NULL, false, true);
				for(int i = n; i < n + nadd; ++i){
					int target = n * myRandom::Rnd();
					if(metropolis(population(target)->cost(), population(i)->cost())) population(target)->copy(population(i));
				}
				population.resize(n);
				break;}
			case KILL_TOURNAMENT:{
				
				break;}
			case REPLACE_OLDEST:{
				
				break;}
			case CONSERVATIVE_SELECTION:{
				
				break;}
			case ELITIST:{
				
				break;}
			case NO_NEED_REPLACEMENT:{
				cerr << "You didn't specified a replacement policy ('No Need'), so you're not allowed to call 'AddMutatedOffspringAndReplace'\n";
				break;}
				
			default:{
				
			}
		}
	}
}

void GeneticGeneral::AddMutatedOffspring(int n_to_add, Population* pop_destination, bool dont_compute_cost, bool save_parents){
	// save former population size,
	// AddOffspringToPop -> so the new ones are between n and n+add,
	// then mutate them on place
	int n = population.size();
    AddOffspring(n_to_add, pop_destination, true, save_parents);
	if(Config.mutation_policy == MUTATE_ONE_POINT){
		for(int i = n; i < n + n_to_add; ++i){
            mutate_one_point(population(i), Config.mutation_distribution);
			updateCost(population(i));
		}
	} else if(Config.mutation_policy == MUTATE_ALL_POINTS){
		for(int i = n; i < n + n_to_add; ++i){     
            mutate_all_points(population(i), Config.mutation_distribution);
			updateCost(population(i));
		}
	}
}






// ------------------------------------------------------------ Mutations ---------------------------------------------------------------

void GeneticGeneral::mutate_one_point(individual* ind, int type_distrib, int position){	// Additive mutations
    if(position == -1) position = size_t(numIndex() * myRandom::Rnd());
    double gene = ind->gene(position);
    double sig = ind->step(position);

    int k = TRY_MUT;
    double teste_val = 0.0;
    while(k > 0){
        switch(type_distrib){
        case UNIFORM_STEP:{
            teste_val = gene + MUNIFORM;
            break;}
        case NORMAL:{
            teste_val = gene + MNORMAL;
            break;}
        case CAUCHY:{
            teste_val = gene + MCAUCHY;
            break;}
        case PUT_RANDOM:{
            teste_val = paraLowVector(position) + BR.Uniform01() * (paraHighVector(position) - paraLowVector(position));
            break;}
        case LEVY:{
            cerr << "Levy distribution is not implemented yet.\n";
            break;}
        case EXPONENTIAL:{
            BR.CreateExponential(sqrt(1/(max(0.0001,sig))));
            teste_val = gene + MEXPONENTIAL;
            break;}
        case GAMMA:{
            cerr << "Gamma distribution for mutation is not implemented yet.\n";
            break;}
            //BR.CreateGamma(sig);
            //ind->setGene(position, gene + (BR.Gamma()));
            //break;}
        case COMBINED:{
            teste_val = gene + MCOMBINED; // *17sqrt(2) to retablish square deviation of 1
            break;}
        default:{
            cerr << "Undefined type of mutation distribution\n";
        }
        }
        k--;
        if((teste_val >= paraLowVector(position)) && (teste_val <= paraHighVector(position))) k = -1;
    }
    if(k == -1) {ind->setGene(position, teste_val); mutationSuccess++;}
    else {ind->setGene(position, paraLowVector(position) + BR.Uniform01() * (paraHighVector(position) - paraLowVector(position)));
        mutationFail++;}
}

void GeneticGeneral::mutate_all_points(individual* ind, int type_distrib){ // Additive mutations
    for(int i = 0; i < (int) numIndex(); ++i){
        mutate_one_point(ind, type_distrib, i);
    }
}















// =================================== Creation of offspring on top of a population with cross-over (indices n ... n+nb-1) ===========================================

/// Note that in this function, only offspring inside the boundaries are generated.
///
// save parents = puts the history of parents inside the created individuals.
void GeneticGeneral::AddOffspring(int nb, Population* pop_destination, bool dont_compute_cost, bool save_parents){
	Population* pop = pop_destination;
	if(pop == NULL) pop = &population;
	
	int n = pop->size();
	int n_to_add = nb;
	int OffspringSize = Config.nb_offsprings_of_cross_over();
	if((n_to_add % OffspringSize) != 0){
		n_to_add += OffspringSize - (n_to_add % OffspringSize);
	}

	vector<int> parents;
	// WARNING : DON'T RESIZE BEFORE selection aprents because it takes in the all population
	if(debug_live) cerr << ".";
	selectionParents(&parents, n_to_add);
	if(debug_live) cerr << ":";
	pop->resize(n + n_to_add);
///////wtf ???	assert(indices_parents_selected.size() >= (Config.nb_parents_of_cross_over *(n_to_add / Config.nb_offsprings_of_crossover)));
	int pos_p1 = 0;// Position of the first parent of the group
	
	for(int i = 0; i < n_to_add; ++i){
        // tries 5 times to get (all the )offspring inside the boundaries. If not, randomizes them all.
        int k = TRY_CROSS;
        while(k > 0){


		if(debug_live) cerr << "_";
		switch(Config.type_cross_over){		
			case ONE_POINT_CROSSOVER:{
				cross_over_one_point((*pop)(n+i), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]));
				break;}
			case TWO_POINT_CROSSOVER:{
				cross_over_two_point((*pop)(n+i), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]));
				break;}
			case WRIGHT1_CROSSOVER:{
				cross_over_wright1((*pop)(n+i), (*pop)(n+i+1), (*pop)(n+i+2), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]));
				break;}
			case WRIGHT2_CROSSOVER:{
				cross_over_wright2((*pop)(n+i), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]));
				break;}
			case ARITHMETIC_CROSSOVER:{
				if(Config.param_cross_over >= 0){
					cross_over_arithmetic((*pop)(n+i), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]), Config.param_cross_over);
				} else cross_over_arithmetic((*pop)(n+i), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]));
				break;}
			case BLXALPHA_CROSSOVER:{
				if(Config.param_cross_over >= 0){
					cross_over_BLXAlpha((*pop)(n+i), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]), Config.param_cross_over);
				} else cross_over_BLXAlpha((*pop)(n+i), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]));
				break;}
			case GEOMETRIC_CROSSOVER:{
				if(Config.param_cross_over >= 0){
					cross_over_geometric((*pop)(n+i), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]), Config.param_cross_over);
				} else cross_over_geometric((*pop)(n+i), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]));
				break;}		
			case SBX_CROSSOVER:{
				// be carefull that the order of offspring is random
				if(myRandom::Rnd() > 0.5){
					if(Config.param_cross_over >= 0){
						cross_over_SBX((*pop)(n+i), (*pop)(n+i+1), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]), Config.param_cross_over);
					} else cross_over_SBX((*pop)(n+i), (*pop)(n+i+1), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]));
				} else {
					if(Config.param_cross_over >= 0){
						cross_over_SBX((*pop)(n+i+1), (*pop)(n+i), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]), Config.param_cross_over);
					} else cross_over_SBX((*pop)(n+i+1), (*pop)(n+i), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]));
				}
				break;}
			case UNDX_CROSSOVER_3P:{
				// be carefull that the order of offspring is random
				int z = myRandom::Rnd() * 6;
				switch(z){
					case 0:{// p1 p2 p3
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1+2]));
						break;}
					case 1:{// p1 p3 p2
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1+1]));
						break;}
					case 2:{// p2 p1 p3
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+2]));
						break;}
					case 3:{// p2 p3 p1
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1]));
						break;}
					case 4:{// p3 p1 p2
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]));
						break;}
					case 5:{// p3 p2 p1
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1]));
						break;}					
				}
				break;}		
			case UNDX_CROSSOVER_4P:{
				int z = myRandom::Rnd() * 24;
				switch(z){
					case 0:{// p1 p2 p3
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+3]),  (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1+2]));
						break;}
					case 1:{// p1 p3 p2
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+3]),  (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1+1]));
						break;}
					case 2:{// p2 p1 p3
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+3]),  (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+2]));
						break;}
					case 3:{// p2 p3 p1
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+3]),  (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1]));
						break;}
					case 4:{// p3 p1 p2
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+3]),  (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]));
						break;}
					case 5:{// p3 p2 p1
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+3]),  (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1]));
						break;}					

					case 6:{// p1 p2 p3
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1]),  (*pop)(parents[pos_p1+3]), (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1+2]));
						break;}
					case 7:{// p1 p3 p2
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1]),  (*pop)(parents[pos_p1+3]), (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1+1]));
						break;}
					case 8:{// p2 p1 p3
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+1]),  (*pop)(parents[pos_p1+3]), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+2]));
						break;}
					case 9:{// p2 p3 p1
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+1]),  (*pop)(parents[pos_p1+3]), (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1]));
						break;}
					case 10:{// p3 p1 p2
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+2]),  (*pop)(parents[pos_p1+3]), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]));
						break;}
					case 11:{// p3 p2 p1
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+2]),  (*pop)(parents[pos_p1+3]), (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1]));
						break;}					

					case 12:{// p1 p2 p3
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]),  (*pop)(parents[pos_p1+3]), (*pop)(parents[pos_p1+2]));
						break;}
					case 13:{// p1 p3 p2
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+2]),  (*pop)(parents[pos_p1+3]), (*pop)(parents[pos_p1+1]));
						break;}
					case 14:{// p2 p1 p3
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1]),  (*pop)(parents[pos_p1+3]), (*pop)(parents[pos_p1+2]));
						break;}
					case 15:{// p2 p3 p1
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1+2]),  (*pop)(parents[pos_p1+3]), (*pop)(parents[pos_p1]));
						break;}
					case 16:{// p3 p1 p2
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1]),  (*pop)(parents[pos_p1+3]), (*pop)(parents[pos_p1+1]));
						break;}
					case 17:{// p3 p2 p1
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1+1]),  (*pop)(parents[pos_p1+3]), (*pop)(parents[pos_p1]));
						break;}					

					case 18:{// p1 p2 p3
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1+2]),  (*pop)(parents[pos_p1+3]));
						break;}
					case 19:{// p1 p3 p2
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1+1]),  (*pop)(parents[pos_p1+3]));
						break;}
					case 20:{// p2 p1 p3
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+2]),  (*pop)(parents[pos_p1+3]));
						break;}
					case 21:{// p2 p3 p1
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1]),  (*pop)(parents[pos_p1+3]));
						break;}
					case 22:{// p3 p1 p2
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1]), (*pop)(parents[pos_p1+1]),  (*pop)(parents[pos_p1+3]));
						break;}
					case 23:{// p3 p2 p1
						cross_over_UNDX((*pop)(n+i),  (*pop)(parents[pos_p1+2]), (*pop)(parents[pos_p1+1]), (*pop)(parents[pos_p1]),  (*pop)(parents[pos_p1+3]));
						break;}					
				}
				break;}
			case RANDOMIZE_ONE_POINT_CROSSOVER:{
				(*pop)(n+i)->copy((*pop)(parents[pos_p1]));
				simple_random((*pop)(n+i));
				break;}
			default:{
				cerr << "No defined cross-over operator 'No Need' -> Add comlpetely random instead\n";
				randomize((*pop)(n+i));
				break;
				};
            }

            // tries 5 times to get (all the )offspring inside the boundaries. If not, randomizes them all.
            k--;
            switch (OffspringSize) {
            case 1: {
                if(InsideBoundaries((*pop)(n+i))) k = -1;
                break;
            }
            case 2: {
                if(InsideBoundaries((*pop)(n+i)) && InsideBoundaries((*pop)(n+i+1))) k = -1;
                break;
            }
            case 3: {
                if(InsideBoundaries((*pop)(n+i)) && InsideBoundaries((*pop)(n+i+1)) && InsideBoundaries((*pop)(n+i+2))) k = -1;
                break;
            }
            }
            if (k == 0) {
                for(int k2 = 0; k2 < OffspringSize; ++k2)
                {randomize((*pop)(n+i+k2));}
                crossOverFail++;
            }
            if(k == -1){
                crossOverSuccess++;
            }
        }
		
		for(int j = 0; j < Config.nb_offsprings_of_cross_over(); ++j){
			if(j) i++;			// VERY IMPORTANT !!!
			if(save_parents){
				(*pop)(n+i)->add_parent(parents[pos_p1]);
				(*pop)(n+i)->add_parent(parents[pos_p1+1]);
			}
			if(!dont_compute_cost){
				updateCost((*pop)(n+i));
			}
		}
		
		pos_p1 += Config.nb_parents_of_cross_over();

	}
	
	if(n_to_add != nb){
		pop->resize(n + nb);
	}
	if(debug_live) cerr << "-";
}
	






















// =================================== Different Strategies (to update the steps of individuals) ===========================================

void GeneticGeneral::updateSigmas(int generation, int genMax){
    double val = Config.strategy_parameter;
    switch(Config.type_mutation_strategy){
        case CONSTANT:{
            int n = population.size();
            for(int i = 0; i < n; ++i){
                for(size_t j = 0; j < numIndex(); ++j){
                    population(i)->setStepSize(j, val);
                }
            }
            break;
        }
        case EXPONENTIAL_DOWN:{
            double newVal = val * std::exp(-((double) generation * 10.) / (double) genMax);
            int n = population.size();
            for(int i = 0; i < n; ++i){
                for(size_t j = 0; j < numIndex(); ++j){
                    population(i)->setStepSize(j, newVal);
                }
            }
            //cerr << "S" << newVal << "\t";
            break;
        }
        case DISTANCE_BEST:{
            int n = population.size();
            statistique a;
            for(int i = 0; i < n; ++i){
                double lgt = 0;
                for(size_t j = 0; j < numIndex(); ++j){
                    double difference = std::abs(bestGlobal.gene(j) - population(i)->gene(i));
                    lgt += difference * difference;
                }
                for(size_t j = 0; j < numIndex(); ++j){
                    population(i)->setStepSize(j, val*sqrt(lgt));
                }
                a.add(val*sqrt(lgt));
            }
            a.sumUp();
            break;
        }
        case DISTANCE_BEST_SEPARATED:{
            int n = population.size();
            statistique a;
            for(int i = 0; i < n; ++i){
                double mean = 0;
                for(size_t j = 0; j < numIndex(); ++j){
                    double difference = std::abs(bestGlobal.gene(j) - population(i)->gene(i));
                    population(i)->setStepSize(j, val*difference);
                    mean += population(i)->step(j);
                }
                a.add(mean / (double) numIndex());
            }
            a.sumUp();
            break;
        }
        case PROPORTIONAL_NORMALIZED:{
            int n = population.size();
            vector<double> probability(n, 0);
            double sum_corr_energies = 0;
            for(int i = 0; i < n; ++i){
                double fitcorrec =  1 / population(i)->cost(); //min(bestGlobal.cost() * 1e2, worstGlobal - population(i)->cost());
                probability[i] = fitcorrec;
                sum_corr_energies += fitcorrec;
            }
            for(int i = 0; i < n; ++i){
                if((sum_corr_energies == 0) || std::isnan(sum_corr_energies) || (std::isinf(sum_corr_energies)))
                    probability[i] = 1.0 / n;
                else probability[i] /= sum_corr_energies;
            }
            //cerr << "!" << sum_corr_energies << "\n";
            statistique a;
            for(int i = 0; i < n; ++i){
                a.add(val*n*probability[i]);
                for(size_t j = 0; j < numIndex(); ++j){
                    population(i)->setStepSize(j, val*n*probability[i]);
                }
            }
            a.sumUp();
            break;
        }
        case LOGNORMAL:{
            int n = population.size();
            if(generation == 0){
                for(int i = 0; i < n; ++i){
                    for(size_t j = 0; j < numIndex(); ++j){
                        population(i)->setStepSize(j, val);
                    }
                }
            } else {
                statistique a;
                for(int i = 0; i < n; ++i){
                    double coeff_indiv = BR.Normal();
                    double tho = 1/(sqrt(2. * (double) numIndex()));
                    double thoprime = 1 / sqrt(2. * sqrt((double) numIndex()));
                    double mean = 0;
                    for(size_t j = 0; j < numIndex(); ++j){
                        double coeff_direction = BR.Normal();
                        population(i)->setStepSize(j, abs(population(i)->step(j) * (exp(tho * coeff_indiv + thoprime * coeff_direction))));
                        mean += population(i)->step(j);
                    }
                    a.add(mean / (double) numIndex());
                }
                a.sumUp();
            }
            break;
        }
        case MUTATIVE:{
            int n = population.size();
            statistique a;
            for(int i = 0; i < n; ++i){
                bool mutate = (BR.Normal() < val ? true : false);
                double mean = 0;
                if(mutate){
                    double coeff = exp(BR.Normal());
                    for(size_t j = 0; j < numIndex(); ++j){
                        population(i)->setStepSize(j, population(i)->step(j) * coeff);
                        mean += population(i)->step(j) * coeff;
                    }
                }
                for(size_t j = 0; j < numIndex(); ++j){
                    mean += population(i)->step(j);
                }
                a.add(mean / (double) numIndex());
            }
            a.sumUp();
            break;
        }
        case MUTATIVE_SEPARATED:{
            int n = population.size();
            statistique a;
            for(int i = 0; i < n; ++i){
                double mean;
                for(size_t j = 0; j < numIndex(); ++j){
                    bool mutate = (BR.Normal() < val ? true : false);
                    if(mutate){
                        population(i)->setStepSize(j, population(i)->step(j)* exp(BR.Normal()));
                    }
                    mean += population(i)->step(j);
                }
                mean /= (double) numIndex();
            }
            a.sumUp();
            break;
        }
    }
}






























// =================================== Selection of parents (calls the good politics function) ===========================================

void GeneticGeneral::selectionParents(vector<int>* indices_selected, int nb_offspring){
	switch(Config.selection_for_parents){
		case BEST_SELECTION:{
            //cerr << "For now, the selection of the best is made for the selection step, not for choosing parents\n";
			break;
		}
		case RANDOM_SELECTION:{
			random_selection(indices_selected, 1 + nb_offspring / Config.nb_offsprings_of_cross_over() , Config.nb_parents_of_cross_over(), Config.retake_for_parents, false);
			break;}
		case PROPORTIONAL_SELECTION:{
			proportional_selection(indices_selected, 1 + nb_offspring / Config.nb_offsprings_of_cross_over() , Config.nb_parents_of_cross_over(), Config.cost_norm_for_parents, Config.sampling_method_for_parents, Config.retake_for_parents, false);
			break;}
		case TOURNAMENT_SELECTION_5PC:{
			int n = population.size();
			tournament_selection(indices_selected, 1 + nb_offspring / Config.nb_offsprings_of_cross_over() , Config.nb_parents_of_cross_over(), (int) (0.05 * n), Config.retake_for_parents, false);
			break;}			
		case TOURNAMENT_SELECTION_10PC:{
			int n = population.size();
			tournament_selection(indices_selected, 1 + nb_offspring / Config.nb_offsprings_of_cross_over() , Config.nb_parents_of_cross_over(), (int) (0.1 * n), Config.retake_for_parents, false);
			break;}			
		case TOURNAMENT_SELECTION_25PC:{
			int n = population.size();
			tournament_selection(indices_selected, 1 + nb_offspring / Config.nb_offsprings_of_cross_over() , Config.nb_parents_of_cross_over(), (int) (0.25 * n), Config.retake_for_parents, false);
			break;}			
		case TOURNAMENT_SELECTION_50PC:{
			int n = population.size();
			tournament_selection(indices_selected, 1 + nb_offspring / Config.nb_offsprings_of_cross_over() , Config.nb_parents_of_cross_over(), (int) (0.50 * n), Config.retake_for_parents, false);
			break;}			
		case RANK_BASED_SELECTION:{
			rank_based_selection(indices_selected, 1 + nb_offspring / Config.nb_offsprings_of_cross_over() , Config.nb_parents_of_cross_over(), Config.retake_for_parents, false);
			break;}
		case NO_NEED_SELECTION:{
			cerr << "You didn't define a selection policy for parents ('NO_NEED'), so you shouldn't call the selectionParents function.\n";
			break;
		}
		default:{
			cerr << "Undefined selection policy for parents\n";
		}
	}
}

// =================================== Selection of survivors (calls the good politics function) ===========================================

void GeneticGeneral::selection(int finalPopSize){
	vector<int> indices_survivors;
	switch(Config.selection_for_selec){
		case BEST_SELECTION:{
			int n = population.size();
			if(n < finalPopSize) { cerr << "You can not select the " << finalPopSize << " best ones out of a population of " << n << " !!!\n"; return;}
			population.sort();
			population.resize(finalPopSize);
		}
		case RANDOM_SELECTION:{
			random_selection(&indices_survivors, finalPopSize, 1, Config.retake_for_selec, false);
			break;}
		case PROPORTIONAL_SELECTION:{
			proportional_selection(&indices_survivors, finalPopSize, 1, Config.cost_norm_for_selec, Config.sampling_method_for_selec, Config.retake_for_selec, false);
			break;}
		case TOURNAMENT_SELECTION_5PC:{
			int n = population.size();
			tournament_selection(&indices_survivors, finalPopSize, 1, (int) (0.05 * n), Config.retake_for_selec, false);
			break;}			
		case TOURNAMENT_SELECTION_10PC:{
			int n = population.size();
			tournament_selection(&indices_survivors, finalPopSize, 1, (int) (0.1 * n), Config.retake_for_selec, false);
			break;}			
		case TOURNAMENT_SELECTION_25PC:{
			int n = population.size();
			tournament_selection(&indices_survivors, finalPopSize, 1, (int) (0.25 * n), Config.retake_for_selec, false);
			break;}			
		case TOURNAMENT_SELECTION_50PC:{
			int n = population.size();
			tournament_selection(&indices_survivors, finalPopSize, 1, (int) (0.50 * n), Config.retake_for_selec, false);
			break;}			
		case RANK_BASED_SELECTION:{
			rank_based_selection(&indices_survivors, finalPopSize, 1, Config.retake_for_selec, false);
			break;}
		case NO_NEED_SELECTION:{
			cerr << "You didn't define a selection policy for selection ('NO_NEED'), so you shouldn't call the selection function.\n";
			break;
		}
		default:{
			cerr << "Undefined selection policy for selection\n";
		}
	}	
	
	selection_keeping_indices(&indices_survivors);
}





















// =================================== Different politics of selection (for parents / survivors) ===========================================

void GeneticGeneral::selection_keeping_indices(vector<int>* indices_selected){
	sort(indices_selected->begin(), indices_selected->end());//, greater<int>() );
	int final_size = indices_selected->size(); // can be bigger with repetitions

	Population population2(final_size);

	/*cerr << "List of selected : (" << final_size << ")\n";
	for(int i = 0; i < final_size; ++i){
		cerr << (*indices_selected)[i] << " ";
	}
	cerr << "\n";*/
	
	for(int i = 0; i < final_size; ++i){
		//cerr << "Exchange " << i << "\t" << (*indices_selected)[i] << "\n";
		population2(i)->copy(population((*indices_selected)[i]));		
		//if((*indices_selected)[i] != i) population.replace(i, (*indices_selected)[i]); 
	}
	
	population.resize(0);
	population.copy(&population2);
	//population2.resize(0); NIOOOOO ! so ity keeps the pointers alive
}

// Warning ! popSize is the target size. The population could be bigger in inner steps
void GeneticGeneral::random_selection(vector<int>* indices_selected, int num_groups_to_select, int num_parents_by_group, 
					bool retake_allowed_between_groups,	bool same_parent_allowed_in_one_group)
{				      
	int currentSize = population.size();
	if((!same_parent_allowed_in_one_group) && (currentSize < num_parents_by_group)) {cerr << "CanNot chose " << num_parents_by_group << " different parents in a population of " << currentSize << " individuals\n"; return;}	
	if((!retake_allowed_between_groups) && (currentSize < num_groups_to_select*num_parents_by_group)) {cerr << "CanNot chose " << num_groups_to_select << " groups of " << num_parents_by_group << " totally different individuals in a population of " << currentSize << " individuals\n"; return;}	

	indices_selected->resize(num_groups_to_select * num_parents_by_group, -1);
	vector<bool> chosen(currentSize, false);
	for(int i = 0; i < num_groups_to_select; ++i){
		vector<bool> chosen_in_group(currentSize, false);
		for(int j = 0; j < num_parents_by_group; ++j){
			bool found = false;
			int cpt = 0;
			while(!found){
				cpt++;
				found = true;
				int position = currentSize * myRandom::Rnd();
				if((!retake_allowed_between_groups) && (chosen[position])) found = false;
				if((!same_parent_allowed_in_one_group) && (chosen_in_group[position])) found = false;
				if(cpt > 10000) found = true; //endl
				chosen[position] = true;	
				chosen_in_group[position] = true;
				(*indices_selected)[i*(num_parents_by_group)+j] = position;
			}
		}
	}
}

void GeneticGeneral::proportional_selection (vector<int>* indices_selected, int num_groups_to_select, int num_parents_by_group, 
					     COST_NORMALISATION_T cost_normalization_type, SAMPLING_METHOD_T sampling_method, 
					     bool retake_allowed_between_groups, bool same_parent_allowed_in_one_group, double temperature)
{
	int currentSize = population.size();
	if((!same_parent_allowed_in_one_group) && (currentSize < num_parents_by_group)) {cerr << "CanNot chose " << num_parents_by_group << " different parents in a population of " << currentSize << " individuals\n"; return;}	
	if((!retake_allowed_between_groups) && (currentSize < num_groups_to_select*num_parents_by_group)) {cerr << "CanNot chose " << num_groups_to_select << " groups of " << num_parents_by_group << " totally different individuals in a population of " << currentSize << " individuals\n"; return;}	

	// Computes probabilities of each individual with the correction_type
	vector<double> probability(currentSize, 0);
	double sum_corr_energies = 0;
	/*for(int i = 0; i < currentSize; ++i){
		worstever = max(worstever, population(i)->cost());
	} in principle, no need since updated at each updateCost call*/
	for(int i = 0; i < currentSize; ++i){
		double fitcorrec = 0;
		if(cost_normalization_type == CORRECTION_FROM_WORST) fitcorrec =  worstGlobal - population(i)->cost();
		else if(cost_normalization_type == CORRECTION_FROM_BEST) fitcorrec =  1.0 / (1.0 - (bestGlobal.cost()) + population(i)->cost());
		else if(cost_normalization_type == CORRECTION_BOLTZMANN) {
			double x = population(i)->cost() / temperature;
			fitcorrec = 1 / (1 + 1 + x + (x*x/2) + (x*x*x/6)); 
		}
		else if(cost_normalization_type == NO_CORRECTION) fitcorrec = 1.0/(1.0+population(i)->cost());
		probability[i] = fitcorrec;
		sum_corr_energies += fitcorrec;
	}
	for(int i = 0; i < currentSize; ++i){
        if((sum_corr_energies == 0) || std::isnan(sum_corr_energies) || (std::isinf(sum_corr_energies)))
			probability[i] = 1.0 / currentSize;
		else probability[i] /= sum_corr_energies;
	}
	
	if(DBG){
		cerr << "Probabilities : (" << num_groups_to_select * num_parents_by_group << ")\n";
		for(int i = 0; i < currentSize; ++i){
			cerr << probability[i] << "\t";
		}
		cerr << "\n";
	}

	indices_selected->resize(num_groups_to_select * num_parents_by_group, -1);
		
	if(sampling_method == BASIC_SAMPLING){
		vector<bool> chosen(currentSize, false);
		for(int i = 0; i < num_groups_to_select; ++i){
			vector<bool> chosen_in_group(currentSize, false);
			for(int j = 0; j < num_parents_by_group; ++j){
				bool found = false;
				int cpt = 0;
				while(!found){
					cpt++;
					int position = currentSize * myRandom::Rnd();
					if(myRandom::Rnd() < probability[position]){
						found = true;
						if((!retake_allowed_between_groups) && (chosen[position])) found = false;
						if((!same_parent_allowed_in_one_group) && (chosen_in_group[position])) found = false;
						if(cpt > 10000) found = true; //endl
						chosen[position] = true;	
						chosen_in_group[position] = true;
						if(found) (*indices_selected)[i*num_parents_by_group+j] = position;
					}
				}
			}
		}
	}
					     
	if(sampling_method == SAMPLING_ROULETTE_WHEEL){
		vector<bool> chosen(currentSize, false);
		for(int i = 0; i < num_groups_to_select; ++i){
			vector<bool> chosen_in_group(currentSize, false);
			for(int j = 0; j < num_parents_by_group; ++j){
				bool found = false;
				int cpt = 0;
				while(!found){
					cpt++;
					// -- Sampling here :
					double unif = myRandom::Rnd();
					int position = 0;
					double sum = probability[position];
					while((sum < unif) && (position < currentSize)){
						position++;
						sum += probability[position];
					}
					// --
					found = true;
					if((!retake_allowed_between_groups) && (chosen[position])) found = false;
					if((!same_parent_allowed_in_one_group) && (chosen_in_group[position])) found = false;
					if(cpt > 10000) found = true; //endl
					chosen[position] = true;	
					chosen_in_group[position] = true;
					if(found) (*indices_selected)[i*num_parents_by_group+j] = position;
				}
			}
		}
	}

	if(sampling_method == SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING){
		if((num_parents_by_group != 1) || (retake_allowed_between_groups == false)){cerr << "Stochastic Sampling can only be used with parameters :\n    - One parent by group\n    - Retake Allowed Between groupe\n"; return; }
		
		double step = 1.0/ (double) num_groups_to_select;
		double unif = myRandom::Rnd() * step;
		double sum = 0;
		vector<int> offsprings(currentSize, 0);
		
		for(int i = 0; i < currentSize; ++i){
			sum += probability[i];
			while(unif < sum){
				offsprings[i]++;
				unif += step;
			}
		}
		
		int nb_taken = 0;
		for(int i = 0; i < currentSize; ++i){
			for(int j = 0; j < offsprings[i]; ++j){
				(*indices_selected)[nb_taken] = i;
				nb_taken++;
			}			
		}
		if(nb_taken !=  num_groups_to_select) cerr << "Prop.selection with sampling stochastic universal failed to select\n";
	}
}

void GeneticGeneral::tournament_selection(vector<int>* indices_selected, int num_groups_to_select, int num_parents_by_group,int size_subpop, 
					  bool retake_allowed_between_groups,	bool same_parent_allowed_in_one_group)
{	
	int currentSize = population.size();
	if((!same_parent_allowed_in_one_group) && (currentSize < num_parents_by_group)) {cerr << "CanNot chose " << num_parents_by_group << " different parents in a population of " << currentSize << " individuals\n"; return;}	
	if((!retake_allowed_between_groups) && (currentSize < num_groups_to_select*num_parents_by_group)) {cerr << "CanNot chose " << num_groups_to_select << " groups of " << num_parents_by_group << " totally different individuals in a population of " << currentSize << " individuals\n"; return;}	

	indices_selected->resize(num_groups_to_select * num_parents_by_group, -1);

	vector<bool> chosen(currentSize, false);
	for(int i = 0; i < num_groups_to_select; ++i){
		vector<bool> chosen_in_group(currentSize, false);
		for(int j = 0; j < num_parents_by_group; ++j){
			bool found = false;
			int cpt = 0;
			while(!found){
				cpt++;
				// -- Sampling here :
				int position =  currentSize * myRandom::Rnd();
				double bestLocalFit = population(position)->cost();
				for(int k = 0; k < size_subpop - 1; ++k){
					int other = currentSize * myRandom::Rnd();
					if(population(other)->cost() < bestLocalFit){
						position = other;
						bestLocalFit = population(other)->cost();
					}
				}
				// --
				found = true;
				if((!retake_allowed_between_groups) && (chosen[position])) found = false;
				if((!same_parent_allowed_in_one_group) && (chosen_in_group[position])) found = false;
				if(cpt > 10000) found = true; //endl
				chosen[position] = true;	
				chosen_in_group[position] = true;
				if(found) (*indices_selected)[i*num_parents_by_group+j] = position;
			}
		}
	}

}

void GeneticGeneral::rank_based_selection(vector<int>* indices_selected, int num_groups_to_select, int num_parents_by_group,
					  bool retake_allowed_between_groups,	bool same_parent_allowed_in_one_group)
{
	int currentSize = population.size();
	if((!same_parent_allowed_in_one_group) && (currentSize < num_parents_by_group)) {cerr << "CanNot chose " << num_parents_by_group << " different parents in a population of " << currentSize << " individuals\n"; return;}	
	if((!retake_allowed_between_groups) && (currentSize < num_groups_to_select*num_parents_by_group)) {cerr << "CanNot chose " << num_groups_to_select << " groups of " << num_parents_by_group << " totally different individuals in a population of " << currentSize << " individuals\n"; return;}	

	population.sort();

	indices_selected->resize(num_groups_to_select * num_parents_by_group, -1);

	vector<bool> chosen(currentSize, false);
	for(int i = 0; i < num_groups_to_select; ++i){
		vector<bool> chosen_in_group(currentSize, false);
		for(int j = 0; j < num_parents_by_group; ++j){
			bool found = false;
			int cpt = 0;
			while(!found){
				cpt++;
				// -- Sampling here :
				int position =  currentSize * myRandom::Rnd() * myRandom::Rnd();
				// --
				found = true;
				if((!retake_allowed_between_groups) && (chosen[position])) found = false;
				if((!same_parent_allowed_in_one_group) && (chosen_in_group[position])) found = false;
				if(cpt > 10000) found = true; //endl;
				chosen[position] = true;
				chosen_in_group[position] = true;
				if(found) (*indices_selected)[i*num_parents_by_group+j] = position;
			}
		}
	}
}























// =================================== The cross-Overs ===========================================

void GeneticGeneral::cross_over_one_point(individual* fils, individual* parent1, individual* parent2){
	int position = 1 + (int) (((double) numIndex() - 2) * myRandom::Rnd()); // position to cross
	//if ((position == 0) || (position == (int) numIndex()-1)) cout << "oh\n";
	if(myRandom::Rnd() <= 0.5){
		fils->copy(parent2);
		for(int i = 0; i < position; ++i){
			fils->setGene(i, parent1->gene(i));
		}
	} else {
		fils->copy(parent1);
		for(int i = 0; i < position; ++i){
			fils->setGene(i, parent2->gene(i));
		}			
	}
}

void GeneticGeneral::cross_over_two_point(individual* fils, individual* parent1, individual* parent2){
	int position1 = 0, position2 = 0;
	if(numIndex() < 1){cerr << "NimIndex < 2\n"; return;}		
	while(position1 == position2){
		position1 = 0 + (int) (((double) numIndex() - 1) * myRandom::Rnd()); // position to cross
		position2 = 1 + (int) (((double) numIndex() - 1) * myRandom::Rnd()); // position to cross
	}
	if(position1 > position2){int tp = position1; position1 = position2; position2 = tp;};
	//if(DBG) cout << "Pos1 " << position1 << "\tPos2 " << position2 << "\n";
	if(myRandom::Rnd() <= 0.5){
		fils->copy(parent2);
		for(int i = position1; i < position2; ++i){
			fils->setGene(i, parent1->gene(i));
		}
	} else {
		fils->copy(parent1);
		for(int i = position1; i < position2; ++i){
			fils->setGene(i, parent2->gene(i));
		}		
	}
}

void GeneticGeneral::cross_over_wright1(individual* fils1, individual* fils2, individual* fils3, individual* parent1, individual* parent2){
	fils1->resize(numIndex());
	fils2->resize(numIndex());
	fils3->resize(numIndex());
	for(size_t i = 0; i < numIndex(); ++i){
		if(fils1) fils1->setGene(i, parent1->gene(i) + parent2->gene(i));
		if(fils2) fils2->setGene(i, 1.5*parent1->gene(i) - 0.5*parent2->gene(i));
		if(fils3) fils3->setGene(i, -0.5*parent1->gene(i) + 1.5*parent2->gene(i));
	}
}

void GeneticGeneral::cross_over_wright2(individual* fils, individual* parent1, individual* parent2){
	fils->resize(numIndex());
	if(parent2->cost() < parent1->cost()){
		for(size_t i = 0; i < numIndex(); ++i){
			fils->setGene(i, (myRandom::Rnd()*(parent2->gene(i) - parent1->gene(i))) + parent2->gene(i));
		}		
	} else {
		for(size_t i = 0; i < numIndex(); ++i){
			fils->setGene(i, (myRandom::Rnd()*(parent1->gene(i) - parent2->gene(i))) + parent1->gene(i));
		}		
	}
}

void GeneticGeneral::cross_over_arithmetic(individual* fils, individual* parent1, individual* parent2, double gamma){
	fils->resize(numIndex());
	if(myRandom::Rnd() <= 0.5){
		for(size_t i = 0; i < numIndex(); ++i){
			fils->setGene(i, (1 - gamma)*(parent1->gene(i)) + gamma * (parent2->gene(i)));
		}
	} else {
		for(size_t i = 0; i < numIndex(); ++i){
			fils->setGene(i, (1 - gamma)*(parent2->gene(i)) + gamma * (parent1->gene(i)));
		}
	}
}

void GeneticGeneral::cross_over_arithmetic(individual* fils, vector<individual*>* parents, vector<double>* gamma){
	fils->resize(numIndex());
	int numParents = parents->size();
	bool think_to_delete = false;

	if((!gamma) || ((int) gamma->size() != numParents)){
		if(gamma) cerr << "Warn : Cross_over_arithmetic : gamma[] not given or gamma should have the same size as parents. Take arithmetic mean as default:\n";
		gamma = new vector<double>(numParents, 1.0 / (double) numParents);
		think_to_delete = true;
	} else {
		double sum = 0;
		for(int i = 0; i < numParents; ++i){
			sum += (*gamma)[i];
		}
		if (abs(sum-1.0) > 1e-10) cerr << "cross_arithmetic : gamma (" << sum << ") should have sum = 1\n";
	}
	for(size_t i = 0; i < numIndex(); ++i){
		double accum = 0;
		for(int j = 0; j < numParents; ++j){
			accum += ((*gamma)[j])*(((*parents)[j])->gene(i));
		}
		fils->setGene(i, accum);
	}
	if(think_to_delete) delete gamma;
}

void GeneticGeneral::cross_over_BLXAlpha(individual* fils, individual* parent1, individual* parent2, double alpha){
	fils->resize(numIndex());
	if(parent2->cost() < parent1->cost()){cross_over_BLXAlpha(fils, parent2, parent1, alpha);}
	// so, x1 is better than x2
	
	for(size_t j = 0; j < numIndex(); ++j){
		double gammaj = - alpha + myRandom::Rnd() * (1.0+2*alpha);
		fils->setGene(j, (1 - gammaj)*(parent1->gene(j)) + gammaj * (parent2->gene(j)));
	}
}

void GeneticGeneral::cross_over_geometric(individual* fils, individual* parent1, individual* parent2, double gamma){
	fils->resize(numIndex());
	if(myRandom::Rnd() <= 0.5){
		for(size_t i = 0; i < numIndex(); ++i){
			fils->setGene(i, std::pow(parent1->gene(i), gamma) * std::pow(parent2->gene(i), 1-gamma));
		}
	} else {
		for(size_t i = 0; i < numIndex(); ++i){
			fils->setGene(i, std::pow(parent2->gene(i), gamma) * std::pow(parent1->gene(i), 1-gamma));
		}
	}
}

void GeneticGeneral::cross_over_geometric(individual* fils, vector<individual*>* parents, vector<double>* gamma){
	fils->resize(numIndex());
	int numParents = parents->size();
	bool think_to_delete = false;

	if((!gamma) || ((int) gamma->size() != numParents)){
		if(gamma) cerr << "Warn : Cross_over_arithmetic : gamma[] not given or gamma should have the same size as parents. Take arithmetic mean as default:\n";
		gamma = new vector<double>(numParents, 1.0 / (double) numParents);
		think_to_delete = true;
	} else {
		double sum = 0;
		for(int i = 0; i < numParents; ++i){
			sum += (*gamma)[i];
		}
		if (abs(sum-1) < 1e-10) cerr << "cross_arithmetic : gamma should have sum = 1\n";
	}
	for(size_t i = 0; i < numIndex(); ++i){
		double accum = 1;
		for(int j = 0; j < numParents; ++j){
			accum *= std::pow((*parents)[j]->gene(i), (*gamma)[j]);
		}
		fils->setGene(i, accum);
	}
	if(think_to_delete) delete gamma;
}

void GeneticGeneral::cross_over_SBX(individual* fils1, individual* fils2, individual* parent1, individual* parent2, double eta){
	fils1->resize(numIndex());
	fils2->resize(numIndex());
	// eta should be > 0
	double coeff = 1 / (eta + 1);
	for(size_t i = 0; i < numIndex(); ++i){
		double rj = myRandom::Rnd();
		double gammaj = ((rj <= 0.5) ? (std::pow(2*rj, coeff)) : (std::pow(1 / (2 - 2*rj), coeff)));				
		fils1->setGene(i, 0.5*((1 - gammaj)*(parent1->gene(i)) + (1 + gammaj) * (parent2->gene(i))));
		fils2->setGene(i, 0.5*((1 + gammaj)*(parent1->gene(i)) + (1 - gammaj) * (parent2->gene(i))));
	}	
}

void GeneticGeneral::simple_random(individual* fils){
	if(fils->size() != (int) numIndex()) {cerr << "You call simple random crossover to a child that has no initial values\n"; return;}; 
	int i = numIndex() * myRandom::Rnd();
	double rndValue = paraLowVector(i) + ((paraHighVector(i) - paraLowVector(i)) * myRandom::Rnd());
	fils->setGene(i,rndValue);
}

void GeneticGeneral::cross_over_UNDX(individual* fils1, individual* parent1, individual* parent2, individual* parent3){
	if (numIndex() < 5) cerr << "Can not do a UNDX if less than 5 dimensions\n"; 
	
	fils1->resize(numIndex());
	
	static vector<vector<double>* > support;
	static bool instanciated = false;
	if(!instanciated){
		support.resize(numIndex());
		for(size_t i = 0; i < numIndex(); ++i){
			support[i] = new vector<double>(numIndex(), 0);
		}
		instanciated = true;
	}
	
	vector<vector<double>*> Parents;
	individual middle = individual();
	middle.resize(numIndex());
	for(size_t i = 0; i < numIndex(); ++i){
		middle.setGene(i, (parent1->gene(i) + parent2->gene(i))/2.0);
	}
	
	Parents.resize(2);
	Parents[0] = new vector<double>(numIndex(),0);
	Parents[1] = new vector<double>(numIndex(),0);
	//Parents[2] = new vector<double>(numIndex(),0);
	for(size_t i = 0; i < numIndex(); ++i){
		(*Parents[0])[i] = parent1->gene(i) - middle.gene(i);
		//(*Parents[1])[i] = parent2->gene(i) - middle.gene(i);
		(*Parents[1])[i] = parent3->gene(i) - middle.gene(i); // Important !!- middle.gene(i);
	}

	/*if(!instanciated){
		for(int i = 0; i < 3; ++i){
			cout << "Parents after  " << i << ":\t";
			for(int j = 0; j < (int) numIndex(); ++j){
				cout << (*Parents[i])[j] << "\t";
			}
			cout << "\n";
		}
	
			cout << "Middle:\t";
			for(int j = 0; j < (int) numIndex(); ++j){
				cout << middle.gene(j) << "\t";
			}
			cout << "\n";
		
	}*/

	double delta;
	bool res = Ortho::orthogonalise(&support, &Parents, numIndex(), &delta);
	if(!res){
		mutate_one_point(parent1, Config.mutation_distribution);
		updateCost(parent1);
		for(size_t i = 0; i < numIndex(); ++i){
			middle.setGene(i, (parent1->gene(i) + parent2->gene(i))/2.0);
		}	
		for(size_t i = 0; i < numIndex(); ++i){
			(*Parents[0])[i] = parent1->gene(i) - middle.gene(i);
		}
		res = Ortho::orthogonalise(&support, &Parents, numIndex(), &delta);
	}
	
	//if((!instanciated) && (!res)) cerr << "Pb during orthogonalisation\n";
	//if(!res) cerr << "PB of orthogonalisation \n!";
	double sigma1 = 1;
	double sigma2 = 0.35 / sqrt(max(1,(int) numIndex() - 5));
	
	for(size_t j = 0; j < numIndex(); ++j){
		fils1->setGene(j, middle.gene(j));
	}
	
	if(res){
		for(int v = 0; v < 1; ++v){
			double coeff = sigma1*(BR.Normal());
			for(size_t j = 0; j < numIndex(); ++j){
				//fils1->setGene(j, fils1->gene(j) + coeff*((*support[v])[j]));
				fils1->setGene(j, fils1->gene(j) + coeff*((*Parents[v])[j])); // warning : can't use that for the last parent because -middle has not been done
			}	
		}
		for(size_t v = 1; v < numIndex(); ++v){
			double coeff = sigma2*(BR.Normal());
			for(size_t j = 0; j < numIndex(); ++j){
				fils1->setGene(j, fils1->gene(j) + coeff*((*support[v])[j]));
			}	
		}
	} else {
        //cerr << "Orthog problem\n";
		for(size_t j = 0; j < numIndex(); ++j){
			fils1->setGene(j, middle.gene(j)*(1+0.1*myRandom::Rnd()));
		}	
	}
		
	/*if(!instanciated){
		for(size_t i = 0; i < numIndex(); ++i){
			cout << "Vector " << i << ":\t";
			for(int j = 0; j < (int) numIndex(); ++j){
				cout << (*support[i])[j] << "\t";
			}
			cout << "\n";
		}
	}*/

	
	
	delete Parents[0];
	delete Parents[1];
	//delete Parents[2];
}

void GeneticGeneral::cross_over_UNDX(individual* fils1, individual* parent1, individual* parent2, individual* parent3, individual* parent4){
	if (numIndex() < 6) cerr << "Can not do a UNDX 4parents if less than 6 dimensions\n"; 
	fils1->resize(numIndex());
	
	static vector<vector<double>* > support;
	static bool instanciated = false;
	if(!instanciated){
		support.resize(numIndex());
		for(size_t i = 0; i < numIndex(); ++i){
			support[i] = new vector<double>(numIndex(), 0);
		}
		instanciated = true;
	}
	
	vector<vector<double>*> Parents;
	individual middle = individual();
	middle.resize(numIndex());
	for(size_t i = 0; i < numIndex(); ++i){
		middle.setGene(i, (parent1->gene(i) + parent2->gene(i) + parent3->gene(i))/3.0);
	}
	
	Parents.resize(3);
	Parents[0] = new vector<double>(numIndex(),0);
	Parents[1] = new vector<double>(numIndex(),0);
	Parents[2] = new vector<double>(numIndex(),0);
	//Parents[3] = new vector<double>(numIndex(),0);
	for(size_t i = 0; i < numIndex(); ++i){
		(*Parents[0])[i] = parent1->gene(i) - middle.gene(i);
		(*Parents[1])[i] = parent2->gene(i) - middle.gene(i);
		//(*Parents[2])[i] = parent3->gene(i) - middle.gene(i);
		(*Parents[2])[i] = parent4->gene(i) - middle.gene(i);
	}

	double delta;
	bool res = Ortho::orthogonalise(&support, &Parents, numIndex(), &delta);
	if(!res){
		mutate_one_point(parent1, Config.mutation_distribution);
		updateCost(parent1);
		for(size_t i = 0; i < numIndex(); ++i){
			middle.setGene(i, (parent1->gene(i) + parent2->gene(i))/2.0);
		}	
		for(size_t i = 0; i < numIndex(); ++i){
			(*Parents[0])[i] = parent1->gene(i) - middle.gene(i);
		}
		res = Ortho::orthogonalise(&support, &Parents, numIndex(), &delta);
	}
	
	//if((!instanciated) && (!res)) cerr << "Pb during orthogonalisation\n";
	//if(!res) cerr << "PB of orthogonalisation \n!";
	double sigma1 = 1;
	double sigma2 = 0.35 / sqrt(max(1,(int) numIndex() - 6));
	
	for(int j = 0; j < (int) numIndex(); ++j){
		fils1->setGene(j, middle.gene(j));
	}
	
	if(res){
		for(int v = 0; v < 2; ++v){
			double coeff = sigma1*(BR.Normal());
			for(int j = 0; j < (int) numIndex(); ++j){
				//fils1->setGene(j, fils1->gene(j) + coeff*((*support[v])[j]));
				fils1->setGene(j, fils1->gene(j) + coeff*((*Parents[v])[j])); // warning : can't use that for the last parent because -middle has not been done
			}	
		}
		for(int v = 2; v < (int) numIndex(); ++v){
			double coeff = sigma2*delta*(BR.Normal());
			for(int j = 0; j < (int) numIndex(); ++j){
				fils1->setGene(j, fils1->gene(j) + coeff*((*support[v])[j]));
			}	
		}
	} else {
        //cerr << "Orthog problem\n";
		for(int j = 0; j < (int) numIndex(); ++j){
			fils1->setGene(j, middle.gene(j)*(1+0.1*myRandom::Rnd()));
		}	
	}
	/*if(!instanciated){
		for(size_t i = 0; i < numIndex(); ++i){
			cout << "Vector " << i << ":\t";
			for(int j = 0; j < (int) numIndex(); ++j){
				cout << (*support[i])[j] << "\t";
			}
			cout << "\n";
		}
	}*/

	delete Parents[0];
	delete Parents[1];
	delete Parents[2];
	//delete Parents[3];
	/*for(size_t i = 0; i < numIndex(); ++i){
		delete support[i];
	};
	cout << "FIN\n";*/
}














// =========================================== Auxiliary functions =============================================

void print(vector<int>* v){
	int n = v->size();
	cerr << n << " IND : ";
	for(int i = 0; i < n; ++i){
		cerr << ",   " << (*v)[i];
	}
	cerr << "\n";
}

void GeneticGeneral::initRandoms(){
    BR.CreateNormal(0,1); // Usefull in all cases
    switch(Config.mutation_distribution){
        case UNIFORM_STEP:{
            // Nothing to do for U(0,1)
            break;}
        case NORMAL:{
            BR.CreateNormal(0,1);
            break;}
        case CAUCHY:{
            BR.CreateCauchy(0,0.0155);
            // Will depend on the value of sig each time !!
            break;}
        case PUT_RANDOM:{
            // Nothing to do
            break;}
        case LEVY:{
            // Not implemented
            break;}
        case EXPONENTIAL:{
            // Will depend on the value of sig each time !!
            break;}
        case GAMMA:{
            // Will depend on the value of sig each time !!
            break;}
        case COMBINED:{
            BR.CreateNormal(0,1);
            BR.CreateCauchy(0,1);
            break;}
        default:{
            cerr << "Config : undefined Mutation Policy (" << Config.mutation_distribution << "\n";
        }
    }
}

bool GeneticGeneral::metropolis(double E1, double E2){
    double proba_change = std::exp((E1 - E2)/temperature);
    return (myRandom::Rnd() <= proba_change);
}

void GeneticGeneral::MegaTest(){
	cerr << setiosflags(ios::fixed);
    resetCostCalls();
	int numMutations;
	int popSize;

	if(false){
		// Test the functions one by one :
		// Configuration :
		cerr << " - Config : - \n";
		cerr << "Sel : " << Config.type_of_selection_selec() << "\n";
		cerr << "C-O : " << Config.type_of_cross_over() << "\n";
		cerr << "Off : " << Config.nb_offsprings_of_cross_over() << "\n";
		cerr << "Par : " << Config.nb_parents_of_cross_over() << "\n";

		initializeRandomPop(popSize);
		population.print();
		cerr << "Ageing\n";
		AgePopulation();
		cerr << "Add " << numMutations << " Mutants\n";
		AddMutants(numMutations);
		population.print();
		cerr << "Ageing\n";
		AgePopulation();
		cerr << "Add 5 Random individuals\n";
		addRandom(5);
		population.print();
		cerr << "Kill older than 1\n";
		killAgeing(1);	
		population.print();
		return;
	}
	

	if(false){
		population.resize(0);
		population.resize(10);
		for(int i = 0; i < 10; ++i){
			population(i)->resize(10);
			population(i)->setGene(i,pow((double) 2,9-i));
			updateCost(population(i));
		}
		population.print();

		
		vector<int> selected;
		
		cerr << "Random : 4 groupes de 3 parents Avec reprise pour les diff groupes\n";
		random_selection(&selected, 10, 3,true, false);
		print(&selected);

		cerr << "Random : 2 groupes de 3 parents SANS reprise pour les diff groupes\n";
		random_selection(&selected, 2, 3, false, false);
		print(&selected);

		cerr << "Proportional : 4 groupes de 3 parents Avec reprise pour les diff groupes\n";
		cerr << "\nBasic Sampling - NO CORREC (inv prop cost (1/(1+x)))\n";
		proportional_selection (&selected, 30, 1, NO_CORRECTION, BASIC_SAMPLING, true, false);
		print(&selected);
		cerr << "\nBasic Sampling - CORREC FROM WORST (fworst - f))\n";
		proportional_selection (&selected, 30, 1, CORRECTION_FROM_WORST, BASIC_SAMPLING, true, false);
		print(&selected);
		cerr << "\nBasic Sampling - CORREC FROM BEST (1/(1 + f - fmin)))\n";
		proportional_selection (&selected, 30, 1, CORRECTION_FROM_BEST, BASIC_SAMPLING, true, false);
		print(&selected);
		cerr << "\nBasic Sampling - CORREC BOLZMANN \n";
		proportional_selection (&selected, 30, 1, CORRECTION_BOLTZMANN, BASIC_SAMPLING, true, false, 10);
		print(&selected);

		cerr << "\nRoulette - NO CORREC (inv prop cost (1/(1+x)))\n";
		proportional_selection (&selected, 30, 1, NO_CORRECTION, SAMPLING_ROULETTE_WHEEL, true, false);
		print(&selected);
		cerr << "\nRoulette - CORREC FROM WORST (fworst - f))\n";
		proportional_selection (&selected, 30, 1, CORRECTION_FROM_WORST, SAMPLING_ROULETTE_WHEEL, true, false);
		print(&selected);
		cerr << "\nRoulette - CORREC FROM BEST (1/(1 + f - fmin)))\n";
		proportional_selection (&selected, 30, 1, CORRECTION_FROM_BEST, SAMPLING_ROULETTE_WHEEL, true, false);
		print(&selected);
		cerr << "\nRoulette - CORREC BOLZMANN \n";
		proportional_selection (&selected, 30, 1, CORRECTION_BOLTZMANN, SAMPLING_ROULETTE_WHEEL, true, false, 10);
		print(&selected);

		cerr << "\nUniversal - NO CORREC (inv prop cost (1/(1+x)))\n";
		proportional_selection (&selected, 30, 1, NO_CORRECTION, SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING, true, false);
		print(&selected);
		cerr << "\nUniversal - CORREC FROM WORST (fworst - f))\n";
		proportional_selection (&selected, 30, 1, CORRECTION_FROM_WORST, SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING, true, false);
		print(&selected);
		cerr << "\nUniversal - CORREC FROM BEST (1/(1 + f - fmin)))\n";
		proportional_selection (&selected, 30, 1, CORRECTION_FROM_BEST, SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING, true, false);
		print(&selected);
		cerr << "\nUniversal - CORREC BOLZMANN \n";
		proportional_selection (&selected, 30, 1, CORRECTION_BOLTZMANN, SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING, true, false, 10);
		print(&selected);
		
		cerr << "tournament selection 4 groups of 3, subpop 1, retake allowed\n";
		tournament_selection     (&selected, 4, 3, 1, true, false);
		print(&selected);
		cerr << "tournament selection 4 groups of 3, subpop 2, retake allowed\n";
		tournament_selection     (&selected, 4, 3, 2, true, false);
		print(&selected);
		cerr << "tournament selection 4 groups of 3, subpop 4, retake allowed\n";
		tournament_selection     (&selected, 4, 3, 4, true, false);
		print(&selected);
		cerr << "tournament selection 4 groups of 3, subpop 10, retake allowed\n";
		tournament_selection     (&selected, 4, 3, 10, true, false);
		print(&selected);
		cerr << "tournament selection 4 groups of 3, subpop 20, retake allowed\n";
		tournament_selection     (&selected, 4, 3, 20, true, false);
		print(&selected);

		// Warning : the rank selection do a sorting of the population, so indices are new ones !
		cerr << "rank selection 4 groups of 3, retake allowed\n";
		rank_based_selection     (&selected, 4, 3,    true, false);		
		print(&selected);
	
		selected.resize(4);
		selected[0] = 5;
		selected[1] = 6;
		selected[2] = 7;
		selected[3] = 8;

		selection_keeping_indices(&selected);	

		population.print();
	};

	
	if(false){
		statistique a = statistique();
		BR.CreateCauchy(0,0.0155);
		statistique b = statistique();
		double sig = 1;
		for(int j = 0; j < 20; ++j){
			for(int i = 0; i < 200000; ++i){
				double tirage;
				//tirage = BR.Cauchy();
				tirage = MCOMBINED;
				if(abs(tirage) < 100) a.add(tirage);
			}
			a.sumUp();
			b.add(a.variance);
			a.reset();
		}
		b.sumUp();
		cout << "VARIANCE : " << b.mean() << "\n";
		return;
		
		//0.001 --->0,2
		//1 ---> 7,8
		//0.1 --> 2.5
		//0.01 --> 0.75
		
		// 5 * cauchy(0,1) -> exp(8)
		// cauchy(0,0.1) -> exp(4.1)
		// cauchy(0,0.001) -> exp(2.3)
		
	}
	
	if(false){
		individual* p1 = new individual();
		individual* p2 = new individual();
		individual* p3 = new individual();
		individual* p4 = new individual();
		//individual* p5 = new individual();
		individual* f1 = new individual();	// Cross-over functions should be able to take empty sons 
		individual* f2 = new individual(); 
		individual* f3 = new individual(); 

		if(numIndex() != 10) cerr << "PPPBBB\n";
		p1->resize(10);
		for(int i = 0; i < 3; ++i){
			p1->setGene(i,i+3);
		}
		updateCost(p1);
		p1->print();
		p2->resize(10);
		for(int i = 0; i < 3; ++i){
			p2->setGene(i,1);
		}	
		updateCost(p2);
		p2->print();
		
		p3->resize(10);
		p3->setGene(0,-1);
		p3->setGene(1,2);
		p3->setGene(2,-1);
		p3->setGene(4,-1);
		
		updateCost(p3);
		p3->print();

		p4->resize(10);
		p4->setGene(0,5);
		p4->setGene(1,2);
		p4->setGene(2,-1);
		p4->setGene(3,6);
		updateCost(p4);
		p4->print();

		
		FILE* PAR = fopen("CrossOvers/P1.txt", "w");	
		fprintf(PAR, "%f\t%f\n", p1->gene(0), p1->gene(1));
		fclose(PAR);
		PAR = fopen("CrossOvers/P2.txt", "w");	
		fprintf(PAR, "%f\t%f\n", p2->gene(0), p2->gene(1));
		fclose(PAR);
		PAR = fopen("CrossOvers/P3.txt", "w");	
		fprintf(PAR, "%f\t%f\n", p3->gene(0), p3->gene(1));
		fclose(PAR);
		PAR = fopen("CrossOvers/P4.txt", "w");	
		fprintf(PAR, "%f\t%f\n", p4->gene(0), p4->gene(1));
		fclose(PAR);
		BR.CreateNormal(0,0.3);
		
		FILE* TCRO = NULL;
		BR.CreateCauchy(0,0.2);
		for(int tp = 0; tp < 27; ++tp){
			if(tp == 1) TCRO = fopen("CrossOvers/COnePoint.txt", "w");
			if(tp == 2) TCRO = fopen("CrossOvers/CTwoPoints.txt", "w");
			if(tp == 3) TCRO = fopen("CrossOvers/CWright1.txt", "w");
			if(tp == 4) TCRO = fopen("CrossOvers/CWright2.txt", "w");
			if(tp == 5) TCRO = fopen("CrossOvers/CArith0p5.txt", "w");
			if(tp == 6) TCRO = fopen("CrossOvers/CArith0p1.txt", "w");
			if(tp == 7) TCRO = fopen("CrossOvers/CArith3parents.txt", "w");
			if(tp == 8) TCRO = fopen("CrossOvers/CBLXAlpha0p5.txt", "w");
			if(tp == 9) TCRO = fopen("CrossOvers/CBLXAlpha0p1.txt", "w");
			if(tp == 10) TCRO = fopen("CrossOvers/CBLXAlpha0p9.txt", "w");
			if(tp == 11) TCRO = fopen("CrossOvers/CGeometric0p5.txt", "w");
			if(tp == 12) TCRO = fopen("CrossOvers/CGeometric0p1.txt", "w");
			if(tp == 13) TCRO = fopen("CrossOvers/CGeometric0p9.txt", "w");
			if(tp == 14) TCRO = fopen("CrossOvers/CGeometric3parents.txt", "w");
			if(tp == 15) TCRO = fopen("CrossOvers/CSBX1p0.txt", "w");
			if(tp == 16) TCRO = fopen("CrossOvers/CSBX0p2.txt", "w");
			if(tp == 0) TCRO = fopen("CrossOvers/CSBX0p01.txt", "w");
			if(tp == 17) TCRO = fopen("CrossOvers/CSBX5p0.txt", "w");
			if(tp == 18) TCRO = fopen("CrossOvers/CSRandom.txt", "w");
			
			if(tp == 19) TCRO = fopen("CrossOvers/MNormal.txt", "w");
			if(tp == 20) TCRO = fopen("CrossOvers/MCauchy.txt", "w");
			if(tp == 21) TCRO = fopen("CrossOvers/MPutRandom.txt", "w");
			if(tp == 22) TCRO = fopen("CrossOvers/MExponential.txt", "w");
			if(tp == 23) TCRO = fopen("CrossOvers/MCombined.txt", "w");
			if(tp == 24) TCRO = fopen("CrossOvers/MUnifStep.txt", "w");

			if(tp == 25) TCRO = fopen("CrossOvers/CUNDX3p.txt", "w");
			if(tp == 26) TCRO = fopen("CrossOvers/CUNDX4p.txt", "w");

			
			if(tp == 1) fprintf(TCRO, "cross_over_one_point(f1, p1, p2);\n");
			if(tp == 2) fprintf(TCRO, "cross_over_two_point(f1, p1, p2);\n");
			if(tp == 3) fprintf(TCRO, "cross_over_wright1(f1, f2, f3, p1, p2);\n");
			if(tp == 4) fprintf(TCRO, "cross_over_wright2(f1, p1, p2);\n");
			if(tp == 5) fprintf(TCRO, "cross_over_arithmetic(f1, p1, p2, 0.5);\n");
			if(tp == 6) fprintf(TCRO, "cross_over_arithmetic(f1, p1, p2, 0.1);\n");
			if(tp == 7) fprintf(TCRO, "cross_over_arithmetic(f1, &lp, NULL);\n");
			if(tp == 8) fprintf(TCRO, "cross_over_BLXAlpha(f1, p1, p2, 0.5);\n");
			if(tp == 9) fprintf(TCRO, "cross_over_BLXAlpha(f1, p1, p2, 0.1);\n");
			if(tp == 10) fprintf(TCRO, "cross_over_BLXAlpha(f1, p1, p2, 0.9);\n");
			if(tp == 11) fprintf(TCRO, "cross_over_geometric(f1, p1, p2, 0.5);\n");
			if(tp == 12) fprintf(TCRO, "cross_over_geometric(f1, p1, p2, 0.1);\n");
			if(tp == 13) fprintf(TCRO, "cross_over_geometric(f1, p1, p2, 0.9);\n");
			if(tp == 14) fprintf(TCRO, "cross_over_geometric(f1, &lp, NULL);\n");
			if(tp == 15) fprintf(TCRO, "cross_over_SBX(f1, f2, p1, p2, 1);\n");
			if(tp == 16) fprintf(TCRO, "cross_over_SBX(f1, f2, p1, p2, 0.2);\n");
			if(tp == 0) fprintf(TCRO, "cross_over_SBX(f1, f2, p1, p2, 0.01);\n");
			if(tp == 17) fprintf(TCRO, "cross_over_SBX(f1, f2, p1, p2, 5);\n");
			if(tp == 18) fprintf(TCRO, "simple_random(f1);\n");
			
			if(tp == 19) fprintf(TCRO, "mutation NORMAL;\n");
			if(tp == 20) fprintf(TCRO, "mutation CAUCHY;\n");
			if(tp == 21) fprintf(TCRO, "mutation PUTRANDOM;\n");
			if(tp == 22) fprintf(TCRO, "mutation EXOPONENTIAL;\n");
			if(tp == 23) fprintf(TCRO, "mutation COMBINED;\n");
			if(tp == 24) fprintf(TCRO, "mutation UNIFORM_STEP;\n");

			if(tp == 25) fprintf(TCRO, "cross_over_UNDX(f1, p1, p2, p3, p4);\n");
			if(tp == 26) fprintf(TCRO, "cross_over_UNDX(f1, p1, p2, p3, p4);\n");

			
			for(int i = 0; i < (100 * (((tp < 25) && (tp > 18)) ? 10 : 1)); ++i){
				if(tp == 1) cross_over_one_point(f1, p1, p2);
				if(tp == 2) cross_over_two_point(f1, p1, p2);
				if(tp == 3) cross_over_wright1(f1, f2, f3, p1, p2);
				if(tp == 4) cross_over_wright2(f1, p1, p2);
				if(tp == 5) cross_over_arithmetic(f1, p1, p2, 0.5);
				if(tp == 6) cross_over_arithmetic(f1, p1, p2, 0.1);
				vector<individual*> lp;
				lp.resize(3);
				lp[0] = p1;
				lp[1] = p2;
				lp[2] = p3;
				if(tp == 7) cross_over_arithmetic(f1, &lp, NULL);
				if(tp == 8) cross_over_BLXAlpha(f1, p1, p2, 0.5);
				if(tp == 9) cross_over_BLXAlpha(f1, p1, p2, 0.1);
				if(tp == 10) cross_over_BLXAlpha(f1, p1, p2, 0.9);
				if(tp == 11) cross_over_geometric(f1, p1, p2, 0.5);
				if(tp == 12) cross_over_geometric(f1, p1, p2, 0.1);
				if(tp == 13) cross_over_geometric(f1, p1, p2, 0.9);
				if(tp == 14) cross_over_geometric(f1, &lp, NULL);
				if(tp == 15) cross_over_SBX(f1, f2, p1, p2, 1);
				if(tp == 16) cross_over_SBX(f1, f2, p1, p2, 0.2);
				if(tp == 0)  cross_over_SBX(f1, f2, p1, p2, 0.01);
				if(tp == 17) cross_over_SBX(f1, f2, p1, p2, 5);
				if(tp == 18) {
					/*paraLowVector(0) = -10;
					paraLowVector(1) = -10;
					paraHighVector(0) = 10;
					paraHighVector(1) = 10;*/
					f1->copy(p1);
					simple_random(f1);
				}
				if(tp == 25) cross_over_UNDX(f1, p1, p2, p3);
				if(tp == 26) cross_over_UNDX(f1, p1, p2, p3, p4);
				
				
				if(tp == 19){
					f1->copy(p1);
					mutate_all_points(f1, NORMAL);
				}
				if(tp == 20){
					f1->copy(p1);
					mutate_all_points(f1, CAUCHY);
				}
				if(tp == 21){
					f1->copy(p1);
					mutate_all_points(f1, PUT_RANDOM);
				}
				if(tp == 22){
					f1->copy(p1);
					mutate_all_points(f1, EXPONENTIAL);
				}
				if(tp == 23){
					f1->copy(p1);
					mutate_all_points(f1, COMBINED);
				}
				if(tp == 24){
					f1->copy(p1);
					mutate_all_points(f1, UNIFORM_STEP);
				}	
				
				if(tp == 3){
					fprintf(TCRO, "%f\t%f\t%f\t%f\t%f\t%f\n", f1->gene(0), f1->gene(1), f2->gene(0), f2->gene(1), f3->gene(0), f3->gene(1));
				} else if ((tp == 15) || (tp == 16) || (tp == 17) || (tp == 0)){
					fprintf(TCRO, "%f\t%f\t%f\t%f\n", f1->gene(0), f1->gene(1), f2->gene(0), f2->gene(1));
				} else fprintf(TCRO, "%f\t%f\n", f1->gene(0), f1->gene(1));

			}
			fclose(TCRO);
		}
	
		return;
	}
	
	if(false){
		//population.resize(10); //NO !!! You can not mutate a population of empty elements !!!
		//initializeRandomPop(10);
		
			population.resize(0);
			population.resize(10);
			for(int i = 0; i < 10; ++i){
				population(i)->resize(10);
				population(i)->setGene(i,pow((double) 2,9-i));
				updateCost(population(i));
			}
			population.print();
		AddMutants(5);
		population.print();

		
		AddMutants(5.9);
		population.print();
		
		AddOffspring(5);
		population.print();
		
		AddMutatedOffspring(5);
		population.print();

		selection(10);
		
		Config.replacement_policy = REPLACE_WORST_PARENT;
		AddMutatedOffspringAndReplace(5); // let's take n_it = p_c * size_pop    is the p_c necessary????, int proba_cross_over);
		population.print();

		temperature = 10;
		Config.replacement_policy = REPLACE_WORST_PARENT_METROPOLIS;
		AddMutatedOffspringAndReplace(5); // let's take n_it = p_c * size_pop    is the p_c necessary????, int proba_cross_over);
		population.print();

		Config.replacement_policy = REPLACE_WORST;
		AddMutatedOffspringAndReplace(5); // let's take n_it = p_c * size_pop    is the p_c necessary????, int proba_cross_over);
		population.print();

		Config.replacement_policy = REPLACE_RANDOM;
		AddMutatedOffspringAndReplace(5); // let's take n_it = p_c * size_pop    is the p_c necessary????, int proba_cross_over);
		population.print();

		Config.replacement_policy = REPLACE_RANDOM_METROPOLIS;
		AddMutatedOffspringAndReplace(5); // let's take n_it = p_c * size_pop    is the p_c necessary????, int proba_cross_over);
		population.print();

		//	population.doStatistics();
		//	std::cout << "Evolution : " << generation << "\t" << bestGlobal.cost() << "  \t" << population.bestLocalCost << "  \t" <<  population.meanCost << "\t" << population.worstLocalCost << "\n";
	}
}
