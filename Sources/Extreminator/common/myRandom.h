#ifndef MYRANDOM_H
#define MYRANDOM_H
//
// Filename     : myRandom.h
// Description  : Defining namespace for random functions
// Author(s)    : Henrik Jonsson (henrik@thep.lu.se)
//              : Bo Soderberg (bosse@thep.lu.se)
// Created      : 
// Revision     : $Id: myRandom.h 474 2010-08-30 15:23:20Z henrik $
//

//#define USE_BOOST

#include <climits>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <cstdlib>
#include <sys/time.h>

#ifndef USE_BOOST
#include <random>
#endif

///
/// @brief Namespace including functions for generating random numbers
///
/// This namespace includes several functions for generating random
/// numbers of various kinds. It also includes Randomize functions for
/// seeding the random number generators. Some of these functions are
/// inherited from Bo Soderberg.
///
namespace myRandom {
	
	///
	/// @brief generates a random number in [0:1) by using ran3()
	///
	/// This function is provided for backward compability.
	/// 
	/// @see ran3()
	///
	double Rnd( void );
	
	///
	/// @brief Returns a Gaussian random number using Rnd()
	///
	/// @see Rnd()
	///
	double Grand( void );
	
	///
	/// @brief Calls the ran3Randomize() function
	///
	/// This function is provided for backward compability.
	/// 
	/// @see ran3Randomize()
	///
	long int Randomize( void );
	
	///
	/// @brief (0,1) random number generator
	///
	/// This random number generator returns a number in [0:1). It is
	/// adopted from Numerical Recipes and is credited to Knuth. This
	/// function is supposed to create better random properties compared
	/// to the standard random().
	///
	double ran3( void );
	//double ran3(long *idum);

	///
	/// @brief Generatess an idum based on time for the ran3() function
	///
	long int ran3Randomize( void ); 

	///
	/// @brief Sets the idum variable for the ran3 random sequence
	///
	void sran3(long idumVal);

	///
	/// @brief Generates a random number between 0 and 1 with the
	/// resolution of step
	///
	/// The step sets the number of possible levels.
	///
	double Rndstep(int step);
}

#include<iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <ctime>            // std::time
using namespace std;
   
#ifdef USE_BOOST
//#include  <boost/nondet_random.hpp>
#include  <boost/random.hpp>
#include  <boost/random/additive_combine.hpp>
#include  <boost/random/bernoulli_distribution.hpp>
#include  <boost/random/binomial_distribution.hpp>
#include  <boost/random/cauchy_distribution.hpp>
#include  <boost/random/discard_block.hpp>
#include  <boost/random/exponential_distribution.hpp>
#include  <boost/random/gamma_distribution.hpp>
#include  <boost/random/geometric_distribution.hpp>
#include  <boost/random/inversive_congruential.hpp>
#include  <boost/random/lagged_fibonacci.hpp>
#include  <boost/random/linear_congruential.hpp>
#include  <boost/random/linear_feedback_shift.hpp>
#include  <boost/random/lognormal_distribution.hpp>
#include  <boost/random/mersenne_twister.hpp>
#include  <boost/random/normal_distribution.hpp>
#include  <boost/random/poisson_distribution.hpp>
#include  <boost/random/random_number_generator.hpp>
#include  <boost/random/ranlux.hpp>
#include  <boost/random/shuffle_output.hpp>
#include  <boost/random/subtract_with_carry.hpp>
#include  <boost/random/triangle_distribution.hpp>
#include  <boost/random/uniform_01.hpp>
#include  <boost/random/uniform_int.hpp>
#include  <boost/random/uniform_on_sphere.hpp>
#include  <boost/random/uniform_real.hpp>
#include  <boost/random/uniform_smallint.hpp>
#include  <boost/random/variate_generator.hpp>
#include  <boost/random/xor_combine.hpp>
using namespace boost;

// Sun CC doesn't handle boost::iterator_adaptor yet
// #if !defined(__SUNPRO_CC) || (__SUNPRO_CC > 0x530)
// #include <boost/generator_iterator.hpp>
// #endif

