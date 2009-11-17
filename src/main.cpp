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
   Element::initList();
   Element::printList();
   Element::countElements();

   Reaction::initList();
   Reaction::printList();

   return 0;
}

