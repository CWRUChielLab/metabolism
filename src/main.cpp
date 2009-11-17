/* main.cpp
 */

// #include <QApplication>
#include <stdio.h>
#include "element.h"
#include "reaction.h"

int
main ( int argc, char *argv[] )
{
   // QCoreApplication *app;
   fprintf( stdout, "This is a test.\n" );

   Reaction::initList();
   Reaction::printList();

   Element::initList();
   Element::printList();
   Element::countElements();

   return 0;
}

