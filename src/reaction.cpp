/* reaction.cpp
 */

#include <algorithm>
#include <iostream>
#include "reaction.h"


typedef std::map<int,Reaction*> ReactionMap;
typedef std::vector<Element*> ElementVector;


// Static members
ReactionMap Reaction::list;


// Constructor
Reaction::Reaction( ElementVector initReactants, ElementVector initProducts, double initRate )
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
   ElementVector p1;
   p1.push_back(Element::getElement(2));
   p1.push_back(Element::getElement(3));
   ElementVector r1;
   r1.push_back(Element::getElement(5));
   r1.push_back(Element::getElement(7));
   new Reaction( p1, r1, 0.02 );

   ElementVector p2;
   p2.push_back(Element::getElement(11));
   p2.push_back(Element::getElement(13));
   ElementVector r2;
   r2.push_back(Element::getElement(17));
   r2.push_back(Element::getElement(19));
   new Reaction( p2, r2, 0.03 );
   
   ElementVector p3;
   p3.push_back(Element::getElement(2));
   p3.push_back(Element::getElement(2));
   p3.push_back(Element::getElement(7));
   ElementVector r3;
   r3.push_back(Element::getElement(19));
   new Reaction( p3, r3, 0.1 );

   ElementVector p4;
   p4.push_back(Element::getElement(3));
   ElementVector r4;
   r4.push_back(Element::getElement(13));
   r4.push_back(Element::getElement(7));
   r4.push_back(Element::getElement(19));
   new Reaction( p4, r4, 1 );
}


void
Reaction::printList()
{
   for( ReactionMap::iterator i = list.begin(); i != list.end(); i++ )
   {
      Reaction* rxn = i->second;
      std::cout << "Key: " << rxn->getKey() << "  \t" << rxn->reactants[0]->getName();
      for( unsigned int j = 1; j < rxn->reactants.size(); j++ )
      {
         std::cout << " + " << rxn->reactants[j]->getName();
      }
      std::cout << " -> " << rxn->products[0]->getName();
      for( unsigned int j = 1; j < rxn->products.size(); j++ )
      {
         std::cout << " + " << rxn->products[j]->getName();
      }
      std::cout << std::endl;
   }
}


int
Reaction::getKey()
{
   return key;
}


ElementVector
Reaction::getReactants()
{
   return reactants;
}


int
Reaction::getNumReactants()
{
   return reactants.size();
}


ElementVector
Reaction::getProducts()
{
   return products;
}


int
Reaction::getNumProducts()
{
   return products.size();
}


void
Reaction::setProducts( ElementVector newProducts )
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

