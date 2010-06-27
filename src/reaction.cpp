/* reaction.cpp
 */

#include <cstdlib> // exit
#include <iostream>
#include "reaction.h"


// Constructor
Reaction::Reaction( std::vector<Element*> initReactants, std::vector<Element*> initProducts, double initProb )
{
   // Ensure that products and reactants are balanced
   if( initReactants.size() != initProducts.size() )
   {
      std::cout << "Loading rxn: reaction is not balanced!" << std::endl;
      exit( EXIT_FAILURE );
   }

   // Ensure number of products and reactants are fine
   if( initReactants.size() < 1 || initReactants.size() > 2 )
   {
      std::cout << "Loading rxn: reactions must contain at least one and no more than two reactants and products each!" << std::endl;
      exit( EXIT_FAILURE );
   }

   // Ensure that probabilities are sane
   if( initProb < 0 || initProb > 1 )
   {
      std::cout << "Loading rxn: probability must fall between 0 and 1!" << std::endl;
      exit( EXIT_FAILURE );
   }

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
   if( newProducts.size() != reactants.size() )
   {
      std::cout << "setProducts: reaction is not balanced!" << std::endl;
      exit( EXIT_FAILURE );
   }

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
   // Ensure that probabilities are sane
   if( newProb < 0 || newProb > 1 )
   {
      std::cout << "setProb: probability must fall between 0 and 1!" << std::endl;
      exit( EXIT_FAILURE );
   }

   prob = newProb;
}

