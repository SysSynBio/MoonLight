//
// Filename     : simulatedAnnealing.cc
// Description  : Classes defining optimization algorithms
// Author(s)    : Pontus Melke (pontus@thep.lu.se)
// Created      : August 2006
// Revision     : $Id: simulatedAnnealing.cc 478 2011-02-21 08:34:45Z henrik $
//

#include "simulatedAnnealing.h"
#include "baseOptimizer.h"
#include "../common/myRandom.h"
using namespace OrganismLib;
//
// Simulated Annealing
//
SimulatedAnnealing::SimulatedAnnealing(BaseOptimizationProblem *E, const std::string &file) 
  : BaseOptimizer(E,file)
{
  //S_ = S;
  //readOptimizer(file);
  if(numArguments() !=5) {
    std::cerr << "\nSimulated annealing takes four arguments" 
	      << "TInit, TFinal, TUpdate, parameterStepFactor and numMetropolisStep\n";
    exit(-1);
  }
}

SimulatedAnnealing::SimulatedAnnealing(BaseOptimizationProblem *E, std::ifstream & F_IN)
  : BaseOptimizer(E,F_IN)
{
  //S_ = S;
  //readOptimizer(file); already done by the constructor-
  if(numArguments() !=5) {
    std::cerr << "\nSimulated annealing takes four arguments" 
	      << "TInit, TFinal, TUpdate, parameterStepFactor and numMetropolisStep\n";
    exit(-1);
  }
}

void SimulatedAnnealing::optimize() 
{
	double TInit = argument(0);
	double TFinal = argument(1);
	double TStep = argument(2);
	double stepSize = argument(3);
	size_t numMetropolisStep = static_cast<size_t>(argument(4)); 
	if (!numParaSet())
		randomizeParameter();
	double E = computeCost();
	setGlobal(E);
	std::cerr << "Starting SimulatedAnnealing::optimize()\n"; 
	
	//Starting Simulated Annealing
	int numSuccess = 0,numUphill=0;

	std::cerr << "#T\tnumStep\tnumSucc\tnumUp\tE\tE_opt\tcpu" << std::endl;
	for(double T=TInit; T>TFinal; T *= TStep) {
		numSuccess = metropolis(E, 1/T, stepSize, numMetropolisStep,numUphill);
		std::cerr << T << "\t" << numMetropolisStep << "\t" << numSuccess << "\t" << numUphill << "\t"
			<< E << "\t" << globalOptima() << "\t" << myTimes::getDiffTime() << std::endl; 
        if(PRINT){
            std::cerr << "Best cost value so far: " << globalOptima() << "\n";
            std::cerr << "Temporary cost value: " << E << "\n";
            std::cerr << "T = " << T << "\n";
            std::cerr << "CPU-time: " << myTimes::getDiffTime() << "\n";
            std::cerr << "Succeeded in " << numSuccess << " cases out of " << numMetropolisStep << "\n";
        }
        printState(E);
	}
   /* if(PRINT){
        std::cout << "#" << globalOptima() << " ";
        for ( size_t i =0; i<numPara(); i++)
            std::cout << optimalParameter(i) << " ";
        std::cout << "\n";
        std::cout.flush();
    }*/
	printOptima();	
}

//
// Simulated Tempering
//
SimulatedTempering::SimulatedTempering(BaseOptimizationProblem *E, const std::string &file) :BaseOptimizer(E,file)
{
	//S_ = S;
	//readOptimizer(file);
	if(numArguments() !=5) {
		std::cerr << "\nSimulated annealing takes four arguments" 
			<< "TMax, TMin, numT, parameterStepFactor and numMetropolisStep\n";
		exit(-1);
	}
}

SimulatedTempering::SimulatedTempering(BaseOptimizationProblem *E, std::ifstream & F_IN) :BaseOptimizer(E, F_IN)
{
	//S_ = S;
	//readOptimizer(file);
	if(numArguments() !=5) {
		std::cerr << "\nSimulated annealing takes four arguments" 
			<< "TMax, TMin, numT, parameterStepFactor and numMetropolisStep\n";
		exit(-1);
	}
}

