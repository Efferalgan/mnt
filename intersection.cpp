#include "intersection.h"
#include "mnt.h"
#include <vector>
#include <iostream>
#include <math.h>
using namespace std;


double altitudePointGpx(double x, double y, int taillex, int tailley, double cellSize, double** mntMnt){

    //find in which pixel and then in which triangle of the pixel is located the point (x,y)
    //return the altitude z of the point (x,y)

    //A, B, C, D are the vertices of the pixel in which is located the point (x, y); B and C are on the diagonal

    int posA, posB, posC, posD;// pos are positions of the vertices in the matrix mnt
    double dA, dB, dC, dD, z, xa, xb, xc, xd, ya, yb, yc, yd, za, zb, zc, zd;//coordinates of vertices

    //finding the pixel in which is located the point (x,y)
    posA = taillex*((int)floor((y-mntMnt[0][1])/cellSize))+(int)floor(x-mntMnt[0][0])/cellSize;//pos of A without x and y of the first pixel in the mnt

    if(x == mntMnt[0][0]+cellSize*taillex) //last column
    {
        posA -= 1;
    }
    if(y == mntMnt[0][1]+cellSize*tailley) //last line
    {
        posA -= taillex;
    }

    posB=posA+1;
    posC=posA+taillex;
    posD=posC+1;

    //after finding the pixel in which is located the point (x,y), finding in which triangle (ABC) or (BDC) is located the point (x,y)
    //distances from the point (x,y) to the vertices A, B, C, D
    dA=sqrt(pow(mntMnt[posA][0]-x,2.0)+pow(mntMnt[posA][1]-y,2.0));
    dB=sqrt(pow(mntMnt[posB][0]-x,2.0)+pow(mntMnt[posB][1]-y,2.0));
    dC=sqrt(pow(mntMnt[posC][0]-x,2.0)+pow(mntMnt[posC][1]-y,2.0));
    dD=sqrt(pow(mntMnt[posD][0]-x,2.0)+pow(mntMnt[posD][1]-y,2.0));

    xa=mntMnt[posA][0];
    ya=mntMnt[posA][1];
    za=mntMnt[posA][2];
    xb=mntMnt[posB][0];
    yb=mntMnt[posB][1];
    zb=mntMnt[posB][2];
    xc=mntMnt[posC][0];
    yc=mntMnt[posC][1];
    zc=mntMnt[posC][2];
    xd=mntMnt[posD][0];
    yd=mntMnt[posD][1];
    zd=mntMnt[posD][2];

    if (dA!=0 && dB!=0 && dC!=0 && dD!=0 ){
        if (dA<=dD) {// triangle superior (ABC)
            z=(fabs((xb-x)*(yc-y)-(xc-x)*(yb-y))*za+fabs((x-xa)*(yc-ya)-(xc-xa)*(y-ya))*zb+fabs((xb-xa)*(y-ya)-(x-xa)*(yb-ya))*zc)/(fabs((xb-xa)*(yc-ya)-(xc-xa)*(yb-ya)));
        }
        else { // triangle inferior (BDC)
            z=(fabs((xb-x)*(yc-y)-(xc-x)*(yb-y))*zd+fabs((x-xd)*(yc-yd)-(xc-xd)*(y-yd))*zb+fabs((xb-xd)*(y-yd)-(x-xd)*(yb-yd))*zc)/(fabs((xb-xd)*(yc-yd)-(xc-xd)*(yb-yd)));
        }

    }
    //if the point (x,y) is on a vertex
    else if (dA==0) z=mntMnt[posA][2];
    else if (dB==0) z=mntMnt[posB][2];
    else if (dC==0) z=mntMnt[posC][2];
    else if (dD==0) z=mntMnt[posD][2];

    return z;
}

double altitudePointIntersection(double x,double y,double x1,double y1,double z1,double x2,double y2, double z2){

    //Calculating the point altitude with barycentres method
    //Deprecated function

    double d1,d2,z;
    if (x1!=x2){
        if(x==x1 && y==y1){return z1;}
        if(x==x2 && y==y2){return z2;}

        d1=1/sqrt(pow(x1-x,2.0)+pow(y1-y,2.0));
        d2=1/sqrt(pow(x2-x,2.0)+pow(y2-y,2.0));

        z=(z1*d1+z2*d2)/(d1+d2);}
    else{
        z=z1+((y-y1)/(y2-y1))*(z2-z1);
    }

    return z;

}

vector<vector<double> > ajoutPoint(vector <vector<double> > &vect, double x, double y, double z, double t){

    vector<double> tab;
    tab.push_back(x);
    tab.push_back(y);
    tab.push_back(z);
    tab.push_back(t);
    vect.push_back(tab);
    tab.clear();
    return vect;

}


