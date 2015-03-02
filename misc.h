/**
    * \class misc
    * \brief Library of miscellaneous functions
    * \author clement.drouadaine@ensg.eu, marie.aladenise@ensg.eu, philemon.renaud@ensg.eu
    * \date autumn 2014
    * \file misc.h
*/


#ifndef MISC_H
#define MISC_H

#include <QFile>
#include <QTextStream>
#include <QXmlStreamReader>
#include <iostream>
#include <QString>
#include <QDateTime>
#include <math.h>
#include <vector>
#include <proj_api.h>


    /**
        \fn static double timeString2secondsSinceEpoch(QString timeString)
        \brief converts a QString containing a date to a number of seconds since 01-01-2000
        \param QString timeString: a date, in format yyyy-MM-dd'T'hh:mm:ss'Z' (ie 2014-12-02T11:34:25Z)
        \return the number of seconds between the date in the string and 01-01-2000
    */
    double timeString2secondsSinceEpoch(QString timeString);

    /**
        \fn static std::vector<std::vector<double> > wgs2l93(std::vector<std::vector<double> > wgsVector)
        \brief converts coordinates from wgs84 to lambert93
        \param std::vector<std::vector<double> > wgsVector: a vector of points to convert
        \return a std::vector<std::vector<double> > containing the converted points
    */
    std::vector<std::vector<double> > wgs2l93(std::vector<std::vector<double> > wgsVector);

    /**
        \fn static std::string qstring2string(QString qtext)
        \brief converts a qstring to a string
        \param QString qtext: the QString to convert
        \return std::string
    */
    std::string qstring2string(QString qtext);

    /**
        \fn static double** toArray(std::vector<std::vector<double> > &myVector)
        \brief converts a bidimensional vector to a bidimensional array
        \param std::vector<std::vector<double> > &myVector: the vector to convert
        \return a pointer to the converted array
    */
    double** toArray(std::vector<std::vector<double> > &myVector);

    /**
        \fn static double* toArray(std::vector<double> &myVector)
        \brief converts a monodimensional vector to a monodimensional array
        \param std::vector<double> &myVector: the vector to convert
        \return a pointer to the converted array
    */
    double* toArray(std::vector<double> &myVector);

    /**
        \fn static std::vector<std::vector<double> > retourne(std::vector<std::vector<double> > myVector)
        \brief flips a vector upside-down, so that the first element becomes last, and the last first.
        \param std::vector<std::vector<double> > myVector: the vector to flip
        \return std::vector<std::vector<double> > flipped: the flipped vector
    */
    std::vector<std::vector<double> > retourne(std::vector<std::vector<double> > myVector);

    /**
        \fn static void concatenate(std::vector<std::vector<double> >* vect1, std::vector<std::vector<double> >* vect2)
        \brief concatenates two vectors given by pointers
        \param std::vector<std::vector<double> >* vect1, a pointer to the initial vector that will be modified
        \param std::vector<std::vector<double> >* vect2, the vector to be added at the end of vect1
    */
    void concatenate(std::vector<std::vector<double> >* vect1, std::vector<std::vector<double> >* vect2);

    /**
        \fn static std::vector<std::vector<double> > concatenate(std::vector<std::vector<double> > vect1, std::vector<std::vector<double> > vect2)
        \brief concatenates two vectors
        \param std::vector<std::vector<double> > vect1: the first vector
        \param std::vector<std::vector<double> > vect2: the vector to be added at the end of vect1
        \return std::vector<std::vector<double> > vect: the result of the concatenation
    */
    std::vector<std::vector<double> > concatenate(std::vector<std::vector<double> > vect1, std::vector<std::vector<double> > vect2);

    /**
        \fn static double distCalc(double x1, double y1, double x2, double y2)
        \brief conmputes the euclidian distance between to points, on a plane
        \param double x1, y1, x2, y2: the coordinates of the two points
        \return the distance
    */
    double distCalc(double x1, double y1, double x2, double y2);

#endif // MISC_H
