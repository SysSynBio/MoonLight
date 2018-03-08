#include "baseOptimizer.h"
#include "simulatedAnnealing.h"
#include "GradientDescent.h"
#include "GeneticGeneral.h" 
#include "SRES.h"
#include "Genetic.h"
#include "CMAES.h"
using namespace OrganismLib;

#define SPEAK false

// ================================ I - Constructors =================================================

/* This funciton is called from outside (and the first calls the second) */
BaseOptimizer* BaseOptimizer::createOptimizer(BaseOptimizationProblem* E, const std::string &file)
{
    //if(VERB) cerr << "CreateOptimizer summoned with opt file " << file << endl;
    std::istream *F_IN = myFiles::openFile(file);
    if (!F_IN) {std::cerr << "BaseOptimizer::createOptimizer() - " << "Cannot open file " << file << std::endl;
        exit(-1);}
    //if(VERB) cerr << "   File opened as a stream\n";
    BaseOptimizer* z = createOptimizer(E, (std::ifstream &) *F_IN);
    delete F_IN;
    return z;   //OK : returns the value, so, even if the lovation of z is destroyed,
                // the value points towards a still existing class, ouf !
}

BaseOptimizer* BaseOptimizer::createOptimizer(BaseOptimizationProblem* E, std::ifstream &F_IN)
{
    std::string idValue;
    F_IN >> idValue;
    if(SPEAK) std::cout << "\tOptimization using " << idValue << "\n";
    if(idValue == "simulatedAnnealing")         return new SimulatedAnnealing(E, F_IN);
    else if (idValue == "simulatedTempering")   return new SimulatedTempering(E, F_IN);
    else if (idValue == "geneticAlgorithm")     return (new GeneticGeneral(E, F_IN));
    else if (idValue == "SRES")                 return (new SRES(E, F_IN));
    else if (idValue == "oldGenetic")           return (new GeneticAlgo(E, F_IN));
    else if (idValue == "GradientDescent")      return (new GradientDescent(E, F_IN));
    else if (idValue == "CMAES")                return (new CMAES(E, F_IN));
    else {std::cerr << "\nBaseOptimizer::createOptimizer() WARNING: Optimizertype " << idValue << " not known, no optimizer created.\n\7";
        delete &F_IN;exit(-1);}
}

/* Constructor, for all subclasses, called by the subclasses before instanciating */

BaseOptimizer::BaseOptimizer(BaseOptimizationProblem* E, const std::string &file) : E_(E) {
  debug_live = 0;
  readOptimizer(file);
  randomize(&bestGlobal);
  worstGlobal = 0;
  updateCost(&bestGlobal);
  worstGlobal = bestGlobal.cost();
  //cerr << "random individual as first 'best global'" << endl;
}

BaseOptimizer::BaseOptimizer(BaseOptimizationProblem *E, std::ifstream &F_IN) : E_(E)
{
  debug_live = 0;
  readOptimizer(F_IN);
  randomize(&bestGlobal);
  worstGlobal = 0;
  updateCost(&bestGlobal);
  worstGlobal = bestGlobal.cost();
  //if(VERB) cerr << "random individual as first 'best global'" << endl;
}

BaseOptimizer::~BaseOptimizer(){}

void BaseOptimizer::optimize(){
  std::cerr << "BaseOptimizer::optimize() ->If this appears, it means that the " << "function optimize() has not been found for the type of optimizer you asked.\n";
  exit(-1);
}

/* this function is called from the constructor of base optimizer */

void BaseOptimizer::readOptimizer(const std::string &file) 
{
  std::istream *F_IN = myFiles::openFile(file);
  if (!F_IN) {
    std::cerr << "BaseOptimizer::readOptimizer() - " << "Cannot open file " << file << std::endl;
    exit(-1);
  }
  // read in the type of analyzer and how many arguments it takes
  std::string idValue;
  *F_IN >> idValue;
  if(SPEAK)  std::cout << "\tOptimization using " << idValue;
  readOptimizer((std::ifstream &) *F_IN);
}









