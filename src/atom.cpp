/* atom.cpp
 */

#include "atom.h"


// Constructor
Atom::Atom( Element* initType, int initX, int initY )
{
   // Copy constructor arguments
   type = initType;
   x = initX;
   y = initY;

   // Initialize the Atom
   dx_actual = 0;
   dy_actual = 0;
   dx_ideal = 0;
   dy_ideal = 0;
   collisions = 0;
   tracked = false;

   // Increment Element type counter
   type->count++;
}


// Deconstructor
Atom::~Atom()
{
   // Decrement Element type counter
   type->count--;
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
   type->count--;

   // Set new Element type
   type = newType;

   // Increment new Element type counter
   type->count++;
}


bool
Atom::isTracked()
{
   return tracked;
}


void
Atom::setTracked( bool newTracked )
{
   tracked = newTracked;
}


void
Atom::toggleTracked()
{
   tracked = !tracked;
}

