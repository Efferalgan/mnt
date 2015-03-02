#include "mnt.h"

using namespace std;

mnt::mnt(){}

mnt::mnt(QString path, bool computeIsohypses)
{
    bool readTheSpecifiedFile = true;

    if(computeIsohypses)
    {
        QStringList splitPath = path.split("."); //checking the extension
        QString ascPath;

        if(splitPath[splitPath.length()-1] == "xyz") //if .xyz, checks if a .asc of the same DEM exists
        {
            QString extensionlessPath = QString();
            for(int i=0;i<splitPath.length()-1;i++)
            {
                extensionlessPath+=splitPath[i];
            }

            ascPath = extensionlessPath+".asc";
            string ascPath2 = qstring2string(ascPath);
            //TESTING FOR EXISTENCE
            struct stat buffer;
            if(stat (ascPath2.c_str(), &buffer) == 0) //if file exists
            {
                _path = ascPath;
                readTheSpecifiedFile = false;
            }
        }
    }

    if(readTheSpecifiedFile){_path = path;}

    _mnt = fromFile();
    //resizeMnt(250);
    if(computeIsohypses)
    {
        update();
    }
}

mnt::mnt(mnt &copy)
{
    _sizeX = copy.getSizeX();
    _sizeY = copy.getSizeY();
    _nodataValue = copy.getNodataValue();
    _cellSize = copy.getCellSize();
    _numberOfIsohypses = copy.getNumberOfIsohypses();
    _altitudesOfIsohypses = copy.getAltitudesOfIsohypses();
    _isohypses = copy.getIsohypses();
    _triangles = copy.getTriangles();
    _path = copy.getPath();

    double **myMnt = new double* [_sizeX*_sizeY];
    double **mnt2copy = copy.getMnt();

    for(int i=0;i<_sizeX*_sizeY;i++) //line by line
    {
        myMnt[i] = new double[3];
        myMnt[i][0] = mnt2copy[i][0];
        myMnt[i][1] = mnt2copy[i][1];
        myMnt[i][2] = mnt2copy[i][2];
    }

    _mnt = myMnt;
}

mnt::mnt(double** mntMnt, int sizeX, int sizeY, double nodataValue, double cellSize, int numberOfIsohypses, double* altitudeOfIsohypses)
{
    _sizeX = sizeX;
    _sizeY = sizeY;
    _nodataValue = nodataValue;
    _cellSize = cellSize;
    _numberOfIsohypses = numberOfIsohypses;
    _altitudesOfIsohypses = altitudeOfIsohypses;

    double **myMnt = new double* [_sizeX*_sizeY];

    for(int i=0;i<_sizeX*_sizeY;i++) //line by line
    {
        myMnt[i] = new double[3];
        myMnt[i][0] = mntMnt[i][0];
        myMnt[i][1] = mntMnt[i][1];
        myMnt[i][2] = mntMnt[i][2];
    }

    _mnt = myMnt;
}

void mnt::update()
{
    QStringList splitPath = _path.split("."); //CREATION OF THE ISOHYPSE FILE PATH
    QString isoPath;
    QString extensionlessPath = QString();
    for(int i=0;i<splitPath.length()-1;i++)
    {
        extensionlessPath+=splitPath[i];
    }
    isoPath = extensionlessPath+".xml";

    string isoPath2 = qstring2string(isoPath);
    //TESTING FOR EXISTENCE
    struct stat buffer;
    if(stat (isoPath2.c_str(), &buffer) == 0) //if file exists
    {
        readIsohypsesFromFile(isoPath);
        cout << "isohypses file read" << endl;
    } else
    {
        cout << "file read" << endl;
        setAltitudesOfIsohypses();
        /*cout << "alti set" << endl << "making triangles" << endl;
        triangles();
        cout << "triangles made" << endl;
        makeIsohypses();
        cout << "isohypses made" << endl;*/
        cout << "alti set" << endl;

        isohypsesFromPoints(intersectionPointsZ());

        writeIsohypsesToFile(isoPath);
    }

    if(splitPath[splitPath.length()-1] == "xyz") //if .xyz, checks if a .asc of the same DEM exists
    {
        QString ascPath = extensionlessPath+".asc";
        string ascPath2 = qstring2string(ascPath);
        //TESTING FOR EXISTENCE
        struct stat buffer;
        if(stat (ascPath2.c_str(), &buffer) != 0) //if file does not exist
        {
            xyz2asc();
        }
    }
}

mnt::~mnt()
{
}

double** mnt::fromFile()
{
    QFile file(_path);
    file.open(QIODevice::ReadOnly);

    QTextStream in(&file);

    QString fileType = _path.split(".", QString::SkipEmptyParts)[1];

    if(fileType == "asc")
    {
        double xlltopleft, ylltopleft, xllcenter, yllcenter;
        bool iscenter= true;

        //HEADER
        QString line = in.readLine();
        int ncols = line.split(" ", QString::SkipEmptyParts)[1].toInt();
        line = in.readLine();
        int nrows = line.split(" ", QString::SkipEmptyParts)[1].toInt();
        line = in.readLine();
        if(line.split(" ", QString::SkipEmptyParts)[0] == "xllcorner")
        {
            xlltopleft = line.split(" ", QString::SkipEmptyParts)[1].toDouble();
            line = in.readLine();
            ylltopleft = line.split(" ", QString::SkipEmptyParts)[1].toDouble();
            iscenter = false;
        } else //xllcenter
        {
            xllcenter = line.split(" ", QString::SkipEmptyParts)[1].toDouble();
            line = in.readLine();
            yllcenter = line.split(" ", QString::SkipEmptyParts)[1].toDouble();
        }
        line = in.readLine();
        double cellsize = line.split(" ", QString::SkipEmptyParts)[1].toDouble();
        line = in.readLine();
        double nodata_value = line.split(" ", QString::SkipEmptyParts)[1].toDouble();

        if(iscenter)
        {
            xlltopleft = xllcenter-cellsize*(ncols-1.)/2;
            ylltopleft = yllcenter-cellsize*(nrows-1.)/2;
        }

        _cellSize = cellsize;
        _sizeX = ncols;
        _sizeY = nrows;
        _nodataValue = nodata_value;

        //MATRIX
        double **myMnt = new double* [ncols*nrows];
        QStringList fields;
        double y;
        int index;

        for(int i=0;i<nrows;i++) //reading line by line
        {
            line = in.readLine();
            fields = line.split(" ",QString::SkipEmptyParts);
            y = i*cellsize+ylltopleft;
            for(int j=0;j<ncols;j++)
            {
                index = ncols*i+j;
                myMnt[index] = new double[3];
                myMnt[index][0] = j*cellsize+xlltopleft;
                myMnt[index][1] = y;
                myMnt[index][2] = fields[j].toDouble();
            }
        }      

        file.close();
        return myMnt;

    }else if(fileType == "xyz")
    {
        vector< vector<double> > myMntVector;
        QStringList fields;
        vector<double> line;
        cout << "Reading MNT file" << endl;
        int nbLines=0;
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
            myMntVector.push_back(line);
            line.clear();
            nbLines++;
        }

        cout << "Converting to array" << endl;

        double **myMnt = toArray(myMntVector);

        _sizeX = tailleX(myMnt);
        _sizeY = nbLines/_sizeX; //tailleY(myMnt,_sizeX);
        _cellSize = tailleCellule(myMnt);
        _nodataValue = -99999;
        cout << "Reversing" << endl;

        double** myReversedMnt = reverseY(myMnt,_sizeX, _sizeY);

        cout << "Loaded. Ready to go!" << endl;
        return myReversedMnt;
    }
    cout << "Extension non reconnue" << endl;

    vector< vector<double> > myMntVector;
    vector<double> line;
    line.push_back(0);
    myMntVector.push_back(line);

    return toArray(myMntVector);
}

int mnt::tailleX(double **mnt1){

    int elem=0;
    double y=mnt1[0][1];
    do{
        elem=elem+1;
        y=mnt1[elem][1];
    }while (y==mnt1[0][1]);

    return elem;// taille x du mnt
}

int mnt::tailleY(double **mnt1, double x){

    int y;
    int nbre_points=0;//nombre de points dans le mnt1
    while (mnt1[nbre_points]!=NULL)
    {
            nbre_points+=1;
    }

    y=nbre_points/x;

    return y;// taille y du mnt
}

