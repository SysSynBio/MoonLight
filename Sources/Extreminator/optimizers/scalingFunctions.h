#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <vector>
using namespace std;

namespace Scaling{
	
double identity(double x, vector<double>*);

double exponential(double x, vector<double>* boundaries);

double logarithmic(double y, vector<double>* boundaries);

double simple_exponential(double x, vector<double>*);

double simple_logarithmic(double y, vector<double>*);

double linear_symmetric(double x, vector<double>*);

double linear_symmetric_rev(double y, vector<double>*);

} 
