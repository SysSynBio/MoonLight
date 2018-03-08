Notes :
	05-Dec-2016, took sources back from BRICS


To compile in a new computer :

- using Qt :

in common.h, define your system :
#define QT4 or #define QT5
#define WINDOWS or #define UNIX
//#define WITHOUT_QT
#define USE_QWT or #define USE_QCP (plotting libraries, QWT is fast while QCP is more beautiful - Exporting bitmaps only with QCP)

delete the ui_....h, they should be generated automatically by qmake

with QtCreator, just open Optimize.pro, and launch the compilation

by command line, using qmake (from qt)
qmake Optimize.pro
make


- to run the code without using Qt

in common.h, define your system :
#define QT4 or #define QT5
#define WINDOWS or #define UNIX
#define WITHOUT_QT
#define USE_QWT or #define USE_QCP (plotting libraries, QWT is fast while QCP is more beautiful - Exporting bitmaps only with QCP)

Either you have to make your makefile manually (pain), 
the files to compile are listed in OptimizeNoQt.pro

or you can use qmake to create the makefile, but when the program will be run, the code is independent of qt
qmake OptimizeNoQt.pro
make