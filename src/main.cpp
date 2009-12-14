/* main.cpp
 */

#include <ctime>
#include <iostream>
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
   Sim* mySim = safeNew( Sim(o) );

   if( o->useGUI )
   {
      // Initialize ncurses
      initscr();   // Startup
      raw();       // Disable special terminal commands, such as CTRL+c to quit
      timeout(0);  // Makes getch a nonblocking call
   }

   // Print extra information about the simulation
   if( o->verbose && o->useGUI )
   {
      // Print using ncurses
      printw( "Press Ctrl-c to quit.\n" );
      printw( "------\n" );
      mySim->printElements();
      printw( "------\n" );
      mySim->printReactions();
      printw( "------\n" );
   }
   else if( o->verbose && !o->useGUI )
   {
      // Print using cout
      std::cout << "Press Ctrl-c to quit." << std::endl;
      std::cout << "------" << std::endl;
      mySim->printElements();
      std::cout << "------" << std::endl;
      mySim->printReactions();
      std::cout << "------" << std::endl;
   }

   int x = 0;
   int y = 0;
   int lastProgressUpdate = 0;

   if( o->useGUI )
   {
      getyx( stdscr, y, x );
      mySim->printWorld();
   }

   // Execute the simulation
   while( mySim->iterate() )
   {
      if( o->useGUI )
      // **********************
      // Print using ncurses
      // **********************
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
         mySim->printWorld();
         
         if( o->progress )
         {
            // Print out the progress of the simulation
            // once each second
            if( time(NULL) - lastProgressUpdate > 0 )
            {
               lastProgressUpdate = time(NULL);
               printw( "Iteration: %d of %d | %d%% complete\n",
                  mySim->getCurrentIter(),
                  o->maxIters,
                  (int)( 100 * (double)mySim->getCurrentIter() / (double)o->maxIters ) );
               refresh();
            }
         }
      }
      else if( o->progress )
      // **********************
      // Print using cout
      // **********************
      {
         // Print out the progress of the simulation
         // once each second
         if( time(NULL) - lastProgressUpdate > 0 )
         {
            lastProgressUpdate = time(NULL);
            std::cout << "                                                                                   \r" << std::flush;
            std::cout << "Iteration: " << mySim->getCurrentIter() << " of " << o->maxIters << " | ";
            std::cout << (int)( 100 * (double)mySim->getCurrentIter() / (double)o->maxIters ) << "\% complete\r" << std::flush;
         }
      }

      // Take a census of the atoms in the world
      // occasionally
      if( mySim->getCurrentIter() % 128 == 0 )
      {
         mySim->takeCensus();
      }

      // Sleep the simulation each iteration
      if( o->sleep != 0 )
      {
         usleep(o->sleep * 1000);
      }
   }

   // Write the simulation parameters and diffusion data
   // and clean up ncurses
   mySim->writeConfig();
   mySim->writeAtoms();
   if( o->useGUI )
   {
      endwin();
   }

   return 0;
}

