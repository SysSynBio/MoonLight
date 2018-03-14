#include "Config.h"
#include <iostream>
using namespace std;


const char* Configuration::type_of_cross_over(){
	switch(type_cross_over){
		case NO_NEED_CROSSOVER:{
			return "No Cross-Over specified ('NO_NEED_CROSSOVER')";
			break;}
		case ONE_POINT_CROSSOVER:{
			return "One Point Cross-Over";
			break;}
		case TWO_POINT_CROSSOVER:{
			return "Two Point Cross-Over";
			break;}
		case WRIGHT2_CROSSOVER:{
			return "Cross-over of Wright (U(0,1) * (x2 - x1) + x1)";
			break;}
		case ARITHMETIC_CROSSOVER:{
			return "Arithmetic cross-over";
			break;}
		case BLXALPHA_CROSSOVER:{
			return "BLX-alpha cross-over";
			break;}
		case GEOMETRIC_CROSSOVER:{
			return "Geometric cross-over";
			break;}
		case SBX_CROSSOVER:{
			return "SBX cross-over";
			break;}		
		case RANDOMIZE_ONE_POINT_CROSSOVER:{
			return "One-random-nutation cross-over";
			break;}
		case WRIGHT1_CROSSOVER:{
			return "Cross-Over of Wright 1 : 3 sons, p1+p2 ; 1.5p1 - 0.5p2 ; -0,5p1 + 1.5p2";
			break;}	
		case ARITHMETIC_MULTIPARENTS_CROSSOVER:{
			return "Arithmetic Multiparents Cross-Over";
			break;}		
		case GEOMETRIC_MULTIPARENTS_CROSSOVER:{
			return "Geometric Multiparents Cross-Over";
			break;}
		case SPX_CROSSOVER:{
			return "SPX Cross-Over";
			break;}
		case PCX_CROSSOVER:{
			return "PCX Cross-Over";
			break;}
		case UNDX_CROSSOVER_3P:{
			return "UNDX Cross-Over 3 parents (ie close to the line)";
			break;}
		case UNDX_CROSSOVER_4P:{
			return "UNDX Cross-Over 4 parents (ie close to the triangle)";
			break;}
		default:{
			return "UNKNOWN cross-over !!!";
			break;};
	};	
}

// TODO Find a way to give more parameters to the öultiple-parents-cross-overs

int Configuration::nb_offsprings_of_cross_over(){
	// Can depend on parameter
	switch(type_cross_over){
		case NO_NEED_CROSSOVER:{
			return 0;
			break;}
		case ONE_POINT_CROSSOVER:{
			return 1;
			break;}
		case TWO_POINT_CROSSOVER:{
			return 1;
			break;}
		case WRIGHT2_CROSSOVER:{
			return 1;
			break;}
		case ARITHMETIC_CROSSOVER:{
			return 1;
			break;}
		case BLXALPHA_CROSSOVER:{
			return 1;
			break;}
		case GEOMETRIC_CROSSOVER:{
			return 1;
			break;}
		case SBX_CROSSOVER:{
			return 2;
			break;}		
		case RANDOMIZE_ONE_POINT_CROSSOVER:{
			return 1;
			break;}
		case WRIGHT1_CROSSOVER:{
			return 3;
			break;}	
		case ARITHMETIC_MULTIPARENTS_CROSSOVER:{
			return 1;
			break;}		
		case GEOMETRIC_MULTIPARENTS_CROSSOVER:{
			return 1;
			break;}
		case UNDX_CROSSOVER_3P:{
			return 1;
			break;}
		case UNDX_CROSSOVER_4P:{
			return 1;
			break;}
		case SPX_CROSSOVER:{
			return (int) param_cross_over;
			break;}
		case PCX_CROSSOVER:{
			return (int) param_cross_over;
			break;}
		default:{
			return 0;
			break;};
	};	
}


