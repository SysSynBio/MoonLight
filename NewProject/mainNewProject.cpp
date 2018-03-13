// The file common.h checks the system and defines WINDOWS/UNIX and QT4/QT5.
#include "../common.h"

#include "../Interface/simuwin.h"
#include "../Interface/starter.h"

#include "namesNewProject.h"
#include "expsNewProject.h"
#include "Models/model1.h"

#include <vector>
#include <ctime>
using namespace std;


int main(int argc, char *argv[]){

    // From common.h, you can define whether you want to use graphical interface or not (#define WITHOUT_QT)
    #ifndef WITHOUT_QT
    QApplication b(argc, argv);     // Starts the Qt application

    // note: when launched from QtCreator, the running folder is the buildxxx generated folder. Might need to use ../ or absolute folder here.
    TableCourse* Data1 = new TableCourse(string("DATA/Data1.txt"));
    TableCourse* Data2 = new TableCourse(string("DATA/Data1.txt"));

    string configFile = string("configXXX.txt");

    Modele* currentModel = new model1();

    Experiment* currentExperiment = new expOne(currentModel);
    currentExperiment->giveData(Data1, EXP_KIN1);
    currentExperiment->giveData(Data2, EXP_KIN2);
    //currentExperiment->giveStdDevs(DataXX, EXP_XXX);
    currentExperiment->giveHowToReadNamesInKinetics(getGlobalNames());
    currentExperiment->loadEvaluators();

    simuWin* p = new simuWin(currentExperiment);
    cout << "Launching Graphical Interface ..." << endl;
    p->loadConfig(configFile);
    p->show();


    return b.exec();                // to leave the control to Qt instead of finishing the program
    #endif


    // if no Qt defined
    return 0;
}








//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///
///         5 - Tool functions (in case of interest)
///
///
///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum typeOptimizer {GeneticFast, SRESFast, Genetic25k, Genetic50k, Genetic100k, Genetic250k, Genetic500k, Genetic1M, SRES25k, SRES50k, SRES100k, SRES250k, SRES500k, SRES1M, GeneticAllCombs25k, GeneticAllCombs50k, GeneticAllCombs100k, GeneticAllCombs250k};

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

    /* example of optimizer file
    headerOptimizer << "geneticAlgorithm	14\n";
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
    */

}


/* I/O functions
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

/// @brief tool functions implemented below
string codeTime();                                              /// @brief a function to give a different name each time called (based on the time)
string textFileForFolder(string explanations);                  /// @brief generates the .tex to analyze a folder with results (pictures)
void compileLatex(string folderRes, string texFile);            /// @brief ask the system to compile a tex file and put the result in folderRes
void mergePDFs(vector<string> & listFiles, string outputFile);  /// @brief merges PDF files into one with a new name
string removeFolderFromFile(string file);                       /// @brief keeps only the file name (and not folder)


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

    st << "\\begin{figure}[ht!]\n";
    //st << "\\captionsetup{labelformat=empty}\n";
    st << "\\caption{" << explanations << "}\n";
    st << "\\begin{center}\n";
    st << "\\includegraphics[" << picOpt << "]{XXX.png}\n";
    st << "\\includegraphics[" << picOpt << "]{XXX.png}\n";
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


*/
