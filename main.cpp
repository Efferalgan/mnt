/*
 * \file main.cpp
 * \brief MainWindow execution
 * \author Clement Marie Philemon
 * \date November 2014
 *
 * Execution of the mainwindow launching the whole application
 *
 */

#include <GL/glut.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <drawmnt.h>
#include <tracegpx.h>
#include <main_opengl.h>
#include <oeil.h>

#include "gpx.h"
#include "misc.h"
#include "mnt.h"
#include "intersection.h"
#include "mafenetre.h"
#include <QApplication>

int main(int argc, char *argv[]){

    QApplication a(argc, argv);
    mafenetre w;
    glutInit(&argc, argv);
    w.show();

    return a.exec();
}