int Configuration::nb_parents_of_cross_over(){
	// can depend of param_cross-over
	switch(type_cross_over){
		case NO_NEED_CROSSOVER:{
			return 0;
			break;}
		case ONE_POINT_CROSSOVER:{
			return 2;
			break;}
		case TWO_POINT_CROSSOVER:{
			return 2;
			break;}
		case WRIGHT2_CROSSOVER:{
			return 2;
			break;}
		case ARITHMETIC_CROSSOVER:{
			return 2;
			break;}
		case BLXALPHA_CROSSOVER:{
			return 2;
			break;}
		case GEOMETRIC_CROSSOVER:{
			return 2;
			break;}
		case SBX_CROSSOVER:{
			return 2;
			break;}		
		case RANDOMIZE_ONE_POINT_CROSSOVER:{
			return 1;
			break;}
		case WRIGHT1_CROSSOVER:{
			return 2;
			break;}	
		case ARITHMETIC_MULTIPARENTS_CROSSOVER:{
			return (int) param_cross_over;
			break;}		
		case GEOMETRIC_MULTIPARENTS_CROSSOVER:{
			return (int) param_cross_over;
			break;}
		case UNDX_CROSSOVER_3P:{
			return 3;
			break;}
		case UNDX_CROSSOVER_4P:{
			return 4;
			break;}
		case SPX_CROSSOVER:{
			return (int) param_cross_over;
			break;}
		case PCX_CROSSOVER:{
			return (int) param_cross_over;
			break;}
		default:{
			return 0;
			break;};
	};	
}
	


const char* Configuration::type_of_selection_parents(){
	switch(selection_for_parents){
		case NO_NEED_SELECTION:{
			if(retake_for_parents) return "NOT GIVEN";
			else return "NOT GIVEN + Not same parent in diff. Cr-Ov.";
		break;}
		case BEST_SELECTION:{
			if(retake_for_parents) return "BEST INDIVIDUALS - NOT ALLOWED FOR PARENTS SELECTION !!";
			else return "BEST INDIVIDUALS - NOT ALLOWED FOR PARENTS SELECTION !! + Not same parent in diff. Cr-Ov.";
		break;}
		case TOURNAMENT_SELECTION_5PC:{
			if(retake_for_parents) return "Selection by TOURNAMENT of groups of 5% of the pop.";
			else return "Selection by TOURNAMENT of groups of 5% of the pop. + Not same parent in diff. Cr-Ov.";
		break;}
		case TOURNAMENT_SELECTION_10PC:{
			if(retake_for_parents) return "Selection by TOURNAMENT of groups of 10% of the pop.";
			else return "Selection by TOURNAMENT of groups of 10% of the pop. + Not same parent in diff. Cr-Ov.";
		break;}
		case TOURNAMENT_SELECTION_25PC:{
			if(retake_for_parents) return "Selection by TOURNAMENT of groups of 25% of the pop.";
			else return "Selection by TOURNAMENT of groups of 25% of the pop. + Not same parent in diff. Cr-Ov.";
		break;}
		case TOURNAMENT_SELECTION_50PC:{
			if(retake_for_parents) return "Selection by TOURNAMENT of groups of 50% of the pop.";
			else return "Selection by TOURNAMENT of groups of 50% of the pop. + Not same parent in diff. Cr-Ov.";
		break;}
		case RANK_BASED_SELECTION:{
			if(retake_for_parents) return "Selection based on the RANK : pos = U(0,1)*U(0,1)*popSize";
			else return "Selection based on the RANK : pos = U(0,1)*U(0,1)*popSize + Not same parent in diff. Cr-Ov.";
		break;}
		case RANDOM_SELECTION:{
			if(retake_for_parents) return "Selection RANDOM (Bad !!)";
			return "Selection RANDOM (Bad !!) + Not same parent in diff. Cr-Ov.";
		break;}
		case PROPORTIONAL_SELECTION:{
			switch(cost_norm_for_parents){
				case NO_NEED_NORM:{
					switch(sampling_method_for_parents){
						case NO_NEED_SAMPLING:{
							return "Prop - NO NORM GIVEN - NO SAMPLING GIVEN !!";
						break;}
						case BASIC_SAMPLING:{
							return "Prop - NO NORM GIVEN - Basic Sampling";
						break;}
						case SAMPLING_ROULETTE_WHEEL:{
							return "Prop - NO NORM GIVEN - Roulette Wheel Sampling";
						break;}
						case SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING:{
							return "Prop - NO NORM GIVEN - Stochastic Universal Sampling";
						break;}	
					}
				break;}
				case NO_CORRECTION:{
					switch(sampling_method_for_parents){
						case NO_NEED_SAMPLING:{
							return "Prop - No Correction (1/f) - NO SAMPLING GIVEN !!";
						break;}
						case BASIC_SAMPLING:{
							return "Prop - No Correction (1/f) - Basic Sampling";
						break;}
						case SAMPLING_ROULETTE_WHEEL:{
							return "Prop - No Correction (1/f) - Roulette Wheel Sampling";
						break;}
						case SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING:{
							return "Prop - No Correction (1/f) - Stochastic Universal Sampling";
						break;}	
					}
				break;}
				case CORRECTION_FROM_WORST:{
					switch(sampling_method_for_parents){
						case NO_NEED_SAMPLING:{
							return "Prop - Correction from WORST - NO SAMPLING GIVEN !!";
						break;}
						case BASIC_SAMPLING:{
							return "Prop - Correction from WORST - Basic Sampling";
						break;}
						case SAMPLING_ROULETTE_WHEEL:{
							return "Prop - Correction from WORST - Roulette Wheel Sampling";
						break;}
						case SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING:{
							return "Prop - Correction from WORST - Stochastic Universal Sampling";
						break;}	
					}
				break;}
				case CORRECTION_FROM_BEST:{
					switch(sampling_method_for_parents){
						case NO_NEED_SAMPLING:{
							return "Prop - Correction from BEST - NO SAMPLING GIVEN !!";
						break;}
						case BASIC_SAMPLING:{
							return "Prop - Correction from BEST - Basic Sampling";
						break;}
						case SAMPLING_ROULETTE_WHEEL:{
							return "Prop - Correction from BEST - Roulette Wheel Sampling";
						break;}
						case SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING:{
							return "Prop - Correction from BEST - Stochastic Universal Sampling";
						break;}	
					}
				break;}
				case CORRECTION_BOLTZMANN:{
					switch(sampling_method_for_parents){
						case NO_NEED_SAMPLING:{
							return "Prop - Correction Boltzmann - NO SAMPLING GIVEN !!";
						break;}
						case BASIC_SAMPLING:{
							return "Prop - Correction Boltzmann - Basic Sampling";
						break;}
						case SAMPLING_ROULETTE_WHEEL:{
							return "Prop - Correction Boltzmann - Roulette Wheel Sampling";
						break;}
						case SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING:{
							return "Prop - Correction Boltzmann - Stochastic Universal Sampling";
						break;}
					}
				break;}
			}
			
		break;}
	}
	return "Failed to detect the selection";
}

