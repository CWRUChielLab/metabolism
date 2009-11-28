/* sim.cpp
 */

//#ifndef _GNU_SOURCE
//#define _GNU_SOURCE   /* Needed for posix_memalign... ? */
//#endif

#include <cassert>
#include <cstdarg>
#include <cstdlib>
#include <map>
#include <ncurses.h>
#include <SFMT.h>
#include <vector>
#include "sim.h"


// Constructor
Sim::Sim()
{
   // Nothing to do yet
}


void
Sim::initialize()
{
   // Setup the world
   worldX = 16;
   worldY = 16;
   world = new Atom*[ worldX * worldY ];
   claimed = new int[ worldX * worldY ];
   
   // SFMT prep
   init_gen_rand( (uint32_t)(42) );
   int rc = 0;
   for( direction_sz64 = get_min_array_size64() * 8; direction_sz64 < (unsigned int)( worldX * worldY ); direction_sz64 *= 2 );
   rc = posix_memalign( (void**)&direction, getpagesize(), direction_sz64 );
   assert( rc == 0 );
   assert( direction );

   // Initialize the periodicTable
   Element* tempEle;
   for( char c = 'A'; c <= 'H'; c++ )
   {
      std::string s(1,c);
      tempEle = new Element( s, 0, 0 );
      periodicTable[ s ] = tempEle;
   }

   // Initialize the rxnTable
   Reaction* tempRxn;

   tempRxn = new Reaction( ev(2,"A","B"), ev(2,"C","D"), 0.02 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   tempRxn = new Reaction( ev(2,"E","F"), ev(2,"G","H"), 0.03 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   tempRxn = new Reaction( ev(2,"A","A","D"), ev(1,"H"), 0.1 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   tempRxn = new Reaction( ev(1,"B"), ev(3,"F","D","H"), 1 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   // Initialize the world
   Atom* tempAtom;
   int x,y;

   x = 4; y = 13;
   tempAtom = new Atom( periodicTable["A"], x, y );
   world[ getWorldIndex(x,y) ] = tempAtom;

   x = 10; y = 1;
   tempAtom = new Atom( periodicTable["C"], x, y );
   world[ getWorldIndex(x,y) ] = tempAtom;

   x = 11; y = 2;
   tempAtom = new Atom( periodicTable["F"], x, y );
   world[ getWorldIndex(x,y) ] = tempAtom;

   x = 8; y = 15;
   tempAtom = new Atom( periodicTable["D"], x, y );
   world[ getWorldIndex(x,y) ] = tempAtom;
}



void
Sim::printElements()
{
   printw( "There are %d elements.\n", periodicTable.size() );
   for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
   {
      Element* ele = i->second;
      printw( "periodicTable[\"%s\"] has key:\t%d\n", ele->getName().c_str(), periodicTable[ele->getName()]->getKey() );
   }
   refresh();
}


void
Sim::printReactions()
{
   printw( "There are %d reactions.\n", rxnTable.size() );
   for( ReactionMap::iterator i = rxnTable.begin(); i != rxnTable.end(); i++ )
   {
      Reaction* rxn = i->second;
      std::map<std::string,int> reactantCount;
      std::map<std::string,int> productCount;
      for( unsigned int j = 0; j < rxn->getReactants().size(); j++ )
      {
         reactantCount[ rxn->getReactants()[j]->getName() ]++;
      }
      for( unsigned int j = 0; j < rxn->getProducts().size(); j++ )
      {
         productCount[ rxn->getProducts()[j]->getName() ]++;
      }

      printw( "Key: %d  \t", rxn->getKey() );
      for( std::map<std::string,int>::iterator j = reactantCount.begin(); j != reactantCount.end(); j++ )
      {
         int coefficient = j->second;
         if( coefficient == 1 )
         {
            if( j == reactantCount.begin() )
            {
               printw( "%s", j->first.c_str() );
            }
            else
            {
               printw( " + %s", j->first.c_str() );
            }
         }
         else
         {
            if( j == reactantCount.begin() )
            {
               printw( "%d%s", j->second, j->first.c_str() );
            }
            else
            {
               printw( " + %d%s", j->second, j->first.c_str() );
            }
         }
      }
      printw( " -> " );
      for( std::map<std::string,int>::iterator j = productCount.begin(); j != productCount.end(); j++ )
      {
         int coefficient = j->second;
         if( coefficient == 1 )
         {
            if( j == productCount.begin() )
            {
               printw( "%s", j->first.c_str() );
            }
            else
            {
               printw( " + %s", j->first.c_str() );
            }
         }
         else
         {
            if( j == productCount.begin() )
            {
               printw( "%d%s", j->second, j->first.c_str() );
            }
            else
            {
               printw( " + %d%s", j->second, j->first.c_str() );
            }
         }
      }
      printw( "\n" );
   }
   refresh();
}


void
Sim::printWorld()
{
   for( int y = 0; y < worldY; y++ )
   {
      for( int x = 0; x < worldX; x++ )
      {
         if( world[ getWorldIndex(x,y) ] == NULL )
         {
            printw( ". " );
         }
         else
         {
            printw( "%s ", world[ getWorldIndex(x,y) ]->getType()->getName().c_str() );
         }
      }
      printw( "\n" );
   }
   printw( "\n" );
   refresh();
}


void
Sim::iterate()
{
   generateRandNums();
   moveAtoms();
}


void
Sim::generateRandNums()
{
   // Get a new set of directions
   fill_array64( (uint64_t*)(direction), direction_sz64 / 8 );
}


void
Sim::moveAtoms()
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
            int newX = x + dx(x,y);
            int newY = y + dy(x,y);
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
            int newX = x + dx(x,y);
            int newY = y + dy(x,y);
            if( claimed[ getWorldIndex(x,y) ] == 1 && claimed[ getWorldIndex(newX,newY) ] == 1 )
            {
               swapAtoms( x, y, newX, newY );
               claimed[ getWorldIndex(x,y) ]++;
               claimed[ getWorldIndex(newX,newY) ]++;
            }
         }
      }
   }
}


