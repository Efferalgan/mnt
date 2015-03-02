/**
    * \class mnt
    * \brief Deals with the DEM & the isohypses
    * \author clement.drouadaine@ensg.eu, marie.aladenise@ensg.eu, philemon.renaud@ensg.eu
    * \date autumn 2014
    * \file mnt.h
*/


#ifndef MNT_H
#define MNT_H

#include <iostream>
#include <vector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <misc.h>
#include <iomanip>
#include <cmath>
#include <sys/stat.h>

class mnt
{
public:
    /**
        \fn mnt(QString path, bool computeIsohypses=true)
        \brief constructor
        \param QString path: the path to the file to read, which must be either a \.asc or a \.xyz format\. Will also check for .xml with the same name in the folder, and will try to read it as the isolines file if it exists\. If it does not exist, it will be written with the same name on the same folder.
        \param bool computeIsohypses: set to true by default\. If false, will deal with the isohypses
    */
    mnt(QString path, bool computeIsohypses=true);

    mnt();

    mnt(mnt &);

    mnt(double** mntMnt, int sizeX, int sizeY, double nodataValue, double cellSize, int numberOfIsohypses, double* altitudeOfIsohypses);

    /**
      \fn update
      \brief adds isohypses to a DEM which has only been created by reading its file
     */
    void update();

    /**
        \fn ~mnt()
        \brief destructor
    */
    ~mnt();

    /**
        \fn double** fromFile()
        \brief the main subroutine of the constructor
        \return the mnt grid corresponding to the file read, whose path is contained in _path, as a double**; also sets the private attributes
    */
    double** fromFile();

    /**
        \fn static int tailleX(double** mnt1)
        \brief computes the width of the DEM
        \param double** mnt1: a pointer to the DEM grid
        \return the size of the DEM grid on the x axis
    */
    static int tailleX(double** mnt1);

    /**
        \fn static int tailleY(double** mnt1, double x)
        \brief computes the length of the DEM
        \param double** mnt1: a pointer to the DEM grid
        \param double x: the size along the other axis
        \return the size of the DEM grid along the y axis
    */
    static int tailleY(double** mnt1, double x);

    /**
        \fn static double tailleCellule(double** mnt1)
        \brief computes the distance between two consecutive summits of the DEM grid, ie the size of a cell
        \param double** mnt1: a pointer to the DEM grid
        \return the distance between two consecutive summits
    */
    static double tailleCellule(double* *mnt1);

    /**
        \fn static double** reverseY(double**mnt1, int sizeX, int sizeY)
        \brief reverses the order of the stored DEM points along the y axis: does not change the DEM but only the order of storage of points
        \param double** mnt1: a pointer to the DEM grid
        \param int sizeX: the size of the DEM grid along the first axis
        \param int sizeY: the size of the DEM gris along the other axis
        \return a double** pointer to the reversed grid
    */
    static double** reverseY(double**mnt1, int sizeX, int sizeY);

    /**
        \fn int getSizeX
        \brief getter
        \return  _sizeX, the size of the DEM grid on the x axis
    */
    int getSizeX();

    /**
        \fn int getSizeY
        \brief getter
        \return  _sizeY, the size of the DEM grid on the y axis
    */
    int getSizeY();

    /**
        \fn int getNodataValue
        \brief getter
        \return  _nodataValue, the value taken by missing data
    */
    int getNodataValue();

    /**
        \fn double getCellSize
        \brief getter
        \return  _cellSize, the distance between two consecutive summits
    */
    double getCellSize();

    /**
        \fn double** getMnt
        \brief getter
        \return  _mnt, a double** pointer to the DEM grid
    */
    double** getMnt();

    /**
        \fn void print
        \brief prints a DEM
    */
    void print();

    /**
        \fn void resizeMnt(int newSize)
        \brief cuts the DEM to make a square of the specified size, starting with the top left corner\. Stores result in _mnt.
        \param int newSize: the size for the new DEM
    */
    void resizeMnt(int newSize);

    /**
        \fn void setAltitudesOfIsohypses;
        \brief computes the optimal altitudes for the isolines to be computed, and stores it into _altitudesOfIsohypses
    */
    void setAltitudesOfIsohypses();