/*
List of available generators
-----------------------------------------------------------------------------------
generator		length of  	memory 			speed compared
			cycle approx.				to fastest comment 
-----------------------------------------------------------------------------------
minstd_rand0		2^31-2		sizeof(int32_t)		30%
minstd_rand		2^31-2		sizeof(int32_t)		29%
rand48			2^48-1		sizeof(uint64_t)	100%
ecuyer1988		approx.2^61	2*sizeof(int32_t)	19%
kreutzer1986		?		1368*sizeof(uint32_t)	31%
taus88			~2^88		3*sizeof(uint32_t)	78%
hellekalek1995		2^31-1		sizeof(int32_t)		0%		good uniform distribution in severaldimensions
mt11213b		2^11213-1	352*sizeof(uint32_t)	44%		good uniform distribution in up to 350 dimensions
mt19937			2^19937-1	625*sizeof(uint32_t)	44%		good uniform distribution in up to 623 dimensions
lagged_fibonacci607	~2^32000	607*sizeof(double)	26%
lagged_fibonacci1279	~2^67000	1279*sizeof(double)	26%
lagged_fibonacci2281	~2^120000	2281*sizeof(double)	26%
lagged_fibonacci3217	~2^170000	3217*sizeof(double)	26%	-	
lagged_fibonacci4423	~2^230000	4423*sizeof(double)	25%	-
lagged_fibonacci9689	~2^510000	9689*sizeof(double)	25%
lagged_fibonacci19937	~2^1050000	19937*sizeof(double)	25%	-
lagged_fibonacci23209	~2^1200000	23209*sizeof(double)	25%	-
lagged_fibonacci44497	~2^2300000	44497*sizeof(double)	25%	-
ranlux3			~10^171		24*sizeof(int)		2%	-
ranlux4			~10^171		24*sizeof(int)		1%	-
ranlux64_3		~10^171		24*sizeof(int64_t)	2%	-
ranlux64_4		~10^171		24*sizeof(int64_t)	1%	-
ranlux3_01		~10^171		24*sizeof(float)	2%	-
ranlux4_01		~10^171		24*sizeof(float)	1%	-
ranlux64_3_01		~10^171		24*sizeof(double)	2%	-
ranlux64_4_01		~10^171		24*sizeof(double)	1%	-
-----------------------------------------------------------------------------------
*/


typedef boost::mt19937 generator_type;

class BoostRandoms{
	public:
        generator_type* generator;
        BoostRandoms(unsigned int init_seed = 0);
    /*
	private:
		typedef variate_generator<generator_type&, boost::uniform_smallint<> > uni_smi_type;
		uni_smi_type* uni_smi;
		bool uni_smi_created;
	public:
		void CreateUniformui(unsigned int x_start, unsigned int x_end);
		unsigned int Uniformui();
        void DestroyUniformui();*/
		
	private:
		typedef variate_generator<generator_type&, boost::uniform_int<> > uni_i_type;
		uni_i_type* uni_i;
		bool uni_i_created;
	public:
		void CreateUniformi(int x_start, int x_end);
		int Uniformi();
		void DestroyUniformi();	
		
	private:
		typedef variate_generator<generator_type&, boost::uniform_01<> > uni_01_type;
		uni_01_type* uni_01;
		bool uni_01_created;
	public:	
		double Uniform01();
			
	private:
		typedef variate_generator<generator_type&, boost::uniform_real<> > uni_d_type;
		uni_d_type* uni_d;
		bool uni_d_created;
	public:
		void CreateUniformd(int x_start, int x_end);
		double Uniformd();
		void DestroyUniformd();
		
	private:
		typedef variate_generator<generator_type&, boost::bernoulli_distribution<> > bernoulli_type;
		bernoulli_type* bernoulli;
		bool bernoulli_created;
	public:
		void CreateBernoulli(double p);
		bool Bernoulli();
		void DestroyBernoulli();
		
	private:
		typedef variate_generator<generator_type&, boost::binomial_distribution<> > binomial_type;
		binomial_type* binomial;
		bool binomial_created;
	public:
		void CreateBinomial(int nb, double p);
		int Binomial();
		void Destroybinomial();	
		
	private:
		typedef variate_generator<generator_type&, boost::cauchy_distribution<> > cauchy_type;
		cauchy_type* cauchy;
		bool cauchy_created;
	public:
		void CreateCauchy(double m, double sig);
		double Cauchy();
		void Destroycauchy();	
		
	private:
		typedef variate_generator<generator_type&, boost::gamma_distribution<> > gamma_type;
		gamma_type* gamma;
		bool gamma_created;
	public:
        void CreateGamma(double shape, double scale);
		double Gamma();
		void Destroygamma();	
		
	private:
		typedef variate_generator<generator_type&, boost::poisson_distribution<> > poisson_type;
		poisson_type* poisson;
		bool poisson_created;
	public:
		void CreatePoisson(double lambda);
		int Poisson();
		void Destroypoisson();

	private:
		typedef variate_generator<generator_type&, boost::geometric_distribution<> > geometric_type;
		geometric_type* geometric;
		bool geometric_created;
	public:
		void CreateGeometric(double lambda);
		int Geometric();
		void Destroygeometric();	
		
