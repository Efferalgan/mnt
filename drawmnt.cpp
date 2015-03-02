#include "drawmnt.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <iostream>

using namespace std;

drawMNT::drawMNT()
{
    //ctor
}
drawMNT::~drawMNT()
{
    //dtor
}


void drawMNT::draw(double** tab, vector<vector<vector<vector<double> > > > vect2, int tailleMnt, int tailleIsos, int a, int b, double x, double y, double z, double* colortriangle, double* colorline, int firstboucle, int triangles, int aveccolor, int aveciso){

    glColor3d(0.3,0.4,0.3);

    //a et b sont la longueur et la largeur du MNT,x, y et z servent à décaler le MNT
    glTranslated(x,y,z);

    //Initialise VBO - do only once, at start of program
    //Create a variable to hold the VBO identifier
    GLuint triangleVBO;


    //Partie uniquement si première boucle (création buffer, transfert sur la carte graphique)

    int indice=0;
    if(firstboucle==1)
    {
        double* data = new double [tailleMnt+tailleIsos];

        if (triangles==1){

            for(int i=0;i<b-1;i++)
            {
                for(int j=0;j<a-1;j++)
                {
                    //Create the first triangle with 9 coordinates
                    data[indice]= tab[a*i+j][0];
                    data[indice+1]= tab[a*i+j][1];
                    data[indice+2]= tab[a*i+j][2];
                    data[indice+3]= tab[a*i+j+1][0];
                    data[indice+4]= tab[a*i+j+1][1];
                    data[indice+5]= tab[a*i+j+1][2];
                    data[indice+6]= tab[a*i+a+j][0];
                    data[indice+7]= tab[a*i+a+j][1];
                    data[indice+8]= tab[a*i+a+j][2];
                    //Create the second triangle with 9 coordinates
                    data[indice+9]= tab[a*i+j+1][0];
                    data[indice+10]= tab[a*i+j+1][1];
                    data[indice+11]= tab[a*i+j+1][2];
                    data[indice+12]= tab[a*i+a+j+1][0];
                    data[indice+13]= tab[a*i+a+j+1][1];
                    data[indice+14]= tab[a*i+a+j+1][2];
                    data[indice+15]= tab[a*i+a+j][0];
                    data[indice+16]= tab[a*i+a+j][1];
                    data[indice+17]= tab[a*i+a+j][2];

                    indice = indice + 18;
                }
            }
        }
        if (triangles==0){
            for(int i=0;i<b-1;i++)
            {
                for(int j=0;j<a-1;j++)
                {
                    //On trace des lignes
                    //Crée la première ligne avec 6 coordonnées
                    data[indice]= tab[a*i+j][0];
                    data[indice+1]= tab[a*i+j][1];
                    data[indice+2]= tab[a*i+j][2];
                    data[indice+3]= tab[a*i+j+1][0];
                    data[indice+4]= tab[a*i+j+1][1];
                    data[indice+5]= tab[a*i+j+1][2];
                    //Crée la seconde ligne avec 6 coordonnées
                    data[indice+6]= tab[a*i+j+1][0];
                    data[indice+7]= tab[a*i+j+1][1];
                    data[indice+8]= tab[a*i+j+1][2];
                    data[indice+9]= tab[a*i+a+j][0];
                    data[indice+10]= tab[a*i+a+j][1];
                    data[indice+11]= tab[a*i+a+j][2];
                    //Crée la troisième ligne avec 6 coordonnées
                    data[indice+12]= tab[a*i+a+j][0];
                    data[indice+13]= tab[a*i+a+j][1];
                    data[indice+14]= tab[a*i+a+j][2];
                    data[indice+15]= tab[a*i+j][0];
                    data[indice+16]= tab[a*i+j][1];
                    data[indice+17]= tab[a*i+j][2];
                    indice = indice + 18;
                }
            }
            }


     //Partie uniquement si première boucle (création buffer, transfert sur la carte graphique)
        int a2=vect2.size();
          for (int nbalt=0; nbalt<a2; nbalt++){
              int b3 = vect2[nbalt].size();
              for (int nbline=0; nbline<b3; nbline++){
                  int c3 = vect2[nbalt][nbline].size();
                  if (c3>1){
                  for (int nbarc=0; nbarc<c3-1; nbarc++)
                              {
                                //Point Initial
                                data[indice]=vect2[nbalt][nbline][nbarc][0];
                                data[indice+1]=vect2[nbalt][nbline][nbarc][1];
                                data[indice+2]=vect2[nbalt][nbline][nbarc][2];
                                //Point Final
                                data[indice+3]=vect2[nbalt][nbline][nbarc+1][0];
                                data[indice+4]=vect2[nbalt][nbline][nbarc+1][1];
                                data[indice+5]=vect2[nbalt][nbline][nbarc+1][2];
                                indice+=6;
                              }
                     }
              }
          }


        //Create a new VBO and use the variable id to store the VBO id
        glGenBuffers(1, &triangleVBO);

        //Make the new VBO active
        glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);

        //Upload vertex data to the video device
        glBufferData(GL_ARRAY_BUFFER, tailleMnt*8 + tailleIsos*8, data, GL_STATIC_DRAW);

        //Make the new VBO active. Repeat here incase changed since initialisation
        glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);

        //Draw Triangle from VBO - do each time window, view point or data changes
        //Establish its 3 coordinates per vertex with zero stride in this array; necessary here
        glVertexPointer(3, GL_DOUBLE, 0, NULL);

        //Establish array contains vertices (not normals, colours, texture coords etc)
        glEnableClientState(GL_VERTEX_ARRAY);

        //_data = *data;

    }

    //Si on veut faire avec des couleurs
    if(aveccolor==1){
        if (triangles==0){
        //Actually draw the lines, giving the number of vertices provided
            for (int i=0; i<tailleMnt/6; i++){
                glColor3d(colorline[i]*3/4,colorline[i],colorline[i]*3/4);
                glDrawArrays(GL_LINES, i*2, 2 );
            }
        }

        if (triangles==1){
        //Actually draw the triangle, giving the number of vertices provided
            for (int i=0; i<tailleMnt/9; i++){
                glColor3d(colortriangle[i]*3/4,colortriangle[i],colortriangle[i]*3/4);
                glDrawArrays(GL_TRIANGLES, i*3, 3 );
            }
        }
    }

    //Si on ne veut pas les couleurs (+rapide)
    if(aveccolor==0){
        if (triangles==0){
        //Actually draw the lines, giving the number of vertices provided
        glDrawArrays(GL_LINES, 0, tailleMnt/3 );
        }

        if (triangles==1){
        //Actually draw the triangle, giving the number of vertices provided
        glDrawArrays(GL_TRIANGLES, 0, tailleMnt/3 );
        }
    }

    glTranslated(0,0,0.1);
    glColor3d(1,0,0);

    //Trace les IsoHypses
    if(aveciso==1){
    glDrawArrays(GL_LINES, tailleMnt/3, tailleIsos/3 );
    }

    glTranslated(0,0,-0.1);

    //Force display to be drawn now
    glFlush();


    glTranslated(-x,-y,-z);

}
