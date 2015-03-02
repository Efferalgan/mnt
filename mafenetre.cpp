#include "mafenetre.h"

mafenetre::mafenetre() : QMainWindow()
{
    //Mainwindow

    this->setPosition();
    this->setMinimumSize(270,300);
    setWindowTitle("Choix des paramètres ");
    setPalette(QPalette(QColor(204,229,255)));
    setAutoFillBackground(true);

    vbox1=new  QGridLayout;// widgets position adapt automatically to the mainwindow size
    QWidget *widget = new QWidget();
    widget->setLayout(vbox1);
    setCentralWidget(widget);

    //Buttons and labels
    m_buttonDialogueMNT= new QPushButton("Charger un MNT", this);
    m_buttonDialogueMNT->setCursor(Qt::PointingHandCursor);
    m_buttonDialogueMNT->setStyleSheet(" background-color: white;padding: 3px;");

    m_label = new QLabel(this);

    m_buttonDialogueGPX= new QPushButton("Charger un GPX", this);
    m_buttonDialogueGPX->setCursor(Qt::PointingHandCursor);
    m_buttonDialogueGPX->setStyleSheet("background-color: white;padding: 3px;");

    m_label2 = new QLabel(this);

    queMnt=new QCheckBox("Afficher uniquement un Mnt", this);

    aide=new QPushButton("Aide",this);
    aide->setCursor(Qt::PointingHandCursor);

    fermer=new QPushButton("Fermer", this);
    fermer->setCursor(Qt::PointingHandCursor);
    fermer->setStyleSheet("background-color: white; padding: 3px;");

    valider= new QPushButton("Valider", this);
    valider->setCursor(Qt::PointingHandCursor);
    valider->setStyleSheet(" background-color: white;padding: 3px;");
    if (boolmnt && boolgpx) valider->setEnabled(true);
    else if (!boolmnt || !boolgpx) valider->setEnabled(false);

    connect(m_buttonDialogueMNT, SIGNAL(clicked()), this, SLOT(adresseMNT()));
    connect(m_buttonDialogueGPX, SIGNAL(clicked()), this, SLOT(adresseGPX()));
    connect(queMnt, SIGNAL(clicked()), this, SLOT(desactiverGpx()));
    connect(aide, SIGNAL(clicked()), this, SLOT(activerAide()));
    connect(fermer, SIGNAL(clicked()),qApp, SLOT(quit()));
    connect(valider,  SIGNAL(clicked()),this, SLOT(lancerOpenGl()));


    //Speed choice

    groupbox = new QGroupBox("Vitesse initiale du parcours", this);
    groupbox->setStyleSheet("background-color: white");
    groupbox->setStyleSheet( "subcontrol-origin: margin");

    vitesseReelle = new QRadioButton("Vitesse réelle", this);
    vitesseConstante = new QRadioButton("Vitesse constante", this);
    choixVitesse= new QSpinBox(this);
    choixVitesse->setRange(1,10);//minimum and maximum speeds

    vitesseReelleDisponible=false;//by default

    vitesseConstante->setChecked(true);//by default, not real speed is selected
    choixVitesse->setEnabled(true);//choosing a constant speed is not possible
    vitesseReelle->setCheckable(false);

    //connect(vitesseConstante, SIGNAL(clicked()), this, SLOT(activerSpin()));
    //connect(choixVitesse, SIGNAL(valueChanged(int)), this, SLOT(activerSpin()));
    connect(choixVitesse, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    if (vitesseReelleDisponible) (connect(vitesseReelle, SIGNAL(clicked()), this, SLOT(desactiverSpin())));

    connect(vitesseReelle, SIGNAL(clicked()), this, SLOT(retournerChoix()));
    connect(vitesseConstante, SIGNAL(clicked()), this, SLOT(retournerChoixConstante()));
    QGridLayout *vboxVitesse=new QGridLayout;
    vboxVitesse->addWidget(vitesseReelle,0,0,1,1);
    vboxVitesse->addWidget(vitesseConstante,1,0, 1, 1);
    vboxVitesse->addWidget(choixVitesse,2,0,1,1);
    groupbox->setLayout(vboxVitesse);

    //Widgets position in the mainwindow
    vbox1->addWidget(m_buttonDialogueMNT,0,1,1,2);
    vbox1->addWidget(m_buttonDialogueGPX,1,1,1,2);
    vbox1->addWidget(m_label,0,3,1,1);
    vbox1->addWidget(m_label2,1,3,1,1);
    vbox1->setVerticalSpacing(15);
    vbox1->addWidget(queMnt, 2,1);
    vbox1->addWidget(groupbox, 3,0,3,4);
    vbox1->addWidget(aide,7,1,1,2);
    vbox1->addWidget(valider,8,1,1,2);
    vbox1->addWidget(fermer,9,1,1,2);
}

mafenetre::~mafenetre(){

    delete this;
}

void mafenetre::retournerChoix(){
    choixVitesseReelle=true;
    spinval=5;//default value set  to 4


}

void mafenetre::setValue(int a){
    spinval=a;

}
void mafenetre::retournerChoixConstante(){
    choixVitesseReelle=false;

}

void mafenetre::adresseMNT(){
    string extension= "mnt";
    string adresse=selectionFichier(extension);
}

void mafenetre::adresseGPX(){
    string extension= "gpx";
    string adresse=selectionFichier(extension);

}

string mafenetre::selectionFichier(string & extensions)
{
    QFileDialog *fd = new QFileDialog( this, "selectionner fichier");
    fd->setFileMode(QFileDialog::ExistingFile );
    if (extensions== "mnt") fd->setNameFilter(tr("fichiers(*.asc *.xyz)"));
    else fd->setNameFilter(tr("fichiers(*.gpx)"));
    fd->setViewMode(QFileDialog::List);

    QStringList fileNames;
    QString fichier;
    string titreParcours;

    if (fd->exec()){
        fileNames = fd->selectedFiles();
        fichier = fileNames.join(" ");
        //retrieve absolute path of the selected file
        string chemin=fichier.toStdString().c_str();


        if (extensions== "mnt"){
            mntPath=fichier;
            QPixmap ok("../Data/tickok2.gif");
            m_label->setPixmap(ok);
            boolmnt=true;

        }
        else if (extensions== "gpx"){
            gpxPath=fichier;
            QPixmap ok("../Data/tickok2.gif");
            m_label2->setPixmap(ok);
            boolgpx=true;
        }
        if (boolmnt && boolgpx && !boolqueMnt) {
            newMnt = *(new mnt(mntPath,false));
            gpx newGpx(gpxPath);
            vitesseReelleDisponible=newGpx.doesTimeExist();

            if (vitesseReelleDisponible) {
                //if time is available in the gpx, possibility to select real speed
                vitesseReelle->setCheckable(true);
                choixVitesse->setEnabled(false);

            }
            //if the gpx is within the mnt
            if (newGpx.inMnt(newMnt))
            {
                valider->setEnabled(true);
                QMessageBox::information(fd, "Information","Chargement réussi de: \n"+mntPath+"\n"+gpxPath );
            }
            else {
                valider->setEnabled(false);
                QMessageBox::critical(fd, "Erreur","Le gpx n'est pas dans le mnt \n Recommencer votre selection" );
                QPixmap no("../Data/tickno4.png");
                m_label->setPixmap(no);
                m_label2->setPixmap(no);
                mntPath.clear();
                gpxPath.clear();
                boolmnt=false;
                boolgpx=false;
            }

        }
        else if (boolqueMnt && boolmnt){
            newMnt = *(new mnt(mntPath,false));
            valider->setEnabled(true);
            QMessageBox::information(fd, "Information","Chargement réussi de: \n"+mntPath);
            //if  (!boolmnt) valider->setEnabled(false);
        }
        else if (!boolmnt){
            //if (!boolgpx || !boolqueMnt) {
                valider->setEnabled(false);
            //}
        }
        return chemin;}

    else{
        QMessageBox::critical(fd, "Attention","Aucun fichier \n selectionné" );

        if (extensions== "mnt"){
            QPixmap no("../Data/tickno4.png");
            m_label->setPixmap(no);
            boolmnt=false;}
        else if (extensions== "gpx"){
            QPixmap no("../Data/tickno4.png");
            m_label2->setPixmap(no);
            boolgpx=false;}
        if (boolmnt && boolgpx) valider->setEnabled(true);
        else if (!boolmnt|| !boolgpx) valider->setEnabled(false);

        return "erreur";
    }
}



void mafenetre::activerSpin(){
    choixVitesse->setEnabled(true);
    connect(choixVitesse, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
}


void mafenetre::desactiverSpin(){

    choixVitesse->setEnabled(false);
}

void mafenetre::lancerOpenGl(){
    main_opengl w;
    mnt* myMnt = new mnt(newMnt);
    cout<<"valuer spinval retenue "<<spinval<<endl;
    w.func_main(*myMnt,gpxPath,spinval,choixVitesseReelle);
}

void mafenetre::setPosition(){

    //QSize size = this->sizeHint();
    QDesktopWidget* desktop = QApplication::desktop();
    int width = desktop->width();
    int height = desktop->height();
    int centerW = (width/2) -width/10;
    int centerH = (height/2)-height/6;
    this->move(centerW, centerH);
}

void mafenetre::activerAide(){

    QDialog fenAide(this);
    fenAide.setWindowTitle("Aide");
    QVBoxLayout *layout=new QVBoxLayout;
    QLabel *image=new QLabel(&fenAide);
    image->setPixmap(QPixmap("../Data/keybordfleche.png"));
    layout->addWidget(image);
    fenAide.setLayout(layout);
    fenAide.exec();
}
void mafenetre::desactiverGpx(){
    if (queMnt->isChecked()){
        //if showing only a mnt
        m_buttonDialogueGPX->setEnabled(false);
        boolqueMnt=true;
        vitesseReelle->setEnabled(false);
        vitesseConstante->setEnabled(false);
        choixVitesse->setEnabled(false);
        m_label2->clear();
        gpxPath="../Data/fakeGpx.gpx";

    }
    else {
        //if showing a mnt and a gpx
        m_buttonDialogueGPX->setEnabled(true);
        boolqueMnt=false;
        vitesseReelle->setEnabled(true);
        vitesseConstante->setEnabled(true);
        choixVitesse->setEnabled(true);
        valider->setEnabled(false);
    }
}
