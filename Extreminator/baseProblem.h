#ifndef BASEOPTPROBLEM_H
#define BASEOPTPROBLEM_H

#include <cmath>        // for NAN
using namespace std;

class BaseOptimizationProblem{
	public:
        BaseOptimizationProblem(){}
        virtual double getCost(){return 0;}
        virtual inline void setParameter(size_t, double){}
        virtual void initialize() {}
        virtual inline size_t numPara() const {return 0;}
        virtual inline double parameter(size_t) const {return 0;}
        virtual void printState(double,std::ostream & = std::cout) const {}
};

class GeneralImplementation : public BaseOptimizationProblem {
protected:
    vector<double> parameters;
    int dimension;
public:

    GeneralImplementation(int dim) : BaseOptimizationProblem(){
        dimension = dim;
        parameters.clear();         // I do not use initialize() here because it might use the initialize of the subclass ...
        parameters.resize(dim, NAN);
    }
    virtual double getCost(){
        cerr << "Implementation of getCost not done (if you see this message)" << endl;
        return 0;
    }
    inline void setParameter(size_t i, double value){
        if(((int) i > dimension)) return; // removed (i <0) because size_t never negative
        parameters[i] = value;
    }
    inline double parameter(size_t i) const{
        if(((int) i > dimension)) return 0.0;// removed if (i <0) because size_t never negative
        return parameters[i];
    }
    inline size_t numPara() const{
        return dimension;
    }
    void printState(double E, std::ostream &os = std::cout) const{
        os << "E= " << E;
        for(int i = 0; i < dimension; ++i){
            os << "\t" << parameters[i];
        }
        os << "\n";
    }
    virtual void initialize(){
        parameters.clear();
        parameters.resize(dimension, NAN);
    }

};
#endif
