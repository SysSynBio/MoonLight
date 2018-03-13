// The file common.h checks the system and defines WINDOWS/UNIX and QT4/QT5.
#include "../common.h"
#include "Extreminator/common/myRandom.h"

#ifdef WINDOWS
#include <windows.h>
#endif

#ifdef UNIX
#include <sys/stat.h>
#endif

#ifndef WITHOUT_QT
#ifdef QT5
#include <QMainWindow>
#include <QApplication>
#endif
#ifdef QT4
#include <QtGui/QMainWindow>
#include <QtGui/QApplication>
#endif
#endif

#include <vector>
#include <ctime>
using namespace std;

#include "../Interface/simuwin.h"
#include "../Interface/starter.h"

#include "namesGradCourse.h"
#include "expsGradCourse.h"
#include "Models/1-modeleLogistic.h"
#include "Models/2-modeleLotkaVolterra.h"
#include "Models/3-modeleDevelopment.h"
#include "Models/4-modeleDevelopmentNoFeedback.h"
#include "Models/5-modeleInfluenza.h"


/// @brief tool functions implemented below
string codeTime();                                              /// @brief a function to give a different name each time called (based on the time)
string textFileForFolder(string explanations);                  /// @brief generates the .tex to analyze a folder with results (pictures)
void compileLatex(string folderRes, string texFile);            /// @brief ask the system to compile a tex file and put the result in folderRes
void mergePDFs(vector<string> & listFiles, string outputFile);  /// @brief merges PDF files into one with a new name
string removeFolderFromFile(string file);                       /// @brief keeps only the file name (and not folder)

/// @brief MAIN SCRIPTS (called by the main or can be called manually)
void scriptsWithMainExp(int nb, string newConfigFile = string(""), string newParameterSet = string(""));        // mentioning the script that the main will call (one argument)
//string scriptsWithPerturbations(int nb, double parameter, string analysisName = string(""), string outputFolder = string(""), string chosenConfig = string(""), string chosenSet = string(""));                                                        // mentioning the script that the main will call (two arguments)
//void scriptsWith20HoursExp(int nb,string chosenConfig = string(""), string newParameterSet = string(""));
//void MultiObjective(int nb = 0,string chosenConfig = string(""), string newParameterSet = string(""));

enum{ ANA_CANO = 1, ANA_PERTURB = 2, ANA_DENS = 4, ANA_TITR = 8, ANA_PARAM_AROUND = 16, ANA_PARAM_SCAN = 32, ANA_CYCLO = 64, ANA_BACKGR = 128, ANA_PERTURB20 = 256, ANA_ALL = ANA_CANO | ANA_PERTURB | ANA_DENS | ANA_TITR | ANA_PARAM_AROUND | ANA_PARAM_SCAN | ANA_CYCLO | ANA_BACKGR | ANA_PERTURB20};
//void TotalAnalysis(int ChosenAnalysis = -1, string exportFolder = string(""), string chosenSet = string(""));
//void reAnalyzeFolder(int ChosenAnalysis, string _folder, bool includingSubFolders);

/// @brief Define here the working folder to open/write files, where The project file is is.
//#define folder string("C:/Users/Philippe/Desktop/Work/RestartV2016/Sources/TasosLMajor/")
//#define defaultfolder string("/home/phr13/Archeopteryx/2016-11-23/Sources/TasosLMajor/")
//#define defaultfolderBaseResults string("/home/phr13/Archeopteryx/2016-11-23/Results/")
string folder;
string folderBaseResults;


/// @brief standardization of the possible options for optimizing.
enum typeOptimizer {GeneticFast, SRESFast, Genetic25k, Genetic50k, Genetic100k, Genetic250k, Genetic500k, Genetic1M, SRES25k, SRES50k, SRES100k, SRES250k, SRES500k, SRES1M, GeneticAllCombs25k, GeneticAllCombs50k, GeneticAllCombs100k, GeneticAllCombs250k};
string optName(typeOptimizer toUse);
string optFileHeader(typeOptimizer toUse);
// To switch the optimizers in fast mode (just a few simulations).
#define TESTINGMODE 0




