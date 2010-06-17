/* main.cpp
 */

#include <csignal>
#ifdef HAVE_QT
#include <QApplication>
#include "gui-mainwindow.h" // gui-mainwindow.h must be included before ncurses.h
#endif
#ifdef HAVE_NCURSES
#include <ncurses.h>
#endif
#include "options.h"
#include "safecalls.h"
#include "sim.h"
using namespace SafeCalls;


Options* o;
Sim* sim;


void
handleExit( int sig )
{
   sig = 0;  // silence the compiler
   sim->end();
   std::cout << std::endl;
}


int
main ( int argc, char* argv[] )
{
#ifdef HAVE_QT
   // Create the core Qt controller application
   QCoreApplication *app;
#endif

   // Import command line options and initialize the simulation
   o = safeNew( Options( argc, argv ) );
   sim = safeNew( Sim(o) );

   // Set up handling of Ctrl-c abort
   signal( SIGINT, handleExit );

   // Execute the simulation
   if( o->gui == Options::GUI_QT )
   {
#ifdef HAVE_QT
      app = new QApplication( argc, argv );
      GuiMainWindow* gui = safeNew( GuiMainWindow( o, sim ) );
      gui->show();
      return app->exec();
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

      return 0;
   }
}

