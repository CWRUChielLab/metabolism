/* main.cpp
 */

// #include <QApplication>
#include <stdio.h>
#include "particle.h"
#include "reaction.h"

int
main ( int argc, char *argv[] )
{
   // QCoreApplication *app;
   fprintf( stdout, "This is a test.\n" );

   Particle *test = new Particle( "Anion", 0xFF0000, -1 );
   fprintf( stdout, "test:\n\tKey: %d\n\tName: %s\n\tColor: %d\n\tCharge: %d\n", test->getKey(), test->getName(), test->getColor(), test->getCharge() );
   test->setName( "Cation" );
   test->setColor( 0x00FF00 );
   test->setCharge( 1 );
   fprintf( stdout, "test:\n\tKey: %d\n\tName: %s\n\tColor: %d\n\tCharge: %d\n", test->getKey(), test->getName(), test->getColor(), test->getCharge() );

   int primes = 5;
   Particle *list[primes];
   for( int i = 0; i < primes; i++ )
   {
      list[i] = new Particle( "test", 0, 0 );
      fprintf( stdout, "next key: %d\n", list[i]->getKey() );
   }

   Reaction::initList();
   Reaction::printList();

   return 0;
}

