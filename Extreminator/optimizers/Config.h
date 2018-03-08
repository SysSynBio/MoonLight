/* ---------------- Configuration : different choices in terms of  selection / mutation / parents policy ------------------------ */

/* 	These parameters will be global in the GeneticGeneral Class, but you can change them when you want. the point is to not pass 
	them to the functions each time ! */

typedef enum{
	NO_NEED_SELECTION = -1,
	BEST_SELECTION = 0,
	PROPORTIONAL_SELECTION = 1,
	TOURNAMENT_SELECTION_5PC = 2,
	TOURNAMENT_SELECTION_10PC = 3,
	TOURNAMENT_SELECTION_25PC = 4,
	TOURNAMENT_SELECTION_50PC = 5,
	RANK_BASED_SELECTION = 6,
	RANDOM_SELECTION = 7
} SELECTION_TYPE_T;

typedef enum {
	NO_NEED_NORM = -2,
	NO_CORRECTION = -1,
	CORRECTION_FROM_WORST = 0,
	CORRECTION_FROM_BEST = 1,
	CORRECTION_BOLTZMANN = 2
} COST_NORMALISATION_T;

typedef enum {
	NO_NEED_SAMPLING = -1,
	BASIC_SAMPLING = 0,
	SAMPLING_ROULETTE_WHEEL = 1,
	SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING = 2
} SAMPLING_METHOD_T;

typedef enum {
	UNIFORM_STEP = 0,
	NORMAL = 1,
	CAUCHY = 2,
	PUT_RANDOM = 3,
	LEVY = 4,		// Not implemented yet
	EXPONENTIAL = 5,
	GAMMA = 6,
	COMBINED = 7
} DISTRIBUTION_T;


typedef enum {
	MUTATE_ONE_POINT = 0,
	MUTATE_ALL_POINTS = 1
} POLICY_MUTATE_T;

typedef enum {
	NO_NEED_CROSSOVER = -1,
	ONE_POINT_CROSSOVER = 0,
	TWO_POINT_CROSSOVER = 1,
	WRIGHT1_CROSSOVER = 2,
	WRIGHT2_CROSSOVER = 3,
	ARITHMETIC_CROSSOVER = 4,
	ARITHMETIC_MULTIPARENTS_CROSSOVER = 5,
	BLXALPHA_CROSSOVER = 6,
	GEOMETRIC_CROSSOVER = 7,
	GEOMETRIC_MULTIPARENTS_CROSSOVER = 8,
	SBX_CROSSOVER = 9,
	RANDOMIZE_ONE_POINT_CROSSOVER = 10,
	UNDX_CROSSOVER_3P = 11,
	UNDX_CROSSOVER_4P = 12,
	SPX_CROSSOVER = 13,
	PCX_CROSSOVER = 14
} CROSS_OVER_T;

typedef enum {
	NO_NEED_REPLACEMENT = -1,
	REPLACE_WORST_PARENT = 0,
	REPLACE_WORST_PARENT_METROPOLIS = 1,
	REPLACE_WORST = 2,
	REPLACE_RANDOM = 3,
	REPLACE_RANDOM_METROPOLIS = 4,
	KILL_TOURNAMENT = 5,
	REPLACE_OLDEST = 6,
	CONSERVATIVE_SELECTION = 7,
	ELITIST = 8
} REPLACEMENT_POLICY_T;

typedef enum {
	CONSTANT = 0,
	EXPONENTIAL_DOWN = 1,
	DISTANCE_BEST = 2,
	DISTANCE_BEST_SEPARATED = 3,
	PROPORTIONAL_NORMALIZED = 4,
	LOGNORMAL = 5,
	MUTATIVE = 6,
	MUTATIVE_SEPARATED = 7
} STRATEGY_T;

struct Configuration{
	SELECTION_TYPE_T selection_for_selec;
	COST_NORMALISATION_T cost_norm_for_selec;
	SAMPLING_METHOD_T sampling_method_for_selec;
	bool retake_for_selec;

	SELECTION_TYPE_T selection_for_parents;
	COST_NORMALISATION_T cost_norm_for_parents;
	SAMPLING_METHOD_T sampling_method_for_parents;
	bool retake_for_parents;

	REPLACEMENT_POLICY_T replacement_policy;
	
	int type_cross_over;
	double param_cross_over;
	
	DISTRIBUTION_T mutation_distribution;
	POLICY_MUTATE_T mutation_policy;
	
	STRATEGY_T type_mutation_strategy;
	double strategy_parameter;
	
	Configuration(){
		selection_for_selec =		BEST_SELECTION;
		cost_norm_for_selec = 		NO_NEED_NORM;
		sampling_method_for_selec =	NO_NEED_SAMPLING;
		retake_for_selec =		false;
			
		selection_for_parents =		PROPORTIONAL_SELECTION;
		cost_norm_for_parents =		CORRECTION_FROM_WORST;
		sampling_method_for_parents =	BASIC_SAMPLING;
		retake_for_parents =		true;

		replacement_policy = 		REPLACE_WORST_PARENT;
		
		type_cross_over =		UNDX_CROSSOVER_3P;
		param_cross_over = 		-1;
		
		mutation_distribution =		NORMAL;
		mutation_policy = 		MUTATE_ONE_POINT;
		
		type_mutation_strategy = 	EXPONENTIAL_DOWN;
		strategy_parameter = 		-1;
	}

	const char* type_of_cross_over();	// To get the name  of a cross-over
	
	const char* type_of_selection_selec();
	const char* type_of_selection_parents();
	const char* type_of_mutation();
	const char* type_of_replacement();
	const char* type_of_strategy();
	
	
	int nb_offsprings_of_cross_over();
	int nb_parents_of_cross_over();
	void setParentsPolicy(int v);
	void setCrossOverPolicy(int v, double p);
	void setMutationPolicy(int v);
	void setReplacementPolicy(int v);
	void setSelectPolicy(int v);
	void setStrategyPolicy(int v, double p);
}; 

