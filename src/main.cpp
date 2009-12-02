/* main.cpp
 */

//#include <QApplication>
#include <ncurses.h>
#include <unistd.h>   /* Might not be compatible with Windows */
#include "sim.h"

int
main ( int argc, char *argv[] )
{
   // Initialize ncurses
   initscr();  /* Startup */
   cbreak();   /* Allow special commands, like CTRL+c to quit */

   //QCoreApplication *app;

   printw( "Press Ctrl-c to quit.\n" );
   printw( "------\n" );
   Sim* mySim = new Sim();
   mySim->initialize();
   printw( "------\n" );
   mySim->printElements();
   printw( "------\n" );
   mySim->printReactions();
   printw( "------\n" );

   int x, y;
   getyx( stdscr, y, x );
   mySim->printWorld();

   for( int i = 0; i < 10000; i++ )
   {
      usleep(200000);
      mySim->iterate();
      move( y, x );
      if( i % 32 == 0 )
      {
         mySim->takeCensus(i);
      }
      mySim->printWorld();
   }
   mySim->finalizeAtoms();
   printw( "DONE!\n" );
   refresh();

   // Pause and cleanup
   getch();
   endwin();

   return 0;
}

