/* atom.cpp
 */

#include "atom.h"


// Constructor
Atom::Atom( Element* initType, int initX, int initY, int initDxActual, int initDyActual, int initDxIdeal, int initDyIdeal, int initCollisions, int initTracked )
{
   // Copy constructor arguments
   type = initType;
   x = initX;
   y = initY;
   dx_actual = initDxActual;
   dy_actual = initDyActual;
   dx_ideal = initDxIdeal;
   dy_ideal = initDyIdeal;
   collisions = initCollisions;
   tracked = initTracked;

   // Increment Element type counter
   type->setCount( type->getCount() + 1 );
}


// Deconstructor
Atom::~Atom()
{
   // Decrement Element type counter
   type->setCount( type->getCount() - 1 );
}


Element*
Atom::getType()
{
   return type;
}


void
Atom::setType( Element* newType )
{
   // Decrement old Element type counter
   type->setCount( type->getCount() - 1 );

   type = newType;

   // Increment new Element type counter
   type->setCount( type->getCount() + 1 );
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
Atom::getDxActual()
{
   return dx_actual;
}


void
Atom::setDxActual( int newDxActual )
{
   dx_actual = newDxActual;
}


int
Atom::getDyActual()
{
   return dy_actual;
}


void
Atom::setDyActual( int newDyActual )
{
   dy_actual = newDyActual;
}


int
Atom::getDxIdeal()
{
   return dx_ideal;
}


void
Atom::setDxIdeal( int newDxIdeal )
{
   dx_ideal = newDxIdeal;
}


int
Atom::getDyIdeal()
{
   return dy_ideal;
}


void
Atom::setDyIdeal( int newDyIdeal )
{
   dy_ideal = newDyIdeal;
}


int
Atom::getCollisions()
{
   return collisions;
}


void
Atom::setCollisions( int newCollisions )
{
   collisions = newCollisions;
}


int
Atom::isTracked()
{
   return tracked;
}


void
Atom::setTracked( int newTracked )
{
   tracked = newTracked;
}


void
Atom::toggleTracked()
{
   tracked = !tracked;
}

