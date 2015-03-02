#include "gpx.h"
using namespace std;

gpx::gpx(QString gpxPath)
{
    _path = gpxPath;
    _gpx = this->fromFile(_path);
}


//reads a GPX file
vector<vector<double> > gpx::fromFile(QString gpxPath)
{
    // Opens the document
    QFile file(gpxPath);
    file.open(QIODevice::ReadOnly);

    // Reads the document
    QXmlStreamReader xml;
    xml.setDevice(&file);

    xml.readNext();

    vector< vector<double> > gpxData;
    vector<double> line;
    double lon;
    double lat;
    double h;
    double time;
    QString name;

    while (!xml.atEnd() && xml.name()!="trk" && xml.name()!="wpt" && xml.name()!="rte")
    {
        xml.readNext();
    }

    while(!xml.atEnd())
    {
        if(xml.isStartElement())
        {
            name = xml.name().toString();

            if (name=="name")
            {
                _title = xml.readElementText().toStdString();
            }

            if (name=="trkpt" || name=="wpt" || name=="rtept")
            {
                lat = xml.attributes().value("lat").toDouble();
                lon = xml.attributes().value("lon").toDouble();

                do{xml.readNext();}while(xml.name()=="");
                name = xml.name().toString();

                if (xml.isStartElement() && (name=="trkpt" || name=="wpt" || name=="rtept"))
                {
                    line.push_back(lat);
                    line.push_back(lon);
                    line.push_back(0);
                    line.push_back(-1);
                    gpxData.push_back(line);
                    line.clear();
                    continue;
                }

                if (xml.name()=="ele")
                {
                    h = xml.readElementText().toDouble();

                    xml.readNext();
                    name = xml.name().toString();
                    if (xml.isStartElement() && (name=="trkpt" || name=="wpt" || name=="rtept"))
                    {
                        line.push_back(lat);
                        line.push_back(lon);
                        line.push_back(h);
                        line.push_back(-1);
                        gpxData.push_back(line);
                        line.clear();
                        continue;
                    }
                }

                if(xml.name()=="time")
                {
                    time = timeString2secondsSinceEpoch(xml.readElementText());
                }
                else
                {
                    //date if none is present
                    time = -1;
                }

                line.push_back(lat);
                line.push_back(lon);
                line.push_back(h);
                line.push_back(time);
                gpxData.push_back(line);
                line.clear();

            } else
            {
                xml.readNext();
            }
        } else
        {
            xml.readNext();
        }
    }

    _timeExists=(time==-1)?false:true; //sets the _timeExists attribute to false if time == -1, true otherwise.

    return(wgs2l93(gpxData));
}

//checks that the GPX is contained within the loaded MNT
bool gpx::inMnt(mnt myMnt)
{
    if(_gpx.size()<2){return false;}

    double minPercentageOfPointsToBeContainedWithinTheMnt = 0.70;
    double** mnt2 = myMnt.getMnt();
    int sizeX = myMnt.getSizeX();
    int sizeY = myMnt.getSizeY();
    double minX = mnt2[0][0];
    double minY = mnt2[0][1];
    double maxX = mnt2[sizeX*sizeY-1][0];
    double maxY = mnt2[sizeX*sizeY-1][1];
    int error = 0;
    vector<double> indicesOfPointsToRemove;

    //checking every point
    for(unsigned i=0; i<_gpx.size(); i++)
    {
        //if out of DEM
        if (_gpx[i][0] < minX || _gpx[i][1] < minY || _gpx[i][0] > maxX || _gpx[i][1] > maxY)
        {
            error +=1;
            indicesOfPointsToRemove.push_back(i);
        }
    }

    if(indicesOfPointsToRemove.size() > 0)
    {
        if (error >= _gpx.size()*(1-minPercentageOfPointsToBeContainedWithinTheMnt))
        {
            return false;
        }

        removePoints(myMnt, indicesOfPointsToRemove);
    }

    return true;
}

string gpx::getTitle()
{
    return _title;
}