    /**
        \fn int getNumberOfIsohypses
        \brief getter
        \return  _numberOfIsohypses, the number of different altitudes at which the isolines will be computed
    */
    int getNumberOfIsohypses();

    /**
        \fn double* getAltitudesOfIsohypses
        \brief getter
        \return  _altitudesOfIsohypses, an array containing the different altitudes at which the isolines will be computed
    */
    double* getAltitudesOfIsohypses();

    /**
        \fn double* getMinMaxZ
        \brief gets the min an max value of z in the DEM
        \return a pointer to an array consisting of {minZ, maxZ}
    */
    double* getMinMaxZ();

    /**
        \fn std::vector<std::vector<double> > intersectionPointsZ
        \brief computes all the points of all the isohypses, in random order
        \return a vector of points
    */
    std::vector<std::vector<double> > intersectionPointsZ(); //deprecated

    /**
        \fn void ajoutPointZ(std::vector <std::vector<double> > &vect, double x, double y, double z)
        \brief adds a point of coordinates x, y, z, to the vector
        \param std::vector <std::vector<double> > &vect: the vector to add the point to
        \param double x: the x coordinate of the point to add
        \param double y: the y coordinate of the point to add
        \param double z: the z coordinate of the point to add
    */
    void ajoutPointZ(std::vector <std::vector<double> > &vect, double x, double y, double z);

    /**
        \fn void isohypsesFromPoints(std::vector<std::vector<double> > vectorOfPoints)
        \brief DEPRECATED\. Makes isohypses from a list of unordered points, and stores it in the _isohypses attribute
        \param std::vector<std::vector<double> > vectorOfPoints: a vector containing xyz points (ie a vector of vectors of doubles)
    */
    void isohypsesFromPoints(std::vector<std::vector<double> > vectorOfPoints); //deprecated

    /**
        \fn std::vector<std::vector<std::vector<std::vector<double> > > > getIsohypses
        \brief getter
        \return  _isohypses, the isolines to be drawn
    */
    std::vector<std::vector<std::vector<std::vector<double> > > > getIsohypses();

    /**
        \fn QString getPath()
        \brief getter
        \return  _path, the path to the DEM file
    */
    QString getPath();

    /**
        \fn std::vector<std::vector<std::vector<double> > > getTriangles
        \brief getter
        \return  _triangles, the list of the DEM triangles
    */
    std::vector<std::vector<std::vector<double> > > getTriangles();

    /**
        \fn void writeIsohypsesToFile(QString path)
        \brief saves the isohypses in an xml file
        \param QString path: the path to the file to write
    */
    void writeIsohypsesToFile(QString path);

    /**
        \fn void readIsohypsesFromFile(QString path)
        \brief reads isohypses from an xml file, and manages the attributes _altitudesOfIsohypses, _numberOfIsohypses and _isohypses accordingly
        \param QString path: the path to the file to read
    */
    void readIsohypsesFromFile(QString path);

    /**
        \fn void triangles
        \brief creates the list of triangles (ie a vector containing vectors of three points) from the DEM, and stores it into _triangles
    */
    void triangles();

    /**
        \fn void makeIsohypses
        \brief makes the isohypses from the DEM
    */
    void makeIsohypses();

    /**
        \fn int huntIsohypse(std::vector<std::vector<double> >* isohypse, const double z, bool** listOfBool, const int indexOfTriangle, const int newSizeX, const int trianglesSize, int numberOfSeparators, int depth)
        \brief for a given altitude and triangle, recursively creates the longest continuous isohypse possible
        \param std::vector<std::vector<double> >* isohypse, a pointer to the vector we are adding the points to
        \param const double z, the altitude at which we want to compute the isohypse
        \param bool** listOfBool, a pointer to an array of booleans marking if a triangle has been (partially) computed or not. The line i corresponds to the triangle i, and consists of 4 booleans: one per side of the triangle, and one for the whole thing.
        \param const int indexOfTriangle, the index of the triangle we will be starting with
        \param const int newSizeX, the number of triangles per line of the DEM
        \param const int trianglesSize, the total number of triangles of the DEM
        \param int numberOfSeparators, the current number of separators contained within the isohypse
        \param int depth, the current depth of the recursions
        \return an int containing the number of separators within the isohypse, or -1 if recursion has been too deep
    */
    int huntIsohypse(std::vector<std::vector<double> >* isohypse, const double z, bool** listOfBool, const int indexOfTriangle, const int newSizeX, const int trianglesSize, int numberOfSeparators, int depth);

