/* main.cpp
 */

#include <csignal>
#ifndef _NO_NCURSES
#include <ncurses.h>
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
   o->maxIters = mySim->getCurrentIter();
   interrupted = 1;
   std::cout << std::endl;
}


int
main ( int argc, char* argv[] )
{
   // Import command line options and initialize the simulation
   o = safeNew( Options( argc, argv ) );
   mySim = safeNew( Sim(o) );
   mySim->writeCensus();

   // Set up handling of Ctrl-c abort
   signal(SIGINT,handleExit);

   if( o->useGUI )
   {
      // Initialize ncurses
#ifndef _NO_NCURSES
      initscr();   // Startup
      timeout(0);  // Makes getch a nonblocking call
#endif
   }

   // Print extra information about the simulation
   if( o->verbose && o->useGUI )
   {
      // Print using ncurses
#ifndef _NO_NCURSES
      printw( "Press Ctrl-c to quit.\n" );
      printw( "------\n" );
      mySim->printEles( (std::ostream*)(NULL) );
      printw( "\n" );
      mySim->printRxns( (std::ostream*)(NULL) );
      printw( "\n" );
      mySim->printInits( (std::ostream*)(NULL) );
      printw( "------\n" );
#endif
   }
   else if( o->verbose && !o->useGUI )
   {
      // Print using cout
      std::cout << "Press Ctrl-c to quit." << std::endl;
      std::cout << "------" << std::endl;
      mySim->printEles( &std::cout );
      std::cout << std::endl;
      mySim->printRxns( &std::cout );
      std::cout << std::endl;
      mySim->printInits( &std::cout );
      std::cout << "------" << std::endl;
   }

#ifndef _NO_NCURSES
   int x = 0;
   int y = 0;
#endif
   int lastProgressUpdate = 0;

   if( o->useGUI )
   {
#ifndef _NO_NCURSES
      getyx( stdscr, y, x );
      mySim->printWorld();
#endif
   }

   // Execute the simulation
   while( !interrupted && mySim->iterate() )
   {
      if( o->useGUI )
      // **********************
      // Print using ncurses
      // **********************
      {
#ifndef _NO_NCURSES
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
#endif
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
      //if( mySim->getCurrentIter() % 32 == 0 )
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


   // Finalize the progress indicator to accurately
   // display how far the simulation got before ending
   // when running batches
   if( o->progress && !o->useGUI )
   {
      std::cout << "                                                                                   \r" << std::flush;
      std::cout << "Iteration: " << mySim->getCurrentIter() << " of " << o->maxIters << " | ";
      std::cout << (int)( 100 * (double)mySim->getCurrentIter() / (double)o->maxIters ) << "\% complete\r" << std::flush;
   }


   // Write the simulation parameters and diffusion data
   // and clean up ncurses
   mySim->writeConfig();
   mySim->writeDiffusion();
   if( o->useGUI )
   {
#ifndef _NO_NCURSES
      endwin();
#endif
   }

   return 0;
}

