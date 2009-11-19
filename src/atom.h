/* atom.h
 */

#ifndef ATOM_H
#define ATOM_H 

class Atom
{
   public:
      // Constructor
      Atom( int initType, int initX, int initY, int initDx = 0, int initDy = 0 );

      // Static functions
      static Atom* getAtom( int x, int y );
      static void initWorld();
      static void printWorld();

      // Get and set functions
      int getType();
      int getX();
      int getY();
      int getDx();
      int getDy();

   private:
      // Atom attributes
      int type;
      int x;
      int y;
      int dx;
      int dy;

      // Static members
      static Atom **world;
      static int worldX;
      static int worldY;
};

#endif /* ATOM_H */
