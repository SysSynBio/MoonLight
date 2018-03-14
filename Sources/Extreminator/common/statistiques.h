#ifndef STATS_H
#define STATS_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm> // for sorting in selection
#include <cmath>

///
/// @brief A general class implementing simple genetic algorithm with cross-over ans mutation
///

#include <iostream>
#include <fstream>
#include "myFiles.h"
#include "myRandom.h"
#include <vector>
using namespace std;


struct statistique{
	int nb;
	double sum;
	double best;
	double worse;
	double variance;
	vector<double> stock;
	int stock_size; // current size (= 2^k) real size = stock_size-1.
	int stock_used; //the pointer of the next to fill = the number of data saved
	statistique(){
		reset();
	}
	void reset(){
		sum = 0;
		nb = 0;
		best = 1e25;
		worse = -1e25;
		stock_used = 0;
		stock_size = 1;
		variance = -1;
	}
	void add(double d){
		sum += d;
		nb++;
		best = min(best, d);
		worse = max(worse, d);
		
		if(stock_used >= (stock_size-1)){
			stock_size *= 2;
			stock.resize(stock_size-1);
		}
		stock[stock_used] = d;
		stock_used++;
		
	}
	double mean(){
		return sum / nb;
	}
	double bestSeen(){
		return best;
	}
	double worseSeen(){
		return worse;
	}
	void sumUp(){
		
		int n = stock_used;
		double sum = 0;
		double sumSquare = 0;
		for(int i = 0; i < n; ++i){
			sum += stock[i];
			sumSquare += (stock[i])*(stock[i]);
		};
		variance = sqrt((sumSquare / n) - ((sum * sum) / (n*n)));
		
        //std::cerr << nb << " Iterations;\tMean = " << mean() << "\tBest = " << bestSeen() << "\tWorse = " << worseSeen() << "\tsaVar = " << variance << "\n";
	}	
};


#endif
