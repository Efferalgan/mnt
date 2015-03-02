/**
  \class main_opengl
  \brief realize all the opengl actions
  \author philemon.renaud@ensg.eu, clement.drouadaine@ensg.eu, marie.aladenise@ensg.eu
  \date autumn 2014
  \file main_opengl.h
*/

#ifndef MAIN_OPENGL_H
#define MAIN_OPENGL_H
//#include <glut>

//#include <windows.h>
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
#include <oeil.h>

#include "gpx.h"
#include "misc.h"
#include "mnt.h"
#include "intersection.h"

class main_opengl
{
public:
    /**
        \fn main_opengl()
        \brief Constructor
    */
    main_opengl();
    /**
        \var ~main_opengl()
        \brief Destructor
    */
    virtual ~main_opengl();
    /** \fn func_main(int argc, char *argv[])
        \brief realize all the opengl actions
        \
        \param argc for glutInit()
        \param *argv[] for glutInit()
    */
    void func_main(mnt myMnt, QString gpxPath, int spinval, bool choixVitesseReelle);

};

#endif // MAIN_OPENGL_H
