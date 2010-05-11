/* reaction.cpp
 */

#include <cassert>
#include "reaction.h"


// Constructors
Reaction::Reaction( std::vector<Element*> initReactants, std::vector<Element*> initFirstProducts, double initFirstProb )
{
   // Ensure that products and reactants are balanced
   assert( initReactants.size() == initFirstProducts.size() );

   // Ensure that probabilities are sane
   assert( initFirstProb >= 0 && initFirstProb <= 1 );

   // Calculate the product of the reactant keys for the Reaction key
   key = 1;
   for( unsigned int i = 0; i < initReactants.size(); i++ )
   {
      key *= initReactants[i]->getKey();
   }

   // Copy constructor arguments
   reactants = initReactants;
   firstProducts = initFirstProducts;
   std::vector<Element*> temp;
   secondProducts = temp;
   firstProb = initFirstProb;
   secondProb = 0.0;
}


Reaction::Reaction( std::vector<Element*> initReactants, std::vector<Element*> initFirstProducts, double initFirstProb, std::vector<Element*> initSecondProducts, double initSecondProb )
{
   // Ensure that products and reactants are balanced
   assert( initReactants.size() == initFirstProducts.size() );
   assert( initReactants.size() == initSecondProducts.size() );

   // Ensure that probabilities are sane
   assert( initFirstProb >= 0 && initFirstProb <= 1 );
   assert( initSecondProb >= 0 && initSecondProb <= 1 );

   // Calculate the product of the reactant keys for the Reaction key
   key = 1;
   for( unsigned int i = 0; i < initReactants.size(); i++ )
   {
      key *= initReactants[i]->getKey();
   }

   // Copy constructor arguments
   reactants = initReactants;
   firstProducts = initFirstProducts;
   secondProducts = initSecondProducts;
   firstProb = initFirstProb;
   secondProb = initSecondProb;
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
Reaction::getFirstProducts()
{
   return firstProducts;
}


std::vector<Element*>
Reaction::getSecondProducts()
{
   return secondProducts;
}


void
Reaction::setFirstProducts( std::vector<Element*> newFirstProducts )
{
   // Ensure that products and reactants are balanced
   assert( newFirstProducts.size() == reactants.size() );

   firstProducts = newFirstProducts;
}


void
Reaction::setSecondProducts( std::vector<Element*> newSecondProducts )
{
   // Ensure that products and reactants are balanced
   assert( newSecondProducts.size() == reactants.size() );

   secondProducts = newSecondProducts;
}


double
Reaction::getFirstProb()
{
   return firstProb;
}


double
Reaction::getSecondProb()
{
   return secondProb;
}


void
Reaction::setFirstProb( double newFirstProb )
{
   // Ensure that probabilities are sane
   assert( newFirstProb >= 0 && newFirstProb <= 1 );

   firstProb = newFirstProb;
}


void
Reaction::setSecondProb( double newSecondProb )
{
   // Ensure that probabilities are sane
   assert( newSecondProb >= 0 && newSecondProb <= 1 );

   secondProb = newSecondProb;
}