// ============================ II - PARSING THE OPTIMIZER FILE ======================================

void BaseOptimizer::readOptimizer(std::ifstream &F_IN)
// Note that then idValue of the optimizer has already been read.
{	
    // read in the arguments for the analyzer
    F_IN >> numArg_;
    argument_.resize(numArg_);
    if (numArg_) {
        if(SPEAK) std::cout << "\tOptimization with " << numArg_ << " arguments:\n\t\t";
        for (size_t i = 0; i < numArg_; ++i) {
            F_IN >> argument_[i];
            if(SPEAK) std::cout  <<  argument_[i] << " ";
        }
    }
    if(SPEAK) std::cout << "\n";
  
    //Checks the number of parameter indices
    F_IN >> numIndex_;
    indexVector_.resize(numIndex_);
    paraLowVector_.resize(numIndex_);
    paraHighVector_.resize(numIndex_);
    paraNotScaledLowVector_.resize(numIndex_);
    paraNotScaledHighVector_.resize(numIndex_);
    //read in indices in the indexVector_ and lower and upper bounds into the parLow(High)Vector_
    if(SPEAK) std::cout << "\tPerforming optimization with the following " << numIndex_ << " ("
        << indexVector_.size() << ") parameter indices (low initial bound, high initial bound):" << std::endl;
    for (size_t i=0; i < numIndex_; ++i) {
        F_IN >> indexVector_[i];
        F_IN >> paraNotScaledLowVector_[i];
        F_IN >> paraNotScaledHighVector_[i];
        if(SPEAK) std::cout << "\t\t" << indexVector_[i] << " (" << paraNotScaledLowVector_[i] << "," << paraNotScaledHighVector_[i] << "), \n";
    }
    if(SPEAK) std::cout << std::endl;
  
    //Checks how many sets of parameters that are to be read from file
    F_IN >> numParaSet_;
    //read parameters from file and sets parametervalues, if flagged
    bool out_bounds = false;
    if (numParaSet_) {
        if(SPEAK) std::cout << "\n\tReading " << numParaSet_ << " parameter sets from the file...";
        NotScaledParameters_.resize(numParaSet_);
        for (size_t j=0; j<numParaSet_; ++j) {
            if(VERB) cout << "\tInitial parameter set nr. " << j << " :" << endl;
            NotScaledParameters_[j].resize(numIndex_);
            for (size_t i=0; i<numIndex_; ++i){
                F_IN >> NotScaledParameters_[j][i];
                if((NotScaledParameters_[j][i] < paraNotScaledLowVector_[i]) || (NotScaledParameters_[j][i] > paraNotScaledHighVector_[i])){
                    out_bounds = true;
                }
                if(VERB) cout << "\tread p" << i << " = " << NotScaledParameters_[j][i] << "\n";
            }
        }
        if(SPEAK) std::cout  << "\tDone\n";
        if(out_bounds) cerr << "Warning : Among set of parameters, low/high boundaries are exceeded." << endl;
        // std::cerr << "Note: by reading parameters, initial parameter boundaries overruled." << std::endl;
    }
    else {
        if(SPEAK) std::cout << "\tNo (" << numParaSet_ << ") initial parameter sets to be read from file."  << std::endl;
    }
  
    std::string scalingMethod;
    F_IN >> scalingMethod;
    if(scalingMethod == "NoScaling"){
        scaling_function_intern_to_solver = &Scaling::identity;
        scaling_function_solver_to_intern = &Scaling::identity;
        if(SPEAK) cout << "\tScaling = identity";
    } else if (scalingMethod == "ExponentialBounded"){
        scaling_function_intern_to_solver = &Scaling::exponential;
        scaling_function_solver_to_intern = &Scaling::logarithmic;
        if(SPEAK) cout << "\tScaling put to (real solver parameter) -> logarithmic -> (intern exploration space) - Bounded version";
    } else if (scalingMethod == "Exponential"){
        scaling_function_intern_to_solver = &Scaling::simple_exponential;
        scaling_function_solver_to_intern = &Scaling::simple_logarithmic;
        if(SPEAK) cout << "\tScaling put to (real solver parameter) -> logarithmic -> (intern exploration space)";
    } else if (scalingMethod == "LinearAndInverse"){
        scaling_function_intern_to_solver = &Scaling::linear_symmetric;
        scaling_function_solver_to_intern = &Scaling::linear_symmetric_rev;
        if(SPEAK) cout << "\tScaling put to (real solver parameter) -> [ x|->1/x if is x < 1,   and x if x > 1 ] -> (intern exploration space)";
    } else {
        scaling_function_intern_to_solver = &Scaling::identity;
        scaling_function_solver_to_intern = &Scaling::identity;
        if(SPEAK) cout << "\tScaling put to identity by default";
    }
  
  int num_scaling_parameters;
  F_IN >> num_scaling_parameters;
  if(SPEAK) std::cout << " with " << num_scaling_parameters << " parameters\n";
  if(num_scaling_parameters > 0){
    scaling_parameters = new vector<double>(5,0);
    if(num_scaling_parameters > 5) 
      cerr << "Max 5 arguments for the scaling : xmin xmax, "
	   << "ymin_forced, ymax_forced, coefficient -> Takes only the five first ones\n";
    for(int i = 0; i < num_scaling_parameters; ++i){
      double res;
      F_IN >> res;
      if(i < 5) (*scaling_parameters)[i] = res;
      if(SPEAK) if(i == 0) cout << "\txmin = " << res << "\n";
      if(SPEAK) if(i == 1) cout << "\txmax = " << res << "\n";
      if(SPEAK) if(i == 2) cout << "\tymin_force = " << res << "\n";
      if(SPEAK) if(i == 3) cout << "\txmax_force = " << res << "\n";
      if(SPEAK) if(i == 4) cout << "\tcoefficient = " << res << "\n";
      //if(i > 4) cerr << "Ignroe\n";
    }
  } else 	
    scaling_parameters = NULL;

  //if(VERB) cout << "Intern boundaries values, after scaling :" << endl;
    for (size_t i=0; i < numIndex_; ++i) {
        paraLowVector_[i] = scaling_function_solver_to_intern(paraNotScaledLowVector_[i], scaling_parameters);
        paraHighVector_[i] = scaling_function_solver_to_intern(paraNotScaledHighVector_[i], scaling_parameters);
        //if(VERB) cout << "\t" << i << "\t" << paraLowVector_[i] << "\t" << paraHighVector_[i] << endl;
    }
  
  if (numParaSet_) {
    parameters_.resize(numParaSet_);
    for (size_t j=0; j<numParaSet_; ++j) {
      parameters_[j].resize(numIndex_);
      for (size_t i=0; i<numIndex_; ++i){
	parameters_[j][i] = scaling_function_solver_to_intern(NotScaledParameters_[j][i], 
							      scaling_parameters);
      }
    }
    setParameter(&NotScaledParameters_[0]);
  }  
}










