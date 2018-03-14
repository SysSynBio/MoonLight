#include "play.h"
#include <QApplication>

#include "generate.h"
#include "fstream"
using namespace std;

#include <QMainWindow>
#include "graphe2.h"
#include "Modeles/modele.h"
#include "Modeles/experiments.h"
#include "Modeles/modele110.h"
#include "Modeles/modele111.h"
#include "Modeles/modele112.h"
#include "Modeles/modele113.h"
#include "Modeles/modele114.h"
#include "Modeles/modele1v1.h"
#include "Modeles/modeleEssai.h"

#include "fitte.h"

int main(int argc, char *argv[])
{
    QApplication b(argc, argv);
    //MainWindow w;
    //w.show();
    

    Generate a = Generate("Essai");
    a.addVar("IL12", PROP_DEG, CST_FROM_EQ, "IL12");
    a.addVar("ST4P", PROP_DEG, CST_FROM_EQ, "STAT4P");
    a.addVar("TBET", PROP_DEG, CST_FROM_EQ, "TBET");
    a.addVar("IFNG", PROP_DEG, CST_FROM_EQ, "IFNG");

    a.addReaction("IL12", "ST4P", +1);
    a.addReaction("ST4P", "TBET", +1);
    a.addReaction("ST4P", "ST4P", +1);
    a.addReaction("TBET", "IFNG", +1);
    a.addReaction("IFNG", "TBET", +1);
    a.addReaction("IFNG", "IFNG", -1);
    a.addReaction("ST4P", "IFNG", +1);



    // enum D {NO_D, PROP_DEG, NB_DEG_POL};
    // enum B {NO_B, CST, CST_FROM_EQ, NB_BAS_POL};
    // Generate(string _MName);
    // void addVar(string nameVar, D degPolicy = PROP_DEG, B basalPolicy = CST_FROM_EQ, string globalName = string(""));
    // void addReaction(string Var1, string Var2, int IDfunction);


    //    vector<string> generateParameters();
    //    string generateActiv(int nbAct, int nbInh);
    //    string generateCodeHeader();
    //    string generateCodeSource();
    //    string generateEquations();
    //    string generateLatex();

    //  cerr << a.generateCodeHeader();
    //  cerr << a.generateCodeSource();
    //  ofstream fichier("C:\Users\parobert\Desktop\Optimisation\ThModeles\Modeles", ios::out);

    ofstream fichier("C:/Users/parobert/Desktop/Optimisation/ThModeles/Modeles/modeleEssai.h", ios::out);
    fichier << a.generateCodeHeader();
    fichier.close();

    ofstream fichier2("C:/Users/parobert/Desktop/Optimisation/ThModeles/Modeles/modeleEssai.cpp", ios::out);
    fichier2 << a.generateCodeSource();
    fichier2.close();


    /* to optimize withoug graphic mode */

    /*Modele* currentModel = new Modele110();
    Experiments* currentExperiment = new ExpTH1(currentModel);
     if(currentExperiment) {
         Optimize(string("C:/Users/parobert/Desktop/Optimisation/ThModeles/Modeles/M110.txt"), currentExperiment );
     }*/






    Play p;
    p.show();


    return b.exec();
}