double mnt::tailleCellule(double **mnt1)
{
    double cellSize = max(abs(mnt1[0][0]-mnt1[1][0]),abs(mnt1[0][1]-mnt1[1][1]));
    return cellSize;
}

int mnt::getSizeX()
{
    return _sizeX;
}

int mnt::getSizeY()
{
    return _sizeY;
}

double mnt::getCellSize()
{
    return _cellSize;
}

int mnt::getNodataValue()
{
    return _nodataValue;
}

double** mnt::getMnt()
{
    return _mnt;
}

vector<vector<vector<double> > > mnt::getTriangles()
{
    return _triangles;
}

QString mnt::getPath()
{
    return _path;
}

double** mnt::reverseY(double** oldMnt, int sizeX, int sizeY)
{
    double** newMnt = new double* [sizeX*sizeY];
    int a, b, c, d, newSizeY;
    newSizeY = sizeY - 1;
    //How the thing is reversed: new[sizeX*i+j][k] = old[(sizeY-i-1)*sizeX+j][k]

    for(int i=0; i<sizeY; i++)
    {
        a = sizeX*i;
        b = (newSizeY-i )*sizeX;
        for(int j=0; j<sizeX; j++)
        {
            c = a+j;
            d = b+j;
            newMnt[c] = new double[3];
            for(unsigned k=0; k<3; k++)
            {
                newMnt[c][k] = oldMnt[d][k];
            }
        }
    }

    return newMnt;
}

void mnt::print()
{
    int Y;
    for(int y=0;y<_sizeY;y++)
    {
        Y = y*_sizeX;
        for(int x=0;x<_sizeX;x++)
        {
            cout << _mnt[x+Y][2] << " ";
        }
        cout << endl;
    }

}

void mnt::resizeMnt(int newSize)
{
    if(newSize<1 || newSize>=_sizeX){return;}

    double** myMnt = new double* [newSize*newSize];
    int tmp;
    for(int i=0;i<newSize*newSize;i++)
    {
        tmp = _sizeX*(int)floor(i/newSize)+i%newSize;
        myMnt[i] = new double[3];
        myMnt[i][0] = _mnt[tmp][0];
        myMnt[i][1] = _mnt[tmp][1];
        myMnt[i][2] = _mnt[tmp][2];
    }
    _mnt = myMnt;
    _sizeX = newSize;
    _sizeY = newSize;
}

void mnt::setAltitudesOfIsohypses()
{
    int approximateNumberOfIsohypsesWeWant = 15;
    int possibleSteps[] = {1,5,10,15,20,25,50,75,100,150,200,250,500};
    int sizeOfPossibleSteps = 13;

    double* minmax = getMinMaxZ();
    double minZ = minmax[0];
    double maxZ = minmax[1];

    double delta = maxZ-minZ;
    double minValue = abs(delta/possibleSteps[0]-approximateNumberOfIsohypsesWeWant); //the smallest difference between the number of isohypses obtained with this step, and the number of isohypses we want
    double value;
    int indexOfStep = 0;

    for(int i=1;i<sizeOfPossibleSteps;i++)
    {
        value = abs(delta/possibleSteps[i]-approximateNumberOfIsohypsesWeWant);
        if(value<minValue)
        {
            minValue = value;
            indexOfStep = i;
        }
    }

    int step = possibleSteps[indexOfStep];
    //cout << "step: " << step << endl;

    int firstIsohypse = (floor(minZ/step)+1)*step; //take the first altitude alt = k*step, with alt > Zmin

    _numberOfIsohypses = ceil(delta/step);
    if (firstIsohypse+step*(_numberOfIsohypses-1)==maxZ){_numberOfIsohypses-=1;} //if last isohypse altitude == exactly the Zmax, remove it

    double* listOfZ = new double[_numberOfIsohypses];

    for(int i=0;i<_numberOfIsohypses;i++)
    {
        listOfZ[i] = firstIsohypse+step*i;
    }

    _altitudesOfIsohypses = listOfZ;
}

double* mnt::getMinMaxZ()
{
    int length = _sizeX*_sizeY;  //get size of array
    double maxZ = _mnt[0][2];
    double minZ = _mnt[0][2];
    double elem;

    //GET MIN AND MAX Z
    for(int i=1; i<length; i++)
    {
        elem = _mnt[i][2];
        if(elem > maxZ){maxZ = _mnt[i][2];}
        if(elem < minZ){minZ = _mnt[i][2];}
    }

    double* minmax = new double[2];
    minmax[0] = minZ;
    minmax[1] = maxZ;
    return minmax;
}

int mnt::getNumberOfIsohypses()
{
    return _numberOfIsohypses;
}

double* mnt::getAltitudesOfIsohypses()
{
    return _altitudesOfIsohypses;
}

void mnt::ajoutPointZ(vector<vector<double> > &vect, double x, double y, double z){
    //fonction d'ajout d'un point (x,y,z) à un vecteur de points

    vector<double> tab;
    tab.push_back(x);
    tab.push_back(y);
    tab.push_back(z);
    vect.push_back(tab);
}

vector<vector<double> > mnt::intersectionPointsZ()
{
// fonction qui retourne l'ensemble des points en désordre de toutes les isohypses

    vector<vector<double> > matIntersectionZ;// tableau des points d'intersection triés
    //cout<<"taille du tableau tab des z fixees : "<<_numberOfIsohypses<<endl;

    double x, y, z;
    double za, zb, zc; //altitudes des trois sommets du triangle supérieur de chaque pixel du mnt

    int nbrepixels=_sizeX*_sizeY;//nombre de pixels total dans le mnt

    for (int s=0;s<nbrepixels-_sizeX;s++){//pour chaque sommet s

        if ((s+1)%_sizeX !=0){ //ni bordure droite, ni inférieure
            za=_mnt[s][2];
            zb=_mnt[s+1][2];
            zc=_mnt[s+_sizeX][2];

            for (int elem=0; elem<_numberOfIsohypses; elem++){
                z=_altitudesOfIsohypses[elem];
                double xa=_mnt[s][0];
                double ya=_mnt[s][1];
                //double xc=_mnt[s+_sizeX][0];
                double yc=_mnt[s+_sizeX][1];
                double xb=_mnt[s+1][0];
                //double yb=_mnt[s+1][1];

                if (z>min(za, zb) && z<max(za, zb)) {// AB coté supérieur horizontal d'un pixel
                    x=xa+((z-za)/(zb-za))*(xb-xa);
                    y=ya;
                    ajoutPointZ(matIntersectionZ, x,y,z);
                }

                if (z>min(za, zc) && z<max(za, zc)) {// AC coté gauche vertical d'un pixel
                    x=xa;
                    y=ya+(z-za)/(zc-za)*(yc-ya);
                    ajoutPointZ(matIntersectionZ, x,y,z);
                }
                if (z>min(zb, zc) && z<max(zb, zc)) {// BC diagonale d'un pixel

                    x=xa+(z-zc)/(zb-zc)*(xb-xa);
                    y=yc+(x-xa)/(xb-xa)*(ya-yc);
                    ajoutPointZ(matIntersectionZ, x,y,z);
                }

                if (z==za){//en cas de plat, un seul point est créé, celui correspondant au sommet A
                    x=xa;
                    y=ya;
                    ajoutPointZ(matIntersectionZ, x,y,z);
                }
            }
        }

        else if ((s+1)%_sizeX ==0){ //bordure latérale droite du mnt

            za=_mnt[s][2];
            zc=_mnt[s+_sizeX][2];

            for (int elem=0; elem<_numberOfIsohypses; elem++){
                z=_altitudesOfIsohypses[elem];

                if (z>min(za, zc) && z<max(za, zc)) {
                    x=_mnt[s][0];
                    y=_mnt[s][1]+ (_mnt[s+_sizeX][1]-_mnt[s][1])*(z-za)/(zc-za);
                    ajoutPointZ(matIntersectionZ, x,y,z);
                }

                if (z==za){
                    x=_mnt[s][0];
                    y=_mnt[s][1];
                    ajoutPointZ(matIntersectionZ, x,y,z);
                }
            }
        }
    }

    for (int s=nbrepixels-_sizeX; s<nbrepixels; s++){//bordure inférieure horizontale du mnt

        if (s<nbrepixels-1){
            za=_mnt[s][2];
            zb=_mnt[s+1][2];

            for (int elem=0; elem<_numberOfIsohypses; elem++){
                z=_altitudesOfIsohypses[elem];
                if (z>min(za, zb) && z<max(za, zb)) {

                    x=_mnt[s][0]+(_mnt[s+1][0]-_mnt[s][0])*((z-za)/(zb-za));
                    y=_mnt[s][1];
                    //y=_mnt[s][1]+fabs(_mnt[s][1]-_mnt[s+1][1])*(fabs((z-za)/(zb-za)));
                    ajoutPointZ(matIntersectionZ, x,y,z);
                }
                if (z==za){
                    x=_mnt[s][0];
                    y=_mnt[s][1];
                    ajoutPointZ(matIntersectionZ, x,y,z);
                }
            }
        }
        if (s==nbrepixels-1){//dernier sommet du mnt en bas à droite
            for (int elem=0; elem<_numberOfIsohypses; elem++){
                z=_altitudesOfIsohypses[elem];
                if (z==_mnt[s][2]){
                    x=_mnt[s][0];
                    y=_mnt[s][1];
                    ajoutPointZ(matIntersectionZ,x,y,z);
                }
            }
        }
    }
    int compte=0;
    for (unsigned i=0;i<matIntersectionZ.size();i++){
        compte+=1;
        //cout<<matIntersectionZ[i][0]<<" "<<matIntersectionZ[i][1]<<" "<<matIntersectionZ[i][2]<<endl;
    }
    return matIntersectionZ;
}

