#include "oeil.h"
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>
oeil::oeil()
{
    // Constructor

}
oeil::~oeil(){
    // Destructor
}

void oeil::draw_oeil(float Xpos, float Ypos, float Zpos, int sizeeye )
{
    glColor3d(0.8,0.2,0.2);
    glTranslated(Xpos,Ypos,Zpos);
    glTranslated(0,0,0.2);
    glRotated(90,0,0,1);
    glRotatef(90,1,0,0);
    glTranslatef(0,0,-0.2);
    glPushMatrix();
        glScalef(sizeeye,sizeeye,sizeeye );
        glutSolidSphere(0.1,10,10); //sphere
//        glutSolidCube(0.5); //cube2
    glPopMatrix();
}
