/**
  * \brief Functions to calculate intersections between a gpx and DEM, including calculation of altitudes from the DEM
  * \author clement.drouadaine@ensg.eu, philemon.renaud@ensg.eu, marie.aladenise@ensg.eu
  * \date November 2014
  * \file intersection.h
*/

#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <vector>
#include <mnt.h>

    /**
     * @brief altitudePointGpx: function to calculate the altitude of a point located on the mnt
     * @param double x, y : coordinates x, y of the point
     * @param mnt myMnt: mnt
     * @param int taillex, tailley: sizes of the mnt
     * @return altitude z of the point (x,y)
     */
    double altitudePointGpx(double x, double y, int taillex, int tailley, double cellSize, double** mntMnt);
    /**
     * @brief altitudePointIntersection: function to calculate the altitude of a point located between two points (x1,y1,z1) and (x2,y2,z2)
     * @param double x,y: coordinates x,y of the point for which we look for the altitude
     * @param double x1, y1, z1: coordinates x,y,z
     * @param double x2,y2,z2: coordinates x,y,z
     * @return altitude z of the point (x,y)
     */
    double altitudePointIntersection(double x,double y,double x1,double y1,double z1,double x2,double y2, double z2);
    /**
     * @brief ajoutPoint: function which adds a point (x,y,z, t) to a vector of points
     * @param std::vector <std::vector<double> > vect: vector of points
     * @param double x,y,z,t: coordinates of the point to add to the vector of points
     * @return the vector of points with the added point
     */
    std::vector<std::vector<double> > ajoutPoint( std::vector <std::vector<double> > &vect,double x, double y, double z, double t=-1);
    /**
     * @brief intersectionPoints: function  which calculates all intersection between a gpx and a mnt
     * @param std::vector<std::vector<double> > gpx: gpx made of points (x,y,t)
     * @param mnt myMnt: mnt made of points (x,y,z)
     * @param int taillex, tailley: sizes of the mnt
     * @param double cellSize: mnt step
     * @return a matrix of intersection and gpx points, all sorted
     */
    std::vector<std::vector<double> > intersectionPoints(const std::vector<std::vector<double> > & gpx, int taillex, int tailley, double cellSize, double** mntMnt, QString path);

    void writeIntersectionToFile(QString path, std::vector<std::vector<double> > matIntersection);

    std::vector<std::vector<double> > readIntersectionFromFile(QString path);

#endif // INTERSECTION_H