    /**
        \fn std::vector<std::vector<std::vector<double> > > tryToStick(std::vector<std::vector<std::vector<double> > > myIsohypseLevel, std::vector<std::vector<double> > myIsohypse_tmp)
        \brief tries to stick the isohypse to one of the other isohypses already computed, if their extremities are close enough\. Otherwise, just add it at the end of the list of isohypses.
        \param std::vector<std::vector<std::vector<double> > > myIsohypseLevel, a vector containing the isohypses already computed
        \param std::vector<std::vector<double> > myIsohypse_tmp, the isohypse we are trying to stick
        \return an upgraded myIsohypseLevel, now containing myIsohypse_tmp
    */
    std::vector<std::vector<std::vector<double> > > tryToStick(std::vector<std::vector<std::vector<double> > > myIsohypseLevel, std::vector<std::vector<double> > myIsohypse_tmp);

    /**
        \fn std::vector<std::vector<std::vector<double> > > tryToClose(std::vector<std::vector<std::vector<std::vector<double> > > > allTheIsohypses)
        \brief tries to join the first and last point of isohypses, if their extremities are close enough
        \param std::vector<std::vector<std::vector<std::vector<double> > > > allTheIsohypses: a vector containing vectors of isohypses
        \return an upgraded vector, with closable isohypses now closed
    */
    std::vector<std::vector<std::vector<std::vector<double> > > > tryToClose(std::vector<std::vector<std::vector<std::vector<double> > > > allTheIsohypses);

    /**
     * \fn void xyz2asc()
     * \brief writes a .asc file corresponding to the DEM in the same folder
     */
    void xyz2asc();

private:
    /**
        \var double** _mnt
        \brief a pointer to the DEM grid
    */
    double** _mnt;

    /**
        \var int _sizeX
        \brief the number of summits of the DEM along the first axis
    */
    int _sizeX;

    /**
        \var int _sizeY
        \brief the number of summits of the DEM along the second axis
    */
    int _sizeY;

    /**
        \var double _nodataValue
        \brief the value taken by missing data
    */
    double _nodataValue;

    /**
        \var double _cellSize
        \brief the distance, in DEM units, between two consecutive summits
    */
    double _cellSize;

    /**
        \var int _numberOfIsohypses
        \brief the length of _altitudesOfIsohypses
    */
    int _numberOfIsohypses;

    /**
        \var double* _altitudesOfIsohypses
        \brief a pointer to the array containing the heights of the isohypses to compute
    */
    double* _altitudesOfIsohypses;

    /**
        \var std::vector<std::vector<std::vector<std::vector<double> > > > _isohypses
        \brief the isohypses of the DEM\. First level: all the isohypses; second level: all the isohypses for a given altitude; third level: all the points for a given isohypse; fourth: all the coordinates for a given point
    */
    std::vector<std::vector<std::vector<std::vector<double> > > > _isohypses;

    /**
        \var std::vector<std::vector<std::vector<double> > > _triangles
        \brief the list of the DEM triangles
    */
    std::vector<std::vector<std::vector<double> > > _triangles;

    /**
        \var QString path
        \brief the path to the DEM file
    */
    QString _path;
};

/**
    \fn std::vector<std::vector<std::vector<double> > > classByAlti(std::vector<std::vector<double> > vectorOfPoints, double* arrayOfAlti, int arraySize)
    \brief DEPRECATED\. Takes a vector of points, and returns a vector of vector of points, sorted by altitude
    \param std::vector<std::vector<double> > vectorOfPoints: a vector of unsorted points
    \param double* arrayOfAlti: an array containing the altitudes of the points
    \param int arraySize: the length of this array
    \return std::vector<std::vector<std::vector<double> > > sorted: a vector of vector of points, sorted by altitude
*/
std::vector<std::vector<std::vector<double> > > classByAlti(std::vector<std::vector<double> > vectorOfPoints, double* arrayOfAlti, int arraySize); //deprecated

#endif // MNT_H
