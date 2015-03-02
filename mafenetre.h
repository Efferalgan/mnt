/**
  * \class mafenetre
  * \brief Creating of a mainwindow with widgets to configure and launch the OpenGL animation
  * \author clement.drouadaine@ensg.eu, philemon.renaud@ensg.eu, marie.aladenise@ensg.eu
  * \date November 2014
  * \file mafenetre.h
*/

#ifndef MAFENETRE_H
#define MAFENETRE_H

#include <QApplication>
#include <QMainWindow>
#include <QRadioButton>
#include <QPushButton>
#include <QGroupBox>
#include <QSpinBox>
#include <QLabel>
#include <QtWidgets>
#include <QLayout>
#include <QCheckBox>
#include "main_opengl.h"
#include "gpx.h"
#include "mnt.h"

class mafenetre: public QMainWindow
{
    Q_OBJECT;

    private:

        QPushButton *m_buttonDialogueMNT;//loading a mnt
        QPushButton *m_buttonDialogueGPX;// loading a gpx
        QCheckBox *queMnt;//only loading a mnt
        QGroupBox *groupbox;
        QRadioButton *vitesseReelle;//real speed available if the gpx contains time information
        QRadioButton *vitesseConstante;//speed chosen by the user arbitrarily
        QSpinBox *choixVitesse;
        QPushButton *aide;// display help
        QPushButton *valider;//launch opengl animation
        QPushButton *fermer;

        /**
         * @var bool vitesseReelleDisponible
         * @brief true if gpx contains time information
         */
        bool vitesseReelleDisponible;
        /**
         * @var bool boolmnt
         * @brief true if a file mnt is selected
         */
        bool boolmnt=false;
        /**
         * @var bool boolgpx
         * @brief true if a file gpx is selected
         */
        bool boolgpx=false;
        /**
         * @var bool boolqueMnt
         * @brief true if the user only wants to display a mnt
         */
        bool boolqueMnt;
        QGridLayout *vbox1;
        QLabel* m_label;//thumbnail for mnt loading
        QLabel* m_label2;//thumbnail for gpx loading


        /**
         * @fn mafenetre();
         * @brief setPosition: defines position of the mainwindow on the screen
         */
        void setPosition();

public:

        /**
         * @fn mafenetre();
         * @brief mafenetre: constructor
         */
        mafenetre();

        /**
         * @fn ~mafenetre();
         * @brief ~mafenetre: destructor
         */
         ~mafenetre();
        /**
         * @var int spinval
         * @brief value of the choosen speed
         */
        int spinval;
        /**
         * @var bool choixVitesseReelle
         * @brief true if real speed is selected by the user
         */
        bool choixVitesseReelle;
        /**
         * @var QString mntPath
         * @brief path of the choosen mnt
         */
        QString mntPath;
        /**
         * @var QString gpxPath
         * @brief path of the choosen gpx
         */
        QString gpxPath;

        mnt newMnt;

public slots:


        /**
         * @fn void retournerChoix();
         * @brief retournerChoix: sets the boolean choixVitesseReelle to true if real speed is selected and set spinval to a default value
         */
        void retournerChoix();

        /**
         * @fn void retournerChoixConstante();
         * @brief retournerChoixConstante: sets the boolean choixVitesseReelle to false if constant real is selected
         */
        void retournerChoixConstante();

        /**
         * @fn string selectionFichier(string & extensions)
         * @brief selectionFichier :to choose a file from a QDialog and return absolute path of the selected file
         * @param extensions : extension of the file for which we are looking for a path
         * @return absolute path to the selected file
         */
        std::string selectionFichier(std::string &extensions);

        /**
         * @fn void adresseMNT()
         * @brief adresseMNT: return absolute path of a mnt file
         */
        void adresseMNT();

        /**
         * @fn void adresseGPX()
         * @brief adresseGPX():return absolute path of a gpx file
         */
        void adresseGPX();

        /**
         * @fn void activerSpin()
         * @brief activerSpin(): desactivate the spinbox on the interface
         */
        void activerSpin();

        /**
         * @fn void desactiverSpin()
         * @brief desactiverSpin(): desactivate the spinbox on the interface
         */
        void desactiverSpin();
        /**
         * @fn void lancerOpenGl()
         * @brief lancerOpenGl():launch OpenGL animation
         */
        void lancerOpenGl();
        /**
         * @fn void activerAide()
         * @brief activerAide(): help tips
         */
        void activerAide();
        /**
         * @fn void desactiverGpx()
         * @brief desactiverGpx() : manage widgets according to the user choice to show only the mnt or the mnt and the gpx
         */
        void desactiverGpx();

        /**
         * @fn void setValue(int a)
         * @brief setValue(int a): set the value of spinVal according to the choice of the user
         */
        void setValue(int a);

};

#endif // MAFENETRE_H
