#include "overrider.h"
#include <sstream>

overrider::overrider() : nbIndices(0) {}
void overrider::reset(){
    nbIndices = 0;
    typeStorage.clear();
    dataFunct.clear();
    override.clear();
    for(int i = 0; i < (int) dataSpl.size(); ++i){
        if(dataSpl[i]) delete dataSpl[i];
    }
}
void overrider::extend(int newNbIndices){
    if(newNbIndices > nbIndices){
        //cout << "Extend Overrider, new size " << newNbIndices << endl;
        typeStorage.resize(newNbIndices, NODATA);
        dataSpl.resize(newNbIndices, NULL);
        dataFunct.resize(newNbIndices);
        override.resize(newNbIndices, false);
        nbIndices = newNbIndices;
    }
}

bool overrider ::hasData(int index){
    if((index < 0) || (index >= nbIndices)) return false;
    //cout << "HasData(index=" << index << "), max = " << nbIndices << endl;
    switch(typeStorage[index]){
        case NODATA:{return false; break;}
        case SPLINE:{return (dataSpl[index] != NULL); break;}
        case FUNCTION:{return (dataFunct[index] != NULL); break;}
        default: return false;
    }
    return false; // this should not happen
}

double overrider::operator()(int index, double value){
    if(!hasData(index)) return -1;
    switch(typeStorage[index]){
        case SPLINE:{return (* (dataSpl[index]))(value); break;}
        case FUNCTION:{return (* (dataFunct[index]))(value); break;}
    default: return -1;
    }
    return -1;
}

bool overrider::operator()(int index){
    if((index < 0) || (index >= nbIndices)) return false;
    return override[index];
}

void overrider::learnSpl(int index, vector<double> xs, vector<double> ys, bool yesSplnoLinearInterpol){
    if(xs.size() != ys.size()) {cerr << "ERR: overrider::learnSpl, the vectors xs and ys should have the same size\n"; return;}
    if(hasData(index)) {cerr << "WRN : overrider::learnSpl, data was already saved for the index " << index << "; data overrided.\n";}
    spline* SP = new spline();

    /*cout << "learning, ID=" << index << " " << CodingName(index) << endl;
    for(int i = 0; i < xs.size(); ++i){
        cout << xs[i] << "\t";
    } cout << endl;
    for(int i = 0; i < ys.size(); ++i){
        cout << ys[i] << "\t";
    } cout << endl;
    cout << endl;*/

    SP->set_points(xs, ys, yesSplnoLinearInterpol);
    extend(index+1);    /// extend the data structure if required
    dataSpl[index] = SP;
    typeStorage[index] = SPLINE;
}

void overrider::setOver(int index, bool value){
    if(index == -1){
        for(int i = 0; i < nbIndices;++i){
            if(hasData(i)) override[i] = value;
        }
        return;
    }
    if((index < 0) || (index >= nbIndices)) {if(!value) return; else {cerr << "ERR : overrider::setOver(index=" << index << ", value = " << (value ? "true" : "false") << "), this index does not refer to any stored data (index out of bounds)\n"; return;}}
    if((!hasData(index)) && (value))   {cerr << "ERR : overrider::setOver(index=" << index << ", value = " << (value ? "true" : "false") << "), this index does not refer to any stored data (no data for this index)\n"; return;}
    override[index] = value;
}


void overrider::learnFunct(int index, double (*f) (double)){
    if(hasData(index)) {cerr << "WRN : overrider::learnFunct, data was already saved for the index " << index << "; data overrided.\n";}
    extend(index+1);
    dataFunct[index] = f;
    typeStorage[index] = FUNCTION;
}

double ff(double x){
    return x * (x - 1) + 3;
}

string overrider::print(){
    stringstream res;
    res << "Overrider with " << nbIndices << " curves\n";
    if(nbIndices != (int) typeStorage.size()) res << "ERR : typeStorage has size " << typeStorage.size() << endl;
    if(nbIndices != (int) override.size()) res << "ERR : typeStorage has size " << override.size() << endl;
    for(int i = 0; i < nbIndices; ++i){
        res << "CurveNb\t" << i << "\t";
        switch(typeStorage[i]){
            case NODATA: {res << "NODATA"; break;}
            case SPLINE: {res << "SPLINE"; break;}
            case FUNCTION: {res << "FUNCTION"; break;}
            default: {res << "ERR: Non Identified type";}
        }
        res << "\t" << (override[i] ? "YES:override" : "NO:dont override") << endl;
    }

    return res.str();
}

void testOverrider(){
    // plan of test : variable 1, function, variable 2, no data, variable 3, spline, variable 4, linear interpol

    // for variable 1, will give ff
    // for variable 3
    vector<double> px;
    px.push_back(0.);
    px.push_back(3.0);
    px.push_back(6.0);
    px.push_back(9.0);
    px.push_back(12.0);
    px.push_back(18.0);
    px.push_back(24.0);

    vector<double> py;
    py.push_back(0.1);
    py.push_back(0.3);
    py.push_back(0.4);
    py.push_back(0.6);
    py.push_back(1.2);
    py.push_back(2.5);
    py.push_back(3.0);

    overrider ov = overrider();
    ov.learnFunct(1, &(ff));
    ov.learnSpl(3, px, py, true);
    ov.reset(); // just to see if resetting doesn't create segFaults ...
    ov.learnFunct(1, &(ff));
    ov.learnSpl(3, px, py, true);
    ov.learnSpl(4, px, py, false);

    for(int i = 0; i < 10; ++i){
        if(ov.hasData(i)) cout << "Ov has data at index " << i << endl;
    }
    for(double dd = 0; dd < 30; dd += 0.5){
        cout << "\n" << dd;
        for(int i = 1; i <= 4; ++i){
            cout << "\t" << ov(i, dd);
        }
    }

    return;

}
