/* main.cpp
 */

// #include <QApplication>
#include <iostream>
#include "atom.h"
#include "element.h"
#include "reaction.h"

int
main ( int argc, char *argv[] )
{
   // QCoreApplication *app;
   Element::initList();
   std::cout << "------" << std::endl;
   Element::printList();

   Reaction::initList();
   std::cout << "------" << std::endl;
   Reaction::printList();

   Atom::initWorld();
   std::cout << "------" << std::endl;
   Atom::printWorld();

   for( int i = 0; i < 2; i++ )
   {
      Atom::moveAtoms();
      Atom::printWorld();
   }

   return 0;
}

