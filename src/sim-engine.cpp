/* sim-engine.cpp
 */

#define __USE_XOPEN2K   // Needed for posix_memalign on louder -- why?
#include <cassert>
#include <cmath>   // ceil
#include <cstdarg> // variable arguments handling
#include <cstring> // memset
#include <fstream>
#include <SFMT/SFMT.h>
#ifdef BLR_USEMAC
#include <sys/malloc.h> // aligned memory retrieval on Mac
#endif
#include "safecalls.h"
#include "sim.h"
using namespace SafeCalls;


// Constructor
Sim::Sim( Options* initOptions )
{
   // Copy constructor arguments
   o = initOptions;

   // Initialize the Sim
   initializeEngine();
   initializeIO();
}


// Set up the random number generator, the world
// data structure, the periodicTable, the rxnTable,
// and initizalize the Atoms
void
Sim::initializeEngine()
{
   static bool initialized = false;
   if( !initialized )
   {
      initialized = true;

      // Set up the world
      itersCompleted = 0;
      world = safeNew( Atom*[ o->worldX * o->worldY ] );
      claimed = safeNew( uint8_t[ o->worldX * o->worldY ] );
      positions = safeNew( unsigned int[ o->worldX * o->worldY ] );
      for( unsigned int i = 0; i < MAX_ELES_NOT_INCLUDING_SOLVENT; i++ )
         maxPositions[ i ] = (o->worldX * o->worldY) / MAX_ELES_NOT_INCLUDING_SOLVENT;
      for( unsigned int i = 0; i < (o->worldX * o->worldY) % MAX_ELES_NOT_INCLUDING_SOLVENT; i++ )
         maxPositions[ i ]++;
      for( unsigned int i = 0; i < MAX_ELES_NOT_INCLUDING_SOLVENT; i++ )
         positionSetReserved[ i ] = false;

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

      // Create Solvent Element
      Element* tempEle;
      tempEle = safeNew( Element( "Solvent", '*', "white", 0.0 ) );
      periodicTable[ "Solvent" ] = tempEle;

      // Load periodicTable, rxnTable, and extinctionTypes if available
      elesLoaded = false;
      rxnsLoaded = false;
      extinctsLoaded = false;
      loadChemistry();

      // Open files after load file has been successfully read
      // in case the load file is also the default config file
      o->openFiles();

      // Set up default periodicTable if one was not loaded
      if( !elesLoaded )
      {
         tempEle = safeNew( Element( "A", 'A', "teal", 0.25 ) );
         reservePositionSet( tempEle );
         periodicTable[ tempEle->getName() ] = tempEle;

         tempEle = safeNew( Element( "B", 'B', "hotpink", 0.24 ) );
         reservePositionSet( tempEle );
         periodicTable[ tempEle->getName() ] = tempEle;

         tempEle = safeNew( Element( "C", 'C', "darkorange", 0.02 ) );
         reservePositionSet( tempEle );
         periodicTable[ tempEle->getName() ] = tempEle;

         tempEle = safeNew( Element( "D", 'D', "yellow", 0.01 ) );
         reservePositionSet( tempEle );
         periodicTable[ tempEle->getName() ] = tempEle;
      }

      // Set up default rxnTable if one was not loaded
      if( !elesLoaded && !rxnsLoaded )
      {
         Reaction* tempRxn;
         tempRxn = safeNew( Reaction( ev(2,"A","B"), ev(2,"C","D"), 0.5 ) );
         rxnTable.insert( std::pair<int,Reaction*>( tempRxn->getKey(), tempRxn ) );
      }

      // Set up default extinctionTypes if one was not loaded
      if( !elesLoaded && !extinctsLoaded )
      {
         extinctionTypes.push_back( ev(1,"A") );
         extinctionTypes.push_back( ev(1,"B") );
      }

      // Initialize the random number generator
      initRNG( o->seed );

      // Initialize the positions array with a random
      // ordering of integers ranging from 0 to
      // worldX*worldY-1
      shufflePositions();

      // Fill the array of random numbers
      generateRandNums();

      // Initialize the world with Atoms
      Atom* tempAtom;
      int x, y;
      for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
      {
         Element* thisEle = i->second;
         unsigned int nAtoms = (unsigned int)((double)thisEle->getStartConc() * (double)maxPositions[ positionSets[ thisEle->getName() ] ] + 0.5);
         unsigned int setStart = 0;
         for( int i = 0; i < positionSets[ thisEle->getName() ]; i++ )
            setStart += maxPositions[ i ];
         for( unsigned int j = setStart; j < setStart + nAtoms; j++ )
         {
            x = positions[j] % o->worldX;
            y = positions[j] / o->worldX;
            tempAtom = safeNew( Atom( thisEle, x, y ) );
            world[ getWorldIndex(x,y) ] = tempAtom;
         }
      }
   }
}


