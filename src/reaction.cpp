/* reaction.cpp
 */

#include <stdio.h>
#include "reaction.h"

static int list[10][2];

void
Reaction::initList()
{
   for( int i = 0; i < 10; i++ )
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
   for( int i = 0; i < 10; i++ )
   {
      for( int j = 0; j < 2; j++ )
      {
         fprintf( stdout, "(%d, %d): %d\n", i, j, list[i][j] );
      }
   }
}


