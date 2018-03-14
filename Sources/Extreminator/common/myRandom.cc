//
// Filename     : myRandom.cc
// Description  : Defining the functions for random number generators
// Author(s)    : Henrik Jonsson (henrik@thep.lu.se)
//              : Bo Soderberg (bosse@thep.lu.se)
// Created      : 
// Revision     : $Id: myRandom.cc 474 2010-08-30 15:23:20Z henrik $
//

#include <iostream>

#include "myRandom.h"

namespace myRandom {

	double Rnd( void )
	{
		return ran3();
		//return random() / double(INT_MAX);
	}
	
	double Grand( void )
	{
		return sqrt( -2 * log(Rnd())) * cos( M_PI * Rnd() );
	}
		
	double Rndstep(int step)
	{
		return (double)((int)((step+1)*Rnd()))/step;
	}
	
	long int Randomize( void ) 
	{
		return ran3Randomize();
	}
		
#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)
//double ran3(long *idum)
	static long idum = 1;
	double ran3( void )
	{
		static int inext,inextp;
		static long ma[56];
		static int iff=0;
		long mj,mk;
		int i,ii,k;
		double ret_val;
		
		//if (*idum < 0 || iff == 0) {
		if (myRandom::idum < 0 || iff == 0) {
			iff=1;
			//mj=MSEED-(*idum < 0 ? -*idum : *idum);
			mj=MSEED-(myRandom::idum < 0 ? -myRandom::idum : myRandom::idum);
			mj %= MBIG;
			ma[55]=mj;
			mk=1;
			for (i=1;i<=54;i++) {
				ii=(21*i) % 55;
				ma[ii]=mk;
				mk=mj-mk;
				if (mk < MZ) mk += MBIG;
				mj=ma[ii];
			}
			for (k=1;k<=4;k++)
				for (i=1;i<=55;i++) {
					ma[i] -= ma[1+(i+30) % 55];
					if (ma[i] < MZ) ma[i] += MBIG;
				}
			inext=0;
			inextp=31;
			//*idum=1;
			myRandom::idum=1;
		}
		if (++inext == 56) inext=1;
		if (++inextp == 56) inextp=1;
		mj=ma[inext]-ma[inextp];
		if (mj < MZ) mj += MBIG;
		ma[inext]=mj;
		ret_val = mj*FAC;
		if (mj == 0) ret_val = FAC;
		return ret_val;
	}
#undef MBIG
#undef MSEED
#undef MZ
#undef FAC
	
	void sran3( long idumVal )
	{
		std::cerr << "sran3()\n";
		// Make sure seed in [1:10000000] 
		// The ran3 function has proven to be instable otherwise
		if( idumVal<1 || idumVal>10000000 ) {
			std::cerr << "myRandom::sran3() Seed (idum) provided in dangerous "
								<< "region. Usevalues in [1:10000000]." << std::endl;
			exit(-1);
		}
		idum=-idumVal;
	}
	
	long int ran3Randomize( void ) 
	{
		struct timeval tp; // { long tv_sec; long tv_usec; }
		long int seed;
		
		gettimeofday(&tp, 0);
		//seed = 1024 * tp.tv_sec + (int) (.001024 * tp.tv_usec)
		seed = 1+tp.tv_usec%9999999;//Put seed in [1:10000000]
		idum = -seed;
        //std::cerr << "Init random numbers with seed : " << -seed << ")" << endl; //ran3Randomize()\n";
		return -seed;
	}
} //end namespace myRandom

#define NB_POINTS_TEST_RANDOM 200
#ifdef USE_BOOST

BoostRandoms::BoostRandoms(unsigned int init_seed){
    unsigned int seed = static_cast<unsigned int>(std::time(0));
    if(init_seed != 0) seed = init_seed;
    cout << "Seed for optimization(Boost::mt19937) : \t" << seed << endl;
    generator = new generator_type(seed);
    //uni_smi_created = false;
	uni_i_created = false;
	uni_01_created = false;
	uni_d_created = false;
	bernoulli_created = false;
	binomial_created = false;
	cauchy_created = false;
	gamma_created = false;
	poisson_created = false;
	geometric_created = false;
	exponential_created = false;
	normal_created = false;
	log_normal_created = false;
}
	
