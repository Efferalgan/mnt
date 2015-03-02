/**
    * \class gpx
    * \brief Deals with the gpx, its creation, and its import/export as a text file
    * \author clement.drouadaine@ensg.eu, marie.aladenise@ensg.eu, philemon.renaud@ensg.eu
    * \date autumn 2014
    * \file gpx.h
*/

#ifndef GPX_H
#define GPX_H

#include <QFile>
#include <QTextStream>
#include <QXmlStreamReader>
#include <iostream>
#include <QString>
#include <vector>

#include <misc.h>
#include <mnt.h>
#include <intersection.h>
class gpx
{
public:
    /**
     * @fn gpx(QString gpxPath);
     * @brief constructor\. Basically, just calls the fromFile function
     * @param QString gpxPath: the path to the file to read\. Must be a \.gpx
    */
    gpx(QString gpxPath);

    /**
     * @fn std::vector<std::vector<double> > fromFile(QString gpxPath);
     * @brief reads a \.gpx file to construct a gpx
     * @param QString gpxPath: the path to the file to read\. Must be a \.gpx
     * @return a std::vector<std::vector<double> > containing the ordered points of the gpx
     */
    std::vector<std::vector<double> > fromFile(QString gpxPath);

    /**
     * @fn bool inMnt(mnt myMNT);
     * @brief checks if at least 70% of the gpx in contained within the DEM, and truncates the out-of-bounds parts
     * @param mnt myMNT: the DEM that will be used for the check
     * @return true if at least 70% of the gpx is in the DEM, false otherwise
     */
    bool inMnt(mnt myMNT);

    /**
     * @fn void removePoints(mnt myMnt, std::vector<double> indicesOfPointsToRemove);
     * @brief deletes the gpx points whose indices are given, and stops the gpx at the border of the DEM
     * @param mnt myMnt: the DEM to check the border against
     * @param std::vector<double> indicesOfPointsToRemove: a vector containing the indices of the gpx points to remove
     */
    void removePoints(mnt myMnt, std::vector<double> indicesOfPointsToRemove);

    /**
     * @fn std::string getTitle()
     * @brief getter
     * @return the title of the gpx
     */
    std::string getTitle();

    /**
     * @fn std::vector<std::vector<double> > getGpx()
     * @brief getter
     * @return the gpx vector
     */
    std::vector<std::vector<double> > getGpx();

    /**
     * @fn bool doesTimeExist()
     * @brief checks if a time had been defined in the gpx
     * @return true if a time was provided, false otherwise
     */
    bool doesTimeExist();

    /**
     * @fn QString getPath()
     * @brief getter
     * @return the path the gpx file
     */
    QString getPath();


private:
    /**
     * @var std::string _title
     * @brief the title of the gpx
     */
    std::string _title;

    /**
     * @var std::vector<std::vector<double> > _gpx
     * @brief the gpx vector
     */
    std::vector<std::vector<double> > _gpx;

    /**
     * @var bool _timeExists
     * @brief true if time is provided by the gpx, false otherwise
     */
    bool _timeExists;

    /**
     * /var _path
     * /brief path to the GPX file
     */
    QString _path;
};

/**
 * @fn std::vector<double> computeWhereOut(double x1, double y1, double x2, double y2, double t1, double t2, mnt myMnt)
 * @brief given two points A & B and a DEM, computes where [AB] gets out of the DEM bounds, and replace the faulty extremity of [AB] with the intersection with the DEM
 * @param double x1, y1, z1: coordinates of the first point
 * @param double x2, y2, z2: coordinates of the second point
 * @param double t1, t2: time for the points
 * @param mnt myMnt: the DEM
 * @return a std::vector<double> containing two points, such as the truncated segment now fits within the DEM
 */
std::vector<double> computeWhereOut(double x1, double y1, double x2, double y2, double t1, double t2, mnt myMnt);

/**
 * @fn doubledistBetweenPointsFollowingMnt(double x1, double x2, double y1, double y2, mnt myMnt)
 * @brief computes the 3D distance one would walk on the DEM if trying to get to point B on a straight line from point A
 * @param double x1, x2: x coordinates of the two points
 * @param double y1, y2: y coordinates of the two points
 * @param mnt myMnt: the DEM on which we walk
 * @return the distance
 */
double distBetweenPointsFollowingMnt(double x1, double x2, double y1, double y2, mnt myMnt);

/**
 * @fn test_WhereOut()
 * @brief tests the computeWhereOut function with a small DEM, to see if it works as expected
 * @return a bool, true if successful, false otherwise
 */
bool test_WhereOut();
#endif // GPX_H