/// @brief Main : to get help, launch without any argument. Graphical window will open, and when quitted, all options will be displayed.
int main(int argc, char *argv[]){

    // ==================================== Initializing Qt ========================================
    #ifndef WITHOUT_QT
    QApplication b(argc, argv);     // Starts the Qt application
    #endif

    // ====================== Setting up the working folders automatically =========================
    // Note : the structure of the project should be : a "Sources" master directory, with all things inside, including the .pro file"
    folder = locateProjectDirectory(string("Examples.pro")) + string("Examples/");
    if (folder.size() == 0) { cout << "ERR: Working folders could not be auto-detected \n"; return 0; } // folder = defaultfolder;  folderBaseResults = defaultfolderBaseResults;  cout << " : \n -> " << folder << endl << " -> " << folderBaseResults << endl;
    else {folderBaseResults = getParentFolder(getParentFolder(folder)) + string("Results/");}
    createFolder(folderBaseResults); // in case it does not exist,
    cout << "Working folders were auto-detected to be : \n -> " << folder << endl << " -> " << folderBaseResults << endl;

    // ======================================= Manual commands =====================================
    if(false){
        {

            /*Modele* ML = new modeleLeishmania();
            ML->dt = 0.00001;
            Experiment* Exp = new expLMajor(ML);
            Exp->init();
            Exp->simulate(Small_Dose);*/


            scriptsWithMainExp(0);
            #ifndef WITHOUT_QT
            return b.exec();                // to leave the control to Qt instead of finishing the program
            #endif
        }
        return 0;
    }

    // =============== Launcher (from command line options or launch GUI) ==========================
    switch(argc){
        case 0: case 1: {       // ================= No arguments : launch the graphical interface -> manual choice ===================

            string exeName = removeFolderFromFile(string(argv[0]));
            cout << "\n   Welcome !\n" << endl;
            cout << "   -> No option chosen from command line ..." << endl;
            cout << "   -> For information, here are command line options to use the program :\n" << endl;

            cout << "\n\n ---------------------------- Canonical Differentiation -----------------------------------\n" << endl;
            cout << "Syntax 1 (2/4/6 args): "  << exeName << " NumScript " << endl;
            //cout << "variants : "  << exeName << " NumScript    -set       parameterSetFile.txt" << endl;
            //cout << "or       : "  << exeName << " NumScript    -config    configFile.txt" << endl;
            //cout << "or both" << endl;
            //scriptsWithMainExp(-1);             // to print all options

            /*cout << "\n\n ---------------------Predictions from all kinds of experimlents --------------------------\n" << endl;
            cout << "Syntax 2 (3/5/7 args): "  << argv[0] << " NumScript   parameter" << endl;
            cout << "variants : "  << exeName << " NumScript    parameter    -set       parameterSetFile.txt" << endl;
            cout << "or       : "  << exeName << " NumScript    parameter    -config    configFile.txt" << endl;
            cout << "or both" << endl;
            scriptsWithPerturbations(-1, 0);    // to print all options

            cout << "\n\n --------------------Making a PDF report with all the predictions--------------------------\n" << endl;
            cout << "Syntax 3 (2 args): " << exeName << " total" << endl;

            cout << "\n\n --------------------Fittings all data together from changing cytokines at 20 hours--------------------------\n" << endl;
            cout << "Syntax 4 (2 args): " << exeName << " 20hours" << endl;

            cout << "\n\n --------------------Multiobjective Fittings from changing cytokines at 20 hours--------------------------\n" << endl;
            cout << "Syntax 6 (3 args): " << exeName << " multi    NumScript" << endl;
            MultiObjective(-1);                 // to print all options

            cout << "\n\n\n\n";

            #ifndef WITHOUT_QT      // If graphical interface is allowed in compiling -> launch a starter GUI
            cout << "   -> launching starting GUI to chose manually what to do ..." << endl;
            Starter* st = new Starter(folder);
            string configFileM0 = string("M3a-SimpleNoIL10/configForModeleSimpleNoIL10.txt");
            Modele* currentModelM0 = new modeleSimpleNoIL10();
            string configFileM1 = string("M3c-MinNoIL10/configForModeleMinNoIL10.txt");
            Modele* currentModelM1 = new modeleMinNoIL10();
            string configFileM2 = string("M4-MinLatent/configForModeleMinLatent.txt");
            Modele* currentModelM2 = new modeleMinLatent();
            string configFileM3 = string("M5-MinLatentTbet/configForModeleLatentTbet.txt");
            Modele* currentModelM3 = new modeleLatentTbet();
            string configFileM4 = string("M6a-LatentTbet2/BestConfigSoFar.txt"); //configLatentTbet2NonOverlap.txt"); //UnBonSetTotalConfig.txt");                 //configFile = string("GoodCombManualTbetGata3ForLatent2NoIL250pourcents.txt");
            Modele* currentModelM4 = new modeleLatentTbet2();
            st->addModel(currentModelM0->name, currentModelM0, configFileM0);
            st->addModel(currentModelM1->name, currentModelM1, configFileM1);
            st->addModel(currentModelM2->name, currentModelM2, configFileM2);
            st->addModel(currentModelM3->name, currentModelM3, configFileM3);
            st->addModel(currentModelM4->name, currentModelM4, configFileM4);
            st->setDefaultModel(currentModelM4->name);
            st->exec();
            cout << "   -> Leaving GUI\n" << endl;
            cout << " ==================================================================================================== \n" << endl << endl;

            pair<int,float> choice = st->getFinalChoice();
            if(choice.first >= 0){
                if(choice.second < 0) {
                    scriptsWithMainExp(choice.first);
                    cout << "Line Command for this :\n" << argv[0] << " " << choice.first << endl;
                } else {
                    scriptsWithPerturbations(choice.first, choice.second);
                    cout << "Line Command for this :\n" << argv[0] << " " << choice.first << " " << choice.second << endl;
                }
            }
            if(choice.first == -2){
                MultiObjective(-1);
            }*/

            #ifndef WITHOUT_QT

           // buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok /*| QDialogButtonBox::Cancel*/);

            QDialogButtonBox* Choser = new QDialogButtonBox(QDialogButtonBox::Ok);
            QObject::connect(Choser, SIGNAL(accepted()), Choser, SLOT(accept()));
            //QObject::connect(buttonBox, SIGNAL(rejected()), buttinBox, SLOT(reject()));
            Choser->setGeometry(100,100,500,200);
            QComboBox* QCB = new QComboBox(Choser);
            QStringList options;
            options << "0 - Logistic growth, dataset 1"
                    << "1 - Logistic growth, dataset 2"
                    << "2 - Lokta Volterra model"
                    << "3 - 3-step population development, no feedback"
                    << "4 - 3-step population development, with feedback"
                    << "5 - Influenza dynamics";
            QCB->addItems(options);
            QCB->setGeometry(70,50,300,40);

            Choser->show();

            QEventLoop loop;
            QObject::connect(Choser, SIGNAL(accepted()), &loop, SLOT(quit()));
            loop.exec();

            Choser->hide();
            scriptsWithMainExp(QCB->currentIndex());
            return b.exec();                // to leave the control to Qt instead of finishing the program
            cerr << "Finished" << endl;
            delete Choser;
            delete QCB;
            #endif
            break;}

        case 2: {
            /*if(!string(argv[1]).compare(string("total"))) {TotalAnalysis(); return 0;}
            if(!string(argv[1]).compare(string("20hours"))) {scriptsWith20HoursExp(-1);
                #ifndef WITHOUT_QT
                return b.exec();                // to leave the control to Qt instead of finishing the program
                #endif
            }*/
            scriptsWithMainExp(atoi(argv[1]));
            if(atoi(argv[1]) <= 9){
                #ifndef WITHOUT_QT
                return b.exec();                // to leave the control to Qt instead of finishing the program
                #endif
            }
            break;}
       /* case 3: {
            if(!string(argv[1]).compare(string("multi"))) {MultiObjective(atoi(argv[2])); return 0;}
            scriptsWithPerturbations(atoi(argv[1]), (double) atof(argv[2])); break;}
        case 4: case 6:{
            string chosenConfig;
            string chosenSet;
            if(!string(argv[2]).compare(string("-set"))) chosenSet = string(argv[3]);
            if(!string(argv[2]).compare(string("-config"))) chosenConfig = string(argv[3]);
            if(argc == 6){
                if(!string(argv[4]).compare(string("-set"))) chosenSet = string(argv[5]);
                if(!string(argv[4]).compare(string("-config"))) chosenConfig = string(argv[5]);
            }
            if(chosenSet.find("/")!=string::npos) chosenSet = folder + chosenSet;
            if(chosenConfig.find("/")!=string::npos) chosenConfig = folder + chosenConfig;
            cout << "   => Chosen Options :\n";
            if(chosenSet.size() > 0) cout << "-set=" << chosenSet << endl;
            if(chosenConfig.size() > 0) cout << "-config=" << chosenConfig << endl;
            scriptsWithMainExp(atoi(argv[1]), chosenConfig, chosenSet);
            break;
        }
        case 5: case 7:{
        string chosenConfig;
            string chosenSet;
            if(!string(argv[3]).compare(string("-set"))) chosenSet = string(argv[4]);
            if(!string(argv[3]).compare(string("-config"))) chosenConfig = string(argv[4]);
            if(argc == 7){
                if(!string(argv[5]).compare(string("-set"))) chosenSet = string(argv[6]);
                if(!string(argv[5]).compare(string("-config"))) chosenConfig = string(argv[6]);
            }
            if(chosenSet.find("/")!=string::npos) chosenSet = folder + chosenSet;
            if(chosenConfig.find("/")!=string::npos) chosenConfig = folder + chosenConfig;
            cout << "   => Chosen Options :\n";
            if(chosenSet.size() > 0) cout << "-set=" << chosenSet << endl;
            if(chosenConfig.size() > 0) cout << "-config=" << chosenConfig << endl;
            scriptsWithPerturbations(atoi(argv[1]), (double) atof(argv[2]), string("CmdLineAnalysisPert") + string(argv[1]), string(""), chosenConfig, chosenSet);
            break;
        }*/
    default:{}
    }

    return 0;
}






