/*void BoostRandoms::CreateUniformui(unsigned int x_start, unsigned int x_end){
	if(uni_smi_created) delete uni_smi;
	uni_smi = new uni_smi_type (*generator, boost::uniform_smallint< > (x_start, x_end));
	uni_smi_created = true;
}

unsigned int BoostRandoms::Uniformui(){
	if(uni_smi_created) 
		return (*uni_smi)();
	else {
		cerr << "UniformSMI Distribution not created !\n";
		CreateUniformui((unsigned int) 0, (unsigned int) 1);
		return (*uni_smi)();
	}
}

void BoostRandoms::DestroyUniformui(){
	delete uni_smi;
	uni_smi_created = false;
}*/

void BoostRandoms::CreateUniformi(int x_start, int x_end){
	if(uni_i_created) delete uni_i;
	uni_i = new uni_i_type (*generator, boost::uniform_int< > (x_start, x_end));
	uni_i_created = true;
}

int BoostRandoms::Uniformi(){
	if(uni_i_created) 
		return (*uni_i)();
	else {
		cerr << "UniformI Distribution not created !\n";
		CreateUniformi((int) 0, (int) 1);
		return (*uni_i)();
	}
}

void BoostRandoms::DestroyUniformi(){
	delete uni_i;
	uni_i_created = false;
}

double BoostRandoms::Uniform01(){
	if(uni_01_created) 
		return (*uni_01)();
	else {
		uni_01 = new uni_01_type (*generator, boost::uniform_01< > ());
		uni_01_created = true;
		return (*uni_01)();
	}
}

void BoostRandoms::CreateUniformd(int x_start, int x_end){
	if(uni_d_created) delete uni_d;
	uni_d = new uni_d_type (*generator, boost::uniform_real< > (x_start, x_end));
	uni_d_created = true;
}

double BoostRandoms::Uniformd(){
	if(uni_d_created) 
		return (*uni_d)();
	else {
		cerr << "UniformD Distribution not created !\n";
		CreateUniformd((double) 0.0, (double) 1.0);
		return (*uni_d)();
	}
}

void BoostRandoms::DestroyUniformd(){
	delete uni_d;
	uni_d_created = false;
}

void BoostRandoms::CreateBernoulli(double p){
	if(bernoulli_created) delete bernoulli;
	bernoulli = new bernoulli_type (*generator, boost::bernoulli_distribution< > (p));
	bernoulli_created = true;
}

bool BoostRandoms::Bernoulli(){
	if(bernoulli_created) 
		return (*bernoulli)();
	else {
		cerr << "Bernouilli Distribution not created !\n";
		CreateBernoulli((double) 0.5);
		return (*bernoulli)();
	}
}

void BoostRandoms::DestroyBernoulli(){
	delete bernoulli;
	bernoulli_created = false;
}

void BoostRandoms::CreateBinomial(int nb, double p){
	if(binomial_created) delete binomial;
	binomial = new binomial_type (*generator, boost::binomial_distribution< > (nb, p));
	binomial_created = true;
}

int BoostRandoms::Binomial(){
	if(binomial_created) 
		return (*binomial)();
	else {
		cerr << "Binomial Distribution not created !\n";
		CreateBinomial(100, (double) 0.5);
		return (*binomial)();
	}
}

void BoostRandoms::Destroybinomial(){
	delete binomial;
	binomial_created = false;
}

void BoostRandoms::CreateCauchy(double m, double sig){
	if(cauchy_created) delete cauchy;
	cauchy = new cauchy_type (*generator, boost::cauchy_distribution< > (m, sig));
	cauchy_created = true;
}

double BoostRandoms::Cauchy(){
	if(cauchy_created) 
		return (*cauchy)();
	else {
		cerr << "Cauchy Distribution not created !\n";
		CreateCauchy((double)0, (double) 1);
		return (*cauchy)();
	}
}

