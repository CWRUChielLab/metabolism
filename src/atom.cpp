/* atom.cpp
 */

#include "atom.h"


// Constructor
Atom::Atom( Element* initType, int initX, int initY, int initDx, int initDy )
{
   // Copy constructor arguments
   type = initType;
   x = initX;
   y = initY;
   dx = initDx;
   dy = initDy;
}


Element*
Atom::getType()
{
   return type;
}


void
Atom::setType( Element* newType )
{
   type = newType;
}


int
Atom::getX()
{
   return x;
}


void
Atom::setX( int newX )
{
   x = newX;
}


int
Atom::getY()
{
   return y;
}


void
Atom::setY( int newY )
{
   y = newY;
}


int
Atom::getDx()
{
   return dx;
}


void
Atom::setDx( int newDx )
{
   dx = newDx;
}


int
Atom::getDy()
{
   return dy;
}


void
Atom::setDy( int newDy )
{
   dy = newDy;
}