//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///
///         1 - Scripts for canonical differentiation. Can do all fittings and identifiability/sensitivity
///
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void scriptsWithMainExp(int nb,string newConfigFile, string newParameterSet){


    cout << " -> Launching scriptsWithMainExp(" << nb << ")" << endl;
    if(newConfigFile.size() > 0)   cout << "      forcing configuration : " << newConfigFile << endl;
    if(newParameterSet.size() > 0) cout << "      forcing parameter set : " << newParameterSet << endl;

    // ======= Loading data for canonical differentiations into TableCourses and overriders ========

    /*TableCourse* lm   = new TableCourse(folder + string("DATA/lm.txt"));
    vector<TableCourse*> kinetics = {lm};   // note : this syntax requires -std=c++11 in the compiler options
    int nbDataSets = kinetics.size();

    bool useSplines = false;                // false = linear interpolation, true = cubic splines
    overrider* Overlm = new overrider();    // Note : never create overrider as a non pointer, to be used by the graphical interface because they will be erased when function closes and gives control to the interface --> use a pointer and new ...
    vector<overrider*> overs = {Overlm};    // saves the overriders and kinetics in a vector so it is easy to get them in a loop by kinetics[i]...
    //*/

    TableCourse* Data_new = NULL;
    TableCourse* Data_mutant = NULL;

    switch(nb){
        case 0: {Data_new = new TableCourse(folder + string("DATA/LogisticDataset1.txt")); break;}
        case 1: {Data_new = new TableCourse(folder + string("DATA/LogisticDataset2.txt")); break;}
        case 2: {Data_new = new TableCourse(folder + string("DATA/LotkaVolterraDataset.txt")); break;}
        case 3: case 4: {
        Data_new = new TableCourse(folder + string("DATA/DevelopmentRealWT.txt"));
        Data_mutant =  new TableCourse(folder + string("DATA/DevelopmentRealMutant.txt")); break;}
        case 5: {Data_new = new TableCourse(folder + string("DATA/InfluenzaDataset.txt")); break;}

    }
    vector<TableCourse*> kinetics = {Data_new};   // note : this syntax requires -std=c++11 in the compiler options
    if((nb == 4) || (nb == 3)) kinetics.push_back(Data_mutant);
    int nbDataSets = kinetics.size();

    /*TableCourse* DataStd_new = new TableCourse(folder + string("DATA/DataStd_new.txt"));
    vector<TableCourse*> kineticsStd = {DataStd_new};   // note : this syntax requires -std=c++11 in the compiler options
    int nbDataSets = kineticsStd.size();*/


    bool useSplines = false;                // false = linear interpolation, true = cubic splines
    overrider* OverData_new = new overrider();    // Note : never create overrider as a non pointer, to be used by the graphical interface because they will be erased when function closes and gives control to the interface --> use a pointer and new ...
    vector<overrider*> overs = {OverData_new};    // saves the overriders and kinetics in a vector so it is easy to get them in a loop by kinetics[i]...

    overrider* OverData_mutant = new overrider();    // Note : never create overrider as a non pointer, to be used by the graphical interface because they will be erased when function closes and gives control to the interface --> use a pointer and new ...
    if((nb == 4) || (nb == 3)) overs.push_back(OverData_mutant);
    //*/

    // ======= Reading the data and interpolating curves into the overriders =====================

    vector<string> GlobalNamesVariables = getGlobalNames();                             // vector saying how to read the names of variables in the kinetics files (v[N::IL2] = 'gIL2', ...)
    for(int ne = 0; ne < nbDataSets; ++ne){                                             //  converts the name of variables (ex : 'gIL2') into their global index (ex: N::IL2)
        for(int i = 0; i < kinetics[ne]->nbVar; ++i){
            string ss = kinetics[ne]->headers[i+1];     // header of this variable

            // converting 'name in kinetics file ("gIL2", ...) --> index of variable (index = N::IL2 in the enum)' ======
            int GlobName = -1;
            for(int j = 0; j < (int) GlobalNamesVariables.size(); ++j){
                if(!GlobalNamesVariables[j].compare(ss)) GlobName = j;
            }

            // giving the data to overriders for interpolation (splines or linear) ======
            if(GlobName > -1){
                overs[ne]->learnSpl(GlobName,kinetics[ne]->getTimePoints(i), kinetics[ne]->getTimeCourse(i), useSplines);
            } else cout << "Variable " << ss << " not found in the kinetics\n";
        }
    }
      //overrider* OverData_new;
     //OverData_new->setOver(0, true);
    //cerr << "Heeee";
    //cerr << OverData_new->print();
    //return;

   // ======= Creating the model depending on the options ========

    string configFile = string("configLMajor.txt");
    if(newConfigFile.length() > 0) configFile = newConfigFile;

    Modele* currentModel = NULL;
    switch(nb){
        case 0: case 10: { currentModel = new modeleLogistic();
        configFile=string("configLogistic.txt");
        break;}
        case 1: case 11: { currentModel = new modeleLogistic();
        configFile=string("configLogistic.txt");
        break;}
        case 2: case 12: { currentModel = new modeleLotkaVolterra();
        configFile=string("configLotkaVolterra.txt");
        break;}
        case 3: case 13: { currentModel = new modeleDevelopmentNoFeedback();
        configFile=string("configDvpNoFeedback.txt");
        break;}
        case 4: case 14: { currentModel = new modeleDevelopment();
        configFile=string("configDvpWithFeedback.txt");
        break;}
        case 5: case 15: { currentModel = new modeleInfluenza();
        configFile=string("configInfluenza.txt");
        break;}
        /*case 5: case 15: { currentModel = new model6();
        configFile=string("configLMajorM6.txt");
        break;}*/
        }
    // just printing
    cout << "   -> Using model : " << currentModel->name << " for canonical differentiation\n   -> performing script nr(" << nb << ")\n";
    if(newConfigFile.length() > 0)   cout << "      ... with configuration (" << newConfigFile << ")" << endl;
    if(newParameterSet.length() > 0) cout << "      ... with parameter set (" << newParameterSet << ")" << endl;

    Experiment* currentExperiment = NULL;
    switch(nb){
        case 0: case 10: case 1: case 11: case 2: case 12: case 5: case 15: {
            currentExperiment = new expOneKin(currentModel);
            break;
        }
        case 4: case 14: case 3: case 13:{
            currentExperiment = new expWithMutant(currentModel);
            break;
        }
    }

     // ======= Giving data to the evaluators of the experiment class (for getting a cost later & knowing what to record) ========

    currentExperiment->giveData(Data_new, WT_EXP);
    if((nb == 4) || (nb == 3)) currentExperiment->giveData(Data_mutant, MUTANT_EXP);
    //currentExperiment->giveStdDevs(DataStd_new, Small_Dose);

    currentExperiment->giveHowToReadNamesInKinetics(GlobalNamesVariables);
    currentExperiment->loadEvaluators();


    // ======= and giving the overriders to the experiment class to replace wanted curves by data ========

    currentExperiment->setOverrider(WT_EXP, OverData_new);
    if((nb == 4) || (nb == 3))  currentExperiment->setOverrider(MUTANT_EXP, OverData_mutant);


    // =============== Scripts 0, 1, 2, 3 & 4 : no optimization, only launches the GUI ================

    if(nb < 10 ){
        #ifndef WITHOUT_QT
        simuWin* p = new simuWin(currentExperiment);
        cout << "Launching Graphical Interface ..." << endl;
        p->loadConfig(folder + configFile);
        if(newParameterSet.length() > 0) currentModel->loadParameters(newParameterSet);
        p->show();
        #else
        cout << "Script finished (without qt, because WITHOUT_QT was defined)\n";
        #endif
        return;
    }



}









































