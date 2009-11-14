/* main.cpp
 */

// #include <QApplication>
#include <stdio.h>
#include "particle.h"

int
main ( int argc, char *argv[] )
{
   // QCoreApplication *app;
   fprintf( stdout, "This is a test.\n" );

   Particle *test = new Particle( (char*)("Anion"), 0xFF0000, -1 );
   fprintf( stdout, "test:\n\tName: %s\n\tColor: %d\n\tCharge: %d\n", test->getName(), test->getColor(), test->getCharge() );
   return 0;
}

