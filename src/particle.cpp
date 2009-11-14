/* particle.cpp
 */

#include "particle.h"

Particle::Particle( char *initName, int initColor, int initCharge )
{
   name = initName;
   color = initColor;
   charge = initCharge;
}


char*
Particle::getName()
{
   return name;
}


void
Particle::setName( char *newName )
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