//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///
///         5 - Tool functions
///
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




void mergePDFs(vector<string> & listFiles, string outputFile){
    cout << "=> Merging files into one PDF ...\n";
    ofstream st(outputFile, ios::out);
    st << "\\documentclass{article}% or something else\n";
    st << "\\usepackage{pdfpages}\n";
    st << "\\begin{document}\n";
    for(int kf = 0; kf < (int) listFiles.size(); ++kf){
        st << "\\includepdf[pages=-]{" << listFiles[kf] << "}\n";
    }
    st << "\\end{document}\n";
    st.close();
    compileLatex(folderBaseResults, outputFile);
}

string textFileForFolder(string explanations){
    replace( explanations.begin(), explanations.end(), '_', '-');
    static int count = 0;
    string picOpt = string("height=1.4in");
    stringstream st;
    st << "\\documentclass[10pt,a4paper,final]{article}\n";
    st << "\\usepackage[utf8]{inputenc}\n";
    st << "\\usepackage{amsmath}\n";
    st << "\\usepackage{amsfonts}\n";
    st << "\\usepackage{amssymb}\n";
    st << "\\usepackage{graphicx}\n";
    st << "\\usepackage{caption}\n";
    st << "\\usepackage{subcaption}\n";
    //st << "\\usepackage[dvips]{epsfig, graphicx, color}\n";
    st << "\\usepackage[left=1.5cm,right=1.5cm,top=1.5cm,bottom=1.5cm]{geometry}\n";
    //st << "\\title{How to simulate a Germinal Centre}\n";
    //st << "\\date{\vspace{-5ex}}\n";
    //st << "\\usepackage[charter]{mathdesign}\n"; // not installed in brics
    st << "\\begin{document}\n";
    st << "\\setcounter{figure}{" << count << "}\n";

    //st << explanations << endl;
    /*st << "\\begin{figure}[]\n";
    st << "\\begin{center}\n";
    st << "\\begin{subfigure}[b]{0.45\\textwidth}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-TBET.png}\n";
    st << "\\end{subfigure}\n";
    st << "\\begin{subfigure}[b]{0.45\\textwidth}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-TBETmRNA.png}\n";
    st << "\\end{subfigure}\n";
    st << "\\begin{subfigure}[b]{0.45\\textwidth}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-GATA3.png}\n";
    st << "\\end{subfigure}\n";
    st << "\\begin{subfigure}[b]{0.45\\textwidth}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-GATA3mRNA.png}\n";
    st << "\\end{subfigure}\n";
    st << "\\begin{subfigure}[b]{0.45\\textwidth}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-FOXP3.png}\n";
    st << "\\end{subfigure}\n";
    st << "\\begin{subfigure}[b]{0.45\\textwidth}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-FOXP3mRNA.png}\n";
    st << "\\end{subfigure}\n";
    st << "\\begin{subfigure}[b]{0.45\\textwidth}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-RORGT.png}\n";
    st << "\\end{subfigure}\n";
    st << "\\begin{subfigure}[b]{0.45\\textwidth}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-RORGTmRNA.png}\n";
    st << "\\end{subfigure}\n";
    st << "\\caption[]{}\n";
    st << "\\end{center}\n";
    st << "\\end{figure}\n";*/

    st << "\\begin{figure}[ht!]\n";
    //st << "\\captionsetup{labelformat=empty}\n";
    st << "\\caption{" << explanations << "}\n";
    st << "\\begin{center}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-TBET.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-GATA3.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-RORGT.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-TBETmRNA.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-GATA3mRNA.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-RORGTmRNA.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-FOXP3.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-IFNG.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-IL4.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-FOXP3mRNA.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-IFNGmRNA.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-IL4mRNA.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-IL21.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-IL17.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-TGFB.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-IL21mRNA.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-IL17mRNA.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-TGFBmRNA.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-IL12.png}\n";
    //st << "\\includegraphics[" << picOpt << "]{Unsim-IL6.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-IL2mRNA.png}\n";
    st << "\\includegraphics[" << picOpt << "]{Unsim-IL2.png}\n";
    st << "\\end{center}\n";
    st << "\\end{figure}\n";
    st << "\\clearpage\n";


    st << "\\end{document}\n";
    count++;
    return st.str();
}




