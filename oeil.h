/**
  \class oeil
  \brief Draw a sphere representing the eye
  \author philemon.renaud@ensg.eu, clement.drouadaine@ensg.eu, marie.aladenise@ensg.eu
  \date autumn 2014
  \file oeil.h
*/

#ifndef OEIL_H
#define OEIL_H

class oeil
{
public:
    /**
        \fn oeil()
        \brief Constructor
    */
    oeil();
    /** \fn draw_oeil(float Xpos, float Ypos, float Zpos , int sizeeye)
        \brief draw the eye
        \
        \param Xpos Absolute x position
        \param Ypos Absolute y position
        \param Zpos Absolute z position
        \param sizeeye size of the eye
    */
    void draw_oeil(float Xpos, float Ypos, float Zpos , int sizeeye);
    /**
        \fn ~oeil()
        \brief Destructor
    */
    virtual ~oeil();
protected:
private:
};

#endif // OEIL_H
