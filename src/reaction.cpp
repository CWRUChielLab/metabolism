/* reaction.cpp
 */

#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include "reaction.h"


// Static members
Reaction **Reaction::list;


// Private constructor
Reaction::Reaction( int* initReactants, int initNumReactants, int* initProducts, int initNumProducts, double initRate )
{
   // Assign the key
   key = 1;
   for( int i = 0; i < initNumReactants; i++ )
   {
      key *= initReactants[i];
   }

   // Copy constructor arguments
   reactants = initReactants;
   numReactants = initNumReactants;
   fprintf( stdout, "-----------\n%d", reactants[0] );
   for( int i = 1; i < numReactants; i++ )
   {
      fprintf( stdout, " + %d", reactants[i] );
   }
   products = initProducts;
   numProducts = initNumProducts;
   fprintf( stdout, " -> %d", products[0] );
   for( int i = 1; i < numProducts; i++ )
   {
      fprintf( stdout, " + %d", products[i] );
   }
   fprintf( stdout, "\n" );
   rate = initRate;
}


// Handles creating new Reactions and adding them
// to the list, indexed by Reaction key
void
Reaction::addReaction( int* initReactants, int initNumReactants, int* initProducts, int initNumProducts, double initRate )
{
   static int listSize = 0;

   // Create the new Reaction
   Reaction *tempReaction = new Reaction( initReactants, initNumReactants, initProducts, initNumProducts, initRate );

   // Grow the list if there is not enough room for the new Reaction
   while( tempReaction->getKey() >= listSize )
   {
      int newListSize = fmax( listSize * 2, 10 );
      Reaction **tempArray = (Reaction **)malloc( newListSize * sizeof( Reaction * ) );
      for( int i = 0; i < listSize; i++ )
      {
         tempArray[i] = list[i];
      }
      list = tempArray;
      listSize = newListSize;
   }

   // Place the new Reaction in the list, indexed by key
   list[tempReaction->getKey()] = tempReaction;

   
   fprintf( stdout, "-----------\n%d", list[tempReaction->getKey()]->reactants[0] );
   for( int i = 1; i < list[tempReaction->getKey()]->numReactants; i++ )
   {
      fprintf( stdout, " + %d", list[tempReaction->getKey()]->reactants[i] );
   }
   fprintf( stdout, " -> %d", list[tempReaction->getKey()]->products[0] );
   for( int i = 1; i < list[tempReaction->getKey()]->numProducts; i++ )
   {
      fprintf( stdout, " + %d", list[tempReaction->getKey()]->products[i] );
   }
   fprintf( stdout, "\n" );

   fprintf( stdout, "-----------\n%d", tempReaction->reactants[0] );
   for( int i = 1; i < tempReaction->numReactants; i++ )
   {
      fprintf( stdout, " + %d", tempReaction->reactants[i] );
   }
   fprintf( stdout, " -> %d", tempReaction->products[0] );
   for( int i = 1; i < tempReaction->numProducts; i++ )
   {
      fprintf( stdout, " + %d", tempReaction->products[i] );
   }
   fprintf( stdout, "\n" );
}


// Create the initial set of Reactions
void
Reaction::initList()
{
   int p1[] = {2,3};
   int r1[] = {5,7};
   addReaction( p1, 2, r1, 2, 0.02 );
   int p2[] = {11,13};
   int r2[] = {17,19};
   addReaction( p2, 2, r2, 2, 0.03 );
}


void
Reaction::printList()
{
   fprintf( stdout, "---------\n" );
   fprintf( stdout, "Reaction 1:\n\tkey: %d\n", list[6]->getKey() );
   fprintf( stdout,            "\treactants: %d, %d\n", list[6]->getReactants()[0], list[6]->getReactants()[1] );
   fprintf( stdout,            "\tnumReactants: %d\n", list[6]->getNumReactants() );
   int *p1 = list[6]->getReactants();
   for( int i = 0; i < list[6]->getNumReactants(); i++ )
   {
      fprintf( stdout, "\t\tlist[6]->getReactants()[%d] = %d\n", i, list[6]->getReactants()[i] );
      fprintf( stdout, "\t\tp1[%d] = %d\n", i, p1[i] );
   }
   /////////////////
   fprintf( stdout,            "\tproducts: %d, %d\n", list[6]->getProducts()[0], list[6]->getProducts()[1] );
   fprintf( stdout,            "\tnumProducts: %d\n", list[6]->getNumProducts() );
   int *r1 = list[6]->getProducts();
   for( int i = 0; i < list[6]->getNumProducts(); i++ )
   {
      //fprintf( stdout, "\t\tlist[6]->getProducts()[%d] = %d\n", i, list[6]->getProducts()[i] );
      fprintf( stdout, "\t\tlist[6]->getProducts()[%d] = %d\n", i, list[6]->products[i] );
      fprintf( stdout, "\t\tr1[%d] = %d\n", i, r1[i] );
   }
   fprintf( stdout,            "\trate: %f\n", list[6]->getRate() );
   /////////////////////////////////////////////////
   fprintf( stdout, "Reaction 2:\n\tkey: %d\n", list[143]->getKey() );
   fprintf( stdout,            "\treactants: %d, %d\n", list[143]->getReactants()[0], list[143]->getReactants()[1] );
   fprintf( stdout,            "\tnumReactants: %d\n", list[143]->getNumReactants() );
   int *p2 = list[143]->getReactants();
   for( int i = 0; i < list[143]->getNumReactants(); i++ )
   {
      fprintf( stdout, "\t\tlist[143]->getReactants()[%d] = %d\n", i, list[143]->getReactants()[i] );
      fprintf( stdout, "\t\tp2[%d] = %d\n", i, p2[i] );
   }
   /////////////////
   fprintf( stdout,            "\tproducts: %d, %d\n", list[143]->getProducts()[0], list[143]->getProducts()[1] );
   fprintf( stdout,            "\tnumProducts: %d\n", list[143]->getNumProducts() );
   int *r2 = list[143]->getProducts();
   for( int i = 0; i < list[143]->getNumProducts(); i++ )
   {
      fprintf( stdout, "\t\tlist[143]->getProducts()[%d] = %d\n", i, list[143]->getProducts()[i] );
      fprintf( stdout, "\t\tr2[%d] = %d\n", i, r2[i] );
   }
   fprintf( stdout,            "\trate: %f\n", list[143]->getRate() );
   fprintf( stdout, "---------\n" ); 
}


int
Reaction::getKey()
{
   return key;
}


int*
Reaction::getReactants()
{
   return reactants;
}


int
Reaction::getNumReactants()
{
   return numReactants;
}


int*
Reaction::getProducts()
{
   return products;
}


int
Reaction::getNumProducts()
{
   return numProducts;
}


void
Reaction::setProducts( int* newProducts, int newNumProducts )
{
   products = newProducts;
   numProducts = newNumProducts;
}


double
Reaction::getRate()
{
   return rate;
}


void
Reaction::setRate( double newRate )
{
   rate = newRate;
}