const char* Configuration::type_of_selection_selec(){
	switch(selection_for_selec){
		case NO_NEED_SELECTION:{
			if(retake_for_selec) return "NOT GIVEN";
			else return "NOT GIVEN + Not same parent in diff. Cr-Ov.";
		break;}
		case BEST_SELECTION:{
			if(retake_for_selec) return "Selection of BEST individuals";
			else return "Selection of BEST individuals + Not same parent in diff. Cr-Ov.";
		break;}
		case TOURNAMENT_SELECTION_5PC:{
			if(retake_for_selec) return "Selection by TOURNAMENT of groups of 5% of the pop.";
			else return "Selection by TOURNAMENT of groups of 5% of the pop. + Not same parent in diff. Cr-Ov.";
		break;}
		case TOURNAMENT_SELECTION_10PC:{
			if(retake_for_selec) return "Selection by TOURNAMENT of groups of 10% of the pop.";
			else return "Selection by TOURNAMENT of groups of 10% of the pop. + Not same parent in diff. Cr-Ov.";
		break;}
		case TOURNAMENT_SELECTION_25PC:{
			if(retake_for_selec) return "Selection by TOURNAMENT of groups of 25% of the pop.";
			else return "Selection by TOURNAMENT of groups of 25% of the pop. + Not same parent in diff. Cr-Ov.";
		break;}
		case TOURNAMENT_SELECTION_50PC:{
			if(retake_for_selec) return "Selection by TOURNAMENT of groups of 50% of the pop.";
			else return "Selection by TOURNAMENT of groups of 50% of the pop. + Not same parent in diff. Cr-Ov.";
		break;}
		case RANK_BASED_SELECTION:{
			if(retake_for_selec) return "Selection based on the RANK : pos = U(0,1)*U(0,1)*popSize";
			else return "Selection based on the RANK : pos = U(0,1)*U(0,1)*popSize + Not same parent in diff. Cr-Ov.";
		break;}
		case RANDOM_SELECTION:{
			if(retake_for_selec) return "Selection RANDOM (Bad !!)";
			return "Selection RANDOM (Bad !!) + Not same parent in diff. Cr-Ov.";
		break;}
		case PROPORTIONAL_SELECTION:{
			switch(cost_norm_for_selec){
				case NO_NEED_NORM:{
					switch(sampling_method_for_selec){
						case NO_NEED_SAMPLING:{
							return "Prop - NO NORM GIVEN - NO SAMPLING GIVEN !!";
						break;}
						case BASIC_SAMPLING:{
							return "Prop - NO NORM GIVEN - Basic Sampling";
						break;}
						case SAMPLING_ROULETTE_WHEEL:{
							return "Prop - NO NORM GIVEN - Roulette Wheel Sampling";
						break;}
						case SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING:{
							return "Prop - NO NORM GIVEN - Stochastic Universal Sampling";
						break;}	
					}
				break;}
				case NO_CORRECTION:{
					switch(sampling_method_for_selec){
						case NO_NEED_SAMPLING:{
							return "Prop - No Correction (1/f) - NO SAMPLING GIVEN !!";
						break;}
						case BASIC_SAMPLING:{
							return "Prop - No Correction (1/f) - Basic Sampling";
						break;}
						case SAMPLING_ROULETTE_WHEEL:{
							return "Prop - No Correction (1/f) - Roulette Wheel Sampling";
						break;}
						case SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING:{
							return "Prop - No Correction (1/f) - Stochastic Universal Sampling";
						break;}	
					}
				break;}
				case CORRECTION_FROM_WORST:{
					switch(sampling_method_for_selec){
						case NO_NEED_SAMPLING:{
							return "Prop - Correction from WORST - NO SAMPLING GIVEN !!";
						break;}
						case BASIC_SAMPLING:{
							return "Prop - Correction from WORST - Basic Sampling";
						break;}
						case SAMPLING_ROULETTE_WHEEL:{
							return "Prop - Correction from WORST - Roulette Wheel Sampling";
						break;}
						case SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING:{
							return "Prop - Correction from WORST - Stochastic Universal Sampling";
						break;}	
					}
				break;}
				case CORRECTION_FROM_BEST:{
					switch(sampling_method_for_selec){
						case NO_NEED_SAMPLING:{
							return "Prop - Correction from BEST - NO SAMPLING GIVEN !!";
						break;}
						case BASIC_SAMPLING:{
							return "Prop - Correction from BEST - Basic Sampling";
						break;}
						case SAMPLING_ROULETTE_WHEEL:{
							return "Prop - Correction from BEST - Roulette Wheel Sampling";
						break;}
						case SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING:{
							return "Prop - Correction from BEST - Stochastic Universal Sampling";
						break;}	
					}
				break;}
				case CORRECTION_BOLTZMANN:{
					switch(sampling_method_for_selec){
						case NO_NEED_SAMPLING:{
							return "Prop - Correction Boltzmann - NO SAMPLING GIVEN !!";
						break;}
						case BASIC_SAMPLING:{
							return "Prop - Correction Boltzmann - Basic Sampling";
						break;}
						case SAMPLING_ROULETTE_WHEEL:{
							return "Prop - Correction Boltzmann - Roulette Wheel Sampling";
						break;}
						case SAMPLING_STOCHASTIC_UNIVERSAL_SAMPLING:{
							return "Prop - Correction Boltzmann - Stochastic Universal Sampling";
						break;}	
					}
				break;}
			}
			
		break;}	
	}
	return "Failed to detect the selection";
}