void mnt::isohypsesFromPoints(vector<vector<double> > vectOfPoints)
{
    vector<vector<vector<double> > > vectorsOfPoints = classByAlti(vectOfPoints,_altitudesOfIsohypses,_numberOfIsohypses);
    cout << "classed by alti" << endl;
    vector<double> myPoint, currentPoint; //a point
    vector<vector<double> > myIsohypse; //one isohypse for a given z
    vector<vector<vector<double> > > myIsohypseLevel; //all the isohypses for a given z
    vector<vector<vector<vector<double> > > > allTheIsohypses; //all the isohypses for all the z;
    double distance;
    int a, b, k, length, indexOfMin;

    for(unsigned i=0;i<vectorsOfPoints.size();i++)  //for each z
    {
        length = vectorsOfPoints[i].size();
        if(length < 2){continue;}
        cout << "isohypse n°" << i+1 << endl;
        while(true) //for all the disconnected isohypses for the same z
        {
            k = 0;
            while(vectorsOfPoints[i][k][0] == _nodataValue and k<length-1){k++;} //get the first non deleted point

            currentPoint = vectorsOfPoints[i][k];

            if (currentPoint[0]==_nodataValue){break;} //if all points have already been removed

            //Adding the current point to the current isohypse
            myPoint.push_back(currentPoint[0]);
            myPoint.push_back(currentPoint[1]);
            myPoint.push_back(currentPoint[2]);
            myIsohypse.push_back(myPoint);
            myPoint.clear();

            a = k;
            b = 1;
            while(indexOfMin != k) //for each connected set of points
            {
                double minDist = _cellSize*sqrt(2);
                double x1 = currentPoint[0];
                double y1 = currentPoint[1];
                indexOfMin = -1;

                for(int j=0;j<length;j++)
                {
                    if (vectorsOfPoints[i][j][0] != _nodataValue && j!=a) //if point had not been removed previously
                    {
                        distance = distCalc(x1,y1,vectorsOfPoints[i][j][0],vectorsOfPoints[i][j][1]);
                        if (distance < minDist)
                        {
                            minDist = distance;
                            indexOfMin = j;
                        }
                    }
                }

                if(b==1){b+=1;} //if first or second iteration, skips first element, otherwise closest element would be first element. b marks the iteration. a stores the index of the value to skip.
                else if(b==2){a=-1;} //"a" is not needed anymore

                if(indexOfMin==-1){vectorsOfPoints[i][k][0] = _nodataValue;break;} //if point alone
                //
                //cout << indexOfMin << endl;
                currentPoint[0] = vectorsOfPoints[i][indexOfMin][0];
                currentPoint[1] = vectorsOfPoints[i][indexOfMin][1];

                myPoint.push_back(currentPoint[0]);
                myPoint.push_back(currentPoint[1]);
                myPoint.push_back(currentPoint[2]);

                myIsohypse.push_back(myPoint);
                myPoint.clear();

                vectorsOfPoints[i][indexOfMin][0] = _nodataValue;

            }
            indexOfMin = -1;
            myIsohypseLevel.push_back(myIsohypse);
            myIsohypse.clear();
        }

        allTheIsohypses.push_back(myIsohypseLevel);
        myIsohypseLevel.clear();
    }

    _isohypses = allTheIsohypses;
}

vector<vector<vector<vector<double> > > > mnt::getIsohypses()
{
    return _isohypses;
}

vector<vector<vector<double> > > classByAlti(vector<vector<double> > vectorOfPoints, double* arrayOfAlti, int arraySize)
{
    vector<vector<double> > myIsohypse; //one isohypse for a given z
    vector<vector<vector<double> > > myIsohypseLevel; //all the isohypses for a given z
    int vectorSize = vectorOfPoints.size();
    double currentAlti;

    for(int i=0;i<arraySize;i++)
    {
        currentAlti = arrayOfAlti[i];

        for(int j=0;j<vectorSize;j++)
        {
            if (vectorOfPoints[j][2] == currentAlti)
            {
                myIsohypse.push_back(vectorOfPoints[j]);
            }
        }
        myIsohypseLevel.push_back(myIsohypse);
        myIsohypse.clear();
    }
    return myIsohypseLevel;
}

void mnt::writeIsohypsesToFile(QString path)
{
    QFile file(path);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    out.setRealNumberPrecision(10);

    out << "<?xml version=\"1.0\"?>\n<vector>\n";

    for(unsigned i=0;i<_isohypses.size();i++) //for each altitude
    {
        out << "\t<altitude z=\"" << _altitudesOfIsohypses[i] << "\">\n";

        for(unsigned j=0;j<_isohypses[i].size();j++) //for each set of curve
        {
            out << "\t\t<curve>\n";

            for(unsigned k=0;k<_isohypses[i][j].size();k++) //for each curve
            {
                out<<"\t\t\t<pt x=\""<<_isohypses[i][j][k][0]<<"\" y=\""<<_isohypses[i][j][k][1]<<"\"/>\n";
            }

            out << "\t\t</curve>\n";
        }

        out << "\t</altitude>\n";
    }
    out << "</vector>\n";

    file.close();
    return;
}

void mnt::readIsohypsesFromFile(QString path)
{
    // Opens the document
    QFile file(path);
    file.open(QIODevice::ReadOnly);

    // Reads the document
    QXmlStreamReader xml;
    xml.setDevice(&file);

    double z;
    QString name;
    vector<double> myPoint, altitudes; //a point
    vector<vector<double> > myIsohypse; //one isohypse for a given z
    vector<vector<vector<double> > > myIsohypseLevel; //all the isohypses for a given z
    vector<vector<vector<vector<double> > > > allTheIsohypses; //all the isohypses for all the z;

    while(!xml.atEnd())
    {
        xml.readNext();

        while(!xml.isStartElement() && !xml.isEndElement() && !xml.atEnd())
        {
            xml.readNext();
        }
        name = xml.name().toString();

        if (xml.isStartElement())
        {
            if (xml.name() == "altitude")
            {
                z = xml.attributes().value("z").toDouble();
                altitudes.push_back(z);
            }
            else if (xml.name() == "pt")
            {
                myPoint.push_back(xml.attributes().value("x").toDouble());
                myPoint.push_back(xml.attributes().value("y").toDouble());
                myPoint.push_back(z);
                myIsohypse.push_back(myPoint);
                myPoint.clear();
            }
        } else
        {
            if (xml.name() == "altitude")
            {
                allTheIsohypses.push_back(myIsohypseLevel);
                myIsohypseLevel.clear();
            }
            else if (xml.name() == "curve")
            {
                myIsohypseLevel.push_back(myIsohypse);
                myIsohypse.clear();
            }
        }
    }

    double* altitudesArray = toArray(altitudes);
    _altitudesOfIsohypses = altitudesArray;
    _numberOfIsohypses = altitudes.size();
    _isohypses = allTheIsohypses;
}