// Execute one step of the simulation;
// returns true if the simulation succeeded in
// executing one step, and false if the
// simulation had already reached its
// maxIters
bool
Sim::iterate()
{
   if( itersCompleted < o->maxIters )
   {
      // Assign atoms new positions in the world
      // randomly to simulate mixing
      if( o->doShuffle )
         shuffleWorld();

      // Fill the array of random numbers with
      // new values
      generateRandNums();

      // Move atoms and handle collisions
      moveAtoms();

      // Scan the world, check for potential
      // reactions, and execute some of them
      if( o->doRxns )
         executeRxns();

      // Increment the iteration counter
      itersCompleted++;

      // Print out the progress of the simulation
      // at most once each second
      if( o->progress )
         reportProgress();

      // Take a census of the atoms in the world
      writeCensus();

      // Check to see if special conditions have
      // been met for ending the simulation early
      for( std::list<ElementVector>::iterator i = extinctionTypes.begin(); i != extinctionTypes.end(); i++ )
      {
         bool allExtinct = true;
         ElementVector thisEleVector = *(i);
         for( unsigned int j = 0; j < thisEleVector.size(); j++ )
         {
            allExtinct = (allExtinct && thisEleVector[j]->count == 0);
         }
         if( allExtinct )
         {
            end();
            break;
         }
      }

      // Sleep the simulation
      if( o->sleep != 0 )
         usleep( o->sleep * 1000 );

      return true;
   }
   else
   {
      // Finish collecting data and clean up
      cleanup();

      return false;
   }
}


// Tell the simulation it is time to end
// prematurely
void
Sim::end()
{
   o->maxIters = itersCompleted;
}


// Finish collecting data and clean up
void
Sim::cleanup()
{
   static bool finalized = false;
   if( !finalized )
   {
      finalized = true;

      // Guard against any more iterations
      o->maxIters = itersCompleted;

      // Finalize the progress indicator to accurately
      // display how many iterations were completed
      // when the simulation ended (noticeable primarily
      // when running batches)
      if( o->progress )
      {
         forceReportProgress();
      }

      // Write the simulation parameters and diffusion
      // data to file and clean up ncurses
      writeConfig();
      writeDiffusion();
      if( o->gui == Options::GUI_NCURSES )
      {
         killncurses();
      }

      // Close the output streams
      delete o->out[ Options::FILE_CONFIG ];
      delete o->out[ Options::FILE_CENSUS ];
      delete o->out[ Options::FILE_DIFFUSION ];
      delete o->out[ Options::FILE_RAND ];
   }
}


int
Sim::getItersCompleted()
{
   return itersCompleted;
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
   // Set the seed
   init_gen_rand( (uint32_t)(initSeed) );

   static bool allocated = false;
   if( !allocated )
   {
      allocated = true;

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
      randNums = (uint64_t*)malloc( bytes_to_be_allocated + 16 );
      randNums += 16 - (long int)randNums % 16;
#endif
#endif
#endif
      assert( rc == 0 );
      assert( randNums );
   }
}


// Fill the array of random numbers with
// new values
void
Sim::generateRandNums()
{
   // fill_array64 fills randNums with 64-bit ints.
   // See initRNG method for more information.
   fill_array64( (uint64_t*)(randNums), randNums_length_in_64_bit_ints );

   // Dump a few random numbers to file if this
   // is the first time the array has been filled
   static bool firstTime = true;
   if( firstTime )
   {
      firstTime = false;

      for( int i = 0; i < 10; i++ )
      {
         *(o->out[ Options::FILE_RAND ]) << randNums[i] << std::endl;
      }
   }
}


// Fill the positions array with successive
// integers ranging from 0 to worldX*worldY-1
// and then shuffle these integers
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


// Reserves one of the available sets of lattice
// positions to the passed Element
void
Sim::reservePositionSet( Element* ele )
{
   unsigned int i;
   for( i = 0; i < MAX_ELES_NOT_INCLUDING_SOLVENT; i++ )
   {
      if( !positionSetReserved[ i ] )
      {
         positionSets[ ele->getName() ] = i;
         positionSetReserved[ i ] = true;
         return;
      }
   }
   std::cout << "reservePositionSet: limit of " << MAX_ELES_NOT_INCLUDING_SOLVENT << " Elements exceeded!" << std::endl;
   exit( EXIT_FAILURE );
}


// Assign atoms new positions in the world
// randomly to simulate mixing
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
            temp[ getWorldIndex(newX,newY) ]->x = newX;
            temp[ getWorldIndex(newX,newY) ]->y = newY;
         }
      }
   }

   delete world;
   world = temp;
}


// Move Atoms in the lattice and handle
// collisions
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

            thisAtom->dx_ideal += dx;
            thisAtom->dy_ideal += dy;

            if( claimed[ getWorldIndex(x,y) ] == 1 && claimed[ getWorldIndex(x+dx,y+dy) ] == 1 )
            // Move if there are no collisions
            {
               thisAtom->dx_actual += dx;
               thisAtom->dy_actual += dy;

               thisAtom->x = ( (x+dx) + o->worldX ) % o->worldX;
               thisAtom->y = ( (y+dy) + o->worldY ) % o->worldY;

               world[ getWorldIndex(x,y) ] = NULL;
               world[ getWorldIndex(x+dx,y+dy) ] = thisAtom;

               // Mark the moved atom as processed
               claimed[ getWorldIndex(x+dx,y+dy) ] = 0;
            }
            else
            // Else increment collisions
            {
               thisAtom->collisions++;

               // Mark the unmoved atom as processed
               claimed[ getWorldIndex(x,y) ] = 0;
            }
         }
      }
   }
}


// Scan the world, check for potential
// reactions, and execute some of them
void
Sim::executeRxns()
{
   Element* solventEle = periodicTable[ "Solvent" ];
   Atom* thisAtom;
   int neighborX, neighborY;
   Atom* neighborAtom;

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
            thisAtom = safeNew( Atom( solventEle, x, y ) );
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
               break;
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
               neighborAtom = safeNew( Atom( solventEle, neighborX, neighborY ) );
            }
         }

         // Determine the appropriate Reaction
         Reaction* thisRxn;
         std::pair<ReactionMap::iterator,ReactionMap::iterator> range;
         if( neighborAtom == NULL )
         // If the reaction is first-order
            range = rxnTable.equal_range( thisAtom->getType()->getKey() );
         else
         // Else the reaction is second-order
            range = rxnTable.equal_range( thisAtom->getType()->getKey() *
                                      neighborAtom->getType()->getKey() );
         // Find the n'th Reaction with the matching set of
         // reactants, where n is a random number between 0 and
         // MAX_RXNS_PER_SET_OF_REACTANTS
         ReactionMap::iterator i = range.first;
         for( unsigned int j = 0; j < (randNums[ getWorldIndex(x,y) ] >> 3) % MAX_RXNS_PER_SET_OF_REACTANTS; j++ )
         {
            if( i != range.second )
               i++;
            else
               break;
         }
         if( i != range.second )
            thisRxn = i->second;
         else
            thisRxn = NULL;

         // Perform the reaction
         if( thisRxn != NULL &&
             (double)(randNums[ getWorldIndex(x,y) ] >> 3) /
             (double)((uint64_t)1 << (8 * sizeof(*randNums) - 3))
               < thisRxn->getProb() )
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
               thisAtom = safeNew( Atom( solventEle, x, y) );
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
                  break;
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
                  neighborAtom = safeNew( Atom( solventEle, neighborX, neighborY ) );
               }
            }

            // Determine the appropriate Reaction
            Reaction* thisRxn;
            std::pair<ReactionMap::iterator,ReactionMap::iterator> range;
            if( neighborAtom == NULL )
            // If the reaction is first-order
               range = rxnTable.equal_range( thisAtom->getType()->getKey() );
            else
            // Else the reaction is second-order
               if( claimed[ getWorldIndex(neighborX,neighborY) ] == 1 )
               // If the neighbor has not been processed yet and
               // could undergo a reaction
                  range = rxnTable.equal_range( thisAtom->getType()->getKey() *
                                            neighborAtom->getType()->getKey() );
            // Find the n'th Reaction with the matching set of
            // reactants, where n is a random number between 0 and
            // MAX_RXNS_PER_SET_OF_REACTANTS
            ReactionMap::iterator i = range.first;
            for( unsigned int j = 0; j < (randNums[ getWorldIndex(x,y) ] >> 3) % MAX_RXNS_PER_SET_OF_REACTANTS; j++ )
            {
               if( i != range.second )
                  i++;
               else
                  break;
            }
            if( i != range.second )
               thisRxn = i->second;
            else
               thisRxn = NULL;

            // Perform the reaction
            if( thisRxn != NULL &&
                (double)(randNums[ getWorldIndex(x,y) ] >> 3) /
                (double)((uint64_t)1 << (8 * sizeof(*randNums) - 3))
                  < thisRxn->getProb() )
            // If the reactant have enough energy
            {
               if( neighborAtom == NULL )
               // If the reaction is first-order
               {
                  // Execute the reaction
                  thisAtom->setType( thisRxn->getProducts()[0] );
                  world[ getWorldIndex(x,y) ] = thisAtom;

                  // Mark the atom as having already reacted
                  claimed[ getWorldIndex(x,y) ] = 0;
               }
               else
               // Else the reaction is second-order
               {
                  // Execute the reaction
                  thisAtom->setType( thisRxn->getProducts()[0] );
                  neighborAtom->setType( thisRxn->getProducts()[1] );
                  world[ getWorldIndex(x,y) ] = thisAtom;
                  world[ getWorldIndex(neighborX,neighborY) ] = neighborAtom;

                  // Propogate tracking
                  thisAtom->setTracked(     thisAtom->isTracked() || neighborAtom->isTracked() );
                  neighborAtom->setTracked( thisAtom->isTracked() || neighborAtom->isTracked() );

                  // Mark the reaction participants as having already reacted
                  claimed[ getWorldIndex(x,y) ] = 0;
                  claimed[ getWorldIndex(neighborX,neighborY) ] = 0;
               }
            }

            // Delete any solvent atoms that are newly created or never reacted
            if( thisAtom->getType() == solventEle )
            {
               delete thisAtom;
               thisAtom = NULL;
               world[ getWorldIndex(x,y) ] = NULL;
            }
            if( neighborAtom != NULL && neighborAtom->getType() == solventEle )
            {
               delete neighborAtom;
               neighborAtom = NULL;
               world[ getWorldIndex(neighborX,neighborY) ] = NULL;
            }
         }
      }
   }
}