int
Sim::dx( int x, int y )
{
   int threeRandBits = direction[ getWorldIndex(x,y) ] & 0x7;
   switch( threeRandBits )
   {
      case 0:
         // N
         return 0;
      case 1:
         // NE
         return 1;
      case 2:
         // E
         return 1;
      case 3:
         // SE
         return 1;
      case 4:
         // S
         return 0;
      case 5:
         // SW
         return -1;
      case 6:
         // W
         return -1;
      case 7:
         // NW
         return -1;
      default:
         assert( 0 );
   }
}


int
Sim::dy( int x, int y )
{
   int threeRandBits = direction[ getWorldIndex(x,y) ] & 0x7;
   switch( threeRandBits )
   {
      case 0:
         // N
         return -1;
      case 1:
         // NE
         return -1;
      case 2:
         // E
         return 0;
      case 3:
         // SE
         return 1;
      case 4:
         // S
         return 1;
      case 5:
         // SW
         return 1;
      case 6:
         // W
         return 0;
      case 7:
         // NW
         return -1;
      default:
         assert( 0 );
   }
}


// Returns an ElementVector containing elementCount
// Elements specified as a comma separated list of
// names, e.g. ev(2,"A","B")
ElementVector
Sim::ev( int elementCount, ... )
{
   ElementVector ev;
   char* name;

   va_list nameList;
   va_start( nameList, elementCount );
   for( int i = 0; i < elementCount; i++ )
   {
      name = va_arg( nameList, char* );
      std::string stringName(name);
      Element* ele = periodicTable[stringName];
      ev.push_back( ele );
   }
   va_end( nameList );

   return ev;
}


// Handles wrapping around the edges of the world
// and translating two-dimensional coordinates
// to a one-dimensional index for the world array
int
Sim::getWorldIndex( int x, int y )
{
   int wrappedX = ( x + worldX ) % worldX;
   int wrappedY = ( y + worldY ) % worldY;
   return ( wrappedX + wrappedY * worldX );
}


void
Sim::swapAtoms( int x1, int y1, int x2, int y2 )
{
   Atom* atom1 = world[ getWorldIndex( x1, y1 ) ];
   Atom* atom2 = world[ getWorldIndex( x2, y2 ) ];
   if( atom1 != NULL )
   {
      atom1->setDx( atom1->getDx() + ( x2 - x1 ) );
      atom1->setDy( atom1->getDy() + ( y2 - y1 ) );
      atom1->setX( ( x2 + worldX ) % worldX );
      atom1->setY( ( y2 + worldY ) % worldY );
   }
   if( atom2 != NULL )
   {
      atom2->setDx( atom2->getDx() + ( x1 - x2 ) );
      atom2->setDy( atom2->getDy() + ( y1 - y2 ) );
      atom2->setX( ( x1 + worldX ) % worldX );
      atom2->setY( ( y1 + worldY ) % worldY );
   }
   world[ getWorldIndex( x1, y1 ) ] = atom2;
   world[ getWorldIndex( x2, y2 ) ] = atom1;
}

