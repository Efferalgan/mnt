#include "misc.h"

using namespace std;

//date in seconds since 01/01/2000
double timeString2secondsSinceEpoch(QString timeString)
{
    QDateTime time = QDateTime::fromString(timeString,"yyyy-MM-dd'T'hh:mm:ss'Z'");
    QDateTime firstJan2000 = QDateTime::fromString("2000-01-01","yyyy-MM-dd");
    double seconds = (time.toMSecsSinceEpoch()-firstJan2000.toMSecsSinceEpoch())/1000;
    return seconds;
}

vector< vector<double> > wgs2l93(vector< vector<double> > wgsVector)
{
    vector< vector<double> > l93Vector;
    vector<double> tmp;
    double phi, lambda, h;

    const char *wgs84 = "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs";

    //lambert 93
    const char *lambert = "+proj=lcc +lat_1=44 +lat_2=49 +lat_0=46.5 +lon_0=3 +x_0=700000 +y_0=6600000 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs";

    projPJ source = pj_init_plus(wgs84);
    projPJ target = pj_init_plus(lambert);

    for (unsigned int i=0;i<wgsVector.size();i++)
    {
        lambda = wgsVector[i][0]*DEG_TO_RAD;
        phi = wgsVector[i][1]*DEG_TO_RAD;
        h = wgsVector[i][2];

        pj_transform(source, target, 1, 1, &phi, &lambda, &h);

        //phi *= RAD_TO_DEG;
        //lambda *= RAD_TO_DEG;

        //cout << success << " " << phi << " " << lambda << endl;

        tmp.push_back(phi);
        tmp.push_back(lambda);
        tmp.push_back(wgsVector[i][3]);
        l93Vector.push_back(tmp);
        tmp.clear();
    }

    return l93Vector;
}

string qstring2string(QString qtext)
{
    return qtext.toUtf8().constData();
}

double** toArray(std::vector<std::vector<double> > &myVector)
{
    double** myArray;
    int N = myVector.size();
    int M = myVector[0].size();
    myArray = new double*[N];
    for(int i=0; i < N; i++)
    {
        myArray[i] = new double[M];
        for(int j=0; j < M; j++)
        {
              myArray[i][j] = myVector[i][j];
        }
    }
    return myArray;
}

double* toArray(vector<double> &myVector)
{
    int M = myVector.size();
    double* myArray = new double[M];
    for(int i=0; i<M; i++)
    {
          myArray[i] = myVector[i];
    }

    return myArray;
}

vector<vector<double> > retourne(vector<vector<double> > myVector)
{
    vector<vector<double> > tmpVector;
    int mySize = myVector.size();

    for(int i=0;i<mySize;i++)
    {
        tmpVector.push_back(myVector[i]);
    }

    for(int i=0;i<mySize;i++)
    {
        myVector[i] = tmpVector[mySize-i-1];
    }

    return myVector;
}

void concatenate(vector<vector<double> >* vect1, vector<vector<double> >* vect2 )
{
    for(unsigned i=0;i<vect2->size();i++)
    {
        vect1->push_back((*vect2)[i]);
    }
}

vector<vector<double> > concatenate(vector<vector<double> > vect1, vector<vector<double> > vect2 )
{
    for(unsigned i=0;i<vect2.size();i++)
    {
        vect1.push_back(vect2[i]);
    }

    return vect1;
}

double distCalc(double x1, double y1, double x2, double y2)
{
    return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}
