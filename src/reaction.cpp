/* reaction.cpp
 */

#include <cstdlib> // exit
#include <iostream>
#include "reaction.h"


// Constructor
Reaction::Reaction( std::vector<Element*> initReactants, std::vector<Element*> initFirstProducts, double initFirstProb )
{
   // Ensure that products and reactants are balanced
   if( initReactants.size() != initFirstProducts.size() )
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
   if( initFirstProb < 0 || initFirstProb > 1 )
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
   firstProducts = initFirstProducts;
   std::vector<Element*> temp;
   secondProducts = temp;
   firstProb = initFirstProb;
   secondProb = 0.0;
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
   if( newFirstProducts.size() != reactants.size() )
   {
      std::cout << "setFirstProducts: reaction is not balanced!" << std::endl;
      exit( EXIT_FAILURE );
   }

   firstProducts = newFirstProducts;
}


void
Reaction::setSecondProducts( std::vector<Element*> newSecondProducts )
{
   // Ensure that products and reactants are balanced
   if( newSecondProducts.size() != reactants.size() )
   {
      std::cout << "setSecondProducts: reaction is not balanced!" << std::endl;
      exit( EXIT_FAILURE );
   }

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
   if( newFirstProb < 0 || newFirstProb > 1 )
   {
      std::cout << "setFirstProb: probability must fall between 0 and 1!" << std::endl;
      exit( EXIT_FAILURE );
   }

   firstProb = newFirstProb;
}


void
Reaction::setSecondProb( double newSecondProb )
{
   // Ensure that probabilities are sane
   if( newSecondProb < 0 || newSecondProb > 1 )
   {
      std::cout << "setSecondProb: probability must fall between 0 and 1!" << std::endl;
      exit( EXIT_FAILURE );
   }

   secondProb = newSecondProb;
}

