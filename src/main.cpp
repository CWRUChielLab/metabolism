/* main.cpp
 */

// #include <QApplication>
#include <iostream>
#include "sim.h"

int
main ( int argc, char *argv[] )
{
   // QCoreApplication *app;
   Sim* mySim = new Sim();
   mySim->initialize();
   std::cout << "------" << std::endl;
   mySim->printElements();
   std::cout << "------" << std::endl;
   mySim->printReactions();
   std::cout << "------" << std::endl;
   mySim->printWorld();

   for( int i = 0; i < 2; i++ )
   {
      mySim->moveAtoms();
      mySim->printWorld();
   }

   return 0;
}

