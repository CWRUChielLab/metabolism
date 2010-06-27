/* element.cpp
 */

#include <cmath> // sqrt
#include <string>
#include "element.h"


// Constructor
Element::Element( std::string initName, char initSymbol, std::string initColor )
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
   symbol = initSymbol;
   color = initColor;

   count = 0;
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


char
Element::getSymbol()
{
   return symbol;
}


std::string
Element::getColor()
{
   return color;
}


void
Element::setColor( std::string newColor )
{
   color = newColor;
}

