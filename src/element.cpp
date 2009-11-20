/* element.cpp
 */

#include <math.h>
#include <stdio.h>
#include "element.h"


// Static members
Element** Element::list;


// Constructor
Element::Element( const char* initName, int initColor, int initCharge )
{
   static int listSize = 0;
   static int lastPrime = 1;
   
   // Calculate a unique prime number for the Element key
   int candidate = lastPrime + 1;
   int i = 2;
   while( i <= sqrt(candidate) )
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

   // Grow the list if there is not room for the new Element
   while( key >= listSize )
   {
      int newListSize = fmax( listSize * 2, 10 );
      Element** tempArray = new Element*[newListSize];
      for( int i = 0; i < listSize; i++ )
      {
         tempArray[i] = list[i];
      }
      delete [] list;
      list = tempArray;
      listSize = newListSize;
   }

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
   printf( "------\n" );
   printf( "list[2]  has name: %s\n", list[2]->getName() );
   printf( "list[3]  has name: %s\n", list[3]->getName() );
   printf( "list[5]  has name: %s\n", list[5]->getName() );
   printf( "list[7]  has name: %s\n", list[7]->getName() );
   printf( "list[11] has name: %s\n", list[11]->getName() );
   printf( "list[13] has name: %s\n", list[13]->getName() );
   printf( "list[17] has name: %s\n", list[17]->getName() );
   printf( "list[19] has name: %s\n", list[19]->getName() );
   printf( "------\n" );
}


int
Element::getKey()
{
   return key;
}


const char*
Element::getName()
{
   return name;
}


void
Element::setName( const char* newName )
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