vector<vector<double> > gpx::getGpx()
{
    return _gpx;
}


void gpx::removePoints(mnt myMnt, vector<double> indicesOfPointsToRemove)
{
    double x1, y1, t1, x2, y2, t2;
    vector<double> myPoint;
    vector<vector <double> > myNewGpx;
    int j = 0;

    for(unsigned i=0; i<_gpx.size(); i++)
    {
        if(i == indicesOfPointsToRemove[j]) //if point is to be removed
        {
            x2 = _gpx[indicesOfPointsToRemove[j]][0]; //inside DEM
            y2 = _gpx[indicesOfPointsToRemove[j]][1];
            t2 = _gpx[indicesOfPointsToRemove[j]][2];

            if(indicesOfPointsToRemove[i]!=0) //DEALING WITH FIRST POINT OF GPX OUT OF DEM
            {
                x1 = _gpx[indicesOfPointsToRemove[j]-1][0]; //outside DEM
                y1 = _gpx[indicesOfPointsToRemove[j]-1][1];
                t1 = _gpx[indicesOfPointsToRemove[j]-1][2];

                myNewGpx.push_back(computeWhereOut(x1,x2,y1,y2,t1,t2,myMnt));
            }

            //while we're out of the DEM
            while(indicesOfPointsToRemove[j] < _gpx.size()-1 && indicesOfPointsToRemove[j+1] == indicesOfPointsToRemove[j]+1)
            {
                i++;
                j++;
            }

            //DEALING WITH LAST POINT
            if(indicesOfPointsToRemove[j]!=_gpx.size()-1)
            {
                x1 = _gpx[indicesOfPointsToRemove[j]+1][0]; //dans le MNT
                y1 = _gpx[indicesOfPointsToRemove[j]+1][1];
                t1 = _gpx[indicesOfPointsToRemove[j]+1][2];

                myNewGpx.push_back(computeWhereOut(x1,x2,y1,y2,t1,t2,myMnt));
            }
        }
        else
        {
            myPoint.push_back(_gpx[i][0]);
            myPoint.push_back(_gpx[i][1]);
            myPoint.push_back(_gpx[i][2]);
            myNewGpx.push_back(myPoint);
            myPoint.clear();
        }
    }
    _gpx = myNewGpx;
}

vector<double> computeWhereOut(double x1, double x2, double y1, double y2, double t1, double t2, mnt myMnt)
{
    double a, b, xOut, yOut, dist, xOut2, yOut2, dist2, total_dist;
    double** mnt2 = myMnt.getMnt();
    int sizeX = myMnt.getSizeX();
    int sizeY = myMnt.getSizeY();
    double minX = mnt2[0][0];
    double minY = mnt2[0][1];
    double maxX = mnt2[sizeX*sizeY-1][0];
    double maxY = mnt2[sizeX*sizeY-1][1];

    a=(y2-y1)/(x2-x1);//équation de droite de la forme y=ax+b entre ces deux points gpx (x1, y1,z1) et (x2, y2,z2)
    b=y1-a*x1;

    if (x1>maxX)
    {
        //find where intersection of line w/ x=maxX
        xOut = maxX;
        yOut = a*xOut+b;
        dist = sqrt(pow(x1-xOut,2.0)+pow(y1-yOut,2.0));
    }

    if (x1<minX)
    {
        //find where intersection of line w/ x=minX
        xOut2 = minX;
        yOut2 = a*xOut+b;
        dist2 = sqrt(pow(x1-xOut2,2.0)+pow(y1-yOut2,2.0));
        if (dist2<dist)
        {
            dist = dist2;
            xOut = xOut2;
            yOut = yOut2;
        }
    }

    if (y1<minY)
    {
        //find where intersection of line w/ y=minY
        yOut2 = minY;
        xOut2 = (yOut2-b)/a;
        dist2 = sqrt(pow(x1-xOut2,2.0)+pow(y1-yOut2,2.0));
        if (dist2<dist)
        {
            dist = dist2;
            xOut = xOut2;
            yOut = yOut2;
        }
    }

    if (y1>maxY)
    {
        //find where intersection of line w/ y=maxY
        yOut2 = maxY;
        xOut2 = (yOut2-b)/a;
        dist2 = sqrt(pow(x1-xOut2,2.0)+pow(y1-yOut2,2.0));
        if (dist2<dist)
        {
            dist = dist2;
            xOut = xOut2;
            yOut = yOut2;
        }
    }

    //total_dist = distBetweenPointsFollowingMnt(x1,x2,y1,y2,myMnt);
    total_dist = sqrt(pow(x1-x2,2)+pow(y1-y2,2));

    vector<double> coord;
    coord.push_back(xOut);
    coord.push_back(yOut);
    coord.push_back((distBetweenPointsFollowingMnt(x1,xOut,y1,yOut,myMnt)/total_dist)*(t2-t1)+t1);

    return coord;
}

