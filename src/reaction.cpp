/* reaction.cpp
 */

#include <math.h>
#include <stdio.h>
#include "reaction.h"


// Static members
Reaction **Reaction::list;


// Constructor
Reaction::Reaction( int* initReactants, int initNumReactants, int* initProducts, int initNumProducts, double initRate )
{
   static int listSize = 0;

   // Calculate the product of the reactant keys for the Reaction key
   key = 1;
   for( int i = 0; i < initNumReactants; i++ )
   {
      key *= initReactants[i];
   }

   // Copy constructor arguments
   reactants = new int[initNumReactants];
   for( int i = 0; i < initNumReactants; i++ )
   {
      reactants[i] = initReactants[i];
   }
   numReactants = initNumReactants;
   products = new int[initNumProducts];
   for( int i = 0; i < initNumProducts; i++ )
   {
      products[i] = initProducts[i];
   }
   numProducts = initNumProducts;
   rate = initRate;

   // Grow the list if there is not enough room for the new Reaction
   while( key >= listSize )
   {
      int newListSize = fmax( listSize * 2, 10 );
      Reaction **tempArray = new Reaction*[newListSize];
      for( int i = 0; i < listSize; i++ )
      {
         tempArray[i] = list[i];
      }
      delete [] list;
      list = tempArray;
      listSize = newListSize;
   }

   // Place the new Reaction in the list, indexed by key
   list[key] = this;
}


// Returns the pointer to a Reaction from the list
Reaction*
Reaction::getReaction( int key )
{
   return list[key];
}


// Create the initial set of Reactions
void
Reaction::initList()
{
   int p1[] = {2,3};
   int r1[] = {5,7};
   new Reaction( p1, 2, r1, 2, 0.02 );
   int p2[] = {11,13};
   int r2[] = {17,19};
   new Reaction( p2, 2, r2, 2, 0.03 );
}


void
Reaction::printList()
{
   printf( "------\nKey: %d       %d", list[6]->key, list[6]->reactants[0] );
   for( int i = 1; i < list[6]->numReactants; i++ )
   {
      printf( " + %d", list[6]->reactants[i] );
   }
   printf( " -> %d", list[6]->products[0] );
   for( int i = 1; i < list[6]->numProducts; i++ )
   {
      printf( " + %d", list[6]->products[i] );
   }
   printf( "\n" );

   printf( "Key: %d     %d", list[143]->key, list[143]->reactants[0] );
   for( int i = 1; i < list[143]->numReactants; i++ )
   {
      printf( " + %d", list[143]->reactants[i] );
   }
   printf( " -> %d", list[143]->products[0] );
   for( int i = 1; i < list[143]->numProducts; i++ )
   {
      printf( " + %d", list[143]->products[i] );
   }
   printf( "\n------\n" );
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
   products = new int[newNumProducts];
   for( int i = 0; i < newNumProducts; i++ )
   {
      products[i] = newProducts[i];
   }
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

