/* reaction.cpp
 */

#include <vector>
#include "reaction.h"


// Constructor
Reaction::Reaction( std::vector<Element*> initReactants, std::vector<Element*> initProducts, double initRate )
{
   // Calculate the product of the reactant keys for the Reaction key
   key = 1;
   for( unsigned int i = 0; i < initReactants.size(); i++ )
   {
      key *= initReactants[i]->getKey();
   }

   // Copy constructor arguments
   reactants = initReactants;
   products = initProducts;
   rate = initRate;
}


int
Reaction::getKey()
{
   return key;
}


std::vector<Element*>
Reaction::getReactants()
{
   return reactants;
}


std::vector<Element*>
Reaction::getProducts()
{
   return products;
}


void
Reaction::setProducts( std::vector<Element*> newProducts )
{
   products = newProducts;
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