const char* Configuration::type_of_mutation(){
	switch(mutation_distribution){
		case NORMAL:{
			if(mutation_policy == MUTATE_ONE_POINT) return "Normale - Une seule position";
			else if (mutation_policy == MUTATE_ALL_POINTS)  return "Normale - Toutes les positions";
		break;}
		case CAUCHY:{
			if(mutation_policy == MUTATE_ONE_POINT) return "Cauchy - Une seule position";
			else if (mutation_policy == MUTATE_ALL_POINTS)  return "Cauchy - Toutes les positions";
		break;}
		case PUT_RANDOM:{
			if(mutation_policy == MUTATE_ONE_POINT) return "Random Value (uniform in range)- Une seule position";
			else if (mutation_policy == MUTATE_ALL_POINTS)  return "Random Value (uniform in range) - Toutes les positions";
		break;}
		case EXPONENTIAL:{
			if(mutation_policy == MUTATE_ONE_POINT) return "Exponential - Une seule position";
			else if (mutation_policy == MUTATE_ALL_POINTS)  return "Exponential - Toutes les positions";
		break;}
		case COMBINED:{
			if(mutation_policy == MUTATE_ONE_POINT) return "Combined N + C - Une seule position";
			else if (mutation_policy == MUTATE_ALL_POINTS)  return "Combined N + C - Toutes les positions";
		break;}
		case UNIFORM_STEP:{
			if(mutation_policy == MUTATE_ONE_POINT) return "Uniform Coeff Box - Une seule position";
			else if (mutation_policy == MUTATE_ALL_POINTS)  return "Uniform Coeff Box - Toutes les positions";
		break;}
		case LEVY:{
			if(mutation_policy == MUTATE_ONE_POINT) return "LEVY - NOT HANDLED YET ! - Une seule position";
			else if (mutation_policy == MUTATE_ALL_POINTS)  return "LEVY - NOT HANDLED YET ! - Toutes les positions";
		break;}
		case GAMMA:{
			if(mutation_policy == MUTATE_ONE_POINT) return "GAMMA - NOT HANDLED YET ! - Une seule position";
			else if (mutation_policy == MUTATE_ALL_POINTS)  return "GAMMA - NOT HANDLED YET ! - Toutes les positions";
		break;}
	}
	
	return "Failed to detect the mutation distribution";
}

