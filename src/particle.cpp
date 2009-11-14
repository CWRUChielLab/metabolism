/* particle.cpp
 */

#include <math.h>
#include "particle.h"

Particle::Particle( const char *initName, int initColor, int initCharge )
{
   // Calculate a unique prime number for the Particle key
   static int prime = 1;
   int temp = prime + 1;
   int i = 2;
   while( i <= sqrt(temp) )
   {
      if( temp % i == 0 )
      {
         temp++;
         i = 2;
      }
      else
      {
         i++;
      }
   }
   prime = temp;
   key = prime;

   // Copy constructor arguments
   name = initName;
   color = initColor;
   charge = initCharge;
}


int
Particle::getKey()
{
   return key;
}


const char*
Particle::getName()
{
   return name;
}


void
Particle::setName( const char *newName )
{
   name = newName;
}


int
Particle::getColor()
{
   return color;
}


void
Particle::setColor( int newColor )
{
   color = newColor;
}


int
Particle::getCharge()
{
   return charge;
}


void
Particle::setCharge( int newCharge )
{
   charge = newCharge;
}