void BoostRandoms::Destroycauchy(){
	delete cauchy;
	cauchy_created = false;
}

void BoostRandoms::CreateGamma(double shape, double scale){
	if(gamma_created) delete gamma;
    gamma = new gamma_type (*generator, boost::gamma_distribution< > (shape, scale));
	gamma_created = true;
}

double BoostRandoms::Gamma(){
	if(gamma_created) 
		return (*gamma)();
	else {
		cerr << "Gamma Distribution not created !\n";
        CreateGamma((double) 1, (double) 1);
		return (*gamma)();
	}
}

void BoostRandoms::Destroygamma(){
	delete gamma;
	gamma_created = false;
}	

void BoostRandoms::CreatePoisson(double lambda){
	if(poisson_created) delete poisson;
	poisson = new poisson_type (*generator, boost::poisson_distribution< > (lambda));
	poisson_created = true;
}

int BoostRandoms::Poisson(){
	if(poisson_created) 
		return (*poisson)();
	else {
		cerr << "Poisson Distribution not created !\n";
		CreatePoisson((double) 1);
		return (*poisson)();
	}
}

void BoostRandoms::Destroypoisson(){
	delete poisson;
	poisson_created = false;
}	

void BoostRandoms::CreateGeometric(double lambda){
	if(geometric_created) delete geometric;		
	geometric = new geometric_type (*generator, boost::geometric_distribution< > (lambda));
	geometric_created = true;
}

int BoostRandoms::Geometric(){
	if(geometric_created) 
		return (*geometric)();
	else {
		cerr << "Geometric Distribution not created !\n";
		CreateGeometric((double) 0.5);
		return (*geometric)();
	}
}

void BoostRandoms::Destroygeometric(){
	delete geometric;
	geometric_created = false;
}	

void BoostRandoms::CreateExponential(double lambda){
	if(exponential_created) delete exponential;
	exponential = new exponential_type (*generator, boost::exponential_distribution< > (lambda));
	exponential_created = true;
}

double BoostRandoms::Exponential(){
	if(exponential_created) 
		return (*exponential)();
	else {
		cerr << "Exponential Distribution not created !\n";
		CreateExponential((double) 1);
		return (*exponential)();
	}
}

void BoostRandoms::Destroyexponential(){
	delete exponential;
	exponential_created = false;
}	

void BoostRandoms::CreateNormal(double mn, double sig){
	if(normal_created) delete normal;
	normal = new normal_type (*generator, boost::normal_distribution< > (mn, sig));
	normal_created = true;
}

double BoostRandoms::Normal(){
	if(normal_created) 
		return (*normal)();
	else {
		cerr << "Normal Distribution not created !\n";
		CreateNormal((double) 0.0, (double) 1);
		return (*normal)();
	}
}

void BoostRandoms::Destroynormal(){
	delete normal;
	normal_created = false;
}	

void BoostRandoms::CreateLog_normal(double mn, double sig){
	if(log_normal_created) delete log_normal;
	log_normal = new log_normal_type (*generator, boost::lognormal_distribution< > (mn, sig));
	log_normal_created = true;
}

double BoostRandoms::Log_normal(){
	if(log_normal_created) 
		return (*log_normal)();
	else {
		cerr << "Log_Normal Distribution not created !\n";
		CreateLog_normal((double) 0.0, (double) 1);
		return (*log_normal)();
	}
}

void BoostRandoms::Destroylog_normal(){
	delete log_normal;
	log_normal_created = false;
}	

BoostRandoms::~BoostRandoms(){
	// suppress all existing
	if(uni_01_created) delete uni_01;
	if(cauchy_created) delete cauchy;
    //if(uni_smi_created) delete uni_smi;
	if(uni_i_created) delete uni_i;
	if(uni_d_created) delete uni_d;
	if(bernoulli_created) delete bernoulli;
	if(binomial_created) delete binomial;		
	if(gamma_created) delete gamma;
	if(log_normal_created) delete log_normal;
	if(normal_created) delete normal;
	if(exponential_created) delete exponential;
	if(geometric_created) delete geometric;
	if(poisson_created) delete poisson;

}





