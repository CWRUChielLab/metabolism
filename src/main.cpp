/* main.cpp
 */

#include <csignal>
#ifdef HAVE_QT
#include <QApplication>
#include "window.h"     // window.h must be included before ncurses.h
#endif
#ifdef HAVE_NCURSES
#include <ncurses.h>
#endif
#include "options.h"
#include "sim.h"


Options* o;
Sim* sim;


void
handleExit( int sig )
{
   sig = 0;  // silence the compiler
   sim->end();
}


int
main( int argc, char* argv[] )
{
   int retval = 0;

#ifdef HAVE_QT
   // Create the core Qt controller application
   QCoreApplication *app;
   //QApplication::setStyle( new QPlastiqueStyle() );
   //QApplication::setStyle( new QWindowsStyle() );
   //QApplication::setStyle( new QMotifStyle() );
#endif

   // Import command line options and initialize the simulation
   o = new Options( argc, argv );
   sim = new Sim( o );

   // Set up handling of Ctrl-c abort
   signal( SIGINT, handleExit );

   // Execute the simulation
   if( o->gui == Options::GUI_QT )
   {
#ifdef HAVE_QT
      app = new QApplication( argc, argv );
      Window* gui = new Window( o, sim );
      gui->show();
      retval = app->exec();
#endif
   } else {
      while( sim->iterate() )
      {
         if( o->gui == Options::GUI_NCURSES )
         {
#ifdef HAVE_NCURSES
            // Print the world using ncurses
            sim->printWorld();
#endif
         }
      }

      // Finish collecting data and clean up
      sim->cleanup();

      retval = 0;
   }

   // Update the progress indicator to accurately
   // display how many iterations were completed
   // when the simulation ended and break the line
   if( o->progress )
      sim->finishProgressReport();

   return retval;
}

