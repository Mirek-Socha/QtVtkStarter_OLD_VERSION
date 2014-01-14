#-------------------------------------------------
#
# Project created by QtCreator 2010-11-03T02:39:23
#
#-------------------------------------------------

QT       += opengl

TARGET = QtVtk
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS    += -L/usr/lib/vtk-5.8 -lvtkCommon -lvtksys -lQVTK -lvtkViews -lvtkWidgets -lvtkInfovis -lvtkRendering -lvtkGraphics -lvtkImaging -lvtkIO -lvtkFiltering -lvtkDICOMParser -lvtkHybrid

INCLUDEPATH += /usr/include/vtk-5.8
