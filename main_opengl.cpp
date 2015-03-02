//============================================================================
// Name        : main_opengl.cpp
// Author      :
// Version     :
// Copyright   :
// Description :
//============================================================================

//#include <glut>

//#include <windows.h>
#include "main_opengl.h"

//

//#define AVEC_TEST

using namespace std;

// Point de vue
int idptdevue;
// Pour ne créer le buffer qu'une seule fois, on indique qu'on est à la première boucle
int firstboucle = 1;

//Initialisation du temps
float t=0;
float t1=0;
float difT=0;
// Initialisation indice des sommets
int ids = 0;
// Initialisation vitesse de deplacement (1000 = temps réel)
int vit = 1000;
// Tableau des vitesses
double vtab[11];
int idv = 5;
// Initialisation déplacements
double dX = 0;
double dY = 0;
double dZ = 0;
double DX = 0;
double DY = 0;
double DZ = 0;
// Initialisation distance
float dist = 0;
double coef[11];
// Initialisation du type d'avancée
int typedist = 0;
int intermed = 0;
int atest = 0;
//Initialisation du type de traçage (par défaut, lignes)
int triangles=0;
double altmax = 0.;
double altmin = 100000.;
//Initialisation des tailles
int tailleMnt = 0;
int tailleIsos = 0;
//Initialisation tableau des couleurs
double colortriangle[10000000];
double colorline[10000000];
//Initialisation de la couleur
int aveccolor = 0;
int aveciso = 0;

// Creation GPX/IsoHypse vector
vector<vector<double> > vect;
vector<vector<vector<vector<double> > > > vectisohypse;
vector<double> tmp;



#if defined(AVEC_TEST)
// dimensions MNT
int a = 4;
int b = 2;
int teste = 0;
// dimensions GPX
int c = 5;
// creation MNT
double **tab;

#else
// dimensions MNT
int a = 0;
int b = 0;
// dimensions GPX
int c = 5;
int teste = 0;
// creation MNT
double** tab;  // Tableau doule**
#endif

// Parametres constants
static float RotX = 0;
static float RotY = 0;
static float RotZ = 0;
float TranX = 0;
float TranY = 0;
float TranZ = 0;