void Configuration::setParentsPolicy(int v){
	//cerr << "Change parents Sel to " << v << "\n";
	switch(v){
		case 0:{
		break;}
		case 1:{
			selection_for_parents =		RANK_BASED_SELECTION;
			cost_norm_for_parents = 	NO_NEED_NORM;
			sampling_method_for_parents =	NO_NEED_SAMPLING;
			retake_for_parents =		true;
		break;}
		case 2:{
			selection_for_parents =		RANDOM_SELECTION;
			cost_norm_for_parents = 	NO_NEED_NORM;
			sampling_method_for_parents =	NO_NEED_SAMPLING;
			retake_for_parents =		true;			
		break;}
		case 3:{
			selection_for_parents =		TOURNAMENT_SELECTION_5PC;
			cost_norm_for_parents = 	NO_NEED_NORM;
			sampling_method_for_parents =	NO_NEED_SAMPLING;
			retake_for_parents =		true;		
		break;}
		case 4:{
			selection_for_parents =		TOURNAMENT_SELECTION_10PC;
			cost_norm_for_parents = 	NO_NEED_NORM;
			sampling_method_for_parents =	NO_NEED_SAMPLING;
			retake_for_parents =		true;		
		break;}
		case 5:{
			selection_for_parents =		TOURNAMENT_SELECTION_25PC;
			cost_norm_for_parents = 	NO_NEED_NORM;
			sampling_method_for_parents =	NO_NEED_SAMPLING;
			retake_for_parents =		true;		
		break;}
		case 6:{
			selection_for_parents =		TOURNAMENT_SELECTION_50PC;
			cost_norm_for_parents = 	NO_NEED_NORM;
			sampling_method_for_parents =	NO_NEED_SAMPLING;
			retake_for_parents =		true;		
		break;}
		case 7:{
			selection_for_parents =		PROPORTIONAL_SELECTION;
			cost_norm_for_parents = 	CORRECTION_FROM_WORST;
			sampling_method_for_parents =	BASIC_SAMPLING;
			retake_for_parents =		true;		
		break;}
		case 8:{
			selection_for_parents =		PROPORTIONAL_SELECTION;
			cost_norm_for_parents = 	CORRECTION_FROM_WORST;
			sampling_method_for_parents =	SAMPLING_ROULETTE_WHEEL;
			retake_for_parents =		true;		
		break;}		
		case 9:{
			selection_for_parents =		PROPORTIONAL_SELECTION;
			cost_norm_for_parents = 	CORRECTION_FROM_BEST;
			sampling_method_for_parents =	BASIC_SAMPLING;
			retake_for_parents =		true;		
		break;}
		case 10:{
			selection_for_parents =		PROPORTIONAL_SELECTION;
			cost_norm_for_parents = 	CORRECTION_FROM_BEST;
			sampling_method_for_parents =	SAMPLING_ROULETTE_WHEEL;
			retake_for_parents =		true;		
		break;}		
		case 11:{
			selection_for_parents =		PROPORTIONAL_SELECTION;
			cost_norm_for_parents = 	NO_CORRECTION;
			sampling_method_for_parents =	BASIC_SAMPLING;
			retake_for_parents =		true;		
		break;}
		case 12:{
			selection_for_parents =		PROPORTIONAL_SELECTION;
			cost_norm_for_parents = 	NO_CORRECTION;
			sampling_method_for_parents =	SAMPLING_ROULETTE_WHEEL;
			retake_for_parents =		true;		
		break;}	
	}
}
void Configuration::setCrossOverPolicy(int v, double p){
	//cerr << "Change Cross-Over " << v << "\n";
	switch(v){
		case 0:{
			type_cross_over =	ONE_POINT_CROSSOVER;
			param_cross_over = 	-1;
		break;}
		case 1:{
			type_cross_over =	TWO_POINT_CROSSOVER;
			param_cross_over = 	-1;
		break;}	
		case 2:{
			type_cross_over =	WRIGHT1_CROSSOVER;
			param_cross_over = 	-1;
		break;}	
		case 3:{
			type_cross_over =	WRIGHT2_CROSSOVER;
			param_cross_over = 	-1;
		break;}	
		case 4:{
			type_cross_over =	ARITHMETIC_CROSSOVER;
            param_cross_over = 	p;
		break;}	
		case 5:{
			type_cross_over =	BLXALPHA_CROSSOVER;
            param_cross_over = 	p;
		break;}	
		case 6:{
			type_cross_over =	GEOMETRIC_CROSSOVER;
            param_cross_over = 	p;
		break;}	
		case 7:{
			type_cross_over =	SBX_CROSSOVER;
            param_cross_over = 	p;
		break;}	
		case 8:{
			type_cross_over =	RANDOMIZE_ONE_POINT_CROSSOVER;
			param_cross_over = 	-1;
		break;}	
		case 9:{
			type_cross_over =	UNDX_CROSSOVER_3P;
			param_cross_over = 	-1;
		break;}	
		case 10:{
			type_cross_over =	UNDX_CROSSOVER_4P;
			param_cross_over = 	-1;
		break;}
	}
}

