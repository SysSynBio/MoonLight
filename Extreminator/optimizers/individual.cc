#include "individual.h"

int GenID()
{
	static int ID = 15;
	ID = (ID + 253) % 2546;
	return ID;	
}

individual::individual(){
	parents.clear();
	age_ = 0;
	nGenes_ = 0;
	Genome_.clear();
	EquivParam_.clear();
	StepSize_.resize(nGenes_, 1.0);
	modified_ = true;
	cost_ = -1;
	OtherValues_.clear();
	ID = GenID();
}

individual::individual(vector<double>* newGenome){
	parents.clear();
	age_ = 0;
	nGenes_ = (*newGenome).size();
	Genome_.resize(nGenes_);
	EquivParam_.resize(nGenes_);
	StepSize_.resize(nGenes_, 1.0);
	for(int i = 0; i < nGenes_; ++i){
		Genome_[i] = (*newGenome)[i];
	};
	modified_ = true;
	cost_ = -1;
	OtherValues_.clear();
	ID = GenID();
}

individual::individual(vector<double>* newGenome, vector<double>* newStepSizes){
	parents.clear();
	age_ = 0;
	nGenes_ = (*newGenome).size();
	Genome_.resize(nGenes_);
	EquivParam_.resize(nGenes_);
	if((int) newStepSizes->size() != nGenes_){
		cerr << "You want to create an individual with not the same number of genes and stepSizes\n";
		return;
	}
	StepSize_.resize(nGenes_, 1.0);
	for(int i = 0; i < nGenes_; ++i){
		Genome_[i] = (*newGenome)[i];
		StepSize_[i] = (*newStepSizes)[i];
	};
	modified_ = true;
	cost_ = -1;
	OtherValues_.clear();
	ID = GenID();
}

void individual::copy(individual* i2){
	parents.clear();
	age_ = i2->age_;
	nGenes_ = i2->Genome_.size();
	Genome_.resize(nGenes_);
	StepSize_.resize(nGenes_);
	EquivParam_.resize(nGenes_);
	if((int) i2->StepSize_.size() != nGenes_){
		cerr << "Copy : pattern parent for copy is incorrect : not the same number of genes and parameters\n";
		return;
	}
	if((int) i2->EquivParam_.size() != nGenes_){
		cerr << "Copy : pattern parent for copy is incorrect : not the same number of EquivPara and parameters\n";
		return;
	}
	for(int i = 0; i < nGenes_; ++i){
		Genome_[i] = i2->Genome_[i];
		StepSize_[i] = i2->StepSize_[i];
		EquivParam_[i] = i2->EquivParam_[i];
	}
	cost_ = i2->cost_;
	modified_ = i2->modified_;
	int N = i2->OtherValues_.size();
	OtherValues_.resize(N);
	for(int i = 0; i < N; ++i){
		int P = i2->OtherValues_[i].size();
		OtherValues_[i].resize(P);
		for(int j = 0; j < P; ++j){
			OtherValues_[i][j] = i2->OtherValues_[i][j];
		}
	}
	ID = i2->myID();
}

individual::individual(individual* i2){
	copy(i2);
}

int individual::size(){
	return Genome_.size();
}

void individual::resize(int n){
	Genome_.resize(n, 0);
	StepSize_.resize(n, 1); 
	EquivParam_.resize(n,0);
	ID = GenID();
	nGenes_ = n; 
	modified_ = true;
	age_ = 0;

}

double individual::gene(int i){
	return Genome_[i];
}

void individual::setGene(int i, double value){
	if((i >= nGenes_) || (i < 0)){ std::cerr << "out of range setGene " << i << " >= " << nGenes_ << "\n"; exit(-1);};
	modified_ = true;
	age_ = 0;
	ID = GenID();
	Genome_[i] = value;
}

void individual::setGenome(vector<double>* newGenome){
	modified_ = true;
	ID = GenID();
	nGenes_ = (*newGenome).size();
	Genome_.resize(nGenes_);
	for(int i = 0; i < nGenes_; ++i){
		Genome_[i] = (*newGenome)[i];
	}
	age_ = 0;
}

double individual::step(int i){
	return StepSize_[i];
}

