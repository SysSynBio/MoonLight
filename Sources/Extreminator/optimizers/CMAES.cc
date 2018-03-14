#include "CMAES.h"


CMAES::CMAES(BaseOptimizationProblem *S, const std::string &file) :BaseOptimizer(S,file) {
	if (numArguments() !=  2) { std::cerr << "Bad argument number (" << numArguments() << ")\n"; exit(-1);};
	num_tries = (int) argument(0);
}

CMAES::CMAES(BaseOptimizationProblem *S, std::ifstream &F_IN) :BaseOptimizer(S,F_IN) {
	if (numArguments() !=  2) { std::cerr << "Bad argument number (" << numArguments() << ")\n"; exit(-1);};
	num_tries = (int) argument(0);
}






// ------- Formating the cost function as a ObjectiveFunctionVS needed for using the shark library ------------

/*
CMAES* CMAESCurrentClass = NULL;
individual* CMAESCurrentIndividual = NULL;

// Static function of cost
double interncost(double *x){
	//cerr << "Called :\n";
	for(int i = 0; i < min((int) CMAESCurrentClass->numIndex(), 42); ++i){
		//cerr << x[i];
		CMAESCurrentIndividual->setGene(i, x[i]); 
		//cerr << " ";
	}
	
	CMAESCurrentClass->updateCost(CMAESCurrentIndividual);
	//cerr << "Updated\n";
	return CMAESCurrentIndividual->cost();
	//cerr << "End\n";
}

class myCost : public ObjectiveFunctionVS<double> {
public:
	myCost(unsigned int d);
	~myCost();
	unsigned int objectives() const;
	void result(double* const& point, std::vector<double>& value);
};
myCost::myCost(unsigned d) : ObjectiveFunctionVS<double>(d, NULL){m_name = "EstimatorCost";}
myCost::~myCost() {}
unsigned int myCost::objectives() const{ return 1; }
void myCost::result(double* const& point, std::vector<double>& value)
{
	double sum = interncost(point);
	value.resize(1);
	value[0] = sum;
	m_timesCalled++;
}
*/

// --------- Optimization 
void CMAES::optimize(){
  /*	CMAESCurrentClass = this;
	CMAESCurrentIndividual = new individual();
	CMAESCurrentIndividual->resize(numIndex());
	//cerr << setiosflags(ios::fixed);	
	resetCostCalls();
	
	Rng::seed(time(NULL));
	
	const unsigned Iterations = (const unsigned) argument(0);
	const double GlobalStepInit = 1.;
	
	int dimension = numIndex();
	myCost f(dimension);
	
	for(int i = 0; i < argument(1); ++i){
		Array<double> start(dimension);
		vector<double> tmp(dimension, 0);
		randomizeSet(&tmp);
		for(int i = 0; i < dimension; ++i){
			start[i] = tmp[i];
		}
		CMASearch cma;
		cma.init(f, start, GlobalStepInit);

		unsigned int i;
		for (i=0; i<Iterations; i++)
		{
			cma.run();
			cout << f.timesCalled() << "\t"  << cma.bestSolutionFitness() << "\tPscaled=\t";
			for(int i = 0; i < dimension; ++i){
				cout << "\t" << ((cma.bestSolution())[i]);
			}
			cout << "\n";
		}
	}
  */
}
