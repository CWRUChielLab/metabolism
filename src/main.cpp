/* main.cpp
 */

//#include <QApplication>
#include <ncurses.h>
#include <unistd.h>
#include "sim.h"

int
main ( int argc, char *argv[] )
{
   // Initialize ncurses
   initscr();  /* Startup */
   cbreak();   /* Allow special commands, like CTRL+c to quit */

   //QCoreApplication *app;

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
      usleep(80000);
      mySim->moveAtoms();
      move( y, x );
      mySim->printWorld();
   }

   // End ncurses
   endwin();

   return 0;
}

