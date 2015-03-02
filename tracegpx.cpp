#include "tracegpx.h"

#include <GL/gl.h>
#include <GL/glut.h>
traceGPX::traceGPX()
{
    //ctor
}
traceGPX::~traceGPX()
{
    //dtor
}
void traceGPX::draw(vector<vector<double> > vect, int a, double x, double y, double z){
     glTranslated(x,y,z);


         glLineWidth(4);
         glColor3d(0.2,0.2,0.2);

         for (int nbarc=0; nbarc<a-1; nbarc++)
         {
             glBegin(GL_LINE_STRIP);
                 // Point initial
                 glVertex3f(vect[nbarc][0],vect[nbarc][1],vect[nbarc][2]);
                 // Point final
                 glVertex3f(vect[nbarc+1][0],vect[nbarc+1][1],vect[nbarc+1][2]);
             glEnd();
         }
     glTranslated(-x,-y,-z);
}