string codeTime(){
    time_t now = time(0);
    stringstream code;
    tm *ltm = localtime(&now);
    code << ltm->tm_mday;
    code << "-"<< 1 + ltm->tm_mon;
    code << "-"<< 1900 + ltm->tm_year;
    code << "at"<< 1 + ltm->tm_hour << "-";
    code << 1 + ltm->tm_min << "-";
    code << 1 + ltm->tm_sec;
    return code.str();
}





/* ---------- Informations about a text file : --------
/home/phr13/Desktop/Restart/RV2/R1/D7 simu Tbet InitFixed.txt
// ------------------ Content of file : ---------------
151	26	23
2.5	1	10	.	1	0	0	0	0	1	1	0	0	1	0	0	0	0	0	1	0	0	0	0	0	0	0
0.005	0.0001	0.2	.	1	0	0	0	0	1	1	0	0	1	0	0	0	0	0	1	0	0	0	0	0	0	0
2500	100	10000	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	1	0
8500	1.1	10000	.	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0
2000	100	10000	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
1000	1.1	10000	.	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	1	0	0
15000	1.1	20000	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
1e-05	1e-06	0.0001	.	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0
1e-05	1e-06	0.0001	.	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0
5e-06	1e-06	0.0001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
5e-06	1e-06	0.0001	.	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0
1e-07	1e-07	0.0001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0
1e-07	1e-07	0.0001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0
1e-05	1e-06	0.0001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1.5e-05	1e-05	0.0001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
2e-05	1e-05	0.0001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	1	0
7e-05	1e-05	0.0001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0
4.9455e-05	1e-05	0.0001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1
1.3e-05	2e-06	5e-05	.	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0
1e-05	2e-06	5e-05	.	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1e-05	2e-06	5e-05	.	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
5e-05	2e-06	5e-05	.	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.0001	2e-06	5e-05	.	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.0002	2e-06	5e-05	.	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	1	0	0
0.0005	2e-06	5e-05	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
0.0001	2e-06	5e-05	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
3e-05	2e-06	5e-05	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1e-05	2e-06	5e-05	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.0003	1e-06	0.001	.	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0
2e-06	1e-06	0.001	.	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0
0.0004	1e-06	0.001	.	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0
1.5e-06	1e-06	0.001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0
6e-06	1e-06	0.001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0
5e-07	1e-07	0.001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1.8e-05	1e-06	0.001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
3e-05	1e-06	0.001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	1	0
8e-05	1e-06	0.001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0
9.10135e-05	1e-06	0.001	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1
0.00025	1e-05	1	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1e-05	1e-05	1	.	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0
0.01	1e-06	0.9	.	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0
10	0.01	1	.	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0
2.5	0.25	4	.	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0
1e-06	1e-05	1	.	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
100	1.1	0.1	.	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
2.1	0.01	1	.	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
2	0.25	4	.	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1e-06	1e-05	1	.	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
80	1.1	0.1	.	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
6	0.01	1	.	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
4	0.25	4	.	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1e-05	1e-05	1	.	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
10	1.1	0.1	.	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
2	0.01	1	.	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
3	0.25	4	.	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1e-05	1e-05	1	.	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
100	1.1	0.1	.	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1	0.01	1	.	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
2	0.25	4	.	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1e-05	1e-05	1	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
3.5	1.1	10	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
1.6	1.1	0.1	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
2	0.01	1	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
1	0.25	4	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
250	1.1	0.1	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
0.2	0.01	1	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
2	0.25	4	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
0.005	1e-06	0.9	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
0.48	0.01	1	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
4	0.25	4	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
1	1e-06	0.9	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
5	0.01	1	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
2	0.25	4	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
0.1	1e-06	0.9	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
5	0.01	1	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
2	0.25	4	.	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
1e-06	1e-05	1	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
5	1.1	0.1	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
40	0.01	1	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
1.2	0.25	4	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
30	1.1	0.1	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
8	0.01	1	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
1.2	0.25	4	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
0.1	1e-06	0.9	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
5	0.01	1	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
1	0.25	4	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
40	1.1	0.1	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
1.2	0.01	1	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
4	0.25	4	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
0.1	1e-06	0.9	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
5	0.01	1	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
2	0.25	4	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
0.5	1e-06	0.9	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
2	0.01	1	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
1.2	0.25	4	.	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
5e-06	1e-05	1	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
6	1.1	10	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
2	1.1	0.1	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1.2	0.01	1	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.9	0.25	4	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
15	1.1	0.1	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.169	0.01	1	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
3	0.25	4	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.5	1e-06	0.9	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
5	0.01	1	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
2	0.25	4	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.2	1e-06	0.9	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
5	0.01	1	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
2	0.25	4	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.01	1e-06	0.9	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1	0.01	1	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
2	0.25	4	.	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1e-05	1e-05	1	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
3	1.1	0.1	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
45	0.01	1	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1	0.25	4	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
25	1.1	0.1	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.4	0.01	1	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
3	0.25	4	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1e-06	1e-06	0.9	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.01	0.01	1	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.25	0.25	4	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1e-06	1e-06	0.9	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
2	0.01	1	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
3	0.25	4	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1e-06	1e-06	0.9	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1	0.01	1	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
3	0.25	4	.	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1e-05	1e-05	0.25	.	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0	0
1e-05	1e-05	0.25	.	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0	0
1e-05	1e-05	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1e-05	1e-05	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0	0
1e-05	1e-05	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0	0
1e-05	1e-05	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	0
1e-05	1e-05	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
1e-05	1e-05	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.416	0.416	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	1	0
1e-06	1e-06	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0
0.0433151	0.04	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1
0.0094	0.0094	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0
0.0165	0.0165	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.00016	0.00016	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.034	0.034	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.041	0.041	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.97	0.97	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0
0.015	0.015	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0
1.36	1.36	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	1	0	0	0
0.032	0.032	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
0.213	0.213	0.25	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
14.6	12	16	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
19.05	18	20	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
.	.	.	.	1	1	1	1	1	1	1	1	1	1	0	1	1	1	1	1	1	1	1	1	1	1	1
.	.	.	.	1	1	1	1	1	1	1	1	1	1	1	0	1	1	1	1	1	1	1	1	1	1	1
.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.
.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.
.	.	.	.	1	1	1	1	1	1	1	1	1	1	1	1	0	1	1	1	1	1	1	1	1	1	1
.	.	.	.	1	1	1	1	1	1	1	1	1	1	1	1	1	0	1	1	1	1	1	1	1	1	1
.	.	.	.	1	1	1	1	1	1	1	1	1	1	1	1	1	1	0	1	1	1	1	1	1	1	1
.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.
.	.	.	.	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0
.	.	.	.	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	0	1	1	1	1	1	0	1
.	.	.	.	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	0	1	1	1	1	1	1
.	.	.	.	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	0
.	.	.	.	0	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	0	1	1	1	1	1
.	.	.	.	1	0	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1
.	.	.	.	1	1	0	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1
.	.	.	.	1	1	1	0	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1
.	.	.	.	1	1	1	1	0	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1
.	.	.	.	1	1	1	1	1	1	1	1	1	0	1	1	1	1	1	1	1	1	1	1	0	1	1
.	.	.	.	1	1	1	1	1	0	1	1	1	1	1	1	1	1	1	1	1	1	0	1	1	1	1
.	.	.	.	1	1	1	1	1	1	0	1	1	1	1	1	1	1	1	1	1	1	1	0	1	1	1
.	.	.	.	1	1	1	1	1	1	1	0	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1
.	.	.	.	1	1	1	1	1	1	1	1	0	1	1	1	1	1	1	1	1	1	1	1	1	1	1
.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.
.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.
.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.
.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.	.
// ---------------------------------------------------- */


