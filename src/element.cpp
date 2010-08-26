/* element.cpp
 */

#include <cmath>   // sqrt
#include <cstdlib> // exit
#include <iostream>
#include <string>
#include "element.h"


// Constructor
Element::Element( std::string initName, char initSymbol, std::string initColor, double initStartConc )
{
   static int lastPrime = 1;

   // Ensure that concentration is sane
   if( initStartConc < 0 || initStartConc > 1 )
   {
      std::cout << "Loading ele: concentration must fall between 0 and 1!" << std::endl;
      exit( EXIT_FAILURE );
   }
   
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
   startConc = initStartConc;

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


double
Element::getStartConc()
{
   return startConc;
}


void
Element::setColor( std::string newColor )
{
   color = newColor;
}


void
Element::setStartConc( double newStartConc )
{
   // Ensure that concentration is sane
   if( newStartConc < 0 || newStartConc > 1 )
   {
      std::cout << "setStartConc: concentration must fall between 0 and 1!" << std::endl;
      exit( EXIT_FAILURE );
   }

   startConc = newStartConc;
}