void mnt::triangles(){
    //création d'un tableau de triangles ordonnés
    vector <vector <double> > triangleSup;//un pixel est composé d'un triangle sup
    vector <vector <double> > triangleInf;// et d'un triangle inf

    for (int s=0; s<((_sizeX)*(_sizeY-1)-1); s++){//pour chaque sommet
        //if(s==1 || (s+1)%_sizeX!=0){
        if((s+1)%_sizeX!=0){
            ajoutPointZ(triangleSup, _mnt[s][0], _mnt[s][1],_mnt[s][2]);
            ajoutPointZ(triangleSup, _mnt[s+1][0], _mnt[s+1][1],_mnt[s+1][2]);
            ajoutPointZ(triangleSup, _mnt[s+_sizeX][0], _mnt[s+_sizeX][1],_mnt[s+_sizeX][2]);

            ajoutPointZ(triangleInf, _mnt[s+1+_sizeX][0], _mnt[s+1+_sizeX][1],_mnt[s+1+_sizeX][2]);
            ajoutPointZ(triangleInf, _mnt[s+_sizeX][0], _mnt[s+_sizeX][1],_mnt[s+_sizeX][2]);
            ajoutPointZ(triangleInf, _mnt[s+1][0], _mnt[s+1][1],_mnt[s+1][2]);


            _triangles.push_back(triangleSup);
            _triangles.push_back(triangleInf);
            triangleInf.clear();
            triangleSup.clear();}
    }
}

void mnt::makeIsohypses()
{
    int z, separators;
    int trianglesSize = _triangles.size();
    int newSizeX = 2*_sizeX-2; //number of triangles per line
    vector<double> tmpPoint;
    vector<vector<double> > myIsohypse, myIsohypse_tmp; //one isohypse for a given z
    vector<vector<vector<double> > > myIsohypseLevel; //all the isohypses for a given z
    vector<vector<vector<vector<double> > > > allTheIsohypses; //all the isohypses for all the z;

    double minDist =2*sqrt(2)*_cellSize;

    bool** listOfBool = new bool*[trianglesSize]; //element [i][4] = has triangle[i] been tested already, element[i][k] = has side k been tested already?
    for(int i=0;i<trianglesSize;i++)
    {
      listOfBool[i] = new bool[4];
    }

    cout << "number of triangles: " << trianglesSize << endl;

    for(int i=0;i<_numberOfIsohypses;i++) //for each altitude
    {
        //initialisation of the array
        for(int j=0;j<trianglesSize;j++)
        {
            for(int k=0;k<4;k++)
            {
                listOfBool[j][k] = false;
            }
        }

        z = _altitudesOfIsohypses[i];
        cout<<"z="<<z<<endl;
        for(int l=0;l<trianglesSize;l+=2)
        {
            separators = huntIsohypse(&myIsohypse,z,listOfBool,l,newSizeX,trianglesSize,0,0);

            if(myIsohypse.size() > 0)
            {
                if(separators == 0) //if no separators, add the isohypse to the vector of isohypses right away
                {
                    myIsohypseLevel = tryToStick(myIsohypseLevel,myIsohypse);
                }
                else
                {
                    for(unsigned m=0;m<myIsohypse.size();m++) //for each value, tests if separator
                    {
                        if(myIsohypse[m][0] == _nodataValue) //if separator
                        {
                            if(myIsohypse_tmp.size()>0)
                            {
                                myIsohypseLevel = tryToStick(myIsohypseLevel,myIsohypse_tmp); //adds the isohypse, prior to clearing it
                                myIsohypse_tmp.clear();
                            }
                        } else if (myIsohypse_tmp.size()>0 && distCalc(myIsohypse[m][0],myIsohypse[m][1],myIsohypse_tmp[myIsohypse_tmp.size()-1][0],myIsohypse_tmp[myIsohypse_tmp.size()-1][1]) > minDist)
                        {
                            myIsohypseLevel = tryToStick(myIsohypseLevel,myIsohypse_tmp); //adds the isohypse, prior to clearing it
                            myIsohypse_tmp.clear();
                            tmpPoint.push_back(myIsohypse[m][0]);
                            tmpPoint.push_back(myIsohypse[m][1]);
                            tmpPoint.push_back(myIsohypse[m][2]);
                            myIsohypse_tmp.push_back(tmpPoint);
                            tmpPoint.clear();
                        }
                        else
                        {
                                tmpPoint.push_back(myIsohypse[m][0]);
                                tmpPoint.push_back(myIsohypse[m][1]);
                                tmpPoint.push_back(myIsohypse[m][2]);
                                myIsohypse_tmp.push_back(tmpPoint);
                                tmpPoint.clear();
                        }
                    }
                    myIsohypseLevel = tryToStick(myIsohypseLevel,myIsohypse_tmp);
                    myIsohypse_tmp.clear();
                }
                myIsohypse.clear();
            }
        }
        allTheIsohypses.push_back(myIsohypseLevel);
        myIsohypseLevel.clear();
    }
    allTheIsohypses = tryToClose(allTheIsohypses);
    _isohypses = allTheIsohypses;
}

