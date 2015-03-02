/**
  \class traceGPX
  \brief Draw the GPX line
  \author philemon.renaud@ensg.eu, clement.drouadaine@ensg.eu, marie.aladenise@ensg.eu
  \date autumn 2014
  \file tracegpx.h
*/

#ifndef TRACEGPX_H
#define TRACEGPX_H
#include <iostream>
#include <vector>
using namespace std;

class traceGPX
{
    public:
    /**
        \fn traceGPX()
        \brief Constructor
    */
        traceGPX ();
        /** \fn draw(vector<vector<double> > vect, int a,double X,double Y,double Z)
            \brief draw the GPX line
            \
            \param vect vector containing the GPX coordinates
            \param a size of the GPX
            \param X translation in x
            \param Y translation in y
            \param Z translation in z
        */
        void draw(vector<vector<double> > vect, int a,double X,double Y,double Z);
        /**
            \fn ~traceGPX()
            \brief Destructor
        */
        virtual ~traceGPX ();
    protected:
    private:
};

#endif // CMNT_H

