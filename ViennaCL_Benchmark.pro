#-------------------------------------------------
#
# Project created by QtCreator 2014-05-24T07:28:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ViennaCL_Benchmark
TEMPLATE = app

SOURCES += src/main.cpp\
    src/ui/mainwindow.cpp \
    src/ui/menulistwidget.cpp \
    src/ui/qcustomplot.cpp \
    src/benchmarks/benchmark_vector.cpp \
    src/benchmarks/benchmark_sparse.cpp \
    src/benchmarks/benchmark_solver.cpp \
    src/benchmarks/benchmark_scheduler.cpp \
    src/benchmarks/benchmark_blas3.cpp \
    src/benchmarks/benchmark_copy.cpp \
    src/benchmark_controller.cpp \
    src/benchmarks/benchmark_qr.cpp \
    src/benchmarks/abstractbenchmark.cpp \
    src/ui/benchmarklistwidget.cpp \
    src/ui/collapsewidget.cpp

HEADERS  += src/ui/mainwindow.h \
    src/ui/qcustomplot.h \
    src/ui/menulistwidget.h \
    src/benchmarks/benchmark-utils.hpp \
    src/benchmarks/benchmark_vector.h \
    src/benchmarks/benchmark_sparse.h \
    src/benchmarks/benchmark_solver.h \
    src/benchmarks/benchmark_scheduler.h \
    src/benchmarks/io.hpp \
    src/benchmarks/benchmark_blas3.h \
    src/benchmarks/Random.hpp \
    src/benchmarks/benchmark_copy.h \
    src/benchmark_controller.h \
    src/benchmarks/benchmark_qr.h \
    src/benchmarks/matrix_market.hpp \
    src/benchmarks/abstractbenchmark.h \
    src/ui/benchmarklistwidget.h \
    src/ui/collapsewidget.h

FORMS    += src/ui/mainwindow.ui \
    src/ui/collapsewidget.ui

RESOURCES += src/ui/resources/otherFiles.qrc \
    src/ui/resources/icons.qrc

#Add benchmark data files
testData.path = $$OUT_PWD/testdata
testData.files += testdata/mat65k.mtx testdata/result65025.txt testdata/rhs65025.txt
INSTALLS += testData

#Notes on qmake variables
# $$myVar - user variable
# $${myVar} - supposed to be another version of user variable...
# $$(envVar) - system environment variable (at the time qmake is run)
# $(envVar) - system environment variable (at the time when makefile is processed)
# $$[varName] - qt config variable

#Add projects sources(MSVC requirement)
INCLUDEPATH += $$OUT_PWD
DEPENDPATH += $$OUT_PWD
INCLUDEPATH += $$OUT_PWD/src/ui
DEPENDPATH += $$OUT_PWD/src/ui
INCLUDEPATH += $$OUT_PWD/src/benchmarks
DEPENDPATH += $$OUT_PWD/src/benchmarks
INCLUDEPATH += .
DEPENDPATH += .

#Add Boost and ViennaCL include folders
INCLUDEPATH += C:\Users\Namik\Documents\GitHub\viennacl-dev
INCLUDEPATH += C:\boost\boost_1_55_0
INCLUDEPATH += C:\boost\boost_1_55_0\stage\lib

DEPENDPATH += C:\Users\Namik\Documents\GitHub\viennacl-dev
DEPENDPATH += C:\boost\boost_1_55_0
LIBS += "-LC:/boost/boost_1_55_0/stage/lib/"

#Disable unused warnings that come from Boost and QCustomPlot
#CONFIG += warn_off
#QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
#QMAKE_CXXFLAGS += -Wno-unused-parameter

#Define Boost variables
DEFINES += BOOST_ALL_DYN_LINK
#DEFINES += Boost_USE_STATIC_LIBS=0
#DEFINES += Boost_USE_MULTITHREADED=1
#DEFINES += Boost_USE_STATIC_RUNTIME=0
#QMAKE_CXXFLAGS += -DBOOST_ALL_DYN_LINK
#QMAKE_CXXFLAGS += -DBoost_USE_STATIC_LIBS
#QMAKE_CXXFLAGS += -DBoost_USE_MULTITHREADED
#QMAKE_CXXFLAGS += -DBoost_USE_STATIC_RUNTIME

#set(Boost_USE_STATIC_LIBS OFF)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME OFF)

#Find OpenCL root folder
OPENCLROOT = $$(OPENCLROOT)

isEmpty(OPENCLROOT){
    OPENCLROOT = "C:\AMDAPPSDK\2.9"
    message("OpenCL not found in evironment, using hard-coded path: "$$OPENCLROOT)
} else{
    message("Automatically detected OpenCL: "$$OPENCLROOT)
}

#Find OpenCL headers include folder
OPENCL_INCLUDE_DIR += $$OPENCLROOT"/include"

isEmpty(OPENCL_INCLUDE_DIR){
    OPENCL_INCLUDE_DIR += "C:\AMDAPPSDK\2.9\include"
    message("Including hard-coded OpenCL headers folder: "$$OPENCL_INCLUDE_DIR)
} else{
    message("Automatically including OpenCL headers folder: "$$OPENCL_INCLUDE_DIR)
}

#Find OpenCL library folder
OPENCL_LIBRARIES = $$OPENCLROOT"/lib/x86"

isEmpty(OPENCL_LIBRARIES){
    OPENCL_LIBRARIES = "C:\AMDAPPSDK\2.9\include\lib\x86"
    message("Using hard-coded OpenCL library folder: "$$OPENCL_LIBRARIES)
} else{
    message("Automatically detected OpenCL library folder: "$$OPENCL_LIBRARIES)
}

#INCLUDEPATH += C:\AMDAPPSDK\2.9\include\CL
#DEPENDPATH += C:\AMDAPPSDK\2.9\include\CL

#INCLUDEPATH += C:\AMDAPPSDK\2.9\include\CL
#DEPENDPATH += C:\AMDAPPSDK\2.9\include\CL

#Release
win32:CONFIG(release, debug|release){
    QMAKE_CXXFLAGS += /MD
#    QMAKE_LIBDIR += C:\boost\boost_1_55_0\stage\lib
    QMAKE_LFLAGS_RELEASE = /INCREMENTAL:NO
    #Enable OpenCL
    QMAKE_CXXFLAGS += -DVIENNACL_WITH_OPENCL
    message("OpenCL library path: "$$OPENCL_LIBRARIES)
    LIBS += -L$${OPENCL_LIBRARIES} -lOpenCL
    message("This is a Release build")
}
#Debug
else:win32:CONFIG(debug, debug|release){
    QMAKE_CXXFLAGS += /MDd
    message("This is a Debug build")
    #Do not enable OpenCL
}
else:unix:{
    LIBS += -L$${OPENCL_LIBRARIES} -lOpenCL
}

INCLUDEPATH += $${OPENCL_LIBRARIES}
DEPENDPATH += $${OPENCL_LIBRARIES}
