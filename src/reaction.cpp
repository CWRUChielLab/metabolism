/* reaction.cpp
 */

#include <stdio.h>
#include "reaction.h"

int Reaction::list[5][2];
//int *Reaction::list;
Reaction *Reaction::testlist;

enum
{
   PRODUCTS,
   RATE
};

void
Reaction::initList()
{
   //list = new int[5][2];
   for( int i = 0; i < 5; i++ )
   {
      for( int j = 0; j < 2; j++ )
      {
         list[i][j] = i + j;
      }
   }
}


void
Reaction::printList()
{
   for( int i = 0; i < 5; i++ )
   {
      for( int j = 0; j < 2; j++ )
      {
         fprintf( stdout, "(%d,%d): %d\n", i, j, list[i][j] );
      }
   }
   fprintf( stdout, "Reaction 1's Products:\t%d\n             Rate:\t%d\n", list[0][PRODUCTS], list[0][RATE] );
   fprintf( stdout, "Reaction 2's Products:\t%d\n             Rate:\t%d\n", list[1][PRODUCTS], list[1][RATE] );
   fprintf( stdout, "Reaction 3's Products:\t%d\n             Rate:\t%d\n", list[2][PRODUCTS], list[2][RATE] );
   fprintf( stdout, "Reaction 4's Products:\t%d\n             Rate:\t%d\n", list[3][PRODUCTS], list[3][RATE] );
   fprintf( stdout, "Reaction 5's Products:\t%d\n             Rate:\t%d\n", list[4][PRODUCTS], list[4][RATE] );
}