void compileLatex(string folderRes, string texFile){
    ofstream action (folderRes + string("doPDF.bat"), ios::out);
    action << string("cd ") + folderRes + string("\npdflatex -interaction=nonstopmode ") + texFile + string(" > resLatexCompil.txt\n");
    action.close();
    #ifdef WINDOWS
    system((folderRes + string("doPDF.bat\n")).c_str());
    #endif
    #ifdef UNIX
    system((string("cd ") + folderRes + string("\nchmod +x doPDF.bat\n./doPDF.bat > resLatexCompil.txt \n")).c_str());
    #endif
}








/*
expFromTh17* currentExperiment = new expFromTh17(currentModel, 0.8);


#ifndef WITHOUT_QT
    simuWin* p = new simuWin(currentExperiment);
    p->show();
    cout << "Launch GUI ..." << endl;
    return b.exec();
#endif

    std::vector<int> v  {1,2,3};


    // ====== Manually overriding some of the variables. Can be done more easily by overriding using a combination ======

        bool overrideRNAs = false;
        bool overrideCytok = true;
        bool overrideTF = true;
        vector<int> VTO; // Vars to override
        if(overrideRNAs){
            VTO.push_back(N::FOXP3mRNA);
            VTO.push_back(N::GATA3mRNA);
            VTO.push_back(N::IFNGmRNA);
            VTO.push_back(N::IL4mRNA);
            VTO.push_back(N::IL21mRNA);
            VTO.push_back(N::IL2mRNA);
            VTO.push_back(N::IL17mRNA);
            VTO.push_back(N::RORGTmRNA);
            VTO.push_back(N::TGFBmRNA);
            VTO.push_back(N::TBETmRNA);
        }
        if(overrideCytok){
            VTO.push_back(N::IFNG);
            VTO.push_back(N::IL4);
            VTO.push_back(N::IL21);
            VTO.push_back(N::IL2);
            VTO.push_back(N::IL17);
            //VTO.push_back(N::TGFB);
        }
        if(overrideTF){
            VTO.push_back(N::FOXP3);
            VTO.push_back(N::GATA3);
            VTO.push_back(N::RORGT);
            //VTO.push_back(N::TBETmRNA);
        }
        for(int i = 0; i < (int) VTO.size(); ++i){
            OverTh1->setOver(VTO[i], true);
            OverTh2->setOver(VTO[i], true);
            OveriTreg->setOver(VTO[i], true);
            OverTh17->setOver(VTO[i], true);
            OverTh0->setOver(VTO[i], true);
        }
        currentExperiment->setOverrider(TH1,    &OverTh1);
        currentExperiment->setOverrider(TH2,    &OverTh2);
        currentExperiment->setOverrider(TREG,   &OveriTreg);
        currentExperiment->setOverrider(TH17,   &OverTh17);
        currentExperiment->setOverrider(TH0,    &OverTh0);



    //cout << generateActivFunction(0, 2);


    */