void individual::setStepSize(int i, double value){
	if((i >= nGenes_) || (i < 0)){ std::cerr << "out of range setStepSize " << i << " >= " << nGenes_ << "\n"; exit(-1);};
	//modified_ = true;
	//ID = GenID();
	StepSize_[i] = value;
}

void individual::setStepSizes(vector<double>* newStepSizes){
	//modified_ = true;
	//ID = GenID();
	if((int) (*newStepSizes).size() != nGenes_){
		cerr << "To set a stepsize vectror, it should have the same length as the genome\n";
		return;
	}
	StepSize_.resize(nGenes_);
	for(int i = 0; i < nGenes_; ++i){
		StepSize_[i] = (*newStepSizes)[i];
	}
}

double individual::equivparam(int i){
	return EquivParam_[i];
}

void individual::setEquivParam(int i, double value){
	if((i >= nGenes_) || (i < 0)){ std::cerr << "SetEquivParam out of range setStepSize " << i << " >= " << nGenes_ << "\n"; exit(-1);};
	EquivParam_[i] = value;
}

void individual::setEquivParams(vector<double> * newEquivParams){
	if((int) (*newEquivParams).size() != nGenes_){
		cerr << "To set a EquivParam vectror, it should have the same length as the genome\n";
		return;
	}
	EquivParam_.resize(nGenes_);
	for(int i = 0; i < nGenes_; ++i){
		EquivParam_[i] = (*newEquivParams)[i];
	}
}

void individual::ComputeEquivParamsFromGenes(double (*scale_from_intern_to_solver)(double, vector<double>*), vector<double>* scaling_params){
	for(int i = 0; i < nGenes_; ++i){
		EquivParam_[i] = scale_from_intern_to_solver(gene(i), scaling_params);
	}
}

void individual::ComputeGenesFromEquivParams(double (*scale_from_solver_to_intern)(double, vector<double>*), vector<double>* scaling_params){
	for(int i = 0; i < nGenes_; ++i){
		Genome_[i] = scale_from_solver_to_intern(equivparam(i), scaling_params);
	}	
}

vector<double>* individual::genome(){
	return &Genome_;
}

vector<double>* individual::stepSizes(){
	return &StepSize_;
}

vector<double>* individual::equivParams(){
	return &EquivParam_;
}

bool individual::modified(){
	return modified_;
}

double individual::cost(){
	if(modified_){std::cerr << "Cost unknown yet !\n"; return -1;};
	return cost_;
}

void individual::setCost(double E){
	cost_ = E;
	modified_ = false;
}

int individual::age(){
	return age_;
}

void individual::setAge(int newAge){
	age_ = newAge;
}

void individual::ageing(){
	age_++;
}


bool individual::improve(individual* contestant){
	if((contestant->modified()) || (modified_)){
        std::cerr << "comparison of individuals whose costs have not been evaluated yet\n";
		return false;
	}
	if((contestant->cost()) < cost_) {
		copy(contestant);
		return true;
	}
	return false;
}

void individual::print(){
    cout << "\t";
    std::cout << "E=" << cost() << "\t";
    std::cout << "ID=" << myID() << "\t";
    std::cout << "Age=" << age() << "\n";
	int size_ = Genome_.size();
	for(int i = 0; i < (int) size_; ++i){
        std::cout << "\t" << i << "\tIntern =\t" << gene(i) << "\t -> real = \t" << (modified() ? -1 : equivparam(i)) << "\tStep = \t" << step(i) << "\n";
	}
}

vector<vector<double> >* individual::OtherValues(){
	return &OtherValues_;
}



void individual::setOtherValues(vector<vector<double> >* OV){
	int N = (*OV).size();
	OtherValues_.resize(N);
	for(int i = 0; i < N; ++i){
		int P = (*OV)[i].size();
		OtherValues_[i].resize(P);
		for(int j = 0; j < P; ++j){
			OtherValues_[i][j] = (*OV)[i][j];
		}
	}
}

int individual::num_parents(){
	return parents.size();
}
void individual::add_parent(int p){
	parents.push_back(p);
}
int individual::parent(int i){
	return parents[i];
}
void individual::clearParents(){
	parents.clear();
}
