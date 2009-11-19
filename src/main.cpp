/* main.cpp
 */

// #include <QApplication>
#include "atom.h"
#include "element.h"
#include "reaction.h"

int
main ( int argc, char *argv[] )
{
   // QCoreApplication *app;
   Element::initList();
   Element::printList();

   Reaction::initList();
   Reaction::printList();

   Atom::initWorld();
   Atom::printWorld();

   return 0;
}