void Configuration::setMutationPolicy(int v){
	//cerr << "Change Mutation Distrib to " << v << "\n";
	switch(v){
	case 0:{
		mutation_distribution =		NORMAL;
		mutation_policy = 		MUTATE_ONE_POINT;
	break;}
	case 1:{
		mutation_distribution =		NORMAL;
		mutation_policy = 		MUTATE_ALL_POINTS;
	break;}
	case 2:{
		mutation_distribution =		CAUCHY;
		mutation_policy = 		MUTATE_ONE_POINT;
	break;}
	case 3:{
		mutation_distribution =		CAUCHY;
		mutation_policy = 		MUTATE_ALL_POINTS;
	break;}
	case 4:{
		mutation_distribution =		PUT_RANDOM;
		mutation_policy = 		MUTATE_ONE_POINT;
	break;}
	case 5:{
		mutation_distribution =		PUT_RANDOM;
		mutation_policy = 		MUTATE_ALL_POINTS;
	break;}
	case 6:{
		mutation_distribution =		EXPONENTIAL;
		mutation_policy = 		MUTATE_ONE_POINT;
	break;}
	case 7:{
		mutation_distribution =		EXPONENTIAL;
		mutation_policy = 		MUTATE_ALL_POINTS;
	break;}
	case 8:{
		mutation_distribution =		COMBINED;
		mutation_policy = 		MUTATE_ONE_POINT;
	break;}
	case 9:{
		mutation_distribution =		COMBINED;
		mutation_policy = 		MUTATE_ALL_POINTS;
	break;}
	case 10:{
		mutation_distribution =		UNIFORM_STEP;
		mutation_policy = 		MUTATE_ONE_POINT;
	break;}
	case 11:{
		mutation_distribution =		UNIFORM_STEP;
		mutation_policy = 		MUTATE_ALL_POINTS;
	break;}
	
	
	
	}
}