string optName(typeOptimizer toUse){
    switch (toUse){
    case GeneticFast: { return string("Genetic Algorithm, only a few rounds for tests"); break;}
    case SRESFast: { return string("SRES, only a few rounds for tests"); break;}
    case Genetic25k: { return string("Genetic Algorithm, 25 000 cost evaluations"); break;}
    case Genetic50k: { return string("Genetic Algorithm, 50 000 cost evaluations"); break;}
    case Genetic100k: { return string("Genetic Algorithm, 100 000 cost evaluations"); break;}
    case Genetic250k: { return string("Genetic Algorithm, 250 000 cost evaluations"); break;}
    case Genetic500k: { return string("Genetic Algorithm, 500 000 cost evaluations"); break;}
    case Genetic1M: { return string("Genetic Algorithm, 1 000 000 cost evaluations"); break;}
    case SRES25k: { return string("SRES, 25 000 cost evaluations"); break;}
    case SRES50k: { return string("SRES, 50 000 cost evaluations"); break;}
    case SRES100k: { return string("SRES, 100 000 cost evaluations"); break;}
    case SRES250k: { return string("SRES, 250 000 cost evaluations"); break;}
    case SRES500k: { return string("SRES, 500 000 cost evaluations"); break;}
    case SRES1M: { return string("SRES, 1 000 000 cost evaluations"); break;}
    case GeneticAllCombs25k: { return string("Genetic Algorithm, All Operators, 25 000 cost evaluations"); break;}
    case GeneticAllCombs50k: { return string("Genetic Algorithm, All Operators, 50 000 cost evaluations"); break;}
    case GeneticAllCombs100k: { return string("Genetic Algorithm, All Operators, 100 000 cost evaluations"); break;}
    case GeneticAllCombs250k: { return string("Genetic Algorithm, All Operators, 250 000 cost evaluations"); break;}
    default: {return string("");}
    }
    return string("");
}

