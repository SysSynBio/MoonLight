#-------------------------------------------------
#
# Project created by QtCreator 2014-09-01T15:12:07
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = FrameworkTest
# CONFIG   += console
# CONFIG   -= app_bundle

TEMPLATE = app

# boost ...
#INCLUDEPATH += C:\Qt\Boost\boost_1_53_0\boost_1_53_0
#INCLUDEPATH += C:\Qt\Boost\boost_1_53_0
# to avoid warnings from boost library files (wtf !)
QMAKE_CXXFLAGS += -o3 -Wno-unused-local-typedefs -Wreorder -Wno-unused-parameter

QMAKE_CXXFLAGS += -std=c++11

#to optimize code ...
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

#to use matlab engine
#INCLUDEPATH += "C:/Program Files (x86)/MATLAB/R2013b/extern/include/"
#INCLUDEPATH += "C:/Program Files (x86)/MATLAB/R2013b/extern/bin/"
#INCLUDEPATH += "C:/Program Files (x86)/MATLAB/R2013b/extern/lib/win32/microsoft"
#LIBS += -L"C:/Program Files (x86)/MATLAB/R2013b/extern/lib/win32/microsoft"  -llibmx -llibeng -llibmex -llibmat



HEADERS += \
	modele.h \
	evaluator.h \
	experiments.h \
	generate.h \
	tableCourse.h \
    overrider.h \
    spline.h

SOURCES += \
    evaluator.cpp \
    experiments.cpp \
    generate.cpp \
    modele.cpp \
    tableCourse.cpp \
    overrider.cpp \
    spline.cpp