// ========================== III - Nb of cost calls ================================

void BaseOptimizer::resetCostCalls(){
  CostCalls = 0;
}

int  BaseOptimizer::nbCostCalls(){
  return CostCalls;
}








// ========================== IV Managing the best individual =======================

// ======================== getting the best set of parameters ======================
double BaseOptimizer::globalOptima(){
  return bestGlobal.cost();
}

void BaseOptimizer::printOptima(std::ostream &os) //Not scaled (solver)
{
    os << "#" << globalOptima() << " " << " ";
    os << globalOptima() << "  ";
    for (size_t i =0; i<E_->numPara(); i++)
        os << optimalParameter(i) << " ";
    os << "\n";
    os.flush();
}

double BaseOptimizer::optimalParameter(int n){ // Not scaled, (solver)
    // If this parameter is one to optimize, then take the value from bstGlobal
    assert(bestGlobal.modified() == false);
    for(size_t i = 0; i < numIndex(); ++i){
        if((int) indexVector(i) == n) return bestGlobal.equivparam(i);
    }
    // Either, take the value in the estimator, which should never change.
    return parameter(n);
}

void BaseOptimizer::getOptimalParameters(vector<double>* save_opt_para){ // Not scaled, (solver)
    assert(bestGlobal.modified() == false);
    if(save_opt_para->size() != numIndex()){
        save_opt_para->resize(numIndex());
    }
    for(size_t i = 0; i < numIndex(); ++i){
        (*save_opt_para)[i] = bestGlobal.equivparam(i);
    }
}

