TEMPLATE = app
CONFIG += console

QT       += core gui

QT += opengl
LIBS += -lglut
LIBS += -lGL
LIBS += -lGLU -lm
LIBS += -lproj
INCLUDEPATH += -L/usr/lib/

HEADERS += \
    gpx.h \
    misc.h \
    mnt.h \
    intersection.h \
    tracegpx.h \
    oeil.h \
    main_opengl.h \
    mafenetre.h \
    drawmnt.h

SOURCES += \
    gpx.cpp \
    misc.cpp \
    mnt.cpp \
    intersection.cpp \
    tracegpx.cpp \
    oeil.cpp \
    main_opengl.cpp \
    main.cpp \
    mafenetre.cpp \
    drawmnt.cpp
