#ifndef CMAES_H
#define CMAES_H

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
#include <stdlib.h>
#include "baseOptimizer.h"
//#include "../solvers/baseSolver.h"
#include "../common/myRandom.h"
#include "../common/myTimes.h"
#include "population.h"
#include "individual.h"
using namespace OrganismLib;
using namespace std;

//#include "shark/EALib/CMA.h"
//#include "shark/EALib/ObjectiveFunctions.h"

class CMAES : public BaseOptimizer {
	private:
		int num_tries;
	public: 
		CMAES(BaseOptimizationProblem *E, const std::string &file);
		CMAES(BaseOptimizationProblem *E, std::ifstream &IN);
		void optimize();
		~CMAES(){};
};

#endif
