/* main.cpp
 */

#include <ctime>
#include <ncurses.h>
#include <unistd.h>   // Might not be compatible with Windows
#include "options.h"
#include "sim.h"


int
main ( int argc, char* argv[] )
{
   Options* o = new Options( argc, argv );

   // Initialize ncurses
   initscr();  // Startup
   cbreak();   // Allow special commands, like CTRL+c to quit

   Sim* mySim = new Sim( o->seed, o->maxIters, o->worldX, o->worldY, o->atomCount );
   mySim->dumpConfig();

   printw( "Press Ctrl-c to quit.\n" );
   printw( "------\n" );
   mySim->printElements();
   printw( "------\n" );
   mySim->printReactions();
   printw( "------\n" );

   int x, y;
   getyx( stdscr, y, x );
   if( o->useGUI )
   {
      mySim->printWorld();
   }

   while( mySim->iterate() )
   {
      move( y, x );
      if( o->useGUI )
      {
         mySim->printWorld();
      }
      if( mySim->getCurrentIter() % 128 == 0 )
      {
         mySim->takeCensus();
      }
      if( mySim->getCurrentIter() % 8 == 0 )
      {
         printw( "Iteration: %d of %d | %.2f%% complete\n", mySim->getCurrentIter(), o->maxIters, 100*(double)mySim->getCurrentIter()/(double)o->maxIters );
         refresh();
      }
      usleep(o->sleep * 1000);
   }
   mySim->dumpAtoms();
   printw( "DONE!\n" );
   refresh();

   // Pause and cleanup
   //getch();
   endwin();

   return 0;
}

