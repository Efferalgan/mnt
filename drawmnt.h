/**
  \class drawMNT
  \brief Draw the DEM and the isohypses
  \author philemon.renaud@ensg.eu, clement.drouadaine@ensg.eu, marie.aladenise@ensg.eu
  \date autumn 2014
  \file drawMNT.h
*/

#ifndef DRAWMNT_H
#define DRAWMNT_H
#include <iostream>
#include <vector>
using namespace std;

class drawMNT
{
    public:
    /**
        \fn drawMNT()
        \brief Constructor
    */
        drawMNT();

        /** \fn draw(double** tab, vector<vector<vector<vector<double> > > > vect2, int tailleMnt, int tailleIsos, int a, int b, double x, double y, double z, double* colortriangle, double* colorline, int firstboucle, int triangles, int aveccolor, int aveciso)
            \brief draws the MNT and the Isohypses
            \param tab table containing the coordinates of the MNT
            \param vect2 vector of all the isohypses
            \param tailleMnt size in the buffer for the MNT coordinates
            \param tailleIso size in the buffer for the isohypes coordinates
            \param a rows in the MNT
            \param a columns in the MNT
            \param x translation in x
            \param y translation in y
            \param z translation in z
            \param colortriangle table containing the colors of all the triangles
            \param colorline table containing the colors of all the lines
            \param firstboucle 1 if it's the first iteration, 0 if not
            \param triangles 1 if drawn with triangles, 0 if drawn with lines
            \param aveccolor 1 if drawn with colors, 0 if not
            \param aveciso 1 if drawn with isohypses, 0 if not
        */
        void draw(double** tab, vector<vector<vector<vector<double> > > > vect2, int tailleMnt, int tailleIsos, int a, int b, double x, double y, double z, double* colortriangle, double* colorline, int firstboucle, int triangles, int aveccolor, int aveciso);
        /**
            \fn ~drawMNT()
            \brief Destructor
        */
        virtual ~drawMNT();
        /**
            \var vaoID[1]
            \brief Vertex Array Object
        */
        unsigned int vaoID[1]; // Our Vertex Array Object
        /**
            \var vboID[1]
            \brief Vertex Buffer Object
        */
        unsigned int vboID[1]; // Our Vertex Buffer Object
        //double _data;
    protected:
    private:
};

#endif // CMNT_H