vector<vector<double> > intersectionPoints(const vector<vector<double> > & gpx, int taillex, int tailley, double cellSize, double** mntMnt, QString gpxPath){

    QString interPath = QString();
    if(gpxPath!="DO_NOT_WRITE")
    {
        QStringList splitPath = gpxPath.split("."); //CREATION OF THE ISOHYPSE FILE PATH
        for(int i=0;i<splitPath.length()-1;i++)
        {
            interPath+=splitPath[i];
        }
        interPath+=".intersection";

        string interPath2 = qstring2string(interPath);
        //TESTING FOR EXISTENCE
        struct stat buffer;
        if(stat (interPath2.c_str(), &buffer) == 0) //if file exists
        {
            return readIntersectionFromFile(interPath);
        }
    }

    vector <vector<double> > matIntersection;// intersection and gpx points; sorted
    vector <vector<double> > matIntersectionTemporaire;// intersection and gpx points; not sorted
    vector<int> tabDoublons;
    int nbrePointsGpx=gpx.size();

    double x,x1,x2,y,y1,y2,z,z1, a, b, bdiag, t1, t2,t,d3d, v, x0, y0, ve, h, va, ha;
    int pos;

    x0 = mntMnt[0][0];
    y0 = mntMnt[0][1];

    // STEP 1: calculating coordinates of intersection points between gpx points i and i+1
    //cout << "STEP 1" << endl;

    for (int i=0; i<nbrePointsGpx-1; i++){
        cout << "point n°" << i+1 << "/" << nbrePointsGpx << endl;
        x1=gpx[i][0];//coordinates of point gpx i
        y1=gpx[i][1];
        z1=altitudePointGpx(x1, y1, taillex, tailley, cellSize, mntMnt);
        t1=gpx[i][2];

        x2=gpx[i+1][0]; //coordinates of point gpx i+1
        y2=gpx[i+1][1];
        //z2=altitudePointGpx(x2,y2,taillex, tailley, cellSize, mntMnt);
        t2=gpx[i+1][2];

        //calculating the 2D distance between points gpx i and i+1
        d3d=sqrt(pow(x1-x2,2.0)+pow(y1-y2,2.0));
        //calculating the speed between points gpx i and i+1, speed is supposed constant
        if(t2!=t1) v=d3d/(t2-t1);

        //adding the gpx point i to the matrix not sorted
        ajoutPoint(matIntersectionTemporaire, x1,y1,z1,t1);

        if (x2!=x1) {
            //segment equation y=ax+b between the two gpx points i and i+1 (x1, y1,z1) et (x2, y2,z2)
            a=(y2-y1)/(x2-x1);
            b=y1-a*x1;

            //intersection with vertical lines of the mnt
            for (int veBis=(int)ceil(min(x1-x0,x2-x0));veBis<(int)ceil(max(x1-x0,x2-x0));veBis++){
                //cout << "vertical n°" << veBis+1 << "/" << (int)ceil(max(x1-x0,x2-x0)) << endl;
                ve = veBis+x0;
                x=(double)ve;
                y=a*ve+b;

                if (x!=x2 && y!=y2 && x!=x1 && y!=y1) {
                    z=altitudePointGpx(x,y,taillex, tailley, cellSize, mntMnt);
                    if (t1!=t2) t=t1+sqrt(pow(x1-x,2.0)+pow(y1-y,2.0))/v;
                    else t=t1;
                    ajoutPoint(matIntersectionTemporaire,x,y,z,t);
                }
                if (x!=x2 && x!=x1 && y2==y1) { //GPX horizontal
                    y=y1;
                    z=altitudePointGpx(x,y,taillex, tailley, cellSize, mntMnt);
                    if (t1!=t2) t=t1+fabs(x1-x)/v;
                    else t=t1;
                    ajoutPoint(matIntersectionTemporaire,x,y,z,t);

                }
            }
            //intersection with horizontal lines of the mnt
            for (int hBis=(int)ceil(min(y1-y0,y2-y0));hBis<(int)ceil(max(y1-y0,y2-y0));hBis++){

                //cout << "horizontal n°" << hBis+1 << "/" << (int)ceil(max(y1-y0,y2-y0)) << endl;
                h = hBis+y0;
                x=(h-b)/a;
                y=(double)h;
                z=altitudePointGpx(x,y,taillex, tailley, cellSize, mntMnt);
                if (t1!=t2) t=t1+sqrt(pow(x1-x,2.0)+pow(y1-y,2.0))/v;
                else t=t1;

                if (y!=y1 && y!=y2) ajoutPoint(matIntersectionTemporaire,x,y,z,t);

            }

            //intersection with diagonals of the mnt. Their slope equals -1
            //diagonals that we look for are in the recatngle fabs(x1-x2) et fabs(y1-y2)

            if (a!=-1) //diagonals of the mnt and segment not parallel
            {
                //cout << "début diag non parallèles" << endl;
                for (int vaBis=(int)floor(min(x1-x0,x2-x0)); vaBis<(int)ceil(max(x1-x0,x2-x0));vaBis++)
                {
                    //cout << "vaBis " << vaBis << "/" << (int)ceil(max(x1-x0,x2-x0)) << endl;
                    va = vaBis + x0;

                    for(int haBis=(int)floor(min(y1-y0,y2-y0));haBis<(int)ceil(max(y1-y0,y2-y0));haBis++)
                    {
                        //cout << "vaBis " << vaBis << "/" << (int)ceil(max(x1-x0,x2-x0)) << endl;
                        //if (haBis%200==0){cout << "haBis " << haBis << "/" << (int)ceil(max(y1-y0,y2-y0)) << endl;}
                        ha = haBis + y0;
                        bdiag = ha+1 - (-1)*va;

                        //coordinates of the intersection
                        x=(b-bdiag)/(-1-a);
                        y=-1*x+bdiag;

                        //check that the intersection point found is on the segment delimited by the two gpx points i and i+1
                        if (x>=min(x1,x2) and x<=max(x1,x2) && y>=min(y1,y2) && y<=max(y1,y2) ){

                            z=altitudePointGpx(x,y,taillex, tailley, cellSize, mntMnt);
                            if (t1!=t2) t=t1+sqrt(pow(x1-x,2.0)+pow(y1-y,2.0))/v;
                            else t=t1;
                            ajoutPoint(matIntersectionTemporaire,x,y,z,t);
                        }
                    }
                }
                //cout << "fin diag" << endl;
            }
            else
            {
                //cout << " début diag parallèles" << endl;
                double xtmp = floor(min(x1-x0,x2-x0));
                double ytmp = ceil(max(y1-y0,y2-y0));
                if(ytmp+xtmp == b)
                {
                    // merged with diagonal
                    for(int verti = (int)ceil(min(x1-x0,x2-x0));verti < (int)ceil(max(x1-x0,x2-x0));verti++)
                    {
                        x = verti+x0;
                        y = -x+b;
                        z = altitudePointGpx(x,y,taillex, tailley, cellSize, mntMnt);
                        if (t1!=t2) t=t1+sqrt(pow(x1-x,2.0)+pow(y1-y,2.0))/v;
                        else t=t1;
                        ajoutPoint(matIntersectionTemporaire,x,y,z,t);

                    }
                }
                //cout << "fin diag parallèles" << endl;
            }
        }


        else //if the segment made of the gpx points i and i+1 is vertical (ie x1==x2)
        {
            //cout << "vertical segment" << endl;

            double tmpTruc = fmod(x1-mntMnt[0][0],cellSize);
            x=x1;
            //merged with a vertical line of the mnt
            if (tmpTruc==floor(tmpTruc)){
                //cout << "merged" << endl;

                for(int truc = floor(min(y1-y0,y2-y0));truc < ceil(max(y1-y0,y2-y0));truc++)
                {
                    //cout << "line n°" << truc+1 << "/" << ceil(max(y1-y0,y2-y0)) <<  endl;
                    y = truc+y0;
                    if(y<max(y1,y2) && y>min(y1,y2))
                    {
                        z=altitudePointGpx(x,y,taillex, tailley, cellSize, mntMnt);
                        if (t1!=t2) t=t1+sqrt(pow(x1-x,2.0)+pow(y1-y,2.0))/v;
                        else t=t1;
                        ajoutPoint(matIntersectionTemporaire,x,y,z,t);
                    }
                }

            } else {
                //cout << "not merged" << endl;

                //intersection with horizontal lines of the mnt
                for (int hBis=(int)ceil(min(y1-y0,y2-y0));hBis<(int)ceil(max(y1-y0,y2-y0));hBis++){
                    //cout << "vertical meeting horizontal n°" << hBis+1 << "/" << (int)ceil(max(y1-y0,y2-y0)) << endl;
                    h = hBis+y0;
                    y=h;

                    if (y!=y1){
                        z=altitudePointGpx(x,y,taillex, tailley, cellSize, mntMnt);
                        if (t1!=t2) t=t1+sqrt(pow(x1-x,2.0)+pow(y1-y,2.0))/v;
                        else t=t1;
                        ajoutPoint(matIntersectionTemporaire,x,y,z,t);
                    }
                }

                //intersection with diagonals of the mnt
                //cout << (int)ceil(max(y1,y2))+1<<endl;
                for (int hBis=(int)floor(min(y1-y0,y2-y0)); hBis<(int)ceil(max(y1-y0,y2-y0))+1;hBis++){
                    //cout << "vertical meeting diag n°" << hBis+1 << "/" << (int)ceil(max(y1-y0,y2-y0)) << endl;
                    ha = hBis + y0;
                    bdiag = ha+1 - (-1)*va;

                    x=(bdiag-b)/(1+a);
                    y=-1*x+bdiag;

                     //check that the intersection point found is on the segment delimited by the two gpx points i and i+1
                    if (x>=min(x1,x2) and x<=max(x1,x2) && y>=min(y1,y2) && y<=max(y1,y2) ){

                        z=altitudePointGpx(x,y,taillex, tailley, cellSize, mntMnt);
                        if (t1!=t2) t=t1+sqrt(pow(x1-x,2.0)+pow(y1-y,2.0))/v;
                        else t=t1;
                        ajoutPoint(matIntersectionTemporaire,x,y,z,t);
                    }
                }
            }
        }

        //add the gpx point i to the final matrix
        ajoutPoint(matIntersection,x1,y1,z1,t1);
        matIntersectionTemporaire.erase(matIntersectionTemporaire.begin()+1);

         // STEP 2: sorting intersection points and suppressing duplicates
        //cout << "STEP 2" << endl;
        if (matIntersectionTemporaire.size()>1){
            vector <vector <double> > distance;


            for (unsigned int vect=0; vect<matIntersectionTemporaire.size(); vect++){
                //cout << "point n°" << vect << "/" << matIntersectionTemporaire.size() << endl;

                // calculation of the distance between the intersection points and the point gpx i
                // distance contains all distances
                a=pow(x1-matIntersectionTemporaire[vect][0],2.0)+pow(y1-matIntersectionTemporaire[vect][1],2.0);
                bool exists = false;
                for (unsigned int vect2=1; vect2<distance.size(); vect2++){
                    if (fabs(distance[vect2][0]-floor(a))<0.0010) exists=true;
                }
                if (!exists){
                    vector <double> distPoint;
                    distPoint.push_back(a);
                    distPoint.push_back(double(vect));
                    distance.push_back(distPoint);
                    distPoint.clear();
                }
            }

            int huge = cellSize*taillex*tailley*1000+1;
            int tailledist=distance.size();

            while(true){
                b = huge;
                pos=-1;

                for (int vect=0; vect<tailledist; vect++){
                    //lloking for the shortest distance from the gpx point i
                    if (distance[vect][0]<b && distance[vect][1] != -1){
                        b=distance[vect][0];
                        pos=distance[vect][1];
                    }
                }
                for (int vect=0; vect<tailledist; vect++){
                    if (fabs(distance[vect][0]-b) < 0.01){
                        //if two distances are equal
                        distance[vect][1] = -1; //suppressing duplicates
                    }
                }

                if(pos==-1){break;}
                matIntersection.push_back(matIntersectionTemporaire[pos]);
            }
        distance.clear();
        }

        matIntersectionTemporaire.clear();
    }

    //STEP 3: adding the last gpx point to the matrix

    //cout << "STEP 3" << endl;

    x=gpx[nbrePointsGpx-1][0];
    y=gpx[nbrePointsGpx-1][1];
    z=altitudePointGpx(x,y,taillex, tailley, cellSize, mntMnt);
    t=gpx[nbrePointsGpx-1][2];
    ajoutPoint(matIntersection,x,y,z,t);


    //final check of the final matrix to look for uncorrect values
    int taille= matIntersection.size()-1;
    for (int i=0; i<taille; i++){
        //cout << "point n°" << i << "/" << taille << endl;
        if ((fabs(matIntersection[i][0]-matIntersection[i+1][0]) > 2) ){
            matIntersection.erase(matIntersection.begin()+i+1);
            taille=taille-1;
            i--;
        }
    }

    if(gpxPath!="DO_NOT_WRITE"){writeIntersectionToFile(interPath, matIntersection);}

    return matIntersection;
}

void writeIntersectionToFile(QString path, vector<vector<double> > matIntersection)
{
    QFile file(path);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    out.setRealNumberPrecision(25);

    for(unsigned i=0;i<matIntersection.size();i++)
    {
        out << matIntersection[i][0] << " " << matIntersection[i][1] << " " << matIntersection[i][2] << " " << matIntersection[i][3] << "\n";
    }
}

vector<vector<double> > readIntersectionFromFile(QString path)
{
    cout << "reading intersections..." << endl;
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);

    vector<vector<double> > myIntersection;
    QStringList fields;
    vector<double> line;

    while(true)
    {
        fields = in.readLine().split(" ",QString::SkipEmptyParts);
        if(fields.length() == 0)
        {
            break; //if eof
        }
        line.push_back(fields[0].toDouble());
        line.push_back(fields[1].toDouble());
        line.push_back(fields[2].toDouble());
        line.push_back(fields[3].toDouble());
        myIntersection.push_back(line);
        line.clear();
    }

    cout << "intersections read" << endl;

    return myIntersection;

}