void Configuration::setReplacementPolicy(int v){
	switch(v){
		case -1:{
			replacement_policy = NO_NEED_REPLACEMENT;
		break;}		
		case 0:{
			replacement_policy = REPLACE_WORST_PARENT;
		break;}
		case 1:{
			replacement_policy = REPLACE_WORST_PARENT_METROPOLIS;
		break;}
		case 2:{
			replacement_policy = REPLACE_WORST;
		break;}
		case 3:{
			replacement_policy = REPLACE_RANDOM;
		break;}
		case 4:{
			replacement_policy = REPLACE_RANDOM_METROPOLIS;
		break;}
		case 5:{
			replacement_policy = KILL_TOURNAMENT;
		break;}
		case 6:{
			replacement_policy = REPLACE_OLDEST;
		break;}
		case 7:{
			replacement_policy = CONSERVATIVE_SELECTION;
		break;}
		case 8:{
			replacement_policy = ELITIST;
		break;}
			
	}
}

const char* Configuration::type_of_replacement(){
	switch(replacement_policy){
		case NO_NEED_REPLACEMENT:{
			return "NO REPLACEMENT GIVEN";
		break;}
		case REPLACE_WORST_PARENT:{
			return "Each Offspring Replaces the worst of the Parents";
		break;}
		case REPLACE_WORST_PARENT_METROPOLIS:{
			return "Each Offspring Replaces worst parent with metropolis criterion.";
		break;}
		case REPLACE_WORST:{
			return "Each Offspring Replaces the Worst individual of the population";
		break;}
		case REPLACE_RANDOM:{
			return "Each Offspring Replaces a random element in the population";
		break;}
		case REPLACE_RANDOM_METROPOLIS:{
			return "Each Offspring Replaces a random element with metropolis criterion";
		break;}
		case KILL_TOURNAMENT:{
			return "KILL Tournament";
		break;}
		case REPLACE_OLDEST:{
			return "Replace Oldest";
		break;}
		case CONSERVATIVE_SELECTION:{
			return "Conservative selection";
		break;}		
		case ELITIST:{
			return "Elitist";
		break;}		
	}
	return "Non Identified replacement policy\n";
}

