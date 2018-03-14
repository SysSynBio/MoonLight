#ifndef ORTHOGONALISE_H
#define ORTHOGONALISE_H

#include<iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <cmath>
using namespace std; 

#define EPSILON 1e-15

namespace Ortho{
	double inner_prod(vector<double>* v1, vector<double>* v2);
	bool is_zero(vector<double>* v);
	void normalize(vector<double>* v);
	void substract_projection(vector<double>* v1, vector<double>* vbase);
	double length(vector<double>* v1);
	bool orthogonalise(vector<vector<double>* >* OrthVects, vector<vector<double>* >* Parents, int dimension, double* delta);
}// end namespace

/* Example of how to use
#include <iomanip>

int main(void){
	int dimension = 6;
	vector<vector<double>*> z;
	z.resize(dimension);
	for(int i = 0; i < dimension; ++i){
		z[i] = new vector<double>(dimension, 0);
	}

	vector<vector<double>*> Parents;
	Parents.resize(3);
	vector<double> p1(6,0);
	vector<double> p2(6,0);
	vector<double> p3(6,0);
	p1[0] = 1;
	p1[1] = 0;
	p1[2] = 0;
	p1[3] = 0;
	
	p2[0] = 1;
	p2[1] = 1;
	p2[2] = 0;
	p2[3] = 0;
	
	//substract_projection(&p2, &p1);
	//for(int i = 0; i < 6; ++i){
	//	cout << p2[i] << " ";
	//}
	
	p3[0] = 1;
	p3[1] = 1;
	p3[2] = 1;
	p3[3] = 2;
	Parents[0] = &p1;
	Parents[1] = &p2;
	Parents[2] = &p3;

	double delta;
	
	orthogonalise(&z, &Parents, 6, &delta);
	
	for(int i = 0; i < 6; ++i){
		for(int j = 0; j < 6; ++j){
			cerr << (*(z[i]))[j] << "\t";
		}
		cerr << "\n";
	}
	
	cerr << "Finished and delta = " << delta << "\n";
	
	
	return 0;
}
*/

#endif




 