	private:
		typedef variate_generator<generator_type&, boost::exponential_distribution<> > exponential_type;
		exponential_type* exponential;
		bool exponential_created;
	public:
		void CreateExponential(double lambda);
		double Exponential();
		void Destroyexponential();	
		
	private:
		typedef variate_generator<generator_type&, boost::normal_distribution<> > normal_type;
		normal_type* normal;
		bool normal_created;
	public:
		void CreateNormal(double mn, double sig);
		double Normal();
		void Destroynormal();

	private:
		typedef variate_generator<generator_type&, boost::lognormal_distribution<> > log_normal_type;
		log_normal_type* log_normal;
		bool log_normal_created;
	public:
		void CreateLog_normal(double mn, double sig);
		double Log_normal();
		void Destroylog_normal();
		
	public:
		~BoostRandoms();
};



#else



typedef std::mt19937 generator_type;

class stdRandoms{
    public:
    //std::mt19937::result_type seed = time(0);
    //std::mt19937* gen  = new std::mt19937 (seed);
        //std::random_device* rng;
        generator_type* generator;
        stdRandoms(unsigned int init_seed = 0);

    private:
        // note :  	works with short, int, long, long long, unsigned short, unsigned int, unsigned long, or unsigned long long
        typedef std::uniform_int_distribution<int> uni_i_type; // 2 params : borders (included)
        uni_i_type* uni_i;
        bool uni_i_created;
    public:
        void CreateUniformi(int x_start, int x_end);
        int Uniformi();
        void DestroyUniformi();

    private:
        //std::uniform_real_distribution<>* RealDistrib = new std::uniform_real_distribution<> (0,1);
        typedef std::uniform_real_distribution<> uni_01_type;
        uni_01_type* uni_01;
        bool uni_01_created;
    public:
        double Uniform01();

    private:
        //std::uniform_real_distribution<>* RealDistrib = new std::uniform_real_distribution<> (0,1);
        typedef std::uniform_real_distribution<> uni_d_type;
        uni_d_type* uni_d;
        bool uni_d_created;
    public:
        void CreateUniformd(int x_start, int x_end);
        double Uniformd();
        void DestroyUniformd();

    private:
       // std::bernoulli_distribution distribution(0.5);
       // for (int i=0; i<nrolls; ++i) if (distribution(generator)) ++count;
        typedef std::bernoulli_distribution bernoulli_type;
        bernoulli_type* bernoulli;
        bool bernoulli_created;
    public:
        void CreateBernoulli(double p);
        bool Bernoulli();
        void DestroyBernoulli();

    private:
        //std::binomial_distribution<int> distribution(9,0.5);
        typedef std::binomial_distribution<int> binomial_type;
        binomial_type* binomial;
        bool binomial_created;
    public:
        void CreateBinomial(int nb, double p);
        int Binomial();
        void Destroybinomial();

    private:
        typedef std::cauchy_distribution<double> cauchy_type; // 2 parameters
        cauchy_type* cauchy;
        bool cauchy_created;
    public:
        void CreateCauchy(double m, double sig);
        double Cauchy();
        void Destroycauchy();

    private:
        typedef std::gamma_distribution<double> gamma_type; // 2 parameters !!!
        gamma_type* gamma;
        bool gamma_created;
    public:
        void CreateGamma(double shape, double scale);
        double Gamma();
        void Destroygamma();

    private:
        typedef std::poisson_distribution<int> poisson_type; // 1 param
        poisson_type* poisson;
        bool poisson_created;
    public:
        void CreatePoisson(double lambda);
        int Poisson();
        void Destroypoisson();

    private:
        typedef std::geometric_distribution<int>  geometric_type;
        geometric_type* geometric;
        bool geometric_created;
    public:
        void CreateGeometric(double lambda);
        int Geometric();
        void Destroygeometric();

    private:
        typedef std::exponential_distribution<double> exponential_type; // 1 param
        exponential_type* exponential;
        bool exponential_created;
    public:
        void CreateExponential(double lambda);
        double Exponential();
        void Destroyexponential();

    private:
        typedef  std::normal_distribution<double> normal_type;
        normal_type* normal;
        bool normal_created;
    public:
        void CreateNormal(double mn, double sig);
        double Normal();
        void Destroynormal();

    private:
        typedef std::lognormal_distribution<double> log_normal_type;
        log_normal_type* log_normal;
        bool log_normal_created;
    public:
        void CreateLog_normal(double mn, double sig);
        double Log_normal();
        void Destroylog_normal();

    public:
        ~stdRandoms();
        //void test();
};






























#endif

void testRandomGenerator();


#endif