int mnt::huntIsohypse(vector<vector<double> >* myIsohypse, const double z, bool** listOfBool, const int indexOfTriangle, const int newSizeX, const int trianglesSize, int numberOfSeparators, int depth)
{
    if(numberOfSeparators < 0){return -1;} //if too many iterations, breaks line into smaller parts to avoid segfault due to lack of available memory.
    if(depth > 10000)
    {
        for(int i=0;i<4;i++)
        {
            listOfBool[indexOfTriangle][i] = false;
        }
        return -1;
    }
    if(listOfBool[indexOfTriangle][3]){return numberOfSeparators;} //if triangle has already been tested, do not process it again.
    if(listOfBool[indexOfTriangle][0] && listOfBool[indexOfTriangle][1] && listOfBool[indexOfTriangle][2]){listOfBool[indexOfTriangle][3]=true;return numberOfSeparators;} //if all sides have been computed, triangle has been computed
    double x[3],y[3];
    double xa,ya,za,xb,yb,zb,xc,yc,zc,tmp;
    int numberOfSides,whichSide, isUp, tmp2, mySize, prevSeparators; //isUp: is the current triangle an up triangle (=1) or not (=-1); whichSide: which side of the triangle is cut by the isohypse (hori 0, diag 1, vert 2, hori&diag 1, hori&vert 2, diag&vert 3, all 4)
    //tmp2 = -1;
    vector<int> tmpArray;

    /*for(int i=0;i<3;i++)
    {
        x[i] = _nodataValue;
        y[i] = _nodataValue;
    }*/

    /*ordre of summits in the triangles:
     *
     * A _____ B         C
     *  |    /          /|
     *  |   /          / |
     *  |  /          /  |
     *  | /          /   |
     *  |/          /____|
     * C           B      A
    */

    xa=_triangles[indexOfTriangle][0][0];
    ya=_triangles[indexOfTriangle][0][1];
    za=_triangles[indexOfTriangle][0][2];
    xb=_triangles[indexOfTriangle][1][0];
    yb=_triangles[indexOfTriangle][1][1];
    zb=_triangles[indexOfTriangle][1][2];
    xc=_triangles[indexOfTriangle][2][0];
    yc=_triangles[indexOfTriangle][2][1];
    zc=_triangles[indexOfTriangle][2][2];

    isUp = (indexOfTriangle & 1) ? -1 : 1; //if j is odd (ie if we have a down triangle), isUp = -1, else 1

    if(z<min(min(za,zb),zc) || z>max(max(za,zb),zc)) //if no intersection possible
    {
        for(int q=0;q<4;q++)
        {
            listOfBool[indexOfTriangle][q] = true;
        }

        if(isUp==1) //if up triangle
        {
            if(indexOfTriangle%newSizeX!=0){listOfBool[indexOfTriangle-1][2] = true;} //side
            if(indexOfTriangle-newSizeX+1>0){listOfBool[indexOfTriangle-newSizeX+1][0] = true;} //top
        } else //if down triangle
        {
            if((indexOfTriangle+1)%newSizeX!=0){listOfBool[indexOfTriangle+1][2] = true;} //side
            if(indexOfTriangle+newSizeX<trianglesSize){listOfBool[indexOfTriangle+newSizeX-1][0] = true;} //bottom
        }
        return numberOfSeparators;
    }

    vector<double> point, separation, previousPoint;
    for(int i=0;i<3;i++){separation.push_back(_nodataValue);} //marks separation

    numberOfSides = 0;
    whichSide = 0;

    if(z==za && za==zb && zb==zc) //if in middle of flat zone
    {
        previousPoint.clear();
        previousPoint.push_back((xa+xb+xc)/3);
        previousPoint.push_back((ya+yb+yc)/3);
        previousPoint.push_back(z);

        //marking things as computed
        for(int q=0;q<4;q++)
        {
            listOfBool[indexOfTriangle][q] = true;
        }
        if(isUp==1) //if up triangle
        {
            if(indexOfTriangle%newSizeX!=0){listOfBool[indexOfTriangle-1][2] = true;} //side
            if(indexOfTriangle-newSizeX+1>0){listOfBool[indexOfTriangle-newSizeX+1][0] = true;} //top
        } else //if down triangle
        {
            if((indexOfTriangle+1)%newSizeX!=0){listOfBool[indexOfTriangle+1][2] = true;} //side
            if(indexOfTriangle+newSizeX<trianglesSize){listOfBool[indexOfTriangle+newSizeX-1][0] = true;} //bottom
        }

        numberOfSeparators++;
        myIsohypse->push_back(separation);
        myIsohypse->push_back(previousPoint);

        //B corner: looks for all touching triangles
        tmpArray.clear();
        if((isUp==1&&(indexOfTriangle+2)%newSizeX!=0) || (isUp==-1&&(indexOfTriangle-1)%newSizeX!=0)) //checking for edges of DEM.
        {
            tmpArray.push_back(indexOfTriangle+isUp*(2-newSizeX));
            tmpArray.push_back(indexOfTriangle+isUp*(3-newSizeX));
            tmpArray.push_back(indexOfTriangle+isUp*2);
        }

        point.clear();
        point.push_back(xb);
        point.push_back(yb);
        point.push_back(z);
        myIsohypse->push_back(point);
        point.clear();

        for(unsigned o=0;o<tmpArray.size();o++)
        {
            tmp = tmpArray[o];
            if(tmp<trianglesSize && tmp>0)
            {
                prevSeparators = numberOfSeparators;
                numberOfSeparators = huntIsohypse(myIsohypse,z,listOfBool,tmp,newSizeX, trianglesSize, numberOfSeparators, depth+1);
                if(numberOfSeparators != prevSeparators && numberOfSeparators>0)
                {
                    myIsohypse->push_back(separation);
                    myIsohypse->push_back(previousPoint);
                    numberOfSeparators++;
                }
            }
        }

        //C corner
        tmpArray.clear();
        tmpArray.push_back(indexOfTriangle+isUp);
        tmpArray.push_back(indexOfTriangle+isUp*newSizeX);
        if((isUp==1&&indexOfTriangle%newSizeX!=0) || (isUp==-1&&(indexOfTriangle+1)%newSizeX!=0)) //checking for edges of DEM.
        {
            tmpArray.push_back(indexOfTriangle+isUp*(newSizeX-1));
            tmpArray.push_back(indexOfTriangle+isUp*(newSizeX-2));
            tmpArray.push_back(indexOfTriangle-isUp);
        }

        point.clear();
        point.push_back(xc);
        point.push_back(yc);
        point.push_back(z);
        myIsohypse->push_back(point);
        point.clear();

        for(unsigned o=0;o<tmpArray.size();o++)
        {
            tmp = tmpArray[o];
            if(tmp<trianglesSize && tmp>0)
            {
                prevSeparators = numberOfSeparators;
                numberOfSeparators = huntIsohypse(myIsohypse,z,listOfBool,tmp,newSizeX, trianglesSize, numberOfSeparators, depth+1);
                if(numberOfSeparators != prevSeparators && numberOfSeparators>0)
                {
                    myIsohypse->push_back(separation);
                    myIsohypse->push_back(previousPoint);
                    numberOfSeparators++;
                }
            }
        }

        //A corner
        tmpArray.clear();
        tmpArray.push_back(indexOfTriangle-isUp*newSizeX);
        tmpArray.push_back(indexOfTriangle+isUp*(1-newSizeX));
        if((isUp==1&&indexOfTriangle%newSizeX!=0) || (isUp==-1&&(indexOfTriangle+1)%newSizeX!=0)) //checking for edges of DEM.
        {
            tmpArray.push_back(indexOfTriangle-isUp*(newSizeX+1));
            tmpArray.push_back(indexOfTriangle-isUp*2);
        }

        point.clear();
        point.push_back(xa);
        point.push_back(ya);
        point.push_back(z);
        myIsohypse->push_back(point);
        point.clear();

        for(unsigned o=0;o<tmpArray.size();o++)
        {
            tmp = tmpArray[o];
            if(tmp<trianglesSize && tmp>0)
            {
                prevSeparators = numberOfSeparators;
                numberOfSeparators = huntIsohypse(myIsohypse,z,listOfBool,tmp,newSizeX, trianglesSize, numberOfSeparators, depth+1);
                if(numberOfSeparators != prevSeparators && numberOfSeparators>0)
                {
                    myIsohypse->push_back(separation);
                    myIsohypse->push_back(previousPoint);
                    numberOfSeparators++;
                }
            }
        }

    return numberOfSeparators;
    }

    if(!listOfBool[indexOfTriangle][0]) //side AB
    {
        listOfBool[indexOfTriangle][0] = true;

        if(isUp==1) //if up triangle
        {
            if(indexOfTriangle-newSizeX+1>0){listOfBool[indexOfTriangle-newSizeX+1][0] = true;} //top
        } else //if down triangle
        {
            if(indexOfTriangle+newSizeX<trianglesSize){listOfBool[indexOfTriangle+newSizeX-1][0] = true;} //bottom
        }

        if (z>min(za, zb) && z<max(za, zb))
        {// AB coté horizontal d'un pixel
            x[0]=xa+((z-za)/(zb-za))*(xb-xa);
            y[0]=ya;
            numberOfSides+=1;
        } else if (z==za && za==zb) //if flat line
        {
            //marking things as computed
            for(int q=0;q<4;q++)
            {
                listOfBool[indexOfTriangle][q] = true;
            }
            previousPoint.clear();
            previousPoint.push_back((xa+xb)/2);
            previousPoint.push_back(ya);
            previousPoint.push_back(z);

            //A corner
            tmpArray.clear();
            tmpArray.push_back(indexOfTriangle-isUp*newSizeX);
            tmpArray.push_back(indexOfTriangle+isUp*(1-newSizeX));
            if((isUp==1&&indexOfTriangle%newSizeX!=0) || (isUp==-1&&(indexOfTriangle+1)%newSizeX!=0)) //checking for edges of DEM.
            {
                tmpArray.push_back(indexOfTriangle-isUp*(1+newSizeX));
                tmpArray.push_back(indexOfTriangle-isUp);
                tmpArray.push_back(indexOfTriangle-isUp*2);
            }

            point.clear();
            point.push_back(xa);
            point.push_back(ya);
            point.push_back(z);
            numberOfSeparators++;
            myIsohypse->push_back(separation);
            myIsohypse->push_back(point);
            point.clear();

            for(unsigned o=0;o<tmpArray.size();o++)
            {
                tmp = tmpArray[o];
                if(tmp<trianglesSize && tmp>0)
                {
                    prevSeparators = numberOfSeparators;
                    numberOfSeparators = huntIsohypse(myIsohypse,z,listOfBool,tmp,newSizeX, trianglesSize, numberOfSeparators, depth+1);
                    if(numberOfSeparators != prevSeparators && numberOfSeparators>0)
                    {
                        myIsohypse->push_back(separation);
                        myIsohypse->push_back(previousPoint);
                        numberOfSeparators++;
                    }
                }
            }

            //B corner
            tmpArray.clear();
            tmpArray.push_back(indexOfTriangle+isUp);
            if((isUp==1&&(indexOfTriangle+2)%newSizeX!=0) || (isUp==-1&&(indexOfTriangle-1)%newSizeX!=0)) //checking for edges of DEM.
            {
                tmpArray.push_back(indexOfTriangle+isUp*(2-newSizeX));
                tmpArray.push_back(indexOfTriangle+isUp*(3-newSizeX));
                tmpArray.push_back(indexOfTriangle+isUp*2);
            }

            point.clear();
            point.push_back(xb);
            point.push_back(yb);
            point.push_back(z);
            myIsohypse->push_back(point);
            point.clear();

            for(unsigned o=0;o<tmpArray.size();o++)
            {
                tmp = tmpArray[o];
                if(tmp<trianglesSize && tmp>0)
                {
                    prevSeparators = numberOfSeparators;
                    numberOfSeparators = huntIsohypse(myIsohypse,z,listOfBool,tmp,newSizeX, trianglesSize, numberOfSeparators, depth+1);
                    if(numberOfSeparators != prevSeparators && numberOfSeparators>0)
                    {
                        myIsohypse->push_back(separation);
                        myIsohypse->push_back(previousPoint);
                        numberOfSeparators++;
                    }
                }
            }

            return numberOfSeparators;
        }
    }

    if(!listOfBool[indexOfTriangle][1]) //Side BC
    {
        listOfBool[indexOfTriangle+isUp][1] = true;

        if(z>min(zb, zc) && z<max(zb, zc))
        {// BC diagonale d'un pixel
            tmp = (z-zc)/(zb-zc);
            x[1] = xc+tmp*(xb-xc);
            y[1] = yc+tmp*(yb-yc);
            numberOfSides+=1;
            whichSide+=1;
        } else if (z==zc && zc==zb) //if flat line
        {
            //marking things as computed
            for(int q=0;q<4;q++)
            {
                listOfBool[indexOfTriangle][q] = true;
            }

            previousPoint.clear();
            previousPoint.push_back((xb+xc)/2);
            previousPoint.push_back((yb+yc)/2);
            previousPoint.push_back(z);

            //B corner
            tmpArray.clear();
            tmpArray.push_back(indexOfTriangle+isUp);
            tmpArray.push_back(indexOfTriangle+isUp*(1-newSizeX));
            if((isUp==1&&(indexOfTriangle+2)%newSizeX!=0) || (isUp==-1&&(indexOfTriangle-1)%newSizeX!=0)) //checking for edges of DEM.
            {
                tmpArray.push_back(indexOfTriangle+isUp*(2-newSizeX));
                tmpArray.push_back(indexOfTriangle+isUp*(3-newSizeX));
                tmpArray.push_back(indexOfTriangle+isUp*2);
            }

            point.clear();
            point.push_back(xb);
            point.push_back(yb);
            point.push_back(z);
            numberOfSeparators++;
            myIsohypse->push_back(separation);
            myIsohypse->push_back(point);
            point.clear();

            for(unsigned o=0;o<tmpArray.size();o++)
            {
                tmp = tmpArray[o];
                if(tmp<trianglesSize && tmp>0)
                {
                    prevSeparators = numberOfSeparators;
                    numberOfSeparators = huntIsohypse(myIsohypse,z,listOfBool,tmp,newSizeX, trianglesSize, numberOfSeparators, depth+1);
                    if(numberOfSeparators != prevSeparators && numberOfSeparators>0)
                    {
                        myIsohypse->push_back(separation);
                        myIsohypse->push_back(previousPoint);
                        numberOfSeparators++;
                    }
                }
            }

            //C corner
            tmpArray.clear();
            tmpArray.push_back(indexOfTriangle+isUp*newSizeX);
            if((isUp==1&&indexOfTriangle%newSizeX!=0) || (isUp==-1&&(indexOfTriangle+1)%newSizeX!=0)) //checking for edges of DEM.
            {
                tmpArray.push_back(indexOfTriangle+isUp*(newSizeX-1));
                tmpArray.push_back(indexOfTriangle+isUp*(newSizeX-2));
                tmpArray.push_back(indexOfTriangle-isUp);
            }

            point.clear();
            point.push_back(xc);
            point.push_back(yc);
            point.push_back(z);
            myIsohypse->push_back(point);
            point.clear();

            for(unsigned o=0;o<tmpArray.size();o++)
            {
                tmp = tmpArray[o];
                if(tmp<trianglesSize && tmp>0)
                {
                    prevSeparators = numberOfSeparators;
                    numberOfSeparators = huntIsohypse(myIsohypse,z,listOfBool,tmp,newSizeX, trianglesSize, numberOfSeparators,depth+1);
                    if(numberOfSeparators != prevSeparators  && numberOfSeparators>0)
                    {
                        myIsohypse->push_back(separation);
                        myIsohypse->push_back(previousPoint);
                        numberOfSeparators++;
                    }
                }
            }

            return numberOfSeparators;
        }
    }

    if(!listOfBool[indexOfTriangle][2]) //Side AC
    {
        if(isUp==1) //if up triangle
        {
            if(indexOfTriangle%newSizeX!=0){listOfBool[indexOfTriangle-1][2] = true;} //side
        } else //if down triangle
        {
            if((indexOfTriangle+1)%newSizeX!=0){listOfBool[indexOfTriangle+1][2] = true;} //side
        }

        if (z>min(za, zc) && z<max(za, zc))
        {// AC coté vertical d'un pixel
            x[2]= xa;
            y[2]= ya+((z-za)/(zc-za))*(yc-ya);
            numberOfSides+=1;
            whichSide+=2;
        } else if (z==zc && za==zc) //if flat line
        {
            //marking things as computed
            for(int q=0;q<4;q++)
            {
                listOfBool[indexOfTriangle][q] = true;
            }

            previousPoint.clear();
            previousPoint.push_back(xa);
            previousPoint.push_back((ya+yc)/2);
            previousPoint.push_back(z);

            //A corner
            tmpArray.clear();
            tmpArray.push_back(indexOfTriangle-isUp*newSizeX);
            tmpArray.push_back(indexOfTriangle+isUp*(1-newSizeX));
            if((isUp==1&&indexOfTriangle%newSizeX!=0) || (isUp==-1&&(indexOfTriangle+1)%newSizeX!=0)) //checking for edges of DEM.
            {
                tmpArray.push_back(indexOfTriangle-isUp*(1+newSizeX));
                tmpArray.push_back(indexOfTriangle-isUp);
                tmpArray.push_back(indexOfTriangle-isUp*2);
            }

            point.clear();
            point.push_back(xa);
            point.push_back(ya);
            point.push_back(z);
            numberOfSeparators++;
            myIsohypse->push_back(separation);
            myIsohypse->push_back(point);            
            point.clear();

            for(unsigned o=0;o<tmpArray.size();o++)
            {
                tmp = tmpArray[o];
                if(tmp<trianglesSize && tmp>0)
                {
                    prevSeparators = numberOfSeparators;
                    numberOfSeparators = huntIsohypse(myIsohypse,z,listOfBool,tmp,newSizeX, trianglesSize, numberOfSeparators, depth+1);
                    if(numberOfSeparators != prevSeparators && numberOfSeparators>0)
                    {
                        myIsohypse->push_back(separation);
                        myIsohypse->push_back(previousPoint);
                        numberOfSeparators++;
                    }
                }
            }

            //C corner
            tmpArray.clear();
            tmpArray.push_back(indexOfTriangle+isUp*newSizeX);
            tmpArray.push_back(indexOfTriangle+isUp);
            if((isUp==1&&indexOfTriangle%newSizeX!=0) || (isUp==-1&&(indexOfTriangle+1)%newSizeX!=0)) //checking for edges of DEM.
            {
                tmpArray.push_back(indexOfTriangle+isUp*(newSizeX-1));
                tmpArray.push_back(indexOfTriangle+isUp*(newSizeX-2));
            }

            point.clear();
            point.push_back(xc);
            point.push_back(yc);
            point.push_back(z);
            myIsohypse->push_back(point);
            point.clear();

            for(unsigned o=0;o<tmpArray.size();o++)
            {
                tmp = tmpArray[o];
                if(tmp<trianglesSize && tmp>0)
                {
                    prevSeparators = numberOfSeparators;
                    numberOfSeparators = huntIsohypse(myIsohypse,z,listOfBool,tmp,newSizeX, trianglesSize, numberOfSeparators, depth+1);
                    if(numberOfSeparators != prevSeparators  && numberOfSeparators>0)
                    {
                        myIsohypse->push_back(separation);
                        myIsohypse->push_back(previousPoint);
                        numberOfSeparators++;
                    }
                }
            }

            return numberOfSeparators;
        }
    }

    if((x[0]==xb && y[0]==yb) || (x[1]==xb && y[1]==yb)) //B corner
    {
        //marking things as computed
        for(int q=0;q<4;q++)
        {
            listOfBool[indexOfTriangle][q] = true;
        }

        mySize = myIsohypse->size();
        if(mySize > 0)
        {
            previousPoint.clear();
            previousPoint.push_back((*myIsohypse)[myIsohypse->size()-1][0]);
            previousPoint.push_back((*myIsohypse)[myIsohypse->size()-1][1]);
            previousPoint.push_back((*myIsohypse)[myIsohypse->size()-1][2]);
        }

        point.clear();
        point.push_back(xb);
        point.push_back(yb);
        point.push_back(z);
        numberOfSeparators++;
        myIsohypse->push_back(separation);
        myIsohypse->push_back(point);
        point.clear();

        //cout << "pile dans le coin B !" << endl;
        tmpArray.clear();
        tmpArray.push_back(indexOfTriangle+isUp);
        tmpArray.push_back(indexOfTriangle+isUp*(1-newSizeX));
        if((isUp==1&&(indexOfTriangle+2)%newSizeX!=0) || (isUp==-1&&(indexOfTriangle-1)%newSizeX!=0)) //checking for edges of DEM.
        {
            tmpArray.push_back(indexOfTriangle+isUp*(2-newSizeX));
            tmpArray.push_back(indexOfTriangle+isUp*(3-newSizeX));
            tmpArray.push_back(indexOfTriangle+isUp*2);
        }

        for(unsigned o=0;o<tmpArray.size();o++)
        {
            tmp = tmpArray[o];
            if(tmp<trianglesSize && tmp>0)
            {
                prevSeparators = numberOfSeparators;
                numberOfSeparators = huntIsohypse(myIsohypse,z,listOfBool,tmp,newSizeX, trianglesSize, numberOfSeparators,depth+1);
                if(numberOfSeparators != prevSeparators && numberOfSeparators>0)
                {
                    myIsohypse->push_back(separation);
                    if(mySize > 0){myIsohypse->push_back(previousPoint);}
                    numberOfSeparators++;
                }
            }
        }

        return numberOfSeparators;
    }
    else if((x[1]==xc && y[1]==yc) || (x[2]==xc && y[2]==yc)) //C corner
    {
        //marking things as computed
        for(int q=0;q<4;q++)
        {
            listOfBool[indexOfTriangle][q] = true;
        }

        mySize = myIsohypse->size();
        if(mySize > 0)
        {
            previousPoint.clear();
            previousPoint.push_back((*myIsohypse)[myIsohypse->size()-1][0]);
            previousPoint.push_back((*myIsohypse)[myIsohypse->size()-1][1]);
            previousPoint.push_back((*myIsohypse)[myIsohypse->size()-1][2]);
        }

        point.clear();
        point.push_back(xc);
        point.push_back(yc);
        point.push_back(z);
        numberOfSeparators++;
        myIsohypse->push_back(separation);
        myIsohypse->push_back(point);
        point.clear();


        //cout << "pile dans le coin C !" << endl;
        tmpArray.clear();
        tmpArray.push_back(indexOfTriangle+isUp);
        tmpArray.push_back(indexOfTriangle+isUp*newSizeX);
        if((isUp==1&&indexOfTriangle%newSizeX!=0) || (isUp==-1&&(indexOfTriangle+1)%newSizeX!=0)) //checking for edges of DEM.
        {
            tmpArray.push_back(indexOfTriangle+isUp*(newSizeX-1));
            tmpArray.push_back(indexOfTriangle+isUp*(newSizeX-2));
            tmpArray.push_back(indexOfTriangle-isUp);
        }

        for(unsigned o=0;o<tmpArray.size();o++)
        {
            tmp = tmpArray[o];
            if(tmp<trianglesSize && tmp>0)
            {
                prevSeparators = numberOfSeparators;
                numberOfSeparators = huntIsohypse(myIsohypse,z,listOfBool,tmp,newSizeX, trianglesSize, numberOfSeparators, depth+1);
                if(numberOfSeparators != prevSeparators && numberOfSeparators>0)
                {
                    myIsohypse->push_back(separation);
                    if(mySize > 0){myIsohypse->push_back(previousPoint);}
                    numberOfSeparators++;
                }
            }
        }

        return numberOfSeparators;
    }

    else if((x[0]==xa && y[0]==ya) || (x[2]==xa && y[2]==ya)) //A corner
    {
        //marking things as computed
        for(int q=0;q<4;q++)
        {
            listOfBool[indexOfTriangle][q] = true;
        }

        mySize = myIsohypse->size();
        if(mySize > 0)
        {
            previousPoint.clear();
            previousPoint.push_back((*myIsohypse)[myIsohypse->size()-1][0]);
            previousPoint.push_back((*myIsohypse)[myIsohypse->size()-1][1]);
            previousPoint.push_back((*myIsohypse)[myIsohypse->size()-1][2]);
        }

        point.clear();
        point.push_back(xa);
        point.push_back(ya);
        point.push_back(z);
        numberOfSeparators++;
        myIsohypse->push_back(separation);
        myIsohypse->push_back(point);
        point.clear();

        //cout << "pile dans le coin A !" << endl;
        tmpArray.clear();
        tmpArray.push_back(indexOfTriangle-isUp*newSizeX);
        tmpArray.push_back(indexOfTriangle+isUp*(1-newSizeX));
        if((isUp==1&&indexOfTriangle%newSizeX!=0) || (isUp==-1&&(indexOfTriangle+1)%newSizeX!=0)) //checking for edges of DEM.
        {
            tmpArray.push_back(indexOfTriangle-isUp*(newSizeX+1));
            tmpArray.push_back(indexOfTriangle-isUp);
            tmpArray.push_back(indexOfTriangle-isUp*2);
        }

        for(unsigned o=0;o<tmpArray.size();o++)
        {
            tmp = tmpArray[o];
            if(tmp<trianglesSize && tmp>0)
            {
                prevSeparators = numberOfSeparators;
                numberOfSeparators = huntIsohypse(myIsohypse,z,listOfBool,tmp,newSizeX, trianglesSize, numberOfSeparators, depth+1);
                if(numberOfSeparators != prevSeparators && numberOfSeparators>0)
                {
                    myIsohypse->push_back(separation);
                    if(mySize > 0){myIsohypse->push_back(previousPoint);}
                    numberOfSeparators++;
                }
            }
        }

        return numberOfSeparators;
    }

    switch(numberOfSides)
    {
        case 0:
            return numberOfSeparators;
        case 1: //beginning of isohypse or isolated point
        {
            switch(whichSide)
            {
                case 0: //hori
                    tmp = indexOfTriangle+isUp*(1-newSizeX);
                    break;
                case 1: //diag
                    tmp = indexOfTriangle+isUp;
                    break;
                case 2: //vert
                    if ((isUp==1&&indexOfTriangle%newSizeX==0) || (isUp==-1&&(indexOfTriangle+1)%newSizeX==0)) //checking for edges of DEM.
                    {
                        tmp = -1;
                    } else
                    {
                        tmp = indexOfTriangle-isUp;
                    }
                    break;
            }

            point.push_back(x[whichSide]);
            point.push_back(y[whichSide]);
            point.push_back(z);
            myIsohypse->push_back(point);
            point.clear();

            //cout << "1 côté; " << "x=" << x[whichSide] << ", y=" << y[whichSide] << ", z=" << z << ", side=" << whichSide << endl;
            if(tmp<trianglesSize && tmp>0){numberOfSeparators = huntIsohypse(myIsohypse, z, listOfBool, tmp, newSizeX, trianglesSize, numberOfSeparators, depth+1);}
            break;
        }
        case 2: //arriving in the middle of an isohypse. Have to process both extremities separately
        {
            mySize = myIsohypse->size();
            if(mySize > 0)
            {
                previousPoint.clear();
                previousPoint.push_back((*myIsohypse)[mySize-1][0]);
                previousPoint.push_back((*myIsohypse)[mySize-1][1]);
                previousPoint.push_back((*myIsohypse)[mySize-1][2]);
            }

            switch(whichSide)
            {
            case 1:
            case 2:
                tmp = indexOfTriangle+isUp*(1-newSizeX); //hori
                tmp2 = 0;
                break;
            case 3:
                tmp = indexOfTriangle+isUp; //diag
                tmp2 = 1;
                break;
            }

            if(mySize > 0)
            {
                myIsohypse->push_back(separation);
                myIsohypse->push_back(previousPoint);
                numberOfSeparators++;
            }

            point.push_back(x[tmp2]);
            point.push_back(y[tmp2]);
            point.push_back(z);
            myIsohypse->push_back(point);
            point.clear();

            prevSeparators = numberOfSeparators;
            if(tmp<trianglesSize && tmp>0){numberOfSeparators = huntIsohypse(myIsohypse, z, listOfBool, tmp, newSizeX, trianglesSize, numberOfSeparators, depth+1);}

            tmp2 = whichSide-tmp2;
            switch(tmp2)
            {
            case 0: //hori
                tmp = indexOfTriangle+isUp*(1-newSizeX);
                break;
            case 1: //diag
                tmp = indexOfTriangle+isUp;
                break;
            case 2: //vert
                if ((isUp==1&&indexOfTriangle%newSizeX==0) || (isUp==-1&&(indexOfTriangle+1)%newSizeX==0)) //checking for edges of DEM.
                {
                    tmp = -1;
                } else
                {
                    tmp = indexOfTriangle-isUp;
                }
                break;
            }

            if(numberOfSeparators != prevSeparators && mySize > 0)
            {
                myIsohypse->push_back(separation);
                myIsohypse->push_back(previousPoint);
                numberOfSeparators++;
            }

            point.push_back(x[tmp2]);
            point.push_back(y[tmp2]);
            point.push_back(z);
            myIsohypse->push_back(point);
            point.clear();

            //cout << "2 côtés, fin; " << "x=" << x[tmp2] << ", y=" << y[tmp2] << ", z=" << z << ", side=" << tmp2 << endl;

            if(tmp<trianglesSize && tmp>0){numberOfSeparators = huntIsohypse(myIsohypse, z, listOfBool, tmp, newSizeX, trianglesSize, numberOfSeparators, depth+1);}
            break;
        }
        case 3: //extremely annoying flat zone
            //SHOULD NOT HAPPEN. Could only happen on the first triangle top left. We will not deal with it.
            break;
    }
    return numberOfSeparators;
}