void Configuration::setSelectPolicy(int v){
	//cerr << "Change Selection Method to " << v << "\n";
	switch(v){
		case 0:{
			selection_for_selec =		BEST_SELECTION;
			cost_norm_for_selec = 	NO_NEED_NORM;
			sampling_method_for_selec =	NO_NEED_SAMPLING;
			retake_for_selec =		true;
		break;}
		case 1:{
			selection_for_selec =		RANK_BASED_SELECTION;
			cost_norm_for_selec = 	NO_NEED_NORM;
			sampling_method_for_selec =	NO_NEED_SAMPLING;
			retake_for_selec =		true;
		break;}
		case 2:{
			selection_for_selec =		RANDOM_SELECTION;
			cost_norm_for_selec = 	NO_NEED_NORM;
			sampling_method_for_selec =	NO_NEED_SAMPLING;
			retake_for_selec =		true;			
		break;}
		case 3:{
			selection_for_selec =		RANK_BASED_SELECTION;
			cost_norm_for_selec = 	NO_NEED_NORM;
			sampling_method_for_selec =	NO_NEED_SAMPLING;
			retake_for_selec =		true;		
		break;}
		case 4:{
			selection_for_selec =		TOURNAMENT_SELECTION_5PC;
			cost_norm_for_selec = 	NO_NEED_NORM;
			sampling_method_for_selec =	NO_NEED_SAMPLING;
			retake_for_selec =		true;		
		break;}
		case 5:{
			selection_for_selec =		TOURNAMENT_SELECTION_10PC;
			cost_norm_for_selec = 	NO_NEED_NORM;
			sampling_method_for_selec =	NO_NEED_SAMPLING;
			retake_for_selec =		true;		
		break;}
		case 6:{
			selection_for_selec =		TOURNAMENT_SELECTION_25PC;
			cost_norm_for_selec = 	NO_NEED_NORM;
			sampling_method_for_selec =	NO_NEED_SAMPLING;
			retake_for_selec =		true;		
		break;}
		case 7:{
			selection_for_selec =		TOURNAMENT_SELECTION_50PC;
			cost_norm_for_selec = 	NO_NEED_NORM;
			sampling_method_for_selec =	NO_NEED_SAMPLING;
			retake_for_selec =		true;		
		break;}
		case 8:{
			selection_for_selec =		PROPORTIONAL_SELECTION;
			cost_norm_for_selec = 	CORRECTION_FROM_WORST;
			sampling_method_for_selec =	BASIC_SAMPLING;
			retake_for_selec =		true;		
		break;}
		case 9:{
			selection_for_selec =		PROPORTIONAL_SELECTION;
			cost_norm_for_selec = 	CORRECTION_FROM_WORST;
			sampling_method_for_selec =	SAMPLING_ROULETTE_WHEEL;
			retake_for_selec =		true;		
		break;}		
		case 10:{
			selection_for_selec =		PROPORTIONAL_SELECTION;
			cost_norm_for_selec = 	CORRECTION_FROM_BEST;
			sampling_method_for_selec =	BASIC_SAMPLING;
			retake_for_selec =		true;		
		break;}
		case 11:{
			selection_for_selec =		PROPORTIONAL_SELECTION;
			cost_norm_for_selec = 	CORRECTION_FROM_BEST;
			sampling_method_for_selec =	SAMPLING_ROULETTE_WHEEL;
			retake_for_selec =		true;		
		break;}		
		case 12:{
			selection_for_selec =		PROPORTIONAL_SELECTION;
			cost_norm_for_selec = 	NO_CORRECTION;
			sampling_method_for_selec =	BASIC_SAMPLING;
			retake_for_selec =		true;		
		break;}
		case 13:{
			selection_for_selec =		PROPORTIONAL_SELECTION;
			cost_norm_for_selec = 	NO_CORRECTION;
			sampling_method_for_selec =	SAMPLING_ROULETTE_WHEEL;
			retake_for_selec =		true;		
		break;}	
	}
}
void Configuration::setStrategyPolicy(int v, double d){
	strategy_parameter = d;
	switch(v){
		case 0:{
			type_mutation_strategy = CONSTANT;
			if (d < 0) strategy_parameter = 1;
			break;}
		case 1:{
			type_mutation_strategy = EXPONENTIAL_DOWN;
			if (d < 0) strategy_parameter = 1;
			break;}
		case 2:{
			type_mutation_strategy = DISTANCE_BEST;
			if (d < 0) strategy_parameter = 1;
			break;}
		case 3:{
			type_mutation_strategy = DISTANCE_BEST_SEPARATED;
			if (d < 0) strategy_parameter = 1;
			break;}
		case 4:{
			type_mutation_strategy = PROPORTIONAL_NORMALIZED;
			if (d < 0) strategy_parameter = 1;
			break;}
		case 5:{
			type_mutation_strategy = LOGNORMAL;
			if (d < 0) strategy_parameter = 1;
			break;}
		case 6:{
			type_mutation_strategy = MUTATIVE;
			if (d < 0) strategy_parameter = 5./100.;
			break;}
		case 7:{
			type_mutation_strategy = MUTATIVE_SEPARATED;
			if (d <= 0) strategy_parameter = 1./1000.;
			break;}
		default:{
			cerr << "Unknown strategy policy :"<< v << "\n"; 
		}
	}
}

const char* Configuration::type_of_strategy(){
	switch(type_mutation_strategy){
		case CONSTANT:{
			return "Static sigma values = param";
			break;}
		case EXPONENTIAL_DOWN:{
			return "Exponential sigma values : sig = sig0.exp(-t/tho). with sig0 = param  tho = num Gen Max / 10";
			break;}
		case DISTANCE_BEST:{
			return "distance to best individual, all directions same sigma. - no param";
			break;}
		case DISTANCE_BEST_SEPARATED:{
			return "distance to best individual, a sigma for all direction. - no param";
			break;}
		case PROPORTIONAL_NORMALIZED:{
			return "proportional to normalized fitness - correction from worst, coeff = param";
			break;}
		case LOGNORMAL:{
			return "sig(ij)(t+1) = sig(ij)(t) exp(c1 Ni(0,1) + c2 Nij(0,1)) - no param";
			break;}
		case MUTATIVE:{
            return "steps kept the same in each dimension, indiv. mutated with proba (param.), Mutation = exp(N(0,1)).";
			break;}
		case MUTATIVE_SEPARATED:{
            return "step for each dimension mutated with proba (param.), Mutation = exp(N(0,1)).";
			break;}
		default:{
			return "Unknown Strategy policy";
			break;}
	}
}
