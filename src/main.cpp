/* main.cpp
 */

#include <ctime>
#include <ncurses.h>
#include <unistd.h>   // Might not be compatible with Windows
#include "options.h"
#include "safecalls.h"
#include "sim.h"
using namespace SafeCalls;


int
main ( int argc, char* argv[] )
{
   Options* o = safeNew( Options( argc, argv ) );

   // Initialize ncurses
   initscr();   // Startup
   raw();       // Disable special terminal commands, such as CTRL+c to quit
   timeout(0);  // Makes getch a nonblocking call

   Sim* mySim = safeNew( Sim(o) );

   if( o->verbose )
   {
      printw( "Press Ctrl-c to quit.\n" );
      printw( "------\n" );
      mySim->printElements();
      printw( "------\n" );
      mySim->printReactions();
      printw( "------\n" );
   }

   int x, y;
   int lastProgressUpdate = 0;
   getyx( stdscr, y, x );
   if( o->useGUI )
   {
      mySim->printWorld();
   }

   while( mySim->iterate() )
   {
      // Check to see if the user wants to quit
      // by pressing Ctrl-c
      if( getch() == 0x3 )
      {
         o->maxIters = mySim->getCurrentIter();
         break;
      }

      // Move the cursor to the appropriate location
      // for printing with ncurses and print the
      // world
      move( y, x );
      if( o->useGUI )
      {
         mySim->printWorld();
      }

      // Take a census of the atoms in the world
      // occasionally
      if( mySim->getCurrentIter() % 128 == 0 )
      {
         mySim->takeCensus();
      }

      // Print out the progress of the simulation
      // once each second
      if( time(NULL) - lastProgressUpdate > 0 )
      {
         lastProgressUpdate = time(NULL);
         printw( "Iteration: %d of %d | %.2f%% complete\n",
            mySim->getCurrentIter(),
            o->maxIters,
            100 * (double)mySim->getCurrentIter() / (double)o->maxIters );
         refresh();
      }

      // Sleep the simulation each iteration
      usleep(o->sleep * 1000);
   }

   // Write the simulation parameters and diffusion data
   // and clean up ncurses
   mySim->writeConfig();
   mySim->writeAtoms();
   endwin();

   return 0;
}

