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
#include <map>
#include <ncurses.h>
#include <SFMT.h>
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
   for( char c = 'A'; c <= 'H'; c++ )
   {
      std::string s(1,c);
      tempEle = safeNew( Element( s, 0, 0 ) );
      periodicTable[ s ] = tempEle;
   }

   // Initialize the rxnTable
   Reaction* tempRxn;

   tempRxn = safeNew( Reaction( ev(2,"A","B"), ev(2,"C","D"), 0.02 ) );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   tempRxn = safeNew( Reaction( ev(2,"E","F"), ev(2,"G","H"), 0.03 ) );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   tempRxn = safeNew( Reaction( ev(2,"A","A","D"), ev(1,"H"), 0.1 ) );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   tempRxn = safeNew( Reaction( ev(1,"B"), ev(3,"F","D","H"), 1 ) );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   // Initialize the world with random atoms
   Atom* tempAtom;
   int x, y, incr;
   o->atomCount = std::min( o->atomCount, o->worldX * o->worldY );
   for( int i = 0; i < o->atomCount; i++ )
   {
      x = positions[i] % o->worldX;
      y = positions[i] / o->worldX;

      ElementMap::iterator ele = periodicTable.begin();
      incr = gen_rand32() % periodicTable.size();

      for( int j = 0; j < incr; j++ )
      {
         ele++;
      }

      tempAtom = safeNew( Atom( ele->second, x, y ) );
      world[ getWorldIndex(x,y) ] = tempAtom;
   }

   // Initialize the RNG again, since we called
   // gen_rand32() above, but with a different
   // seed
   initRNG( o->seed + 42 );
}


int
Sim::getCurrentIter()
{
   return currentIter;
}


// Output all important experimental parameters
void
Sim::writeConfig()
{
   static std::ofstream configFile;
   static std::ifstream gitFile;

   // Write parameters to file
   configFile.open( o->configFile.c_str() );
   configFile << GIT_TAG << std::endl;
   configFile << "seed: " << o->seed << std::endl;
   configFile << "maxIters: " << o->maxIters << std::endl;
   configFile << "worldX: " << o->worldX << std::endl;
   configFile << "worldY: " << o->worldY << std::endl;
   configFile << "atomCount: " << o->atomCount << std::endl;
   configFile.close();
}


void
Sim::printElements()
{
   printw( "There are %d elements.\n", periodicTable.size() );
   for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
   {
      Element* ele = i->second;
      printw( "periodicTable[\"%s\"] has key:\t%d\n", ele->getName().c_str(), periodicTable[ele->getName()]->getKey() );
   }
   refresh();
}


void
Sim::printReactions()
{
   printw( "There are %d reactions.\n", rxnTable.size() );

   // Loop through the rxnTable, printing each Reaction
   for( ReactionMap::iterator i = rxnTable.begin(); i != rxnTable.end(); i++ )
   {
      Reaction* rxn = i->second;

      // Count up the number of each type of reactant and product
      std::map<std::string,int> reactantCount;
      std::map<std::string,int> productCount;
      for( unsigned int j = 0; j < rxn->getReactants().size(); j++ )
      {
         reactantCount[ rxn->getReactants()[j]->getName() ]++;
      }
      for( unsigned int j = 0; j < rxn->getProducts().size(); j++ )
      {
         productCount[ rxn->getProducts()[j]->getName() ]++;
      }

      printw( "Key: %d  \t", rxn->getKey() );

      // Print the reactants, grouping copies of one type together
      // with stoichiometric coefficients
      for( std::map<std::string,int>::iterator j = reactantCount.begin(); j != reactantCount.end(); j++ )
      {
         int coefficient = j->second;
         if( coefficient == 1 )
         {
            if( j == reactantCount.begin() )
            {
               printw( "%s", j->first.c_str() );
            }
            else
            {
               printw( " + %s", j->first.c_str() );
            }
         }
         else
         {
            if( j == reactantCount.begin() )
            {
               printw( "%d%s", j->second, j->first.c_str() );
            }
            else
            {
               printw( " + %d%s", j->second, j->first.c_str() );
            }
         }
      }

      printw( " -> " );

      // Print the products, grouping copies of one type together
      // with stoichiometric coefficients
      for( std::map<std::string,int>::iterator j = productCount.begin(); j != productCount.end(); j++ )
      {
         int coefficient = j->second;
         if( coefficient == 1 )
         {
            if( j == productCount.begin() )
            {
               printw( "%s", j->first.c_str() );
            }
            else
            {
               printw( " + %s", j->first.c_str() );
            }
         }
         else
         {
            if( j == productCount.begin() )
            {
               printw( "%d%s", j->second, j->first.c_str() );
            }
            else
            {
               printw( " + %d%s", j->second, j->first.c_str() );
            }
         }
      }

      printw( "\n" );
   }
   refresh();
}


void
Sim::printWorld()
{
   for( int y = 0; y < o->worldY; y++ )
   {
      for( int x = 0; x < o->worldX; x++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         {
            printw( "%s ", world[ getWorldIndex(x,y) ]->getType()->getName().c_str() );
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
}


// Execute one step of the simulation
// Returns 1 if the simulation successfully
// and 0 if the simulation has reached its
// maxIters
int
Sim::iterate()
{
   if( currentIter < o->maxIters )
   {
      generateRandNums();
      moveAtoms();
      currentIter++;
      return 1;
   }
   else
   {
      return 0;
   }
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
      int min_64_bit_ints_needed = ceil( min_bytes_needed / 8.0 );

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
      rc = posix_memalign( (void**)&randNums, getpagesize(), bytes_to_be_allocated );
      assert( rc == 0 );
      assert( randNums );

      allocated = 1;
   }
}


// Get a new set of randNums.
// Note: initRNG must always be called
// between calls of generateRandNums and
// shufflePositions.
void
Sim::generateRandNums()
{
   // fill_array64 fills randNums with 64-bit
   // ints.  See initRNG method for more
   // information.
   fill_array64( (uint64_t*)(randNums), randNums_length_in_64_bit_ints );
}


// Fill the positions array with successive
// integers ranging from 0 to worldX*worldY-1
// and then shuffle these integers.
// Note: initRNG must always be called
// between calls of generateRandNums and
// shufflePositions.
void
Sim::shufflePositions()
{
   unsigned int i, highest, lowest, range, rand, temp;

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
      rand = ( gen_rand32() % range ) + lowest;
      temp = positions[i];
      positions[i] = positions[rand];
      positions[rand] = temp;
   }
}


void
Sim::moveAtoms()
{
   // Clear all claimed flags
   std::memset( claimed, 0, o->worldX * o->worldY );
   
   // Stake claims
   for( int y = 0; y < o->worldY; y++ )
   {
      for( int x = 0; x < o->worldX; x++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         {
            int newX = x + dx(x,y);
            int newY = y + dy(x,y);
            claimed[ getWorldIndex(x,y) ]++;
            claimed[ getWorldIndex(newX,newY) ]++;
         }
      }
   }

   // Move if there are no collisions
   for( int y = 0; y < o->worldY; y++ )
   {
      for( int x = 0; x < o->worldX; x++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         {
            int newX = x + dx(x,y);
            int newY = y + dy(x,y);
            if( claimed[ getWorldIndex(x,y) ] == 1 && claimed[ getWorldIndex(newX,newY) ] == 1 )
            {
               swapAtoms( x, y, newX, newY );
               claimed[ getWorldIndex(x,y) ]++;
               claimed[ getWorldIndex(newX,newY) ]++;
            }
         }
      }
   }
}


// Records important information about the state
// of the world and writes it to file.
void
Sim::takeCensus()
{
   static int initialized = 0;
   static std::ofstream censusFile;
   int currentAtomCount = 0;

   if( !initialized )
   {
      initialized = 1;
      censusFile.open( o->censusFile.c_str() );
      censusFile << "iter atoms" << std::endl;
   }

   for( int x = 0; x < o->worldX; x++ )
   {
      for( int y = 0; y < o->worldY; y++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         {
            currentAtomCount++;
         }
      }
   }
   censusFile << currentIter << " " << currentAtomCount << std::endl;
}


// Writes important information about the state
// of the world to file.  To be called when the
// simulation ends.
void
Sim::writeAtoms()
{
   std::ofstream diffusionFile;
   diffusionFile.open( o->diffusionFile.c_str() );
   diffusionFile << "type dx dy\n";
   for( int x = 0; x < o->worldX; x++ )
   {
      for( int y = 0; y < o->worldY; y++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         {
            Atom* thisAtom = world[ getWorldIndex(x,y) ];
            diffusionFile << thisAtom->getType()->getName() << " " << thisAtom->getDx() << " " << thisAtom->getDy() << std::endl;
         }
      }
   }
   diffusionFile.close();
}


// Uses the randNum at an Atom's current position
// to determine the direction it moves next
int
Sim::dx( int x, int y )
{
   // Horizontal dimension deltas for directions
   // in the following order:
   // N, NE, E, SE, S, SW, W, NW
   static int directions[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
   int threeRandBits = randNums[ getWorldIndex(x,y) ] & 0x7;
   return directions[ threeRandBits ];
}


// Uses the randNum at an Atom's current position
// to determine the direction it moves next
int
Sim::dy( int x, int y )
{
   // Vertical dimension deltas for directions
   // in the following order:
   // N, NE, E, SE, S, SW, W, NW
   static int directions[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
   int threeRandBits = randNums[ getWorldIndex(x,y) ] & 0x7;
   return directions[ threeRandBits ];
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


// Switches the Atoms in positions (x1,y1) and
// (x2,y2) and handles updating x, y, dx, and dy
// values on both atoms.  Moves a single atom if
// one of the two positions is empty
void
Sim::swapAtoms( int x1, int y1, int x2, int y2 )
{
   Atom* atom1 = world[ getWorldIndex( x1, y1 ) ];
   Atom* atom2 = world[ getWorldIndex( x2, y2 ) ];
   if( atom1 != NULL )
   {
      atom1->setDx( atom1->getDx() + ( x2 - x1 ) );
      atom1->setDy( atom1->getDy() + ( y2 - y1 ) );
      atom1->setX( ( x2 + o->worldX ) % o->worldX );
      atom1->setY( ( y2 + o->worldY ) % o->worldY );
   }
   if( atom2 != NULL )
   {
      atom2->setDx( atom2->getDx() + ( x1 - x2 ) );
      atom2->setDy( atom2->getDy() + ( y1 - y2 ) );
      atom2->setX( ( x1 + o->worldX ) % o->worldX );
      atom2->setY( ( y1 + o->worldY ) % o->worldY );
   }
   world[ getWorldIndex( x1, y1 ) ] = atom2;
   world[ getWorldIndex( x2, y2 ) ] = atom1;
}

