/* element.cpp
 */

#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include "element.h"


// Static members
Element **Element::list;
int Element::lastPrime = 1;


// Private constructor
Element::Element( const char *initName, int initColor, int initCharge )
{
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
}


// Handles creating new Elements and adding them
// to the list, indexed by Element key
void
Element::addElement( const char *initName, int initColor, int initCharge )
{
   static int listSize = 0;

   // Create the new Element
   Element *tempElement = new Element( initName, initColor, initCharge );

   // Grow the list if there is not room for the new Element
   while( tempElement->getKey() >= listSize )
   {
      int newListSize = fmax( listSize * 2, 10 );
      Element **tempArray = (Element **)malloc( newListSize * sizeof( Element * ) );
      for( int i = 0; i < listSize; i++ )
      {
         tempArray[i] = list[i];
      }
      list = tempArray;
      listSize = newListSize;
   }

   // Place the new Element in the list, indexed by key
   list[tempElement->getKey()] = tempElement;
}


// Create the initial set of Elements
void
Element::initList()
{
   addElement( "A", 0, 0 );
   addElement( "B", 0, 0 );
   addElement( "C", 0, 0 );
   addElement( "D", 0, 0 );
   addElement( "E", 0, 0 );
   addElement( "F", 0, 0 );
   addElement( "G", 0, 0 );
   addElement( "H", 0, 0 );
}


void
Element::printList()
{
   fprintf( stdout, "------\n" );
   fprintf( stdout, "list[2]  has name: %s\n", list[2]->getName() );
   fprintf( stdout, "list[3]  has name: %s\n", list[3]->getName() );
   fprintf( stdout, "list[5]  has name: %s\n", list[5]->getName() );
   fprintf( stdout, "list[7]  has name: %s\n", list[7]->getName() );
   fprintf( stdout, "list[11] has name: %s\n", list[11]->getName() );
   fprintf( stdout, "list[13] has name: %s\n", list[13]->getName() );
   fprintf( stdout, "list[17] has name: %s\n", list[17]->getName() );
   fprintf( stdout, "list[19] has name: %s\n", list[19]->getName() );
   fprintf( stdout, "------\n" );
}


void
Element::countElements()
{
   int count = 0;
   for( int i = 0; i <= lastPrime; i++ )
   {
      if( list[i] != 0 )
      {
         count++;
      }
   }
   fprintf( stdout, "count = %d\n", count );
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
Element::setName( const char *newName )
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

