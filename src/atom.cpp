/* atom.cpp
 */

#include <iostream>
#include "atom.h"


// Static members
Atom** Atom::world;
int* Atom::claimed;
int Atom::worldX;
int Atom::worldY;


// Constructor
Atom::Atom( Element* initType, int initX, int initY, int initDx, int initDy )
{
   // Copy constructor arguments
   type = initType;
   x = initX % worldX;
   y = initY % worldY;
   dx = initDx;
   dy = initDy;

   // Place the new Atom in the world
   if( world[ getWorldIndex(x,y) ] == NULL )
   {
      world[ getWorldIndex(x,y) ] = this;
   }
   else
   {
      std::cout << "Replacing atom at (" << x << "," << y << ") with new atom." << std::endl;
      world[ getWorldIndex(x,y) ] = this;
   }
}


// Returns the pointer to an Atom in the world
Atom*
Atom::getAtom( int x, int y )
{
   return world[ getWorldIndex(x,y) ];
}


// Create the world and the initial atoms
void
Atom::initWorld()
{
   worldX = 16;
   worldY = 16;
   world = new Atom*[ worldX * worldY ];
   claimed = new int[ worldX * worldY ];

   new Atom( Element::getElement(2),  4,  13 );
   new Atom( Element::getElement(5),  10, 1  );
   new Atom( Element::getElement(13), 11, 2  );
   new Atom( Element::getElement(7),  8,  15 );
}


void
Atom::printWorld()
{
   for( int y = 0; y < worldY; y++ )
   {
      for( int x = 0; x < worldX; x++ )
      {
         if( world[ getWorldIndex(x,y) ] == NULL )
         {
            std::cout << ". ";
         }
         else
         {
            std::cout << world[ getWorldIndex(x,y) ]->getType()->getName() << " ";
         }
      }
      std::cout << std::endl;
   }
   std::cout << std::endl;
}


void
Atom::moveAtoms()
{
   // Clear all claimed flags
   for( int y = 0; y < worldY; y++ )
   {
      for( int x = 0; x < worldX; x++ )
      {
         claimed[ getWorldIndex(x,y) ] = 0;
      }
   }

   // Stake claims
   for( int y = 0; y < worldY; y++ )
   {
      for( int x = 0; x < worldX; x++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         {
            int newX = x - 1;
            int newY = y + 1;
            claimed[ getWorldIndex(x,y) ]++;
            claimed[ getWorldIndex(newX,newY) ]++;
         }
      }
   }

   // Move if there are no collisions
   for( int y = 0; y < worldY; y++ )
   {
      for( int x = 0; x < worldX; x++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         {
            int newX = x - 1;
            int newY = y + 1;
            if( claimed[ getWorldIndex(x,y) ] == 1 && claimed[ getWorldIndex(newX,newY) ] == 1 )
            {
               world[ getWorldIndex(x,y) ]->setX(newX);
               world[ getWorldIndex(x,y) ]->setY(newY);
               world[ getWorldIndex(newX,newY) ] = world[ getWorldIndex(x,y) ];
               world[ getWorldIndex(x,y) ] = NULL;
               claimed[ getWorldIndex(x,y) ]++;
               claimed[ getWorldIndex(newX,newY) ]++;
            }
         }
      }
   }
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
   x = newX % worldX;
}


int
Atom::getY()
{
   return y;
}


void
Atom::setY( int newY )
{
   y = newY % worldY;
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


int
Atom::getWorldIndex( int x, int y )
{
   return ( x + y * worldX ) % ( worldX * worldY );
}