#else









stdRandoms::stdRandoms(unsigned int init_seed){
    std::mt19937::result_type seed = time(0);
    if(init_seed != 0) seed = init_seed;
    std::cout << "Seed for optimization(std::mt19937) : \t" << seed << endl;
    generator = new std::mt19937(seed);
    //uni_smi_created = false;
    uni_i_created = false;
    uni_01_created = false;
    uni_d_created = false;
    bernoulli_created = false;
    binomial_created = false;
    cauchy_created = false;
    gamma_created = false;
    poisson_created = false;
    geometric_created = false;
    exponential_created = false;
    normal_created = false;
    log_normal_created = false;
}

/*void stdRandoms::CreateUniformui(unsigned int x_start, unsigned int x_end){
    if(uni_smi_created) delete uni_smi;
    uni_smi = new uni_smi_type (*generator, boost::uniform_smallint< > (x_start, x_end));
    uni_smi_created = true;
}

unsigned int stdRandoms::Uniformui(){
    if(uni_smi_created)
        return (*uni_smi)();
    else {
        cerr << "UniformSMI Distribution not created !\n";
        CreateUniformui((unsigned int) 0, (unsigned int) 1);
        return (*uni_smi)();
    }
}

void stdRandoms::DestroyUniformui(){
    delete uni_smi;
    uni_smi_created = false;
}*/

void stdRandoms::CreateUniformi(int x_start, int x_end){
    if(uni_i_created) delete uni_i;
    uni_i = new uni_i_type(x_start, x_end);
    uni_i_created = true;
}

int stdRandoms::Uniformi(){
    if(uni_i_created)
        return (*uni_i)(*generator);
    else {
        cerr << "UniformI Distribution not created !\n";
        CreateUniformi((int) 0, (int) 1);
        return (*uni_i)(*generator);
    }
}

void stdRandoms::DestroyUniformi(){
    delete uni_i;
    uni_i_created = false;
}

double stdRandoms::Uniform01(){
    if(uni_01_created)
        return (*uni_01)(*generator);
    else {
        uni_01 = new uni_01_type(0.0, 1.0);
        uni_01_created = true;
        return (*uni_01)(*generator);
    }
}

void stdRandoms::CreateUniformd(int x_start, int x_end){
    if(uni_d_created) delete uni_d;
    uni_d = new uni_d_type(x_start, x_end);
    uni_d_created = true;
}

double stdRandoms::Uniformd(){
    if(uni_d_created)
        return (*uni_d)(*generator);
    else {
        cerr << "UniformD Distribution not created !\n";
        CreateUniformd((double) 0.0, (double) 1.0);
        return (*uni_d)(*generator);
    }
}

void stdRandoms::DestroyUniformd(){
    delete uni_d;
    uni_d_created = false;
}

void stdRandoms::CreateBernoulli(double p){
    if(bernoulli_created) delete bernoulli;
    bernoulli = new bernoulli_type(p);
    bernoulli_created = true;
}

bool stdRandoms::Bernoulli(){
    if(bernoulli_created)
        return (*bernoulli)(*generator);
    else {
        cerr << "Bernouilli Distribution not created !\n";
        CreateBernoulli((double) 0.5);
        return (*bernoulli)(*generator);
    }
}

void stdRandoms::DestroyBernoulli(){
    delete bernoulli;
    bernoulli_created = false;
}

void stdRandoms::CreateBinomial(int nb, double p){
    if(binomial_created) delete binomial;
    binomial = new binomial_type (nb, p);
    binomial_created = true;
}

int stdRandoms::Binomial(){
    if(binomial_created)
        return (*binomial)(*generator);
    else {
        cerr << "Binomial Distribution not created !\n";
        CreateBinomial(100, (double) 0.5);
        return (*binomial)(*generator);
    }
}