void BaseOptimizer::getAllOptimalParameters(vector<double>* save_opt_para){ // Not Scaled, solver
    assert(bestGlobal.modified() == false);
    if(save_opt_para->size() != E_->numPara()){
        save_opt_para->resize(E_->numPara());
    }
    for(size_t i = 0; i < E_->numPara(); ++i){
        (*save_opt_para)[i] = E_->parameter(i);
    }
    for(size_t i = 0; i < numIndex(); ++i){
        (*save_opt_para)[indexVector(i)] = bestGlobal.equivparam(i);
    }
}

// =================== Setting the best individual without checking the previous cost ========================
void BaseOptimizer::setGlobal(double cost){
    // Implicitely say that the current set of parameters in the estimator is the best and has a cost of cost
    bestGlobal.setCost(cost);
    for (size_t i=0; i<numIndex(); ++i){
        bestGlobal.setEquivParam(i, parameter(i));
    }
    bestGlobal.ComputeGenesFromEquivParams(scaling_function_solver_to_intern, scaling_parameters);
}
void BaseOptimizer::setGlobal(double cost, vector<double>* v){
    assert(v->size() == numIndex());
    for (size_t i=0; i<numIndex(); ++i){
        bestGlobal.setGene(i, (*v)[i]);
    }
    bestGlobal.setCost(cost);
}
void BaseOptimizer::setGlobalNotScaled(double cost, vector<double>* v){
    assert(v->size() == numIndex());
    for (size_t i=0; i<numIndex(); ++i){
        bestGlobal.setEquivParam(i, (*v)[i]);
    }
    bestGlobal.ComputeGenesFromEquivParams(scaling_function_intern_to_solver, scaling_parameters);
    bestGlobal.setCost(cost);
}

// =================== Setting the best optimal only if better cost ========================
void BaseOptimizer::improveGlobal(double cost){
    if(cost < bestGlobal.cost()){
        setGlobal(cost);
    }
}
void BaseOptimizer::improveGlobal(double cost, vector<double>* v){
  if(cost < bestGlobal.cost()){
    setGlobal(cost, v);
  }
}
void BaseOptimizer::improveGlobalNotScaled(double cost, vector<double>* v){
  if(cost < bestGlobal.cost()){
    setGlobalNotScaled(cost, v);
  }
}
// and don't forget the function (above) which computes the cost AND updates the best individual
// BaseOptimizer::updateCost(individual* ind)












// ===================== V - I/O with the parameters inside the estimator (cost) ==========================

void BaseOptimizer::setParameter(vector<double>* v){
    assert(v->size() == numIndex());
    for(int i = 0; i < (int) numIndex_; ++i){
        E_->setParameter(indexVector_[i], scaling_function_intern_to_solver((*v)[i], scaling_parameters));
    }
}

void BaseOptimizer::setParameterNotScaled(vector<double>* v){
    assert(v->size() == numIndex());
    for(int i = 0; i < (int) numIndex_; ++i){
        E_->setParameter(indexVector_[i], (*v)[i]);
    }
}