void SimulatedTempering::optimize() 
{
	double TMax = argument(0);
	double TMin = argument(1);
	unsigned int numT = static_cast<unsigned int>(argument(2));
	double stepSize = argument(3);
	size_t numMetropolisStep = static_cast<size_t>(argument(4));
	std::vector<double> T_r(numT); 
	Gr_.resize(numT);
	randomizeParameter();
	double E = computeCost();
	setGlobal(E);
	int numUphill;
	metropolis(E, 1/TMin, stepSize,5*numMetropolisStep,numUphill); 
	
	initializeTemp(T_r, TMax, TMin);
	GrInit(T_r, 10*numMetropolisStep, stepSize);
	
	GrAdjust(T_r, 3*numMetropolisStep, numMetropolisStep, stepSize);
	GrAdjust(T_r, 3*numMetropolisStep, numMetropolisStep, stepSize);
	GrAdjust(T_r, 3*numMetropolisStep, numMetropolisStep, stepSize);
	GrAdjust(T_r, 3*numMetropolisStep, numMetropolisStep, stepSize);
	size_t r = static_cast<size_t>(myRandom::Rnd()*numT);
	double T = T_r[r];
	
	for(unsigned int count=0; count< 15*numMetropolisStep; ++count) {
		std::vector<double> tmpPara(numIndex()), localOptimalPara(numIndex());
		T=T_r[r];
		metropolis(E, 1/T, stepSize,numMetropolisStep,numUphill); 
		int TAccept = metropolisT(E, r, T_r);
		//Quench from lowest temperature
		if (T_r[r] == TMin && TAccept) {
			std::cerr << "Start quenching...";
			double localOptima = E;
			//save current state
			for (size_t i =0; i<numIndex(); ++i) {
				tmpPara[i] = parameter(i);
				localOptimalPara[i] = parameter(i);
			}
			for (size_t j = 0; j< 40*numMetropolisStep; ++j) { 
				metropolis(E, 1000/TMin, stepSize,1,numUphill);
				if (E< localOptima) {
					localOptima=E;
					for (size_t i =0; i< numIndex(); ++i)
						localOptimalPara[i] = parameter(i);
				}
			}
			for (size_t i =0; i<numIndex(); ++i)
				setParameter(i, tmpPara[i]);
			std::cerr << "Done!\n";
			std::cout << "#" << localOptima << "\n";
			for ( size_t i =0; i<numIndex(); i++)
				std::cout << localOptimalPara[i] << " ";
			std::cout << "\n";
			std::cout.flush();
		}
		std::cerr << count << " " << 15*numMetropolisStep << " "
							<<  E << " " << T_r[r] << " " << globalOptima() 
							<<"\n";
	}
	std::cout << "\n" << globalOptima() << " ";
	printOptima();
	std::cout.flush();
}

void SimulatedTempering::initializeTemp(std::vector<double> &T_r, const double TMax, const double TMin) 
{
	if ( TMax<TMin ) 
	{ std::cerr << "initializeTemp() TMax cannot be smaller than TMin\n";exit(-1);}
	
	double RInv = 1/(static_cast<double>(T_r.size()-1));
	double step = exp( (RInv)*log(TMax/TMin) );		     
	double T = TMin;
	for (size_t r=0; r<T_r.size() ; r++ ) {
		T_r[r] = T;
		T *= step;
	}
	for (size_t r=0; r<T_r.size() ; r++ )
		std::cerr << T_r[r] <<"\n"; 
}

//Sets initial values gor the Gr.parameters
void SimulatedTempering::GrInit(const std::vector<double> &T_r, const unsigned int SERIE, const double stepSize)
{
	std::vector<double> EMean(T_r.size());
	double E = computeCost();
	double g=0;
	for (int r = int(T_r.size()-1); r>=0; r-- ) {
		double beta = 1/T_r[r];
		int numUphill;
		for (unsigned int i = 0; i < SERIE; i++) {
			metropolis(E, beta, stepSize,1,numUphill); 
			EMean[r] += E;
		}
		EMean[r] /=double(SERIE);

		if ( r != int(T_r.size()-1) )
		g -= EMean[r+1]*( 1/T_r[r] - 1/T_r[r+1]);
		Gr_[r] = g;
		std::cerr << T_r[r] << " " << EMean[r] << " " << Gr_[r] << "\n";
	}
}