vector<vector<vector<double> > > mnt::tryToStick(vector<vector<vector<double> > > myIsohypseLevel, vector<vector<double> > myIsohypse_tmp)
{
    int levelLast, tmpLast;

    if(myIsohypse_tmp.size()>0)
    {
        tmpLast = myIsohypse_tmp.size()-1;
    } else
    {
        return myIsohypseLevel;
    }

    double minDist = _cellSize*sqrt(2);
    double minDist2 = 0.001;
    bool isStuck = false;
    vector<double> dist;

    for(unsigned i=0;i<myIsohypseLevel.size();i++) //trying each isohypse
    {
        levelLast = (myIsohypseLevel[i].size()>0) ? myIsohypseLevel[i].size()-1 : 0;

        if(isStuck){break;}

        dist.clear();
        dist.push_back(distCalc(myIsohypseLevel[i][levelLast][0],myIsohypseLevel[i][levelLast][1],myIsohypse_tmp[0][0],myIsohypse_tmp[0][1]));
        dist.push_back(distCalc(myIsohypseLevel[i][levelLast][0],myIsohypseLevel[i][levelLast][1],myIsohypse_tmp[tmpLast][0],myIsohypse_tmp[tmpLast][1]));
        dist.push_back(distCalc(myIsohypseLevel[i][0][0],myIsohypseLevel[i][0][1],myIsohypse_tmp[tmpLast][0],myIsohypse_tmp[tmpLast][1]));
        dist.push_back(distCalc(myIsohypseLevel[i][0][0],myIsohypseLevel[i][0][1],myIsohypse_tmp[0][0],myIsohypse_tmp[0][1]));

        if(dist[0] <= minDist)
        {
            //last of existing = first of new
            if(dist[0]<minDist2){myIsohypse_tmp.erase(myIsohypse_tmp.begin());} //delete element since it is present in both isohypses
            if(myIsohypse_tmp.size()>0){myIsohypseLevel[i] = concatenate(myIsohypseLevel[i],myIsohypse_tmp);}
            isStuck = true;

        } else if(dist[1] <= minDist)
        {
            //last of existing = last of new
            if(dist[1]<minDist2){myIsohypse_tmp.erase(myIsohypse_tmp.end()-1);} //delete element since it is present in both isohypses
            if(myIsohypse_tmp.size()>0){myIsohypseLevel[i] = concatenate(myIsohypseLevel[i],retourne(myIsohypse_tmp));}
            isStuck = true;

        } else if(dist[2] <= minDist)
        {
            //first of existing = last of new
            if(dist[2]<minDist2){myIsohypse_tmp.erase(myIsohypse_tmp.end()-1);} //delete element since it is present in both isohypses
            if(myIsohypse_tmp.size()>0){myIsohypseLevel[i] = concatenate(myIsohypse_tmp,myIsohypseLevel[i]);}
            isStuck = true;

        } else if(dist[3] <= minDist)
        {
            //first of existing = first of new
            if(dist[3]<minDist2){myIsohypse_tmp.erase(myIsohypse_tmp.begin());} //delete element since it is present in both isohypses
            if(myIsohypse_tmp.size()>0){myIsohypseLevel[i] = concatenate(retourne(myIsohypse_tmp),myIsohypseLevel[i]);}
            isStuck = true;
        }
    }

    if(!isStuck)
    {
        //not close to any other isohypse, just insert it right away
        myIsohypseLevel.push_back(myIsohypse_tmp);
    }

    return myIsohypseLevel;
}

