/* atom.h
 */

#ifndef ATOM_H
#define ATOM_H 

#include "element.h"

class Atom
{
   public:
      // Constructor
      Atom( Element* initType, int initX, int initY, int initDx = 0, int initDy = 0 );

      // Get and set functions
      Element* getType();
      void setType( Element* newType );
      int getX();
      void setX( int newX );
      int getY();
      void setY( int newY );
      int getDx();
      void setDx( int newDx );
      int getDy();
      void setDy( int newDy );

   private:
      // Atom attributes
      Element* type;
      int x;
      int y;
      int dx;
      int dy;
};

#endif /* ATOM_H */