void BaseOptimizer::setParameter(int i, double value){
    E_->setParameter(indexVector_[i], scaling_function_intern_to_solver(value, scaling_parameters));
}

void BaseOptimizer::setParameterNotScaled(int i, double value){
    E_->setParameter(indexVector_[i], value);
}


double BaseOptimizer::parameter(int i){
    return scaling_function_solver_to_intern(E_->parameter(indexVector(i)), scaling_parameters);
}

double BaseOptimizer::parameterNotScaled(int i){
    return E_->parameter(indexVector(i));
}

void BaseOptimizer::printState(double E, std::ostream &os){
    E_->printState(E, os);
}

// ============================= randomizing the sets (inside or outside estimator) ========================
void BaseOptimizer::randomizeSet(vector<double> * toRandomize){ // Intern Scaled Version
    toRandomize->resize(numIndex());
    assert (numIndex()==indexVector_.size() &&
        numIndex()==paraLowVector_.size() &&
        numIndex()==paraHighVector_.size() );
    for(size_t i = 0; i < numIndex(); i++) {
        double lMin = (paraLowVector(i));
        double lMax = (paraHighVector(i));
        double lDelta = lMax-lMin;
        double r = lMin+lDelta*myRandom::Rnd();
        /*double lMin = std::log10(paraLowVector(i));
        double lMax = std::log10(paraHighVector(i));
        double lDelta = lMax-lMin;
        double r = std::pow(10.0,lMin+lDelta*myRandom::Rnd());*/
        (*toRandomize)[i] = r;
    }
}
void BaseOptimizer::randomizeSetNotScaledUniform(vector<double> * toRandomize){ // Intern Scaled Version WARNING ! This is uniform on the non scaled range, which is different from randomized in intern scale plus converted to non scaled solver type
    toRandomize->resize(numIndex());
    assert (numIndex()==indexVector_.size() &&
        numIndex()==paraNotScaledLowVector_.size() &&
        numIndex()==paraNotScaledHighVector_.size() );
    for(size_t i = 0; i < numIndex(); i++) {
        double lMin = (paraNotScaledLowVector(i));
        double lMax = (paraNotScaledHighVector(i));
        double lDelta = lMax-lMin;
        double r = lMin+lDelta*myRandom::Rnd();
        (*toRandomize)[i] = r;
    }
}
void BaseOptimizer::randomizeParameter() // Inside the solver -> not scaled BUT !!! randomized in uniform according to the scaling
{
    assert (numIndex()==indexVector_.size() &&
        numIndex()==paraLowVector_.size() &&
        numIndex()==paraHighVector_.size() );
    for(size_t i = 0; i < numIndex(); i++) {
        double lMin = (paraLowVector(i));
        double lMax = (paraHighVector(i));
        double lDelta = lMax-lMin;
        double r = lMin+lDelta*myRandom::Rnd();
        E_->setParameter(indexVector(i),scaling_function_intern_to_solver(r, scaling_parameters));
    }
}
void BaseOptimizer::randomizeParameterUsingNotScaledUniform() // Inside the solver -> not scaled BUT !!! randomized in uniform according to the scaling
{
    assert (numIndex()==indexVector_.size() &&
        numIndex()==paraNotScaledLowVector_.size() &&
        numIndex()==paraNotScaledHighVector_.size() );
    for(size_t i = 0; i < numIndex(); i++) {
        double lMin = (paraNotScaledLowVector(i));
        double lMax = (paraNotScaledHighVector(i));
        double lDelta = lMax-lMin;
        double r = lMin+lDelta*myRandom::Rnd();
        setParameterNotScaled(i,r);
    }
}
individual* BaseOptimizer::newRandomIndividual(){ // NOOOON !!!
    individual* a = new individual();
    a->resize(numIndex());
    assert (numIndex()==indexVector_.size() &&
        numIndex()==paraLowVector_.size() &&
        numIndex()==paraHighVector_.size() );
    for(size_t i = 0; i < numIndex(); i++) {
        double lMin = (paraLowVector(i));
        double lMax = (paraHighVector(i));
        double lDelta = lMax-lMin;
        double r = lMin+lDelta*myRandom::Rnd();
        a->setGene(i, r);
    }
    return a;

   /*    vector<double> tempForCopy;
    tempForCopy.resize(numIndex_);
    randomizeParameter();
    for(int i = 0; i < (int) numIndex_; ++i){
        tempForCopy[i] = E_->parameter(indexVector_[i]); // intern scaled version
    }
    individual* a = new individual(&tempForCopy);
    return a;*/
}
void BaseOptimizer::randomize(individual* i2){
    individual* i1 = newRandomIndividual();
    i2->copy(i1);
    delete i1;
}














