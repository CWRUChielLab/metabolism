/* main.cpp
 */

#include <csignal>
#ifdef HAVE_QT
#include <QApplication>
#include "gui.h" // gui.h must be included before ncurses.h
#endif
#ifdef HAVE_NCURSES
#include <ncurses.h>
#endif
#include "options.h"
#include "safecalls.h"
#include "sim.h"
using namespace SafeCalls;


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
   Options* o = safeNew( Options( argc, argv ) );
   Sim* mySim = safeNew( Sim(o) );

   // Execute the simulation
   if( o->gui == Options::GUI_QT )
   {
#ifdef HAVE_QT
      std::cout << "*** Using Qt GUI! ***" << std::endl;
      app = new QApplication( argc, argv );
      GUI* myGUI = safeNew( GUI( o, mySim ) );
      myGUI->show();
      return app->exec();
#endif
   } else {
      while( !interrupted && mySim->iterate() )
      {
         if( o->gui == Options::GUI_NCURSES )
         {
#ifdef HAVE_NCURSES
            mySim->printWorld();
#endif
         }

         // Print out the progress of the simulation
         // at most once each second
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
}

