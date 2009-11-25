/* reaction.cpp
 */

#include <algorithm>
#include <iostream>
#include "reaction.h"


// Static members
Reaction** Reaction::list;


// Constructor
Reaction::Reaction( Element** initReactants, int initNumReactants, Element** initProducts, int initNumProducts, double initRate )
{
   static int listSize = 0;

   // Calculate the product of the reactant keys for the Reaction key
   key = 1;
   for( int i = 0; i < initNumReactants; i++ )
   {
      key *= initReactants[i]->getKey();
   }

   // Copy constructor arguments
   reactants = new Element*[initNumReactants];
   for( int i = 0; i < initNumReactants; i++ )
   {
      reactants[i] = initReactants[i];
   }
   numReactants = initNumReactants;
   products = new Element*[initNumProducts];
   for( int i = 0; i < initNumProducts; i++ )
   {
      products[i] = initProducts[i];
   }
   numProducts = initNumProducts;
   rate = initRate;

   // Grow the list if there is not enough room for the new Reaction
   while( key >= listSize )
   {
      int newListSize = std::max( listSize * 2, 10 );
      Reaction** tempArray = new Reaction*[newListSize];
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
   Element* p1[] = {Element::getElement(2), Element::getElement(3)};
   Element* r1[] = {Element::getElement(5), Element::getElement(7)};
   new Reaction( p1, 2, r1, 2, 0.02 );
   Element* p2[] = {Element::getElement(11), Element::getElement(13)};
   Element* r2[] = {Element::getElement(17), Element::getElement(19)};
   new Reaction( p2, 2, r2, 2, 0.03 );
}


void
Reaction::printList()
{
   int x = 6;
   std::cout << "Key: " << list[x]->key << "       " << list[x]->reactants[0]->getName();
   for( int i = 1; i < list[x]->numReactants; i++ )
   {
      std::cout << " + " << list[x]->reactants[i]->getName();
   }
   std::cout << " -> " << list[x]->products[0]->getName();
   for( int i = 1; i < list[x]->numProducts; i++ )
   {
      std::cout << " + " << list[x]->products[i]->getName();
   }
   std::cout << std::endl;

   x = 143;
   std::cout << "Key: " << list[x]->key << "     " << list[x]->reactants[0]->getName();
   for( int i = 1; i < list[x]->numReactants; i++ )
   {
      std::cout << " + " << list[x]->reactants[i]->getName();
   }
   std::cout << " -> " << list[x]->products[0]->getName();
   for( int i = 1; i < list[x]->numProducts; i++ )
   {
      std::cout << " + " << list[x]->products[i]->getName();
   }
   std::cout << std::endl;
}


int
Reaction::getKey()
{
   return key;
}


Element**
Reaction::getReactants()
{
   return reactants;
}


int
Reaction::getNumReactants()
{
   return numReactants;
}


Element**
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
Reaction::setProducts( Element** newProducts, int newNumProducts )
{
   delete [] products;
   products = new Element*[newNumProducts];
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

