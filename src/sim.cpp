/* sim.cpp
 */

//#include <iostream>
#include <map>
#include <ncurses.h>
#include <vector>
#include "sim.h"


// Constructor
Sim::Sim()
{
   //
}


void
Sim::initialize()
{
   // Setup the world
   worldX = 16;
   worldY = 16;
   world = new Atom*[ worldX * worldY ];
   claimed = new int[ worldX * worldY ];

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

   ElementVector r1;
   r1.push_back(periodicTable["A"]);
   r1.push_back(periodicTable["B"]);
   ElementVector p1;
   p1.push_back(periodicTable["C"]);
   p1.push_back(periodicTable["D"]);
   tempRxn = new Reaction( r1, p1, 0.02 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   ElementVector r2;
   r2.push_back(periodicTable["E"]);
   r2.push_back(periodicTable["F"]);
   ElementVector p2;
   p2.push_back(periodicTable["G"]);
   p2.push_back(periodicTable["H"]);
   tempRxn = new Reaction( r2, p2, 0.03 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   ElementVector r3;
   r3.push_back(periodicTable["A"]);
   r3.push_back(periodicTable["A"]);
   r3.push_back(periodicTable["D"]);
   ElementVector p3;
   p3.push_back(periodicTable["H"]);
   tempRxn = new Reaction( r3, p3, 0.1 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   ElementVector r4;
   r4.push_back(periodicTable["B"]);
   ElementVector p4;
   p4.push_back(periodicTable["F"]);
   p4.push_back(periodicTable["D"]);
   p4.push_back(periodicTable["H"]);
   tempRxn = new Reaction( r4, p4, 1 );
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
            int newX = x + 1;
            int newY = y;
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
            int newX = x + 1;
            int newY = y;
            if( claimed[ getWorldIndex(x,y) ] == 1 && claimed[ getWorldIndex(newX,newY) ] == 1 )
            {
               world[ getWorldIndex(x,y) ]->setX(newX);     //TODO: Handle wrapping
               world[ getWorldIndex(x,y) ]->setY(newY);     //TODO: Handle wrapping
               world[ getWorldIndex(newX,newY) ] = world[ getWorldIndex(x,y) ];
               world[ getWorldIndex(x,y) ] = NULL;
               claimed[ getWorldIndex(x,y) ]++;
               claimed[ getWorldIndex(newX,newY) ]++;
            }
         }
      }
   }
}


int
Sim::getWorldIndex( int x, int y )
{
   int wrappedX = x % worldX;
   int wrappedY = y % worldY;
   return ( wrappedX + wrappedY * worldX );
}

