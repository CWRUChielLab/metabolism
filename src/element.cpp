/* element.cpp
 */

#include <cmath>
#include <iostream>
#include "element.h"


typedef std::map<int,Element*> ElementMap;


// Static members
ElementMap Element::list;


// Constructor
Element::Element( std::string initName, int initColor, int initCharge )
{
   static int lastPrime = 1;
   
   // Calculate a unique prime number for the Element key
   int candidate = lastPrime + 1;
   int i = 2;
   while( i <= std::sqrt(candidate) )
   {
      if( candidate % i == 0 )
      {
         candidate++;
         i = 2;
      }
      else
      {
         i++;
      }
   }
   lastPrime = candidate;
   key = lastPrime;

   // Copy constructor arguments
   name = initName;
   color = initColor;
   charge = initCharge;

   // Place the new Element in the list, indexed by key
   list[key] = this;
}


// Returns the pointer to an Element from the list
Element*
Element::getElement( int key )
{
   return list[key];
}


// Create the initial set of Elements
void
Element::initList()
{
   new Element( "A", 0, 0 );
   new Element( "B", 0, 0 );
   new Element( "C", 0, 0 );
   new Element( "D", 0, 0 );
   new Element( "E", 0, 0 );
   new Element( "F", 0, 0 );
   new Element( "G", 0, 0 );
   new Element( "H", 0, 0 );
}


void
Element::printList()
{
   for( ElementMap::iterator i = list.begin(); i != list.end(); i++ )
   {
      Element* ele = i->second;
      std::cout << "list[" << ele->getKey() << "] has name: " << list[ele->getKey()]->getName() << std::endl;
   }
}


int
Element::getKey()
{
   return key;
}


std::string
Element::getName()
{
   return name;
}


void
Element::setName( std::string newName )
{
   name = newName;
}


int
Element::getColor()
{
   return color;
}


void
Element::setColor( int newColor )
{
   color = newColor;
}


int
Element::getCharge()
{
   return charge;
}


void
Element::setCharge( int newCharge )
{
   charge = newCharge;
}

