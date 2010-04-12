/* element.cpp
 */

#include <cmath>
#include <string>
#include "element.h"


// Constructor
Element::Element( std::string initName, char initSymbol, int initColor, int initCharge )
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
   charge = initCharge;

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


int
Element::getCount()
{
   return count;
}


void
Element::setCount( int newCount )
{
   count = newCount;
}

