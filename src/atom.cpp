/* atom.cpp
 */

#include <stdio.h>
#include "atom.h"
#include "element.h"


// Static members
Atom **Atom::world;
int Atom::worldX;
int Atom::worldY;


// Constructor
Atom::Atom( int initType, int initX, int initY, int initDx, int initDy )
{
   // Copy constructor arguments
   type = initType;
   x = initX;
   y = initY;
   dx = initDx;
   dy = initDy;

   // Place the new Atom in the world
   if( world[ x + y * worldX ] == NULL )
   {
      world[ x + y * worldX ] = this;
   }
   else
   {
      printf( "Replacing atom at (%d,%d) with new atom.\n", x, y );
      world[ x + y * worldX ] = this;
   }
}


// Returns the pointer to an Atom in the world
Atom*
Atom::getAtom( int x, int y )
{
   return world[ x + y * worldX ];
}


// Create the world and the initial atoms
void
Atom::initWorld()
{
   worldX = 16;
   worldY = 16;
   world = new Atom*[ worldX * worldY ];

   new Atom( 2, 4, 11 );
   new Atom( 5, 7, 1 );
   new Atom( 13, 8, 2 );
   new Atom( 7, 8, 15 );
}


void
Atom::printWorld()
{
   for( int y = 0; y < worldY; y++ )
   {
      for( int x = 0; x < worldX; x++ )
      {
         if( world[ x + y * worldX ] == NULL )
         {
            printf( ". " );
         }
         else
         {
            printf( "%s ", Element::getElement( world[ x + y * worldX ]->getType() )->getName() );
         }
      }
      printf( "\n" );
   }
}


int
Atom::getType()
{
   return type;
}


int
Atom::getX()
{
   return x;
}


int
Atom::getY()
{
   return y;
}


int
Atom::getDx()
{
   return dx;
}


int
Atom::getDy()
{
   return dy;
}