void stdRandoms::Destroybinomial(){
    delete binomial;
    binomial_created = false;
}

void stdRandoms::CreateCauchy(double m, double sig){
    if(cauchy_created) delete cauchy;
    cauchy = new cauchy_type (m, sig);
    cauchy_created = true;
}

double stdRandoms::Cauchy(){
    if(cauchy_created)
        return (*cauchy)(*generator);
    else {
        cerr << "Cauchy Distribution not created !\n";
        CreateCauchy((double)0, (double) 1);
        return (*cauchy)(*generator);
    }
}

void stdRandoms::Destroycauchy(){
    delete cauchy;
    cauchy_created = false;
}

void stdRandoms::CreateGamma(double shape, double scale){
    if(gamma_created) delete gamma;
    gamma = new gamma_type(shape, scale);
    gamma_created = true;
}

double stdRandoms::Gamma(){
    if(gamma_created)
        return (*gamma)(*generator);
    else {
        cerr << "Gamma Distribution not created !\n";
        CreateGamma((double) 1, (double) 1);
        return (*gamma)(*generator);
    }
}

void stdRandoms::Destroygamma(){
    delete gamma;
    gamma_created = false;
}

void stdRandoms::CreatePoisson(double lambda){
    if(poisson_created) delete poisson;
    poisson = new poisson_type(lambda);
    poisson_created = true;
}

int stdRandoms::Poisson(){
    if(poisson_created)
        return (*poisson)(*generator);
    else {
        cerr << "Poisson Distribution not created !\n";
        CreatePoisson((double) 1);
        return (*poisson)(*generator);
    }
}

void stdRandoms::Destroypoisson(){
    delete poisson;
    poisson_created = false;
}

void stdRandoms::CreateGeometric(double lambda){
    if(geometric_created) delete geometric;
    geometric = new geometric_type(lambda);
    geometric_created = true;
}

int stdRandoms::Geometric(){
    if(geometric_created)
        return (*geometric)(*generator);
    else {
        cerr << "Geometric Distribution not created !\n";
        CreateGeometric((double) 0.5);
        return (*geometric)(*generator);
    }
}

void stdRandoms::Destroygeometric(){
    delete geometric;
    geometric_created = false;
}

void stdRandoms::CreateExponential(double lambda){
    if(exponential_created) delete exponential;
    exponential = new exponential_type(lambda);
    exponential_created = true;
}

double stdRandoms::Exponential(){
    if(exponential_created)
        return (*exponential)(*generator);
    else {
        cerr << "Exponential Distribution not created !\n";
        CreateExponential((double) 1);
        return (*exponential)(*generator);
    }
}

void stdRandoms::Destroyexponential(){
    delete exponential;
    exponential_created = false;
}

void stdRandoms::CreateNormal(double mn, double sig){
    if(normal_created) delete normal;
    normal = new normal_type(mn, sig);
    normal_created = true;
}

double stdRandoms::Normal(){
    if(normal_created)
        return (*normal)(*generator);
    else {
        cerr << "Normal Distribution not created !\n";
        CreateNormal((double) 0.0, (double) 1);
        return (*normal)(*generator);
    }
}

void stdRandoms::Destroynormal(){
    delete normal;
    normal_created = false;
}

void stdRandoms::CreateLog_normal(double mn, double sig){
    if(log_normal_created) delete log_normal;
    log_normal = new log_normal_type(mn, sig);
    log_normal_created = true;
}

double stdRandoms::Log_normal(){
    if(log_normal_created)
        return (*log_normal)(*generator);
    else {
        cerr << "Log_Normal Distribution not created !\n";
        CreateLog_normal((double) 0.0, (double) 1);
        return (*log_normal)(*generator);
    }
}

void stdRandoms::Destroylog_normal(){
    delete log_normal;
    log_normal_created = false;
}

