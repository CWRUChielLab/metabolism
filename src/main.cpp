/* main.cpp
 */

//#include <QApplication>
#include <ctime>
#include <ncurses.h>
#include <unistd.h>   // Might not be compatible with Windows
#include "sim.h"

int
main ( int argc, char* argv[] )
{
   // Initialize ncurses
   initscr();  // Startup
   cbreak();   // Allow special commands, like CTRL+c to quit

   //QCoreApplication *app;

   int seed = time(NULL);
   int maxIters = 10000;
   int worldX = 16;
   int worldY = 16;
   int atomCount = worldX * worldY / 4;

   Sim* mySim = new Sim( seed, maxIters, worldX, worldY, atomCount );
   mySim->dumpConfig();

   printw( "Press Ctrl-c to quit.\n" );
   printw( "------\n" );
   mySim->printElements();
   printw( "------\n" );
   mySim->printReactions();
   printw( "------\n" );

   int x, y;
   getyx( stdscr, y, x );
   mySim->printWorld();

   while( mySim->iterate() )
   {
      move( y, x );
      mySim->printWorld();
      if( mySim->getCurrentIter() % 128 == 0 )
      {
         mySim->takeCensus();
      }
      if( mySim->getCurrentIter() % 8 == 0 )
      {
         printw( "Iteration: %d of %d | %.2f%% complete\n", mySim->getCurrentIter(), maxIters, 100*(double)mySim->getCurrentIter()/(double)maxIters );
         refresh();
      }
      usleep(200000);
   }
   mySim->dumpAtoms();
   printw( "DONE!\n" );
   refresh();

   // Pause and cleanup
   getch();
   endwin();

   return 0;
}

