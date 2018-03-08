#ifndef SRES_H
#define SRES_H

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm> // for sorting
#include <stdlib.h>
#include "baseOptimizer.h"
//#include "../solvers/baseSolver.h"
#include "../common/myRandom.h"
#include "../common/myTimes.h"
#include "population.h"
#include "individual.h"
#include "SRES/ESES.h"
#include "SRES/ESSRSort.h"
#include "SRES/sharefunc.h"
using namespace std;
using namespace OrganismLib;

#define DBG 0
///
/// @brief 
///

class SRES : public BaseOptimizer {

	private:
		int num_tries;
	public: 
		SRES(BaseOptimizationProblem *E, const std::string &file);
		SRES(BaseOptimizationProblem *E, std::ifstream &IN);
		void optimize();
        ~SRES(){}
};

#endif