//Adjusts the Gr parameters according to Gr->Gr+ ln(Pr) where Pr is the 
//probabilty of having a certain temperature. This should give a roughly
//uniform temperature distribution
void SimulatedTempering::GrAdjust(const std::vector<double> &T_r, const int SERIE, const int serie, const double stepSize)
{
	std::vector<double> Pr(T_r.size());
	double E = computeCost();
	size_t r = T_r.size()-1;
	int numUphill;
	for (int count = 0; count < SERIE; count++) {
		double T = T_r[r];
		metropolis(E, 1/T, stepSize,serie,numUphill); 
		metropolisT(E, r, T_r);
		Pr[r]++;
		std::cerr << count << " " << SERIE << " " << T_r[r] << " " << Pr[r] 
				<< " " << E << " " << globalOptima() << "\n";
	}
	for(r=0 ; r<Gr_.size() ; r++ )
	Pr[r] /= double(SERIE);

	for(r=0 ; r<Gr_.size() ; r++ )
		std::cerr << Gr_[r] << " " << Pr[r] << "\n";

	double eps = 1./double(SERIE);
	r = Gr_.size()-1;
	double delta = Pr[r]>eps ? Pr[r] : eps;
	double deltaR = log( delta ); 
	for( r=0 ; r<Gr_.size() ; r++ ){
		delta = Pr[r]>eps ? Pr[r] : eps; 
		Gr_[r] += log( delta )- deltaR;
	}
	for( r=0 ; r<Gr_.size() ; r++ )
	std::cerr << Gr_[r] << "\n";
}

//Takes a montecarlo-step in temperature			  
int SimulatedTempering::metropolisT(const double E,size_t &r,const std::vector<double> &T_r) 
{
	size_t rNew;
	if( r==0 ) 
		rNew = 1;
	else if( r==T_r.size()-1 ) 
		rNew = r-1;
	else if( myRandom::Rnd()>0.5 ) 
		rNew = r+1;
	else rNew = r-1;


	double pArg = Gr_[rNew]-Gr_[r]+E*(1/T_r[rNew] - 1/T_r[r] );
	if( pArg<=0. || exp(-pArg)>myRandom::Rnd() ) {
		r = rNew;
		return 1;
	}
	return 0;
}


int SimulatedAnnealing::metropolis(double &E, const double invT, double stepSize, const size_t numMetropolisStep, int &numUphill)
{
	int numSuccess = 0;
	numUphill=0;
	for (size_t i=0; i<numMetropolisStep; ++i) {
		//Pick index at random
		size_t position = size_t(numIndex() * myRandom::Rnd());
		double p = parameter(position);
		//Change f to 1/f in 50 percent of the cases
		if (myRandom::Rnd() > 0.5)
        {stepSize=1/stepSize;}
		setParameter(position, stepSize*p);
		double ENew = computeCost();
		double deltaE = ENew - E;
		double r = myRandom::Rnd();
		if (deltaE < 0 || exp(-invT*deltaE) > r ) {
			E=ENew;
			numSuccess++;
			if (deltaE>0.0) {
				numUphill++;
			}
		}
		else {
			setParameter(position, p);
		}
		if (E<globalOptima())
		setGlobal(E);
	}
	return numSuccess;
}

int SimulatedTempering::metropolis(double &E, const double invT, double stepSize, const size_t numMetropolisStep, int &numUphill)
{
	int numSuccess = 0;
	numUphill=0;
	for (size_t i=0; i<numMetropolisStep; ++i) {
		//Pick index at random
		size_t position = size_t(numIndex() * myRandom::Rnd());
		double p = parameter(position);
		//Change f to 1/f in 50 percent of the cases
		if (myRandom::Rnd() > 0.5)
		stepSize=1/stepSize;
		setParameter(position, stepSize*p);
		double ENew = computeCost();
		double deltaE = ENew - E;
		double r = myRandom::Rnd();
		if (deltaE < 0 || exp(-invT*deltaE) > r ) {
			E=ENew;
			numSuccess++;
			if (deltaE>0.0) {
				numUphill++;
			}
		}
		else {
			setParameter(position, p);
		}
		if (E<globalOptima())
		setGlobal(E);
	}
	return numSuccess;
}
