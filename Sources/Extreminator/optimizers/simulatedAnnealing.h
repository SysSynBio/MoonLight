//
// Filename     : simulatedAnnealing.h
// Description  : Classes defining optimization algorithms
// Author(s)    : Pontus Melke (pontus@thep.lu.se)
// Created      : August 2006
// Revision     : $Id: simulatedAnnealing.h 474 2010-08-30 15:23:20Z henrik $
//

#ifndef SIMULATEDANNEALING_H
#define SIMULATEDANNEALING_H

#include "baseOptimizer.h"
using namespace OrganismLib;





///
///@brief Class defining the simulated annealing algorithm. 
///
class SimulatedAnnealing : public BaseOptimizer {
 public:
	///Main constructor. It calls the constructor in class BaseOptimizer and
	///checks all optimization parameters
	SimulatedAnnealing(BaseOptimizationProblem *E, const std::string &file);
	SimulatedAnnealing(BaseOptimizationProblem *E, std::ifstream &IN);
	int metropolis(double &E, const double invT, double stepSize, const size_t numMetropolisStep, int &numUphill);
	void optimize();
};

///
///@brief Class defining the simulated annealing algorithm. 
///
class SimulatedTempering : public BaseOptimizer {
 private: 
	std::vector<double> Gr_; 
 public:
	///Main constructor. It calls the constructor in class BaseOptimizer and
	///checks all optimization parameters
	SimulatedTempering(BaseOptimizationProblem *E, const std::string &file);
	SimulatedTempering(BaseOptimizationProblem *E, std::ifstream &IN);
	void optimize();
	void initializeTemp(std::vector<double> &T_r, 
											const double TMax, const double TMin);
	void GrInit(const std::vector<double> &T_r, const unsigned int SERIE, const double stepSize);
	void GrAdjust(const std::vector<double> &T_r, const int SERIE, const int serie, const double stepSize);
	int metropolisT(const double E,size_t &r,const std::vector<double> &beta_r);
	int metropolis(double &E, const double invT, double stepSize, const size_t numMetropolisStep, int &numUphill);
};

#endif
