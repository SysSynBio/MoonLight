#-------------------------------------------------------------------#
#                                                                   #
#       Project created by QtCreator 2014-09-01T15:12:07            #
#       DO NOT GENERATE AGAIN !!! it was manually modified          #
#                                                                   #
#-------------------------------------------------------------------#

# QT += widgets or core gui are necessary for the production of the ui_....h automatically.
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

#this options are for the use of QWT library
#QT += printsupport
#QT += opengl
#QT += svg

# ======================== BOOST PART ==================================
# In order to use boost (need to be downloaded and unzipped wherever)
win32: INCLUDEPATH += C:\Qt\Boost\boost_1_62_0\boost_1_62_0
#in windows, boosts raises the following error : "boost unable to find numeric literal operator operator Q" => need to add the following option
win32: QMAKE_CXXFLAGS += -fext-numeric-literals
# to avoid warnings from boost library files (wtf !)
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs -Wreorder -Wno-unused-parameter
# Necessary only for boost - would also be necessary for the syntax vector<int> bla = {1, 2, 3};
QMAKE_CXXFLAGS += -std=c++11

#name of the executable file generated
TARGET = ExamplesGradCourse

#put += console to run in a separate terminal
#CONFIG += console

# note: to allow multiple launching of the code from QtCreator, you might need to go to
# QTcreator preferences (Tools->Options, or File->preferences), Build & Run panel, Choose 'Stop Applications before building: None'

#bundles might be required for MAC OS ??
#CONFIG -= app_bundle

TEMPLATE = app

HEADERS += \
    common.h \
    Framework/evaluator.h \
    Framework/Experiment.h \
    Framework/generate.h \
    Framework/modele.h \
    Framework/overrider.h \
    Framework/spline.h \
    Framework/tableCourse.h \
    Framework/parameterSets.h \
    Extreminator/common/myFiles.h \
    Extreminator/common/myRandom.h \
    Extreminator/common/myTimes.h \
    Extreminator/common/statistiques.h \
    Extreminator/baseProblem.h \
    Extreminator/include/multimedia/AudioDefs.hpp \
    Extreminator/include/multimedia/VideoDefs.hpp \
    Extreminator/include/blas.h \
    Extreminator/include/blascompat32.h \
    Extreminator/include/covrt.h \
    Extreminator/include/emlrt.h \
    Extreminator/include/engine.h \
    Extreminator/include/fintrf.h \
    Extreminator/include/io64.h \
    Extreminator/include/lapack.h \
    Extreminator/include/mat.h \
    Extreminator/include/matrix.h \
    Extreminator/include/mex.h \
    Extreminator/include/mwmathutil.h \
    Extreminator/include/tmwtypes.h \
    Extreminator/optimizers/SRES/ESES.h \
    Extreminator/optimizers/SRES/ESSRSort.h \
    Extreminator/optimizers/SRES/sharefunc.h \
    Extreminator/optimizers/baseOptimizer.h \
    Extreminator/optimizers/CMAES.h \
    Extreminator/optimizers/Config.h \
    Extreminator/optimizers/Genetic.h \
    Extreminator/optimizers/GeneticGeneral.h \
    Extreminator/optimizers/GradientDescent.h \
    Extreminator/optimizers/individual.h \
    Extreminator/optimizers/MultipleGradientDescent.h \
    Extreminator/optimizers/Orthogonalise.h \
    Extreminator/optimizers/population.h \
    Extreminator/optimizers/scalingFunctions.h \
    Extreminator/optimizers/simulatedAnnealing.h \
    Extreminator/optimizers/SRES.h \
    Extreminator/fitteInterface.h \
    Interface/optselect.h \
    Interface/grapheCustom.h \
    Interface/QCustomPlot/qcustomplot.h \
    Interface/starter.h \
    Interface/simuwin.h \
    NewProject/expsNewProject.h \
    NewProject/namesNewProject.h \
    NewProject/Models/model1.h 

SOURCES += \
    Framework/evaluator.cpp \
    Framework/Experiment.cpp \
    Framework/generate.cpp \
    Framework/modele.cpp \
    Framework/overrider.cpp \
    Framework/spline.cpp \
    Framework/tableCourse.cpp \
    Framework/parameterSets.cpp \
    Interface/simuwin.cpp \
    Interface/optselect.cpp \
    Interface/grapheCustom.cpp \
    Interface/QCustomPlot/qcustomplot.cpp \
    Interface/starter.cpp \
    Extreminator/common/myFiles.cc \
    Extreminator/common/myRandom.cc \
    Extreminator/common/myTimes.cc \
    Extreminator/optimizers/SRES/ESES.cc \
    Extreminator/optimizers/SRES/ESSRSort.cc \
    Extreminator/optimizers/SRES/sharefunc.cc \
    Extreminator/optimizers/baseOptimizer.cc \
    Extreminator/optimizers/CMAES.cc \
    Extreminator/optimizers/Config.cc \
    Extreminator/optimizers/Genetic.cc \
    Extreminator/optimizers/GeneticGeneral.cc \
    Extreminator/optimizers/GradientDescent.cc \
    Extreminator/optimizers/individual.cc \
    Extreminator/optimizers/Orthogonalise.cc \
    Extreminator/optimizers/population.cc \
    Extreminator/optimizers/scalingFunctions.cc \
    Extreminator/optimizers/simulatedAnnealing.cc \
    Extreminator/optimizers/SRES.cc \
    Extreminator/fitteInterface.cpp \
    Interface/minimumInterface.cpp \
    common.cpp \
    NewProject/expsNewProject.cpp \
    NewProject/namesNewProject.cpp \
    NewProject/Models/model1.cpp \
    NewProject/mainNewProject.cpp
	
FORMS += \
    Interface/simuwin.ui \
    Interface/optselect.ui 



