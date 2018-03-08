#include "Orthogonalise.h"

namespace Ortho{

double inner_prod(vector<double>* v1, vector<double>* v2){
	int d1 = v1->size();
	if(d1 != (int) v2->size()) return -1;
	double sum = 0;
	for(int i = 0; i < d1; ++i){
		sum += ((*v1)[i]) * ((*v2)[i]);
	}
	if(abs(sum) < EPSILON) sum = 0; 
	return sum;
}

bool is_zero(vector<double>* v){
	return(sqrt(inner_prod(v,v)) < EPSILON);
}


void normalize(vector<double>* v){
	int d = v->size();
	if(!is_zero(v)) {
		double coeff = 1.0 / sqrt(inner_prod(v,v));
		for(int i = 0; i < d;++i){
			(*v)[i] = (*v)[i] * coeff;
			if(abs((*v)[i]) < EPSILON)  (*v)[i] = 0;
		}
	}
	else cerr << "Inverse of zero vector\n";
}

void substract_projection(vector<double>* v1, vector<double>* vbase){
	double coeff = 1 / inner_prod(vbase, vbase); // warning : it is NOT the length but the length square !
	double proj = inner_prod(v1, vbase);
	int dimension = v1->size();
	for(int k = 0; k < dimension; ++k){
		(*v1)[k] = (*v1)[k] - ( proj * ((*vbase)[k]) * coeff );
		if(abs((*v1)[k]) < EPSILON)  (*v1)[k] = 0;
	}
}

double length(vector<double>* v1){
	return sqrt(inner_prod(v1, v1));
}
#define DBG2 0

bool orthogonalise(vector<vector<double>* >* OrthVects, vector<vector<double>* >* Parents, int dimension, double* delta){
	int n_parents = Parents->size();	
	if(DBG2) cout << n_parents << " Parents given as argument\n";
	if(n_parents > dimension){
		cerr << "More parents than dimension\n";
		return false;
	}

	// Check that all parents have the good dimension
	for(int i = 0; i < n_parents; ++i){
		if((int) (*Parents)[i]->size() != dimension) {
			cerr << " Parent " << i << "Has wrong dimension : " << (*Parents)[i]->size() << "\n";
			return false;
		}
	}
	
	// Size the output matrix; no necessary in principle
	// (*OrthVects).resize(dimension);
	if((int) OrthVects->size() != dimension) {
		cerr << "OrthVects has wrong Total dimension" << OrthVects->size() << "\n";
		return false;
	}
	for(int i = 0; i < dimension; ++i){
		if((int) ((*OrthVects)[i])->size() != dimension) {
			cerr << "OrthVects has wrong line " << i << "dimension : " << ((*OrthVects)[i])->size() << "\n";
			return false;
		}
	}
	
	// Orthogonalize the parents
	for(int i = 0; i < n_parents; ++i){
		vector<double> pi(*(*Parents)[i]);
		for(int j = 0; j < i; ++j){
			substract_projection(&pi,  (*OrthVects)[j]); 
		}
/*		cout << "P " << i+1 << " Before accepted : \n";
		for(int j = 0; j < dimension; ++j){
			cout << pi[j] << "\t";
		}
		cout << "\n";*/
		if(is_zero(&pi)) {
			if(DBG2)  cerr << "Cannot even normalize the parents\n"; // this can occur in normal conditions
			return false;
		}
		if(i == (n_parents - 1)) (*delta) = length(&pi);
		normalize(&pi);
		// Copy the orthog parent in the result matrix
		for(int k = 0; k < dimension; ++k){
			(*(*OrthVects)[i])[k] = pi[k];
		}
	};

	// Now, use e_1...e_n as a generatrice family to construct a base for the supplementary orthogonal space
	int n_added = n_parents;
	for(int i = 0; (n_added < dimension) && (i < dimension); ++i){
		vector<double> ei(dimension, 0);
		ei[i] = 1;
		
		for(int j = 0; j < n_added; ++j){
			substract_projection(&ei, (*OrthVects)[j]);
		}
		
		// If the orthogonalized ei is not 0 (meaning if ei was free with Orth0...(N_added-1), then we add it
		if(!is_zero(&ei)){
			normalize(&ei);
			for(int k = 0; k < dimension; ++k){
				(*((*OrthVects)[n_added]))[k] = ei[k];
			}
			n_added++;
		}
	}
	
	if(n_added < dimension){
		cerr << "Unable to make an orthogonal base ???\n";
		return false;
	}
		
		
	// Checking the orthogonality
	/*bool correct = true;
	for(int i = 0; i < dimension; ++i){
		cout << "p" << i+1 << "vs :\t";
		for(int j = 0; j < dimension; ++j){
			if(j >= i) cout << "\tp" << j+1 << ":" << inner_prod((*OrthVects)[i], (*OrthVects)[j]);
			else cout << "\t    ";
		}
		cout << "\n";
	}*/
	return true;	
}

}