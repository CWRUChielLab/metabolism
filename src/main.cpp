/* main.cpp
 */

#include <csignal>
#ifdef HAVE_NCURSES
#include <ncurses.h>
#endif
#ifdef HAVE_QT
#include <QApplication>
#endif
#include "options.h"
#include "safecalls.h"
#include "sim.h"
using namespace SafeCalls;


Sim* mySim;
Options* o;
int interrupted = 0;


void
handleExit( int sig )
{
   sig = 0;  // silence the compiler
   interrupted = 1;
   std::cout << std::endl;
}


int
main ( int argc, char* argv[] )
{
   // Set up handling of Ctrl-c abort
   signal( SIGINT, handleExit );

#ifdef HAVE_QT
   QCoreApplication *app;
#endif

   // Import command line options and initialize the simulation
   o = safeNew( Options( argc, argv ) );
   mySim = safeNew( Sim(o) );

   if( o->gui == Options::GUI_QT )
   {
      std::cout << "*** Using Qt GUI! ***" << std::endl;
   }

   // Execute the simulation
   while( !interrupted && mySim->iterate() )
   {
      if( o->gui == Options::GUI_NCURSES )
      {
#ifdef HAVE_NCURSES
         mySim->printWorld();
#endif
      }

      // Print out the progress of the simulation
      // once each second
      if( o->progress )
      {
         mySim->reportProgress();
      }

      // Take a census of the atoms in the world
      // occasionally
      if( mySim->getCurrentIter() % 8 == 0 )
      {
         mySim->writeCensus();
      }

      // Sleep the simulation each iteration
      if( o->sleep != 0 )
      {
         usleep(o->sleep * 1000);
      }
   }

   // Write final data to file
   mySim->finalizeIO();

   return 0;
}

