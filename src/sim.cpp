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
   tempEle = new Element( "A", 0, 0 );
   periodicTable[ tempEle->getKey() ] = tempEle;
   tempEle = new Element( "B", 0, 0 );
   periodicTable[ tempEle->getKey() ] = tempEle;
   tempEle = new Element( "C", 0, 0 );
   periodicTable[ tempEle->getKey() ] = tempEle;
   tempEle = new Element( "D", 0, 0 );
   periodicTable[ tempEle->getKey() ] = tempEle;
   tempEle = new Element( "E", 0, 0 );
   periodicTable[ tempEle->getKey() ] = tempEle;
   tempEle = new Element( "F", 0, 0 );
   periodicTable[ tempEle->getKey() ] = tempEle;
   tempEle = new Element( "G", 0, 0 );
   periodicTable[ tempEle->getKey() ] = tempEle;
   tempEle = new Element( "H", 0, 0 );
   periodicTable[ tempEle->getKey() ] = tempEle;

   // Initialize the rxnTable
   Reaction* tempRxn;

   ElementVector p1;
   p1.push_back(periodicTable[2]);
   p1.push_back(periodicTable[3]);
   ElementVector r1;
   r1.push_back(periodicTable[5]);
   r1.push_back(periodicTable[7]);
   tempRxn = new Reaction( p1, r1, 0.02 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   ElementVector p2;
   p2.push_back(periodicTable[11]);
   p2.push_back(periodicTable[13]);
   ElementVector r2;
   r2.push_back(periodicTable[17]);
   r2.push_back(periodicTable[19]);
   tempRxn = new Reaction( p2, r2, 0.03 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   ElementVector p3;
   p3.push_back(periodicTable[2]);
   p3.push_back(periodicTable[2]);
   p3.push_back(periodicTable[7]);
   ElementVector r3;
   r3.push_back(periodicTable[19]);
   tempRxn = new Reaction( p3, r3, 0.1 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   ElementVector p4;
   p4.push_back(periodicTable[3]);
   ElementVector r4;
   r4.push_back(periodicTable[13]);
   r4.push_back(periodicTable[7]);
   r4.push_back(periodicTable[19]);
   tempRxn = new Reaction( p4, r4, 1 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   // Initialize the world
   Atom* tempAtom;
   tempAtom = new Atom( periodicTable[2],  4,  13 );
   world[ getWorldIndex(4,13) ] = tempAtom;
   tempAtom = new Atom( periodicTable[5],  10, 1  );
   world[ getWorldIndex(10,1) ] = tempAtom;
   tempAtom = new Atom( periodicTable[13], 11, 2  );
   world[ getWorldIndex(11,2) ] = tempAtom;
   tempAtom = new Atom( periodicTable[7],  8,  15 );
   world[ getWorldIndex(8,15) ] = tempAtom;
}



void
Sim::printElements()
{
   printw( "There are %d elements.\n", periodicTable.size() );
   for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
   {
      Element* ele = i->second;
      printw( "periodicTable[%d] has name:\t%s\n", ele->getKey(), periodicTable[ele->getKey()]->getName().c_str() );
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
      printw( "Key: %d  \t%s", rxn->getKey(), rxn->getReactants()[0]->getName().c_str() );
      for( unsigned int j = 1; j < rxn->getReactants().size(); j++ )
      {
         printw( " + %s", rxn->getReactants()[j]->getName().c_str() );
      }
      printw( " -> %s", rxn->getProducts()[0]->getName().c_str() );
      for( unsigned int j = 1; j < rxn->getProducts().size(); j++ )
      {
         printw( " + %s", rxn->getProducts()[j]->getName().c_str() );
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