vector<vector<vector<vector<double> > > > mnt::tryToClose(vector<vector<vector<vector<double> > > > allTheIsohypses)
{
    double minDist = sqrt(2)*_cellSize;
    vector<vector<double> > currentIsohypse;
    int mySize;

    //dealing with allTheIsohypses
    for(unsigned i=0;i<allTheIsohypses.size();i++)
    {
        //dealing with isohypsesLevel
        for(unsigned j=0;j<allTheIsohypses[i].size();j++)
        {
            //dealing with one isohypse
            currentIsohypse = allTheIsohypses[i][j];
            mySize = currentIsohypse.size()-1;
            if(mySize > 8 && distCalc(currentIsohypse[0][0],currentIsohypse[0][1],currentIsohypse[mySize][0],currentIsohypse[mySize][1]) <= minDist)
            {
                 allTheIsohypses[i][j].push_back(currentIsohypse[0]);
            }
        }
    }
    return allTheIsohypses;
}

void mnt::xyz2asc()
{
    QStringList splitPath = _path.split("."); //CREATION OF THE ASC FILE PATH
    QString ascPath = QString();
    for(int i=0;i<splitPath.length()-1;i++)
    {
        ascPath+=splitPath[i];
    }
    ascPath+=".asc";

    QFile file(ascPath);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    out.setRealNumberPrecision(10);

    out << "ncols " << _sizeX << "\n";
    out << "nrows " << _sizeY << "\n";
    out << "xllcorner " << _mnt[0][0] << "\n";
    out << "yllcorner " << _mnt[0][1] << "\n";
    out << "cellsize " << _cellSize << "\n";
    out << "nodata_value " << _nodataValue << "\n";

    for(int i=0;i<_sizeY;i++)
    {
        for(int j=0;j<_sizeX;j++)
        {
            out << " " << _mnt[i*_sizeX+j][2];
        }
        out << "\n";
    }
    file.close();
}