double distBetweenPointsFollowingMnt(double x1, double x2, double y1, double y2, mnt myMnt)
{
    int taillex = myMnt.getSizeX();
    int tailley = myMnt.getSizeY();
    double** mntMnt = myMnt.getMnt();
    double cellSize = myMnt.getCellSize();

    double z1 = altitudePointGpx(x1,y1,taillex,tailley,cellSize,mntMnt);
    double z2 = altitudePointGpx(x2,y2,taillex,tailley,cellSize,mntMnt);

    vector<double> onePoint;
    vector<vector<double> > fakeGpx;

    onePoint.push_back(x1);
    onePoint.push_back(y1);
    onePoint.push_back(z1);
    onePoint.push_back(1);
    fakeGpx.push_back(onePoint);
    onePoint.clear();
    onePoint.push_back(x2);
    onePoint.push_back(y2);
    onePoint.push_back(z2);
    onePoint.push_back(2);
    fakeGpx.push_back(onePoint);
    onePoint.clear();

    double d3d = 0;
    vector<vector<double> > tmpPoints = intersectionPoints(fakeGpx,taillex,tailley,myMnt.getCellSize(),mntMnt,"DO_NOT_WRITE");
    //for each little segment, computes flat distance
    for(unsigned i=0; i<tmpPoints.size()-1; i++)
    {
        d3d += sqrt(pow(tmpPoints[i][0]-tmpPoints[i+1][0],2.0)+pow(tmpPoints[i][1]-tmpPoints[i+1][1],2.0)+pow(tmpPoints[i][2]-tmpPoints[i+1][2],2.0));
    }

    return d3d;
}

bool test_WhereOut()
{
    mnt* fakeMnt = new mnt("Data/fakeMnt.asc");
    double x1 = 12.5;
    double y1 = 40.5;
    double t1 = 6000;

    double xTheo = 13.5;
    double yTheo = 42;
    double tTheo = 6400;

    double d = sqrt(pow(yTheo-y1,2)+pow(xTheo-x1,2));
    double a = (yTheo-y1)/(xTheo-x1);
    double b = y1-a*x1;
    double a2 = a*a+1;
    double b2 = 2*(a*b-a*y1-x1);
    double c2 = x1*x1+b*b+y1*y1-2*b*y1-(9/4.)*d*d;
    double delta = b2*b2-4*a2*c2;

    double x2 = (-b+sqrt(delta))/2*a; //PEUT-ÊTRE UN MOINS;
    double y2 = a*x2+b;
    double t2 = 6600;
    bool error = true;

    vector<double> computedOut = computeWhereOut(x1, x2, y1, y2, t1, t2, *fakeMnt);

    if(computedOut[0]!=xTheo)
    {
        cout << "Error in x." << endl;
        error = false;
    }

    if(computedOut[1]!=yTheo)
    {
        cout << "Error in y." << endl;
        error = false;
    }

    if(computedOut[2]!=tTheo)
    {
        cout << "Error in t." << endl;
        error = false;
    }

    return error;
}

bool gpx::doesTimeExist()
{
    return _timeExists;
}

QString gpx::getPath()
{
    return _path;
}