stdRandoms::~stdRandoms(){
    // suppress all existing
    if(uni_01_created) delete uni_01;
    if(cauchy_created) delete cauchy;
    //if(uni_smi_created) delete uni_smi;
    if(uni_i_created) delete uni_i;
    if(uni_d_created) delete uni_d;
    if(bernoulli_created) delete bernoulli;
    if(binomial_created) delete binomial;
    if(gamma_created) delete gamma;
    if(log_normal_created) delete log_normal;
    if(normal_created) delete normal;
    if(exponential_created) delete exponential;
    if(geometric_created) delete geometric;
    if(poisson_created) delete poisson;

}

#endif

void testRandomGenerator(){
#ifdef USE_BOOST
    BoostRandoms a = BoostRandoms(153);
#else
    stdRandoms a = stdRandoms(153);
#endif



    /* a.CreateUniformui(1,5);
    unsigned int z;
    for(int i = 0; i < 10; ++i){
        z = a.Uniformui();
        cout << z << "\n";
    } */

    cout << "UniformInt1-5" << endl;
    int z2;
    a.CreateUniformi(1,5);
    for(int i = 0; i < NB_POINTS_TEST_RANDOM; ++i){
        z2 = a.Uniformi();
        cout << z2 << "\n";
    }
    cout << "Uniform01" << endl;
    double z3;
    for(int i = 0; i < NB_POINTS_TEST_RANDOM; ++i){
        z3 = a.Uniform01();
        cout << z3 << "\n";
    }

    cout << "double Uniform 1-10\n";
    double z4;
    a.CreateUniformd(1,10);
    for(int i = 0; i < NB_POINTS_TEST_RANDOM; ++i){
        z4 = a.Uniformd();
        cout << z4 << "\n";
    }

    cout << "bool Bernouilli p=0.3\n";
    bool z5;
    a.CreateBernoulli(0.3);
    for(int i = 0; i < NB_POINTS_TEST_RANDOM; ++i){
        z5 = a.Bernoulli();
        cout << z5 << "\n";
    }

    cout << "int Binoöial 25 tries p = 0.6 \n";
    int z6;
    a.CreateBinomial(25,0.6);
    for(int i = 0; i <NB_POINTS_TEST_RANDOM; ++i){
        z6 = a.Binomial();
        cout << z6 << "\n";
    }

    cout << "double Cauchy mean 0 sig 1\n";
    double z7;
    a.CreateCauchy(0,1);
    for(int i = 0; i < NB_POINTS_TEST_RANDOM; ++i){
        z7 = a.Cauchy();
        cout << z7 << "\n";
    }

    cout << "double Gamma k=2, sc=1\n";
    double z8;
    a.CreateGamma(2.0, 1.0);
    for(int i = 0; i < NB_POINTS_TEST_RANDOM; ++i){
        z8 = a.Gamma();
        cout << z8 << "\n";
    }

    cout << "int Poisson mean 10\n";
    int z9;
    a.CreatePoisson(10.0);
    for(int i = 0; i < NB_POINTS_TEST_RANDOM; ++i){
        z9 = a.Poisson();
        cout << z9 << "\n";
    }

    cout << "int Geometric p=0.5\n";
    int z10;
    a.CreateGeometric(0.5);
    for(int i = 0; i < NB_POINTS_TEST_RANDOM; ++i){
        z10 = a.Geometric();
        cout << z10 << "\n";
    }

    cout << "double Exponential lambda = 2\n";
    double z11;
    a.CreateExponential(2.0);
    for(int i = 0; i < NB_POINTS_TEST_RANDOM; ++i){
        z11 = a.Exponential();
        cout << z11 << "\n";
    }

    cout << "double Normal mean 0  sig 1\n";
    double z12;
    a.CreateNormal(0.0, 1.0);
    for(int i = 0; i < NB_POINTS_TEST_RANDOM; ++i){
        z12 = a.Normal();
        cout << z12 << "\n";
    }

    cout << "double lognormal mean 0 sig 1\n";
    double z13;
    a.CreateLog_normal(0.5, 1.0);
    for(int i = 0; i < NB_POINTS_TEST_RANDOM; ++i){
        z13 = a.Log_normal();
        cout << z13 << "\n";
    }


}
