#-------------------------------------------------
#
# Project created by QtCreator 2014-11-23T18:21:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ModelAnalytics

# this config is very important, it must be added to every Qt project with R integration;
CONFIG += console

TEMPLATE = app


SOURCES += src/main.cpp\
    src/mainwindow.cpp \
    src/model/DataModel.cpp \
    src/model/modelconfigdialog.cpp \
    src/qCustomPlot/qcustomplot.cpp \
    src/model/ModelProfile.cpp \
    src/model/modelTypes/DriftModel.cpp \
    src/model/modelTypes/VolatilityModel.cpp \
    src/model/modelTypes/MovingAvgModel.cpp \
    src/view/ModelView.cpp \
    src/view/ModelRelationView.cpp \
    src/model/LocalModelCfgDialog.cpp \
    src/view/LocalModelView.cpp \
    src/metric/metric.cpp \
    src/view/ModelEvalView.cpp \
    src/model/modelTypes/SeasonalModel.cpp \
    src/view/TimeHeatmapView.cpp \
    src/view/ConsensusView.cpp \
    src/Glyph.cpp

HEADERS  += src/mainwindow.h \
    src/model/DataModel.h \
    src/model/modelconfigdialog.h \
    src/qCustomPlot/qcustomplot.h \
    src/macro.h \
    src/model/ModelProfile.h \
    src/model/modelTypes/DriftModel.h \
    src/model/modelTypes/VolatilityModel.h \
    src/model/modelTypes/MovingAvgModel.h \
    src/view/ModelView.h \
    src/view/ModelRelationView.h \
    src/model/LocalModelCfgDialog.h \
    src/view/LocalModelView.h \
    src/metric/metric.h \
    src/view/ModelEvalView.h \
    src/model/modelTypes/SeasonalModel.h \
    src/view/TimeHeatmapView.h \
    src/view/ConsensusView.h \
    src/Glyph.h

FORMS    += src/mainwindow.ui \
    src/model/modelconfigdialog.ui \
    src/model/LocalModelCfgDialog.ui

INCLUDEPATH += src \
    src/metric \
    src/model/modelTypes \
    src/model \
    src/qCustomPlot \
    src/view



##----------------------start of configuring R environment-----------------------
## comment this out if you need a different version of R,
## and set set R_HOME accordingly as an environment variable
R_HOME =                $$system(R RHOME)

## include headers and libraries for R
RCPPFLAGS =             $$system($$R_HOME/bin/R CMD config --cppflags)
RLDFLAGS =              $$system($$R_HOME/bin/R CMD config --ldflags)
RBLAS =                 $$system($$R_HOME/bin/R CMD config BLAS_LIBS)
RLAPACK =               $$system($$R_HOME/bin/R CMD config LAPACK_LIBS)

## if you need to set an rpath to R itself, also uncomment
RRPATH =               -Wl,-rpath,$$R_HOME/lib

## include headers and libraries for Rcpp interface classes
RCPPINCL =              $$system($$R_HOME/bin/Rscript -e Rcpp:::CxxFlags\(\))
RCPPLIBS =              $$system($$R_HOME/bin/Rscript -e Rcpp:::LdFlags\(\))

## for some reason when building with Qt we get this each time
## so we turn unused parameter warnings off
RCPPWARNING =           -Wno-unused-parameter

## include headers and libraries for RInside embedding classes
RINSIDEINCL =           $$system($$R_HOME/bin/Rscript -e RInside:::CxxFlags\(\))
RINSIDELIBS =           $$system($$R_HOME/bin/Rscript -e RInside:::LdFlags\(\))


## compiler etc settings used in default make rules
QMAKE_CXXFLAGS += $$RCPPWARNING $$RCPPFLAGS $$RINSIDEINCL $$RCPPINCL -std=c++0x -std=gnu++0x
QMAKE_LIBS += $$RLDFLAGS $$RBLAS $$RLAPACK $$RINSIDELIBS $$RCPPLIBS

##------------------------end of configuring R environment-------------------

Release:DESTDIR = release
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui
