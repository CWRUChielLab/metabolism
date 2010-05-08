/* sim.cpp
 */

//#ifndef _GNU_SOURCE
//#define _GNU_SOURCE   // Needed for posix_memalign... ?
//#endif

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <map>
#ifndef _NO_NCURSES
#include <ncurses.h>
#endif
#include <SFMT.h>
#ifdef BLR_USEMAC
#include <sys/malloc.h>
#endif
#include <vector>
#include "safecalls.h"
#include "sim.h"
using namespace SafeCalls;


// Constructor
Sim::Sim( Options* newOptions )
{
   // Copy constructor arguments
   o = newOptions;

   // Setup the world
   currentIter = 0;
   world = safeNew( Atom*[ o->worldX * o->worldY ] );
   claimed = safeNew( uint8_t[ o->worldX * o->worldY ] );
   positions = safeNew( unsigned int[ o->worldX * o->worldY ] );

   dirdx = new int[8];
   dirdx[0] = 0;  // N
   dirdx[1] = 1;  // NE
   dirdx[2] = 1;  // E
   dirdx[3] = 1;  // SE
   dirdx[4] = 0;  // S
   dirdx[5] = -1; // SW
   dirdx[6] = -1; // W
   dirdx[7] = -1; // NW

   dirdy = new int[8];
   dirdy[0] = -1; // N
   dirdy[1] = -1; // NE
   dirdy[2] = 0;  // E
   dirdy[3] = 1;  // SE
   dirdy[4] = 1;  // S
   dirdy[5] = 1;  // SW
   dirdy[6] = 0;  // W
   dirdy[7] = -1; // NW

   // Initialize the world array to NULL
   for( int i = 0; i < o->worldX * o->worldY; i++ )
   {
      world[i] = NULL;
   }

   // Initialize the random number generator
   initRNG( o->seed );

   // Initialize the positions array with a random
   // ordering of integers ranging from 0 to
   // worldX*worldY-1
   shufflePositions();

   // Initialize the periodicTable
   Element* tempEle;

   tempEle = safeNew( Element( "Solvent", '*', 0, 0 ) );
   periodicTable[ "Solvent" ] = tempEle;

   for( char symbol = 'A'; symbol <= 'D'; symbol++ )
   {
      std::string name(1,symbol);
      tempEle = safeNew( Element( name, symbol, 0, 0 ) );
      periodicTable[ name ] = tempEle;
   }

   /*
   tempEle = safeNew( Element( "Enzyme", 'E', 0, 0 ) );
   periodicTable[ "Enzyme" ] = tempEle;

   tempEle = safeNew( Element( "Substrate", 'S', 0, 0 ) );
   periodicTable[ "Substrate" ] = tempEle;

   tempEle = safeNew( Element( "ES", 'F', 0, 0 ) );
   periodicTable[ "ES" ] = tempEle;

   tempEle = safeNew( Element( "Product", 'P', 0, 0 ) );
   periodicTable[ "Product" ] = tempEle;
   */

   // Initialize the rxnTable
   Reaction* tempRxn;

   tempRxn = safeNew( Reaction( ev(2,"A","B"), ev(2,"C","D"), 0.5 ) );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   /*
   tempRxn = safeNew( Reaction( ev(2,"Enzyme","Substrate"), ev(2,"ES","Solvent"), 0.5 ) );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   tempRxn = safeNew( Reaction( ev(2,"ES","Solvent"), ev(2,"Enzyme","Substrate"), 0.01, ev(2,"Enzyme","Product"), 0.01 ) );
   rxnTable[ tempRxn->getKey() ] = tempRxn;
   */

   // Fill the array of random numbers
   generateRandNums();

   // Initialize the world with random atoms
   ElementVector initialTypes = ev(2,"A","B");
   //ElementVector initialTypes = ev(2,"Enzyme","Substrate");
   Atom* tempAtom;
   int x, y;
   o->atomCount = std::min( o->atomCount, o->worldX * o->worldY );
   for( int i = 0; i < o->atomCount; i++ )
   {
      x = positions[i] % o->worldX;
      y = positions[i] / o->worldX;
      tempEle = initialTypes[ randNums[i] % initialTypes.size() ];
      //if( i < 64 ) tempEle = initialTypes[0]; else tempEle = initialTypes[1];
      tempAtom = safeNew( Atom( tempEle, x, y ) );
      world[ getWorldIndex(x,y) ] = tempAtom;
   }
}


// Execute one step of the simulation
// Returns 1 if the simulation succeeded in
// executing one step, and 0 if the
// simulation had already reached its
// maxIters
int
Sim::iterate()
{
   if( currentIter < o->maxIters )
   {
      if( o->doShuffle )
      {
         shuffleWorld();
      }
      generateRandNums();
      moveAtoms();
      if( o->doRxns )
      {
         executeRxns();
      }
      currentIter++;
      return 1;
   }
   else
   {
      return 0;
   }
}


int
Sim::getCurrentIter()
{
   return currentIter;
}


// Returns an ElementVector containing elementCount
// Elements specified as a comma separated list of
// names, e.g. ev(2,"A","B")
ElementVector
Sim::ev( int elementCount, ... )
{
   ElementVector ev;
   char* name;

   va_list nameList;
   va_start( nameList, elementCount );
   for( int i = 0; i < elementCount; i++ )
   {
      name = va_arg( nameList, char* );
      std::string stringName(name);
      Element* ele = periodicTable[stringName];
      ev.push_back( ele );
   }
   va_end( nameList );

   return ev;
}


// Handles wrapping around the edges of the world
// and translating two-dimensional coordinates
// to a one-dimensional index for the world array
int
Sim::getWorldIndex( int x, int y )
{
   int wrappedX = ( x + o->worldX ) % o->worldX;
   int wrappedY = ( y + o->worldY ) % o->worldY;
   return ( wrappedX + wrappedY * o->worldX );
}


// Initialize the random number generator
void
Sim::initRNG( int initSeed )
{
   static int allocated = 0;

   // Set the seed
   init_gen_rand( (uint32_t)(initSeed) );

   if( !allocated )
   {
      // The array randNums will be treated by the
      // RNG as an array of 64-bit ints.  The length
      // of the array (in 64-bit ints) must be a
      // multiple of 2 and the array must be at least
      // get_min_array_size64() 64-bit ints long.
      // With MEXP = 132049, get_min_array_size64() =
      // 2*((MEXP/128)+1) = 2064.
      int min_rand_nums_needed = o->worldX * o->worldY;
      int min_bytes_needed = min_rand_nums_needed * sizeof( *randNums );
      int min_64_bit_ints_needed = (int)ceil( min_bytes_needed / 8.0 );

      // Make sure we have at least the minimum
      // length needed
      randNums_length_in_64_bit_ints = std::max( min_64_bit_ints_needed, get_min_array_size64() );

      // Make sure we have a length (in 64-bit ints)
      // that is a multiple of 2.
      if( randNums_length_in_64_bit_ints % 2 != 0 )
      {
         randNums_length_in_64_bit_ints++;
      }

      int bytes_to_be_allocated = randNums_length_in_64_bit_ints * 8;

      int rc = 0;
#ifdef BLR_USELINUX
      rc = posix_memalign( (void**)&randNums, getpagesize(), bytes_to_be_allocated );
#else
#ifdef BLR_USEMAC
      randNums = (uint64_t*)malloc( bytes_to_be_allocated );
#else
#ifdef BLR_USEWIN
      randNums = (unsigned char*)malloc( bytes_to_be_allocated + 16 );
      randNums += 16 - (long int)randNums % 16;
#endif
#endif
#endif
      assert( rc == 0 );
      assert( randNums );

      allocated = 1;
   }
}


// Get a new set of randNums.
void
Sim::generateRandNums()
{
   static int firstTime = 1;
   
   // fill_array64 fills randNums with 64-bit ints.
   // See initRNG method for more information.
   fill_array64( (uint64_t*)(randNums), randNums_length_in_64_bit_ints );

   if (firstTime)
   {
      static std::ofstream randFile;
      randFile.open( "rand.out" );

      for( int i=0; i<10; i++ )
      {
         randFile << randNums[i] << std::endl;
      }
      randFile.close();

      firstTime = 0;
   }
}


// Fill the positions array with successive
// integers ranging from 0 to worldX*worldY-1
// and then shuffle these integers.
void
Sim::shufflePositions()
{
   unsigned int i, highest, lowest, range, rand, temp;

   // Fill the array of random numbers
   generateRandNums();

   // Fill the positions array with successive integers
   for( i = 0; i < (unsigned int)(o->worldX * o->worldY); i++ )
   {
      positions[i] = i;
   }

   // Shuffle the positions array
   highest = o->worldX * o->worldY - 1;
   for( i = 0; i < highest; i++ )
   {
      lowest = i + 1;
      range = highest - lowest + 1;
      rand = ( randNums[i] % range ) + lowest;
      temp = positions[i];
      positions[i] = positions[rand];
      positions[rand] = temp;
   }
}


// Shuffle the atoms in the world to
// random positions.  Add to iterate
// in for the well-mixed case
void
Sim::shuffleWorld()
{
   shufflePositions();

   Atom** temp = safeNew( Atom*[ o->worldX * o->worldY ] );
   for( int i = 0; i < o->worldX * o->worldY; i++ )
   {
      temp[i] = NULL;
   }

   for( int x = 0; x < o->worldX; x++ )
   {
      for( int y = 0; y < o->worldY; y++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         {
            int newX = positions[ getWorldIndex(x,y) ] % o->worldX;
            int newY = positions[ getWorldIndex(x,y) ] / o->worldX;
            temp[ getWorldIndex(newX,newY) ] = world[ getWorldIndex(x,y) ];
            temp[ getWorldIndex(newX,newY) ]->setX(newX);
            temp[ getWorldIndex(newX,newY) ]->setY(newY);
         }
      }
   }

   delete world;
   world = temp;
}


void
Sim::moveAtoms()
{
   // Initially set all claimed flags to 0
   std::memset( claimed, 0, o->worldX * o->worldY );
   
   // Increment a claimed flag wherever an atom exists and
   // wherever an atom wants to move
   for( int y = 0; y < o->worldY; y++ )
   {
      for( int x = 0; x < o->worldX; x++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         {
            int dx = dirdx[ randNums[ getWorldIndex(x,y) ] & 0x7 ];
            int dy = dirdy[ randNums[ getWorldIndex(x,y) ] & 0x7 ];
            claimed[ getWorldIndex(x,y) ]++;
            claimed[ getWorldIndex(x+dx,y+dy) ]++;
         }
      }
   }

   // By this point, all atoms are guarenteed to have a positive
   // claimed flag in their current position.  An atom that can
   // move (i.e., it has not experienced a collision) has a claimed
   // flag value of exactly 1 in both its current position and the
   // destination position for the atom.  An atom that cannot move
   // has a claimed flag value greater than 1 in one or both of
   // these positions.
   //
   // Below, as the world is scanned, an atom will be marked as
   // having been processed by setting its claimed flag value
   // to 0.  This ensures that if the same atom is encountered
   // again in the same pass (because, e.g., it moved SE into an
   // area that had yet to be processed), its delta and collision
   // variables will not be adjusted a second time in the same
   // iteration.

   for( int y = 0; y < o->worldY; y++ )
   {
      for( int x = 0; x < o->worldX; x++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL &&
               claimed[ getWorldIndex(x,y) ] > 0 )
         // If an atom is encountered that has not been processed yet
         {
            int dx = dirdx[ randNums[ getWorldIndex(x,y) ] & 0x7 ];
            int dy = dirdy[ randNums[ getWorldIndex(x,y) ] & 0x7 ];
            Atom* thisAtom = world[ getWorldIndex(x,y) ];

            thisAtom->setDxIdeal( thisAtom->getDxIdeal() + dx );
            thisAtom->setDyIdeal( thisAtom->getDyIdeal() + dy );

            if( claimed[ getWorldIndex(x,y) ] == 1 && claimed[ getWorldIndex(x+dx,y+dy) ] == 1 )
            // Move if there are no collisions
            {
               thisAtom->setDxActual( thisAtom->getDxActual() + dx );
               thisAtom->setDyActual( thisAtom->getDyActual() + dy );

               thisAtom->setX( ( (x+dx) + o->worldX ) % o->worldX );
               thisAtom->setY( ( (y+dy) + o->worldY ) % o->worldY );

               world[ getWorldIndex(x,y) ] = NULL;
               world[ getWorldIndex(x+dx,y+dy) ] = thisAtom;

               // Mark the moved atom as processed
               claimed[ getWorldIndex(x+dx,y+dy) ] = 0;
            }
            else
            // Else increment collisions
            {
               thisAtom->setCollisions( thisAtom->getCollisions() + 1 );

               // Mark the unmoved atom as processed
               claimed[ getWorldIndex(x,y) ] = 0;
            }
         }
      }
   }
}


// Scan the world, checking for potential
// reactions and executing some of them
void
Sim::executeRxns()
{
   Atom* thisAtom;
   int neighborX, neighborY;
   Atom* neighborAtom;
   std::vector<Element*> thisRxnProducts;
   double thisRxnProb;

   // Initially set all claimed flags to 0
   std::memset( claimed, 0, o->worldX * o->worldY );

   // Increment a claimed flag wherever an atom that wants
   // to react exists and wherever its reactive neighbor exists
   for( int y = 0; y < o->worldY; y++ )
   {
      for( int x = 0; x < o->worldX; x++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         // If an atom is encountered
         {
            thisAtom = world[ getWorldIndex(x,y) ];
         }
         else
         // Else solvent is encountered
         {
            thisAtom = safeNew( Atom( periodicTable[ "Solvent" ], x, y) );
         }

         // Determine which neighbor to attempt to react with, if any
         switch( (randNums[ getWorldIndex(x,y) ] >> 3) % 5 )
         {
            case 0:  // First-order
               neighborX = x+0;
               neighborY = y+0;
               break;
            case 1:  // Second-order (E)
               neighborX = x+1;
               neighborY = y+0;
               break;
            case 2:  // Second-order (SE)
               neighborX = x+1;
               neighborY = y+1;
               break;
            case 3:  // Second-order (S)
               neighborX = x+0;
               neighborY = y+1;
               break;
            case 4:  // Second-order (SW)
               neighborX = x-1;
               neighborY = y+1;
               break;
            default:
               assert( 0 );
         }
         if( neighborX == x && neighborY == y )
         {
            neighborAtom = NULL;
         }
         else
         {
            if( world[ getWorldIndex(neighborX,neighborY) ] != NULL )
            {
               neighborAtom = world[ getWorldIndex(neighborX,neighborY) ];
            }
            else
            {
               neighborAtom = safeNew( Atom( periodicTable[ "Solvent" ], x, y ) );
            }
         }

         thisRxnProb = 0;

         if( neighborAtom == NULL )
         // If the reaction is first-order
         {
            switch( (randNums[ getWorldIndex(x,y) ] >> 3) % 2 )
            // Determine which set of products to consider
            {
               case 0:  // First products
                  if( rxnTable[ thisAtom->getType()->getKey() ] != NULL &&
                     !rxnTable[ thisAtom->getType()->getKey() ]->getFirstProducts().empty() )
                  // If the reaction exists and has first products
                  {
                     thisRxnProducts = rxnTable[ thisAtom->getType()->getKey() ]->getFirstProducts();
                     thisRxnProb     = rxnTable[ thisAtom->getType()->getKey() ]->getFirstProb();
                  }
                  break;
               case 1:  // Second products
                  if( rxnTable[ thisAtom->getType()->getKey() ] != NULL &&
                     !rxnTable[ thisAtom->getType()->getKey() ]->getSecondProducts().empty() )
                  // If the reaction exists and has second products
                  {
                     thisRxnProducts = rxnTable[ thisAtom->getType()->getKey() ]->getSecondProducts();
                     thisRxnProb     = rxnTable[ thisAtom->getType()->getKey() ]->getSecondProb();
                  }
                  break;
               default:
                  assert( 0 );
            }
         }
         else
         // Else the reaction is second-order
         {
            switch( (randNums[ getWorldIndex(x,y) ] >> 3) % 2 )
            // Determine which set of products to consider
            {
               case 0:  // First products
                  if( rxnTable[ thisAtom->getType()->getKey() *
                            neighborAtom->getType()->getKey() ] != NULL &&
                     !rxnTable[ thisAtom->getType()->getKey() *
                            neighborAtom->getType()->getKey() ]->getFirstProducts().empty() )
                  // If the reaction exists and has first products
                  {
                     thisRxnProducts = rxnTable[ thisAtom->getType()->getKey() *
                                             neighborAtom->getType()->getKey() ]->getFirstProducts();
                     thisRxnProb     = rxnTable[ thisAtom->getType()->getKey() *
                                             neighborAtom->getType()->getKey() ]->getFirstProb();
                  }
                  break;
               case 1:  // Second products
                  if( rxnTable[ thisAtom->getType()->getKey() *
                            neighborAtom->getType()->getKey() ] != NULL &&
                     !rxnTable[ thisAtom->getType()->getKey() *
                            neighborAtom->getType()->getKey() ]->getSecondProducts().empty() )
                  // If the reaction exists and has second products
                  {
                     thisRxnProducts = rxnTable[ thisAtom->getType()->getKey() *
                                             neighborAtom->getType()->getKey() ]->getSecondProducts();
                     thisRxnProb     = rxnTable[ thisAtom->getType()->getKey() *
                                             neighborAtom->getType()->getKey() ]->getSecondProb();
                  }
                  break;
               default:
                  assert( 0 );
            }
         }

         if( (double)(randNums[ getWorldIndex(x,y) ] >> 3) /
             (double)((uint64_t)1 << (8 * sizeof(*randNums) - 3))
               < thisRxnProb )
         // If the reactant have enough energy
         {
            if( neighborAtom == NULL )
            // If the reaction is first-order
            {
               // Stake the claim
               claimed[ getWorldIndex(x,y) ]++;
            }
            else
            // Else the reaction is second-order
            {
               // Stake the claim
               claimed[ getWorldIndex(x,y) ]++;
               claimed[ getWorldIndex(neighborX,neighborY) ]++;
            }
         }

         // Delete any temporary solvent atoms
         if( world[ getWorldIndex(x,y) ] == NULL )
         {
            delete thisAtom;
            thisAtom = NULL;
         }
         if( world[ getWorldIndex(neighborX,neighborY) ] == NULL )
         {
            delete neighborAtom;
            neighborAtom = NULL;
         }
      }
   }

   // By this point, all atoms that are trying to react are
   // guarenteed to have a positive claimed flag in their current
   // position.  An atom that can react (i.e., it and its reacting
   // neighbor are trying to participate in exactly 1 reaction) has a
   // claimed flag value of exactly 1 in both its position and the
   // position of the neighboring reactive atom.  An atom that
   // cannot react has a claimed flag value greater than 1 in one
   // or both of these positions.
   //
   // Below, as the world is scanned, an atom will be marked as
   // having reacted by setting its claimed flag value to 0.  This
   // speeds up checking as the scan moves through the world.

   for( int y = 0; y < o->worldY; y++ )
   {
      for( int x = 0; x < o->worldX; x++ )
      {
         if( claimed[ getWorldIndex(x,y) ] == 1)
         // If something is encountered that has not been processed yet
         // and could undergo a reaction
         {
            if( world[ getWorldIndex(x,y) ] != NULL )
            // If an atom is encountered
            {
               thisAtom = world[ getWorldIndex(x,y) ];
            }
            else
            // Else solvent is encountered
            {
               thisAtom = safeNew( Atom( periodicTable[ "Solvent" ], x, y) );
            }

            // Determine which neighbor to attempt to react with, if any
            switch( (randNums[ getWorldIndex(x,y) ] >> 3) % 5 )
            {
               case 0:  // First-order
                  neighborX = x+0;
                  neighborY = y+0;
                  break;
               case 1:  // Second-order (E)
                  neighborX = x+1;
                  neighborY = y+0;
                  break;
               case 2:  // Second-order (SE)
                  neighborX = x+1;
                  neighborY = y+1;
                  break;
               case 3:  // Second-order (S)
                  neighborX = x+0;
                  neighborY = y+1;
                  break;
               case 4:  // Second-order (SW)
                  neighborX = x-1;
                  neighborY = y+1;
                  break;
               default:
                  assert( 0 );
            }
            if( neighborX == x && neighborY == y )
            {
               neighborAtom = NULL;
            }
            else
            {
               if( world[ getWorldIndex(neighborX,neighborY) ] != NULL )
               {
                  neighborAtom = world[ getWorldIndex(neighborX,neighborY) ];
               }
               else
               {
                  neighborAtom = safeNew( Atom( periodicTable[ "Solvent" ], x, y ) );
               }
            }

            thisRxnProb = 0;

            if( neighborAtom == NULL )
            // If the reaction is first-order
            {
               switch( (randNums[ getWorldIndex(x,y) ] >> 3) % 2 )
               // Determine which set of products to consider
               {
                  case 0:  // First products
                     if( rxnTable[ thisAtom->getType()->getKey() ] != NULL &&
                        !rxnTable[ thisAtom->getType()->getKey() ]->getFirstProducts().empty() )
                     // If the reaction exists and has first products
                     {
                        thisRxnProducts = rxnTable[ thisAtom->getType()->getKey() ]->getFirstProducts();
                        thisRxnProb     = rxnTable[ thisAtom->getType()->getKey() ]->getFirstProb();
                     }
                     break;
                  case 1:  // Second products
                     if( rxnTable[ thisAtom->getType()->getKey() ] != NULL &&
                        !rxnTable[ thisAtom->getType()->getKey() ]->getSecondProducts().empty() )
                     // If the reaction exists and has second products
                     {
                        thisRxnProducts = rxnTable[ thisAtom->getType()->getKey() ]->getSecondProducts();
                        thisRxnProb     = rxnTable[ thisAtom->getType()->getKey() ]->getSecondProb();
                     }
                     break;
                  default:
                     assert( 0 );
               }
            }
            else
            // Else the reaction is second-order
            {
               if( claimed[ getWorldIndex(neighborX,neighborY) ] == 1 )
               // If the neighbor has not been processed yet and
               // could undergo a reaction
               {
                  switch( (randNums[ getWorldIndex(x,y) ] >> 3) % 2 )
                  // Determine which set of products to consider
                  {
                     case 0:  // First products
                        if( rxnTable[ thisAtom->getType()->getKey() *
                                  neighborAtom->getType()->getKey() ] != NULL &&
                           !rxnTable[ thisAtom->getType()->getKey() *
                                  neighborAtom->getType()->getKey() ]->getFirstProducts().empty() )
                        // If the reaction exists and has first products
                        {
                           thisRxnProducts = rxnTable[ thisAtom->getType()->getKey() *
                                                   neighborAtom->getType()->getKey() ]->getFirstProducts();
                           thisRxnProb     = rxnTable[ thisAtom->getType()->getKey() *
                                                   neighborAtom->getType()->getKey() ]->getFirstProb();
                        }
                        break;
                     case 1:  // Second products
                        if( rxnTable[ thisAtom->getType()->getKey() *
                                  neighborAtom->getType()->getKey() ] != NULL &&
                           !rxnTable[ thisAtom->getType()->getKey() *
                                  neighborAtom->getType()->getKey() ]->getSecondProducts().empty() )
                        // If the reaction exists and has second products
                        {
                           thisRxnProducts = rxnTable[ thisAtom->getType()->getKey() *
                                                   neighborAtom->getType()->getKey() ]->getSecondProducts();
                           thisRxnProb     = rxnTable[ thisAtom->getType()->getKey() *
                                                   neighborAtom->getType()->getKey() ]->getSecondProb();
                        }
                        break;
                     default:
                        assert( 0 );
                  }
               }
            }

            if( (double)(randNums[ getWorldIndex(x,y) ] >> 3) /
                (double)((uint64_t)1 << (8 * sizeof(*randNums) - 3))
                  < thisRxnProb )
            // If the reactant have enough energy
            {
               if( neighborAtom == NULL )
               // If the reaction is first-order
               {
                  // Execute the reaction
                  thisAtom->setType(thisRxnProducts[0]);
                  world[ getWorldIndex(x,y) ] = thisAtom;

                  // Mark the atom as having already reacted
                  claimed[ getWorldIndex(x,y) ] = 0;
               }
               else
               // Else the reaction is second-order
               {
                  // Execute the reaction
                  thisAtom->setType(thisRxnProducts[0]);
                  neighborAtom->setType(thisRxnProducts[1]);
                  world[ getWorldIndex(x,y) ] = thisAtom;
                  world[ getWorldIndex(neighborX,neighborY) ] = neighborAtom;

                  // Mark the reaction participants as having already reacted
                  claimed[ getWorldIndex(x,y) ] = 0;
                  claimed[ getWorldIndex(neighborX,neighborY) ] = 0;
               }
            }

            // Delete any solvent atoms that are newly created or never reacted
            if( thisAtom->getType() == periodicTable[ "Solvent" ] )
            {
               delete thisAtom;
               thisAtom = NULL;
               world[ getWorldIndex(x,y) ] = NULL;
            }
            if( neighborAtom != NULL && neighborAtom->getType() == periodicTable[ "Solvent" ] )
            {
               delete neighborAtom;
               neighborAtom = NULL;
               world[ getWorldIndex(neighborX,neighborY) ] = NULL;
            }
         }
      }
   }
}


// Output all important experimental parameters
void
Sim::writeConfig()
{
   int colwidth = 12;
   static std::ofstream configFile;
   configFile.open( o->configFile.c_str() );

   // Write parameters to file
   configFile.flags(std::ios::left);
   configFile << std::setw(20) <<
                 "version" << std::setw(colwidth) <<
                 "seed" << std::setw(colwidth) <<
                 "iters" << std::setw(colwidth) <<
                 "x" << std::setw(colwidth) <<
                 "y" << std::setw(colwidth) <<
                 "atoms" << std::setw(colwidth) <<
                 "shuffle" << std::endl;
   configFile << std::setw(20) <<
                 GIT_TAG << std::setw(colwidth) <<
                 o->seed << std::setw(colwidth) <<
                 o->maxIters << std::setw(colwidth) <<
                 o->worldX << std::setw(colwidth) <<
                 o->worldY << std::setw(colwidth) <<
                 o->atomCount << std::setw(colwidth) <<
                 (o->doShuffle?"true":"false") << std::endl;
   configFile.close();
}


// Records important information about the state
// of the world and writes it to file.
void
Sim::takeCensus()
{
   int colwidth = 12;
   static int initialized = 0;
   static std::ofstream censusFile;
   int totalAtoms = 0;

   if( !initialized )
   {
      initialized = 1;
      censusFile.open( o->censusFile.c_str() );
      censusFile.flags(std::ios::left);
      censusFile << std::setw(colwidth) << "iter";
      for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
      {
         Element* ele = i->second;
         if( ele != periodicTable[ "Solvent" ] )
         {
            censusFile << std::setw(colwidth) << ele->getName();
         }
      }
      censusFile << std::setw(colwidth) << "total" << std::endl;
   }

   censusFile << std::setw(colwidth) << currentIter;
   for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
   {
      Element* ele = i->second;
      if( ele != periodicTable[ "Solvent" ] )
      {
         censusFile << std::setw(colwidth) << ele->getCount();
      }
      totalAtoms += ele->getCount();
   }
   censusFile << std::setw(colwidth) << totalAtoms << std::endl;
}


// Writes important information about the state
// of the world to file.  To be called when the
// simulation ends.
void
Sim::dumpAtoms()
{
   int colwidth = 12;
   std::ofstream diffusionFile;
   diffusionFile.open( o->diffusionFile.c_str() );
   diffusionFile.flags(std::ios::left);
   diffusionFile << std::setw(colwidth) <<
                    "type" << std::setw(colwidth) <<
                    "dx_actual" << std::setw(colwidth) <<
                    "dy_actual" << std::setw(colwidth) <<
                    "dx_ideal" << std::setw(colwidth) <<
                    "dy_ideal" << std::setw(colwidth) <<
                    "collisions" << std::endl;
   for( int x = 0; x < o->worldX; x++ )
   {
      for( int y = 0; y < o->worldY; y++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         {
            Atom* thisAtom = world[ getWorldIndex(x,y) ];
            diffusionFile << std::setw(colwidth) <<
                             thisAtom->getType()->getName() << std::setw(colwidth) <<
                             thisAtom->getDxActual() << std::setw(colwidth) <<
                             thisAtom->getDyActual() << std::setw(colwidth) <<
                             thisAtom->getDxIdeal() << std::setw(colwidth) <<
                             thisAtom->getDyIdeal() << std::setw(colwidth) <<
                             thisAtom->getCollisions() << std::endl;
         }
      }
   }
   diffusionFile.close();
}


void
Sim::printWorld()
{
#ifndef _NO_NCURSES
   for( int y = 0; y < o->worldY; y++ )
   {
      for( int x = 0; x < o->worldX; x++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         {
            printw( "%c ", world[ getWorldIndex(x,y) ]->getType()->getSymbol() );
         }
         else
         {
            printw( ". " );
         }
      }
      printw( "\n" );
   }
   printw( "\n" );
   refresh();
#endif
}


void
Sim::printElements()
{
   if( o->useGUI )
   // **********************
   // Print using ncurses
   // **********************
   {
#ifndef _NO_NCURSES
      printw( "There are %d elements.\n", periodicTable.size() );
      for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
      {
         Element* ele = i->second;
         printw( "periodicTable[\"%s\"] has key:\t%d\n", ele->getName().c_str(), periodicTable[ele->getName()]->getKey() );
      }
      refresh();
#endif
   }
   else
   // **********************
   // Print using cout
   // **********************
   {
      std::cout << "There are " << periodicTable.size() << " elements." << std::endl;
      for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
      {
         Element* ele = i->second;
         std::cout << "periodicTable[\"" << ele->getName() << "\"] has key:\t" << periodicTable[ele->getName()]->getKey() << std::endl;
      }
   }
}


void
Sim::printReactions()
{
   std::map<char,int> reactantCount;
   std::map<char,int> firstProductCount;
   std::map<char,int> secondProductCount;

   // Loop through the rxnTable, printing each Reaction
   for( ReactionMap::iterator i = rxnTable.begin(); i != rxnTable.end(); i++ )
   {
      Reaction* rxn = i->second;

      // Count up the number of each type of reactant and product
      for( unsigned int j = 0; j < rxn->getReactants().size(); j++ )
      {
         reactantCount[ rxn->getReactants()[j]->getSymbol() ]++;
      }
      for( unsigned int j = 0; j < rxn->getFirstProducts().size(); j++ )
      {
         firstProductCount[ rxn->getFirstProducts()[j]->getSymbol() ]++;
      }
      if( !rxn->getSecondProducts().empty() )
      {
         for( unsigned int j = 0; j < rxn->getSecondProducts().size(); j++ )
         {
            secondProductCount[ rxn->getSecondProducts()[j]->getSymbol() ]++;
         }
      }

      if( o->useGUI )
      // **********************************
      // Print first reaction using ncurses
      // **********************************
      {
#ifndef _NO_NCURSES
         printw( "Key: %d  Prob: %f  \t", rxn->getKey(), rxn->getFirstProb() );

         // Print the reactants, grouping copies of one type together
         // with stoichiometric coefficients
         for( std::map<char,int>::iterator j = reactantCount.begin(); j != reactantCount.end(); j++ )
         {
            int coefficient = j->second;
            if( coefficient == 1 )
            {
               if( j == reactantCount.begin() )
               {
                  printw( "%c", j->first );
               }
               else
               {
                  printw( " + %c", j->first );
               }
            }
            else
            {
               if( j == reactantCount.begin() )
               {
                  printw( "%d%c", j->second, j->first );
               }
               else
               {
                  printw( " + %d%c", j->second, j->first );
               }
            }
         }

         printw( " -> " );

         // Print the first products, grouping copies of one type together
         // with stoichiometric coefficients
         for( std::map<char,int>::iterator j = firstProductCount.begin(); j != firstProductCount.end(); j++ )
         {
            int coefficient = j->second;
            if( coefficient == 1 )
            {
               if( j == firstProductCount.begin() )
               {
                  printw( "%c", j->first );
               }
               else
               {
                  printw( " + %c", j->first );
               }
            }
            else
            {
               if( j == firstProductCount.begin() )
               {
                  printw( "%d%c", j->second, j->first );
               }
               else
               {
                  printw( " + %d%c", j->second, j->first );
               }
            }
         }

         printw( "\n" );


         if( !rxn->getSecondProducts().empty() )
         // ***********************************
         // Print second reaction using ncurses
         // ***********************************
         {
            printw( "Key: %d  Prob: %f  \t", rxn->getKey(), rxn->getSecondProb() );

            // Print the reactants, grouping copies of one type together
            // with stoichiometric coefficients
            for( std::map<char,int>::iterator j = reactantCount.begin(); j != reactantCount.end(); j++ )
            {
               int coefficient = j->second;
               if( coefficient == 1 )
               {
                  if( j == reactantCount.begin() )
                  {
                     printw( "%c", j->first );
                  }
                  else
                  {
                     printw( " + %c", j->first );
                  }
               }
               else
               {
                  if( j == reactantCount.begin() )
                  {
                     printw( "%d%c", j->second, j->first );
                  }
                  else
                  {
                     printw( " + %d%c", j->second, j->first );
                  }
               }
            }

            printw( " -> " );

            // Print the second products, grouping copies of one type together
            // with stoichiometric coefficients
            for( std::map<char,int>::iterator j = secondProductCount.begin(); j != secondProductCount.end(); j++ )
            {
               int coefficient = j->second;
               if( coefficient == 1 )
               {
                  if( j == secondProductCount.begin() )
                  {
                     printw( "%c", j->first );
                  }
                  else
                  {
                     printw( " + %c", j->first );
                  }
               }
               else
               {
                  if( j == secondProductCount.begin() )
                  {
                     printw( "%d%c", j->second, j->first );
                  }
                  else
                  {
                     printw( " + %d%c", j->second, j->first );
                  }
               }
            }

            printw( "\n" );
         }
#endif
      }
      else
      // *******************************
      // Print first reaction using cout
      // *******************************
      {
         std::cout << "Key: " << rxn->getKey() << "  Prob: " << rxn->getFirstProb() << "  \t";

         // Print the reactants, grouping copies of one type together
         // with stoichiometric coefficients
         for( std::map<char,int>::iterator j = reactantCount.begin(); j != reactantCount.end(); j++ )
         {
            int coefficient = j->second;
            if( coefficient == 1 )
            {
               if( j == reactantCount.begin() )
               {
                  std::cout << j->first;
               }
               else
               {
                  std::cout << " + " << j->first;
               }
            }
            else
            {
               if( j == reactantCount.begin() )
               {
                  std::cout << j->second << j->first;
               }
               else
               {
                  std::cout << " + " << j->second << j->first;
               }
            }
         }

         std::cout << " -> ";

         // Print the first products, grouping copies of one type together
         // with stoichiometric coefficients
         for( std::map<char,int>::iterator j = firstProductCount.begin(); j != firstProductCount.end(); j++ )
         {
            int coefficient = j->second;
            if( coefficient == 1 )
            {
               if( j == firstProductCount.begin() )
               {
                  std::cout << j->first;
               }
               else
               {
                  std::cout << " + " << j->first;
               }
            }
            else
            {
               if( j == firstProductCount.begin() )
               {
                  std::cout << j->second << j->first;
               }
               else
               {
                  std::cout << " + " << j->second << j->first;
               }
            }
         }

         std::cout << std::endl;


         if( !rxn->getSecondProducts().empty() )
         // ********************************
         // Print second reaction using cout
         // ********************************
         {
            std::cout << "Key: " << rxn->getKey() << "  Prob: " << rxn->getSecondProb() << "  \t";

            // Print the reactants, grouping copies of one type together
            // with stoichiometric coefficients
            for( std::map<char,int>::iterator j = reactantCount.begin(); j != reactantCount.end(); j++ )
            {
               int coefficient = j->second;
               if( coefficient == 1 )
               {
                  if( j == reactantCount.begin() )
                  {
                     std::cout << j->first;
                  }
                  else
                  {
                     std::cout << " + " << j->first;
                  }
               }
               else
               {
                  if( j == reactantCount.begin() )
                  {
                     std::cout << j->second << j->first;
                  }
                  else
                  {
                     std::cout << " + " << j->second << j->first;
                  }
               }
            }

            std::cout << " -> ";

            // Print the second products, grouping copies of one type together
            // with stoichiometric coefficients
            for( std::map<char,int>::iterator j = secondProductCount.begin(); j != secondProductCount.end(); j++ )
            {
               int coefficient = j->second;
               if( coefficient == 1 )
               {
                  if( j == secondProductCount.begin() )
                  {
                     std::cout << j->first;
                  }
                  else
                  {
                     std::cout << " + " << j->first;
                  }
               }
               else
               {
                  if( j == secondProductCount.begin() )
                  {
                     std::cout << j->second << j->first;
                  }
                  else
                  {
                     std::cout << " + " << j->second << j->first;
                  }
               }
            }

            std::cout << std::endl;
         }
      }
   }
}