// ===================== VI - Checking if a set of parameters/individual is within the boundaries ==========================

bool BaseOptimizer::InsideBoundaries(vector<double>* intern_parameters){
    int z = intern_parameters->size();
    bool inside = true;
    for(int i = 0; i < z; ++i){
        if(((*intern_parameters)[i] < paraLowVector(i)) || ((*intern_parameters)[i] > paraHighVector(i))) inside = false;
    }
    return inside;
}
bool BaseOptimizer::InsideBoundariesNotScaled(vector<double>* parameters){
    int z = parameters->size();
    bool inside = true;
    for(int i = 0; i < z; ++i){
        if(((*parameters)[i] < paraNotScaledLowVector(i)) || ((*parameters)[i] > paraNotScaledHighVector(i))) inside = false;
    }
    return inside;
}
bool BaseOptimizer::InsideBoundaries(individual* ind){
    int z = ind->size();
    bool inside = true;
    for(int i = 0; i < z; ++i){
        if((ind->gene(i) < paraLowVector(i)) || (ind->gene(i) > paraHighVector(i))) inside = false;
    }
    return inside;
}





// ========================= VII Asking the cost of an individual ========================

double BaseOptimizer::computeCost(){
    double E = E_->getCost();
    CostCalls++;
    return E;
}
void BaseOptimizer::updateCost(individual* ind){
    if(ind->modified()){
        double E = computeCost(ind);
        if(std::isnan(E) || std::isinf(E)) {
            E = worstGlobal;
        };
        if(E > 1e50) E = 1e50;
        bestGlobal.improve(ind);
        if(ind->cost() > worstGlobal) worstGlobal = ind->cost();
    }
}
double BaseOptimizer::computeCost(individual* ind){
    ind->ComputeEquivParamsFromGenes(scaling_function_intern_to_solver, scaling_parameters);
    for(int i = 0; i < (int) numIndex_; ++i){
        E_->setParameter(indexVector_[i], (ind->equivparam(i)));
        //cerr << "P" << i << " = " << E_->parameter(indexVector_[i]) << "\tintern = " << ind->gene(i) << endl;
    }
    double E = E_->getCost();
    CostCalls++;
    ind->setCost(E); // This will do modified_ = false inside ind.
    return E;
}
double BaseOptimizer::computeCost(vector<double>* intern_parameters){
    for(int i = 0; i < (int) numIndex_; ++i){
        E_->setParameter(indexVector_[i], scaling_function_intern_to_solver((*intern_parameters)[i], scaling_parameters));
        //cerr << "given p " << i << "= " << scaling_function_intern_to_solver((*intern_parameters)[i], scaling_parameters) << "\n";
    }
    double E = E_->getCost();
    CostCalls++;
    return E;
}
double BaseOptimizer::computeCostNotScaled(vector<double>* parameters){
    for(int i = 0; i < (int) numIndex_; ++i){
        E_->setParameter(indexVector_[i], (*parameters)[i]);
        //cerr << "given p " << i << "= " << (*parameters)[i] << "\n";
    }
    double E = E_->getCost();
    CostCalls++;
    return E;
}
