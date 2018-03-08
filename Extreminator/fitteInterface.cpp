#include "fitteInterface.h"

Fitte::Fitte(Experiment* _Exp) : GeneralImplementation(_Exp->m->getNbParams()), ExpToOpt(_Exp) {
    #ifndef WITHOUT_QT
    window = new simuWin(_Exp);
    cerr << " Is this called ?? " << endl;
    #endif
}

double Fitte::getCost(){
    int nbParamsModele = ExpToOpt->m->getNbParams();
    cout << "\tParams:";
    for(int i = 0; i < nbParamsModele; ++i){
        if(parameters[i] != 0.0) ExpToOpt->m->setParam(i, parameters[i]);
        cout << "\t" << parameters[i];
    }
    #ifndef WITHOUT_QT
    window->updateParmsFromModel();     /// to display on the screen the new values.
    #endif
    double v2 = ExpToOpt->costPart();
    cout << "\nCost: " << v2 + ExpToOpt->m->penalities;
    cout << "\tDont_Penalty:\t" << ExpToOpt->m->penalities << endl;
    return v2 + ExpToOpt->m->penalities;
}

void Optimize(string optimizerFile, Experiment* _Exp)
{

    std::cerr << "Starting fitting\n";
    // std::string optimizerFile = string("Opt.txt"); //argv[1];
    myRandom::Randomize();
    myTimes::getTime();
    BaseOptimizationProblem *E = new Fitte(_Exp);
    BaseOptimizer *Opt = BaseOptimizer::createOptimizer(E, optimizerFile);
    //mySignal::addOptimizer(Opt);
    Opt->optimize();
    Opt->bestGlobal.print();
    //delete E; // program destructor first...
    delete Opt;
}






// =============== Old pieces of code and tries to make multiple threads ====================

/*Modele2 m2;
m2.initialise();
m2.setBaseParameters();
vector<double> reconstitute(m2.NBPARAM, 0.0);
for(int i = 0; i < m2.NBPARAM; ++i){
    reconstitute[i] = m2.params[i];
    cerr << i << "\t" << m2.params[i]<< endl;
}
cerr << "--------------------------\n";
//sleep(1);
for(int i = 0; i < Opt->bestGlobal.size(); ++i){
    cerr << Opt->indexVector_[i] << "\t" << Opt->bestGlobal.gene(i) << endl;
    reconstitute[Opt->indexVector_[i]] = Opt->bestGlobal.gene(i);
}
//cerr << "Fini1" << endl;
//sleep(2);
cerr << "Best individual, initial problem scale\n";
for(int i =0; i < m2.NBPARAM; ++i){
    cerr << reconstitute[i] << endl;
    //sleep(1);
}*/

//Delete simulator and optimizer


/*class Worker : public QObject {
    Q_OBJECT

public:
    Worker();
    ~Worker();

public slots:
    void process();

signals:
    void finished();
    void error(QString err);

private:
    // add your variables here
};


// --- CONSTRUCTOR ---
Worker::Worker() {
    // you could copy data from constructor arguments to internal variables here.
}

// --- DECONSTRUCTOR ---
Worker::~Worker() {
    // free resources
}

// --- PROCESS ---
// Start processing data.
void Worker::process() {
    // allocate resources using new here
    qDebug("Hello World!");
    emit finished();
}*/
