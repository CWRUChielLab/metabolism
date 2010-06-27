/* atom.h
 */

#ifndef ATOM_H
#define ATOM_H 

#include "element.h"

class Atom
{
   public:
      // Constructor
      Atom( Element* initType, int initX, int initY );

      // Deconstructor
      ~Atom();

      // Get and set functions
      Element* getType();
      void setType( Element* newType );
      bool isTracked();
      void setTracked( bool newTracked );
      void toggleTracked();

      int x;
      int y;
      int dx_actual;
      int dy_actual;
      int dx_ideal;
      int dy_ideal;
      int collisions;

   private:
      // Atom attributes
      Element* type;
      bool tracked;
};

#endif /* ATOM_H */