const GLfloat light_ambient[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

// Fonctions
void Initialise(char *);
void DefineWindow( char*);
void resize(int, int);
void display(void);
void idle(void);
void key(unsigned char, int, int);
void ptdevue(int);
void SpecialKeys(int, int, int);


main_opengl::main_opengl()
{
    //ctor
}

main_opengl::~main_opengl()
{
    //dtor
}


//Fonctions pour la gestion de la fenetre d'affichage

//fonction d'initialisation de la fenetre
void Initialise(char * Titre)
{
    //Récupération des dimensions de l'écran
    int width, height;
    width = glutGet(GLUT_SCREEN_WIDTH);
    height = glutGet(GLUT_SCREEN_HEIGHT);

    GLvoid DefineWindow( char * , int, int) ;
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    DefineWindow(Titre, width, height);
    const float ar = (float) width / (float) height;  //ratio
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
    gluLookAt(15,-20,20,15,8,0,0,0,1);
    idptdevue = 0;
}

void DefineWindow( char * Titre,int width, int height)
{
    // positionnement et création de la fenêtre
    glutInitWindowPosition(width/10,height/10);
    glutInitWindowSize(width,height);
    glutCreateWindow(Titre);
    // Définition de la couleur du fond de la fenêtre
    glClearColor(1.,1.,1.,1.);
}

void resize(int width, int height)
{
    const float ar = (float) width / (float) height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.5, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------

////////////////////////// Fonctions de "dessin" /////////////////////////////


// Fonction principale
void main_opengl::func_main(mnt myMnt, QString gpxPath, int spinval,bool choixVitesseReelle)
{
    if (choixVitesseReelle==false){
        typedist=1;
    }
    idv=spinval;

    // tableau des vitesses
    vtab[0] = 9999999999;
    vtab[1] = 20000;
    vtab[2] = 10000;
    vtab[3] = 5000;
    vtab[4] = 2000;
    vtab[5] = 1000;
    vtab[6] = 500;
    vtab[7] = 100;
    vtab[8] = 30;
    vtab[9] = 10;
    vtab[10] = 0.333;

    // tableau pour vitesses constantes
    coef[0] = 0;
    coef[1] = 0.001;
    coef[2] = 0.01;
    coef[3] = 0.1;
    coef[4] = 0.5;
    coef[5] = 1;
    coef[6] = 2;
    coef[7] = 5;
    coef[8] = 10;
    coef[9] = 15;
    coef[10] = 20;

#if defined(AVEC_TEST)
    cout << "TESTS" << endl;

    // tableau du MNT fictif
    tab = new double*[8];
    for (int i = 0; i < 8; ++i)
        tab[i] = new double[3];
    tab[0][0] = 0;
    tab[1][0] = 1;
    tab[2][0] = 2;
    tab[3][0] = 3;
    tab[4][0] = 0;
    tab[5][0] = 1;
    tab[6][0] = 2;
    tab[7][0] = 3;

    tab[0][1] = 0;
    tab[1][1] = 0;
    tab[2][1] = 0;
    tab[3][1] = 0;
    tab[4][1] = 1;
    tab[5][1] = 1;
    tab[6][1] = 1;
    tab[7][1] = 1;

    tab[0][2] = 1;
    tab[1][2] = 1;
    tab[2][2] = 1.8;
    tab[3][2] = 1.1;
    tab[4][2] = 1.2;
    tab[5][2] = 1.4;
    tab[6][2] = 1.1;
    tab[7][2] = 1.5;


    // Vecteur du GPX fictif
    tmp.push_back(0);
    tmp.push_back(2);
    tmp.push_back(1.5);
    tmp.push_back(1000);
    vect.push_back(tmp);
    tmp.clear();

    tmp.push_back(1);
    tmp.push_back(1);
    tmp.push_back(1.6);
    tmp.push_back(1002);
    vect.push_back(tmp);
    tmp.clear();

    tmp.push_back(1.5);
    tmp.push_back(1.5);
    tmp.push_back(1.3);
    tmp.push_back(1005);
    vect.push_back(tmp);
    tmp.clear();

    tmp.push_back(3);
    tmp.push_back(1);
    tmp.push_back(1.9);
    tmp.push_back(1006);
    vect.push_back(tmp);
    tmp.clear();

    tmp.push_back(4);
    tmp.push_back(2);
    tmp.push_back(1.5);
    tmp.push_back(1007);
    vect.push_back(tmp);
    tmp.clear();

    QString mntPath2 = "Data/MNTpourTest.asc";
    mnt myMnt = mnt(mntPath2);
    vectisohypse = myMnt.getIsohypses();
    c=5;
    a=4;
    b=2;

#else

    myMnt.update();

    tab = myMnt.getMnt();

    gpx myGpx = gpx(gpxPath);

    if(myGpx.inMnt(myMnt))
    {
        vect = intersectionPoints(myGpx.getGpx(), myMnt.getSizeX(), myMnt.getSizeY(), myMnt.getCellSize(), myMnt.getMnt(),myGpx.getPath());
    } else if(myGpx.getTitle() == "fakeGpx")
    {
        vect = myGpx.getGpx();
    } else
    {
        myGpx = gpx("../Data/fakeGpx");
        vect = myGpx.getGpx();



        /*vect.clear();
        tmp.clear();

        // GPX fictif si pas de GPX
        tmp.push_back(99999);
        tmp.push_back(2);
        tmp.push_back(2);
        tmp.push_back(-1);
        vect.push_back(tmp);
        tmp.clear();

        tmp.push_back(99999);
        tmp.push_back(2);
        tmp.push_back(2.01);
        tmp.push_back(-1);
        vect.push_back(tmp);
        tmp.clear();

        tmp.push_back(99999);
        tmp.push_back(2.01);
        tmp.push_back(2.01);
        tmp.push_back(-1);
        vect.push_back(tmp);
        tmp.clear();

        tmp.push_back(99999);
        tmp.push_back(2.01);
        tmp.push_back(2);
        tmp.push_back(-1);
        vect.push_back(tmp);
        tmp.clear();

        tmp.push_back(99999);
        tmp.push_back(2.02);
        tmp.push_back(2.01);
        tmp.push_back(-1);
        vect.push_back(tmp);
        tmp.clear();*/

        cout << "GPX out of bounds. Please load another one." << endl;
    }
    vector<vector<double> > plop = vect;

    vectisohypse = myMnt.getIsohypses();
    // dimension GPX
    c = vect.size();
    // dimensions MNT
    a = myMnt.getSizeX();
    b = myMnt.getSizeY();

#endif


    //----------Décalages----------//
    //On introduit des décalages pour recentrer le MNT
    double decalx=10000.*floor(tab[0][0]/10000.);
    double decaly=10000.*floor(tab[0][1]/10000.);
    double decalz=tab[0][2];

    //On recentre le GPX
    for (int nbarc=0; nbarc<c; nbarc++)
    {
        vect[nbarc][0]-=decalx;
        vect[nbarc][1]-=decaly;
        vect[nbarc][2]-=decalz;
    }

    //On recentre le MNT
    for (int i=0; i<a*b; i++){
        tab[i][0]-=decalx;
        tab[i][1]-=decaly;
        tab[i][2]-=decalz;
    }

    //On recentre les IsoHypse
    int a1 = vectisohypse.size();
    //incrémentation selon les différentes altitudes
    for (int nbalt=0; nbalt<a1; nbalt++){
        int b1 = vectisohypse[nbalt].size();
        //incrémentation selon le nombre de lignes pour une même altitude
        for (int nbline=0; nbline<b1; nbline++){
            int c1 = vectisohypse[nbalt][nbline].size();
            //incrémentation selon le nombre de segments pour une ligne
            if (c1>1){
                for (int nbarc=0; nbarc<c1; nbarc++)
                {
                    vectisohypse[nbalt][nbline][nbarc][0]-=decalx;
                    vectisohypse[nbalt][nbline][nbarc][1]-=decaly;
                    vectisohypse[nbalt][nbline][nbarc][2]-=decalz;
                }
            }
        }
    }


    //----------Altitudes Min/Max----------//
    for (int i=0; i<a*b-1; i++){
        if (tab[i][2] > altmax){altmax = tab[i][2];}
        if (tab[i][2] < altmin){altmin = tab[i][2];}
    }

    //----------Initialisations pour buffer----------//
    // 9 coordinates * 2 triangles per square * nbr of squares
    tailleMnt = 18*(a-1)*(b-1);

    int a2=vectisohypse.size();
    //Calcul de la taille du tableau pour les IsoHypses
    //incrémentation selon les différentes altitudes
    for (int nbalt=0; nbalt<a2; nbalt++){
        int b2 = vectisohypse[nbalt].size();
        //incrémentation selon le nombre de lignes pour une même altitude
        for (int nbline=0; nbline<b2; nbline++){
            int c2 = vectisohypse[nbalt][nbline].size();
            //incrémentation selon le nombre de segments pour une ligne
            for (int nbarc=0; nbarc<c2; nbarc++)
            {
                //Nombre de points * 3 coordonnées * 2 (Point Initial + Point Final)
                tailleIsos+=(vectisohypse[nbalt][nbline].size())*3*2;
            }
        }
    }

    //----------Tableaux des Couleurs----------//
    int altdiff = altmax-altmin;
    int indice=0;
    for(int i=0;i<b-1;i++)
    {
        for(int j=0;j<a-1;j++){
            //On prend l'altitude du triangle pour la couleur
            colortriangle[indice]= 0.4+0.6*(tab[a*i+j][2]-altmin)/altdiff;
            colortriangle[indice+1]= 0.4+0.6*(tab[a*i+j+1][2]-altmin)/altdiff;
            indice+=2;
        }
    }
    indice=0;
    for(int i=0;i<b-1;i++)
    {
        for(int j=0;j<a-1;j++){
            //On prend l'altitude de la ligne pour la couleur
            colorline[indice]= 0.4+0.6*(tab[a*i+j][2]-altmin)/altdiff;
            colorline[indice+1]= 0.4+0.6*(tab[a*i+j+1][2]-altmin)/altdiff;
            colorline[indice+2]= 0.4+0.6*(tab[a*i+j][2]-altmin)/altdiff;
            indice+=3;
        }
    }

    //----------Fonctions OpenGL----------//

    // initialisation
    char* nom_fenetre = "Projet MNT";
    Initialise(nom_fenetre);

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(SpecialKeys);
    glutIdleFunc(idle);

    glClearColor(1,1,1,1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glutMainLoop();

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------

// Fonction d'affichage
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ptdevue(idptdevue);

    //Vertices of a triangle (counter-clockwise winding)
    //Création de la table contenant toutes les coordonnées des triangles du MNT

    //---------MNT--------//
    drawMNT *ground;
    ground=new drawMNT();
    //l'integer "triangles" correspond au type de tracé (0=lignes, 1=triangles)
    ground->draw(tab,vectisohypse,tailleMnt,tailleIsos,a,b,0.,0.,0.,colortriangle, colorline, firstboucle,triangles, aveccolor, aveciso);
    //On change la valeur de firstboucle après la première itération
    if(firstboucle==1){firstboucle = 0;}
    delete ground;


    //---------GPX--------//
    traceGPX *ground2;
    ground2=new traceGPX();
    ground2->draw(vect,c,0,0,0.1);
    delete ground2;


    //oeil non tracé si caméra suit la trace GPX
    if (idptdevue==0){
        //---------oeil--------//
        oeil *ground3;
        ground3=new oeil();
#if defined(AVEC_TEST)
        ground3->draw_oeil(vect[ids][0] + DX +0.1, vect[ids][1] + DY, vect[ids][2] + DZ - 0.2,2);
#else
        ground3->draw_oeil(vect[ids][0] + DX +0.1, vect[ids][1] + DY, vect[ids][2] + DZ - 0.2,10);
#endif
        delete ground3;}

    //Affichage du nombre de frames, pour pouvoir évaluer le nombre de frames par secondes
    //teste = teste+1;
    //cout << teste << endl;

    glutSwapBuffers();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------

//fonction de mouvement
void idle(void)
{

    t1 = glutGet(GLUT_ELAPSED_TIME) ;
    // On calcule le nombre de frames ratées pour compenser la vitesse
    difT = t1-t;
    t = glutGet(GLUT_ELAPSED_TIME) ;
    glutPostRedisplay();

    //Modification de la vitesse
    vit = vtab[idv];

    //Si il n'existe pas de paramètre de temps, on met le type d'avancée linéaire
    if (vect[ids][3] < 1)
    { typedist=1;
    }




    //Avancée dépendant du temps GPX
    if (typedist==0)
    {
        //Calcul des variations de déplacement
        dist = pow(pow(vect[ids+1][0]-vect[ids][0],2)+pow(vect[ids+1][1]-vect[ids][1],2)+pow(vect[ids+1][2]-vect[ids][2],2),0.5);
        //si la distance de l'arc est nulle, on passe à l'arc suivant
        if (dist==0){
            ids+=1;
        }
        else{
            dX =difT*(vect[ids+1][0]-vect[ids][0]) / ((vect[ids+1][3]-vect[ids][3])*vit);
            dY =difT*(vect[ids+1][1]-vect[ids][1]) / ((vect[ids+1][3]-vect[ids][3])*vit);
            dZ =difT*(vect[ids+1][2]-vect[ids][2]) / ((vect[ids+1][3]-vect[ids][3])*vit);
            // Déplacement global
            DX += dX;
            DY += dY;
            DZ += dZ;

        }
    }



    //Avancée linéaire
    if (typedist==1)
    {
        dist = sqrt(pow(vect[ids+1][0]-vect[ids][0],2)+pow(vect[ids+1][1]-vect[ids][1],2)+pow(vect[ids+1][2]-vect[ids][2],2));
        //si la distance de l'arc est nulle, on passe à l'arc suivant
        if (dist==0){
            ids+=1;
        }
        else{
            dX =coef[idv]*(vect[ids+1][0]-vect[ids][0])/dist;
            dY =coef[idv]*(vect[ids+1][1]-vect[ids][1])/dist;
            dZ =coef[idv]*(vect[ids+1][2]-vect[ids][2])/dist;
            // Déplacement global
            DX += dX;
            DY += dY;
            DZ += dZ;
        }
    }

    //On calcule la distance entre notre point et le sommet précédant pour voir si on est toujours sur l'arc.
    //Si non, on passe sur l'arc suivant.

    double DXint = vect[ids+1][0]-vect[ids][0];
    //double DZint = vect[ids+1][2]-vect[ids][2];
    while ( fabs(DX)>=fabs(DXint) )
    //while ( fabs(DZ)>fabs(DZint) )
    {
        //Si on est au bout du GPX, retour au début
        if (ids==c-2){
            ids=0;
            DX = 0;
            DY = 0;
            DZ = 0;
            dX = 0;
            dY = 0;
            dZ = 0;
        }
        else{
            //Quand on arrive au bout d'un arc, on ajoute la distance qui dépasse sur l'arc suivant
            double DYint = vect[ids+1][1]-vect[ids][1];
            double DZint = vect[ids+1][2]-vect[ids][2];
            //d1 = distance "qui dépasse" après le déplacement
            double d1=pow(pow(DX-DXint,2)+pow(DY-DYint,2)+pow(DZ-DZint,2),0.5);
            //d2 = longueur de l'arc suivant
            double d2x=vect[ids+2][0]-vect[ids+1][0];
            double d2y=vect[ids+2][1]-vect[ids+1][1];
            double d2z=vect[ids+2][2]-vect[ids+1][2];
            double d2=pow(pow(d2x,2)+pow(d2y,2)+pow(d2z,2),0.5);
            //On ajoute la différence de distance à DX,DY,DZ (longueur de l'arc*rapport des distances)
            if (d2==0){
                DX=0;
                DY=0;
                DZ=0;
            }
            else {
                DX=d2x*(d1/d2);
                DY=d2y*(d1/d2);
                DZ=d2z*(d1/d2);
            }
            // Remise à 0 des déplacements
            dX = 0;
            dY = 0;
            dZ = 0;
            //On passe sur l'arc suivant
            ids += 1;

            //Si on est sur le point final, retour au début
            DXint = vect[ids+1][0]-vect[ids][0];
        }
    }
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------

// Fonction spéciale pour les fleches directionnelles
void SpecialKeys(int key1, int x, int y)
{
    switch (key1)
    {
    // Rotation autour de l'axe Z
        case GLUT_KEY_LEFT:
        RotZ -= 2;
            break;
        case GLUT_KEY_RIGHT:
        RotZ += 2;
            break;
    // Rotation autour de l'axe X
        case GLUT_KEY_UP:
            RotY -= 2;
            break;
        case GLUT_KEY_DOWN:
        RotY += 2;
            break;
    }
}

int nFullScreen=0;
////////////////////////// Clavier //////////////////////////////
void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    // Quitter
    case 27 : // Touche Echap
    case 'a':
        exit(0);
        break;
        // Rotation autour de l'axe Y
    case 'z':
        RotX -= 2;
        break;
    case 's':
        RotX += 2;
        break;

        // Translation selon l'axe Z
    case 'i':
        TranZ += 5;
        break;
    case 'k':
        TranZ -= 5;
        break;
        // Translation selon l'axe X
    case 'y':
        TranX += 5;
        break;
    case 'h':
        TranX -= 5;
        break;
        // Translation selon l'axe Y
    case 'j':
        TranY += 5;
        break;
    case 'g':
        TranY -= 5;
        break;


        // Réinitialisation de la camera
    case 'e':
        RotX = 0;
        RotY = 0;
        RotZ = 0;
        TranX = 0;
        TranY = 0;
        TranZ = 0;
        break;
        // Changement pt de vue
        // Plateau
    case 'w':
        idptdevue = 0;
        RotX = 0;
        RotY = 0;
        RotZ = 0;
        TranX = 0;
        TranY = 0;
        TranZ = 0;
        break;
        // oeil
    case 'x':
        idptdevue = 1;
        RotX = 0;
        RotY = 0;
        RotZ = 0;
        TranX = 0;
        TranY = 0;
        TranZ = 0;
        break;

        // Changer de type de tracé (triangles/lignes)
    case 'v':
        firstboucle=1;
        triangles = (triangles==0) ? 1 : 0;
        break;
        // Couleurs avec/sans
    case 'c':
        aveccolor = (aveccolor==0) ? 1 : 0;
        break;
        // IsoHypses avec/sans
    case 'b':
        aveciso = (aveciso==0) ? 1 : 0;
        break;
        // Accélerer
    case '+':
        if (idv<10){idv += 1;}
        break;
        // Décélerer
    case '-':
        if (idv>0){idv -= 1;}
        break;
        // Pause
    case 'p':
        idv = 0;
        break;
        // Redémarrer
    case 'm':
        // Si dépassement du temps, retour au départ
        ids = 0;
        // Remise à 0 des déplacements
        DX = 0;
        DY = 0;
        DZ = 0;
        dX = 0;
        dY = 0;
        dZ = 0;
        break;

        // Changer de type de parcours
    case 'l':
        typedist = (typedist==0) ? 1 : 0;
        break;

        // Passer en Mode plein écran
    case 13 : // Touche entrée
        if (nFullScreen==0)
        {
            glutFullScreen();
            nFullScreen=1;
            break;
        }
        if (nFullScreen==1)
        {
            int width = glutGet(GLUT_WINDOW_WIDTH);
            int height = glutGet(GLUT_WINDOW_HEIGHT);
            glutReshapeWindow(width-200,height-200);
            glutPositionWindow (50,50);
            nFullScreen=0;
            break;
        }
        break;

    }
    glutPostRedisplay();
}

// Changement de point de vue (de camera)
void ptdevue(int idptdevue)
{

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (idptdevue == 0)
    {
        // Vue sur le MNT quel que soit l'endroit où on le met (hauteur caméra à modifier selon MNT)
#if defined(AVEC_TEST)
        gluLookAt((tab[0][0]+tab[a*b-1][0])/2 +TranX,
                tab[0][1]-3*(tab[a*b-1][1]-tab[0][1]) +TranY,
                5*tab[0][2] +TranZ,
                (tab[0][0]+tab[a*b-1][0])/2 +TranX,
                (tab[0][1]+tab[a*b-1][1])/2 +TranY,
                (tab[0][2]+tab[a*b-1][2])/2 +TranZ,
                0,0,1);
#else

        gluLookAt((tab[0][0]+tab[a*b-1][0])/2 +TranX,
                (tab[0][1]+tab[a*b-1][1])/2 +TranY,
                (tab[0][2]+tab[a*b-1][2])/2+200 +TranZ,
                tab[a*b/2][0] +TranX,
                tab[a*b/2][1] +TranY,
                tab[a*b/2][2] +TranZ,
                0,0,1);
#endif
        // affichage initial
        glPushMatrix();
        glTranslated((tab[0][0]+tab[a*b-1][0])/2 +TranX,(tab[0][1]+tab[a*b-1][1])/2 +TranY,0);
        glRotated(RotX,1,0,0);
        glRotated(RotY,0,1,0);
        glRotated(RotZ,0,0,1);
        glTranslated(-(tab[0][0]+tab[a*b-1][0])/2 -TranX,-((tab[0][1]+tab[a*b-1][1])/2 +TranY),0);
        //glPopMatrix();
    }
    if (idptdevue == 1)
    {
        // Déplacement de la caméra (placé sur la trace GPX, vise le sommet suivant)
        gluLookAt(vect[ids][0] + DX,
                vect[ids][1] + DY,
                vect[ids][2] + DZ +2,
                vect[ids+1][0],
                vect[ids+1][1],
                vect[ids+1][2] +2,
                0,0,1);
        //
        // Rotations quand la caméra suit la trace
        glPushMatrix();
        glTranslated(vect[ids][0] + DX, vect[ids][1] + DY, vect[ids][2] + DZ +1);
        glRotated(RotX,vect[ids+1][0]-vect[ids][0], vect[ids+1][1]-vect[ids][1],0);
        glRotated(RotY,vect[ids+1][1]-vect[ids][1],-vect[ids+1][0]+vect[ids][0],0);
        glRotated(RotZ,0,0,1);
        glTranslated(-vect[ids][0] - DX,-vect[ids][1] - DY, -vect[ids][2] - DZ -1);
        //glPopMatrix();
    }

}
