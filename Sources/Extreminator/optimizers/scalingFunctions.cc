#include <stdlib.h>
#include <cmath>
#include <iostream>
#include "scalingFunctions.h"
using namespace std;

namespace Scaling{
	
double border(double x, double deriv, double max_value){
	double res = max_value * (1 - (1 / (deriv * x + 1)));
	return res;	
}

double invborder(double y, double deriv, double maxvalue){
	double aux = (y) / maxvalue;
	double res = aux / (deriv * (1 - aux));
	return res;
}

double identity(double x, vector<double>*){
	return x;
}

#define MegaMaxAuthorized 1e50
#define xmin (*boundaries)[0]
#define xmax (*boundaries)[1]
#define ymin (*boundaries)[2]
#define ymax (*boundaries)[3]
#define coeff (*boundaries)[4]


double exponential(double x, vector<double>* boundaries){
	double res = 0;
	if(boundaries){
		res = std::exp(x);
		if(x < xmin) res = std::exp(xmin*coeff) - border(xmin - x, std::exp(xmin*coeff), 0.1*coeff*std::exp(xmin*coeff));
		if(x > xmax) res = std::exp(xmax*coeff) + border(x - xmax, std::exp(xmax*coeff), 0.1*coeff*std::exp(xmax*coeff));
		if(res > ymax) res = ymax;
		if(res < ymin) res = ymin;
	} else { // Then, the function is just exponential(x)
		res = std::exp(x);
	}
	return res;
}

double logarithmic(double y, vector<double>* boundaries){
	if(boundaries){
		if((y > ymax) || (y > 1.1*std::exp(xmax*coeff))) {
			std::cerr << "Problem : Real Parameter out of range. gives xmax as reverse (log)\n";
			return xmax;
		}
		if((y < ymin) || (y < 0.9*std::exp(xmin*coeff))) {
			std::cerr << "Problem : Real Parameter out of range. gives xmin as reverse (log)\n";
			return xmin;
		}
		if(y > std::exp(xmax)){
			return xmax + invborder(y-std::exp(xmax*coeff), std::exp(xmax*coeff), 0.1*coeff*std::exp(xmax*coeff));
		}
		if( y < std::exp(xmin)){
			return xmin - invborder(-y+std::exp(xmin*coeff), std::exp(xmin*coeff), 0.1*coeff*std::exp(xmin*coeff));
		}
		return std::log(y/coeff);
	} else {
		return std::log(y);
	}
}

double simple_exponential(double x, vector<double>*){
    return std::exp(x-15.0);
}

double simple_logarithmic(double y, vector<double>*){
    if(y < 1e-15) return 15.0+std::log(1e-15);
    return 15.0+std::log(y);
}

double linear_symmetric(double x, vector<double>*){
	return(x >= 0 ? x+1 : 1.0 / (-x+1) );
}

double linear_symmetric_rev(double y, vector<double>*){
	return(y >= 1 ? y-1 : 1-1.0/y);
}

}
