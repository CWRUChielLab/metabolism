/* atom.h
 */

#ifndef ATOM_H
#define ATOM_H 

#include "element.h"

class Atom
{
   public:
      // Constructor
      Atom( Element* initType, int initX, int initY, int initDxActual = 0, int initDyActual = 0, int initDxIdeal = 0, int initDyIdeal = 0, int initCollisions = 0 );

      // Get and set functions
      Element* getType();
      void setType( Element* newType );
      int getX();
      void setX( int newX );
      int getY();
      void setY( int newY );
      int getDxActual();
      void setDxActual( int newDxActual );
      int getDyActual();
      void setDyActual( int newDyActual );
      int getDxIdeal();
      void setDxIdeal( int newDxIdeal );
      int getDyIdeal();
      void setDyIdeal( int newDyIdeal );
      int getCollisions();
      void setCollisions( int newCollisions );

   private:
      // Atom attributes
      Element* type;
      int x;
      int y;
      int dx_actual;
      int dy_actual;
      int dx_ideal;
      int dy_ideal;
      int collisions;
};

#endif /* ATOM_H */