string optFileHeader(typeOptimizer toUse){

    stringstream headerOptimizer;
    switch (toUse){
        case SRESFast:{
            headerOptimizer << "SRES	1   \n50\n";
            break;
        }
        case SRES25k:{
            headerOptimizer << "SRES	1   \n25000\n";
            break;
        }
        case SRES50k:{
            headerOptimizer << "SRES	1   \n50000\n";
            break;
        }
        case SRES100k:{
            headerOptimizer << "SRES	1   \n100000\n";
            break;
        }
        case SRES250k:{
            headerOptimizer << "SRES	1   \n250000\n";
            break;
        }
        case SRES500k:{
            headerOptimizer << "SRES	1   \n500000\n";
            break;
        }
        case SRES1M:        {
            headerOptimizer << "SRES	1   \n1000000\n";
            break;
        }

        default:{}
    }
    // finished for SRES

    bool TESTE_ALL_CROSSMUT = false;
    switch (toUse){
        case GeneticAllCombs25k:
        case GeneticAllCombs50k:
        case GeneticAllCombs100k:
        case GeneticAllCombs250k:   {
            TESTE_ALL_CROSSMUT = true;
            break;
        }
        default:{}
    }


    switch (toUse){
        case GeneticFast:
        case Genetic25k:
        case Genetic50k:
        case Genetic100k:
        case Genetic250k:
        case Genetic500k:
        case Genetic1M:
        case GeneticAllCombs25k:
        case GeneticAllCombs50k:
        case GeneticAllCombs100k:
        case GeneticAllCombs250k:   {
            headerOptimizer << "geneticAlgorithm	14\n";
            headerOptimizer << (TESTE_ALL_CROSSMUT ? "10         #CEP  - All Mut&Cross\n" : "0	#CEP  - Classical Evolutionary Programming\n");
            headerOptimizer << "8          #Proportional / From Worst / Basic Sampling\n";
            headerOptimizer << "7	1      #SBX Cross-Over\n";
            headerOptimizer << "1          #Mutation normal all points\n";
            headerOptimizer << "0          #NO_NEED_REPLACEMENT\n";
            headerOptimizer << "0          #Select Best\n";
            headerOptimizer << "7	0.005	#MUTATIVE_SEPARATED\n";
            headerOptimizer << "1      #Nb Repeats\n";
            break;
        }
        default:{}
    }

    switch (toUse){
        case GeneticFast:{
            headerOptimizer << "50	#Max nb of simulations-costs\n";
            headerOptimizer << "50	#Population Size\n";
            break;
        }
        case Genetic25k: case GeneticAllCombs25k:{
            headerOptimizer << "25000	#Max nb of simulations-costs\n";
            headerOptimizer << "250	#Population Size\n";
            break;
        }
        case Genetic50k: case GeneticAllCombs50k:{
            headerOptimizer << "50000	#Max nb of simulations-costs\n";
            headerOptimizer << "250	#Population Size\n";
            break;
        }
        case Genetic100k:  case GeneticAllCombs100k:{
            headerOptimizer << "100000	#Max nb of simulations-costs\n";
            headerOptimizer << "500	#Population Size\n";
            break;
        }
        case Genetic250k: case GeneticAllCombs250k:{
            headerOptimizer << "250000	#Max nb of simulations-costs\n";
            headerOptimizer << "750	#Population Size\n";
            break;
        }
        case Genetic500k:{
            headerOptimizer << "500000	#Max nb of simulations-costs\n";
            headerOptimizer << "1000	#Population Size\n";
            break;
        }
        case Genetic1M:{
            headerOptimizer << "1000000	#Max nb of simulations-costs\n";
            headerOptimizer << "2000	#Population Size\n";
            break;
        }
        default:{}
    }

    switch (toUse){
        case GeneticFast:
        case Genetic25k:
        case Genetic50k:
        case Genetic100k:
        case Genetic250k:
        case Genetic500k:
        case Genetic1M:
        case GeneticAllCombs25k:
        case GeneticAllCombs50k:
        case GeneticAllCombs100k:
        case GeneticAllCombs250k:   {
            headerOptimizer << "0.2	#Proportion of CrossOver (vs offspring) in new individuals\n";
            headerOptimizer << "0.5	#Fork coeff (%renewed each generation)\n";
            break;
        }
    default:{}
    }
    return headerOptimizer.str();

    /*headerOptimizer << "geneticAlgorithm	14\n";
    headerOptimizer << (TESTE_ALL_CROSSMUT ? "10         #CEP  - All Mut&Cross\n" : "0	#CEP  - Classical Evolutionary Programming\n");
    headerOptimizer << "8          #Proportional / From Worst / Basic Sampling\n";
    headerOptimizer << "7	1      #SBX Cross-Over\n";
    headerOptimizer << "1          #Mutation normal all points\n";
    headerOptimizer << "0          #NO_NEED_REPLACEMENT\n";
    headerOptimizer << "0          #Select Best\n";
    headerOptimizer << "7	0.005	#MUTATIVE_SEPARATED\n";
    headerOptimizer << "1      #Nb Repeats\n";
    headerOptimizer << "5000	#Max nb of simulations-costs\n";
    headerOptimizer << "250	#Population Size\n";
    headerOptimizer << "0.2	#Proportion of CrossOver (vs offspring) in new individuals\n";
    headerOptimizer << "0.5	#Fork coeff (%renewed each generation)\n";



    switch (toUse){
    case GeneticFast:   { break;}
    case SRESFast:      { break;}
    case Genetic25k:    { break;}
    case Genetic50k:    { break;}
    case Genetic100k:   { break;}
    case Genetic250k:   { break;}
    case Genetic500k:   { break;}
    case Genetic1M:     { break;}
    case SRES25k:       { break;}
    case SRES50k:       { break;}
    case SRES100k:      { break;}
    case SRES250k:      { break;}
    case SRES500k:      { break;}
    case SRES1M:        { break;}
    case GeneticAllCombs25k:    { break;}
    case GeneticAllCombs50k:    { break;}
    case GeneticAllCombs100k:   { break;}
    case GeneticAllCombs250k:   { break;}
    default:                    {return string("");}
    }

    */

}
