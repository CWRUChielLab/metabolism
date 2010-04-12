/* reaction.cpp
 */

#include <cassert>
#include <vector>
#include "reaction.h"


// Constructor
Reaction::Reaction( std::vector<Element*> initReactants, std::vector<Element*> initProducts, double initProb )
{
   // Ensure that products and reactants are balanced
   assert( initReactants.size() == initProducts.size() );

   // Prob must be constrained between 0 and 0.2 in order for it
   // to be an absolute probability of reaction
   assert( initProb >= 0 && initProb <= 0.2 );

   // Calculate the product of the reactant keys for the Reaction key
   key = 1;
   for( unsigned int i = 0; i < initReactants.size(); i++ )
   {
      key *= initReactants[i]->getKey();
   }

   // Copy constructor arguments
   reactants = initReactants;
   products = initProducts;
   prob = initProb;
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
   // Ensure that products and reactants are balanced
   assert( newProducts.size() == reactants.size() );

   products = newProducts;
}


double
Reaction::getProb()
{
   return prob;
}


void
Reaction::setProb( double newProb )
{
   // Prob must be constrained between 0 and 0.2 in order for it
   // to be an absolute probability of reaction
   assert( newProb >= 0 && newProb <= 0.2 );

   prob = newProb;
}

