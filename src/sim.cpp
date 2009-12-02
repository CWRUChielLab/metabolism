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
#include <ctime>
#include <fstream>
#include <map>
#include <ncurses.h>
#include <SFMT.h>
#include <vector>
#include "sim.h"


// Constructor
Sim::Sim()
{
   // Nothing to do yet
}


void
Sim::initialize()
{
   // Setup the world
   worldX = 16;
   worldY = 16;
   world = new Atom*[ worldX * worldY ];
   claimed = new uint8_t[ worldX * worldY ];

   // Initialize the world array to NULL
   for( int i = 0; i < worldX*worldY; i++ )
   {
      world[i] = NULL;
   }

   // Initialize the periodicTable
   Element* tempEle;
   for( char c = 'A'; c <= 'H'; c++ )
   {
      std::string s(1,c);
      tempEle = new Element( s, 0, 0 );
      periodicTable[ s ] = tempEle;
   }

   // Initialize the rxnTable
   Reaction* tempRxn;

   tempRxn = new Reaction( ev(2,"A","B"), ev(2,"C","D"), 0.02 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   tempRxn = new Reaction( ev(2,"E","F"), ev(2,"G","H"), 0.03 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   tempRxn = new Reaction( ev(2,"A","A","D"), ev(1,"H"), 0.1 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   tempRxn = new Reaction( ev(1,"B"), ev(3,"F","D","H"), 1 );
   rxnTable[ tempRxn->getKey() ] = tempRxn;

   // Initialize the random number generator
   int seed = time(NULL);
   printw( "seed = %d\n", seed );
   initRNG( seed );

   // Initialize the positions array with a random
   // ordering of integers ranging from 0 to
   // worldX*worldY-1
   shufflePositions();

   // Initialize the world with random atoms
   Atom* tempAtom;
   int x, y, incr;
   int maxAtoms = worldX * worldY / 4;
   int minAtoms = std::min( 3, maxAtoms );
   int range = maxAtoms - minAtoms + 1;
   int atomCount = gen_rand32() % range + minAtoms;
   for( int i = 0; i < atomCount; i++ )
   {
      x = positions[i] % worldX;
      y = positions[i] / worldX;

      ElementMap::iterator iter = periodicTable.begin();
      incr = gen_rand32() % periodicTable.size();

      for( int j = 0; j < incr; j++ )
      {
         iter++;
      }

      tempAtom = new Atom( iter->second, x, y );
      world[ getWorldIndex(x,y) ] = tempAtom;
   }

   // Initialize the RNG again, since we called
   // gen_rand32() above, but with a different
   // seed
   initRNG( seed + 42 );
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
   for( int y = 0; y < worldY; y++ )
   {
      for( int x = 0; x < worldX; x++ )
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
void
Sim::iterate()
{
   generateRandNums();
   moveAtoms();
}


// Initialized the random number generator
void
Sim::initRNG( int seed )
{
   static int allocated = 0;

   // Set the seed
   init_gen_rand( (uint32_t)(seed) );

   if( !allocated )
   {
      // The array randNums will be treated by the
      // RNG as an array of 64-bit ints.  The length
      // of the array (in 64-bit ints) must be a
      // multiple of 2 and the array must be at least
      // get_min_array_size64() 64-bit ints long.
      // With MEXP = 132049, get_min_array_size64() =
      // 2*((MEXP/128)+1) = 2064.
      int min_rand_nums_needed = worldX * worldY;
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


// Get a new set of randNums
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
// and then shuffle these integers
void
Sim::shufflePositions()
{
   static int initialized = 0;
   unsigned int i, highest, lowest, range, rand, temp;

   if( !initialized )
   {
      positions = new unsigned int[ worldX * worldY ];
      initialized = 1;
   }

   // Fill the positions array with successive integers
   for( i = 0; i < (unsigned int)(worldX * worldY); i++ )
   {
      positions[ i ] = i;
   }

   // Shuffle the positions array
   highest = worldX * worldY - 1;
   for( i = 0; i < highest; i++ )
   {
      lowest = i + 1;
      range = highest - lowest + 1;
      rand = ( gen_rand32() % range ) + lowest;
      temp = positions[ i ];
      positions[ i ] = positions[ rand ];
      positions[ rand ] = temp;
   }
}


void
Sim::moveAtoms()
{
   // Clear all claimed flags
   std::memset( claimed, 0, worldX * worldY );
   
   // Stake claims
   for( int y = 0; y < worldY; y++ )
   {
      for( int x = 0; x < worldX; x++ )
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
   for( int y = 0; y < worldY; y++ )
   {
      for( int x = 0; x < worldX; x++ )
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
// of the world and writes it to file.  Passing
// a negative value for iter will close the file.
void
Sim::takeCensus( int iter )
{
   static int initialized = 0;
   static std::ofstream censusFile;
   int atomCount = 0;

   if( iter < 0 )
   {
      censusFile.close();
      initialized = 0;
      return;
   }

   if( !initialized )
   {
      initialized = 1;
      censusFile.open( "census.out" );
      censusFile << "iter atomCount" << std::endl;
   }

   for( int x = 0; x < worldX; x++ )
   {
      for( int y = 0; y < worldY; y++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         {
            atomCount++;
         }
      }
   }
   censusFile << iter << " " << atomCount << std::endl;
}


// Writes important information about the final
// state of the world to file.
void
Sim::finalizeAtoms()
{
   takeCensus(-1);
   std::ofstream finalFile;
   finalFile.open( "diffusion.out" );
   finalFile << "type dx dy\n";
   for( int x = 0; x < worldX; x++ )
   {
      for( int y = 0; y < worldY; y++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         {
            Atom* thisAtom = world[ getWorldIndex(x,y) ];
            finalFile << thisAtom->getType()->getName() << " " << thisAtom->getDx() << " " << thisAtom->getDy() << std::endl;
         }
      }
   }
   finalFile.close();
}


// Uses the randNum at an Atom's current position
// to determine the direction it moves next
int
Sim::dx( int x, int y )
{
   int threeRandBits = randNums[ getWorldIndex(x,y) ] & 0x7;
   switch( threeRandBits )
   {
      case 0:
         return 0;  /* N  */
      case 1:
         return 1;  /* NE */
      case 2:
         return 1;  /* E  */
      case 3:
         return 1;  /* SE */
      case 4:
         return 0;  /* S  */
      case 5:
         return -1; /* SW */
      case 6:
         return -1; /* W  */
      case 7:
         return -1; /* NW */
      default:
         assert( 0 );
   }
}


// Uses the randNum at an Atom's current position
// to determine the direction it moves next
int
Sim::dy( int x, int y )
{
   int threeRandBits = randNums[ getWorldIndex(x,y) ] & 0x7;
   switch( threeRandBits )
   {
      case 0:
         return -1; /* N  */
      case 1:
         return -1; /* NE */
      case 2:
         return 0;  /* E  */
      case 3:
         return 1;  /* SE */
      case 4:
         return 1;  /* S  */
      case 5:
         return 1;  /* SW */
      case 6:
         return 0;  /* W  */
      case 7:
         return -1; /* NW */
      default:
         assert( 0 );
   }
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
   int wrappedX = ( x + worldX ) % worldX;
   int wrappedY = ( y + worldY ) % worldY;
   return ( wrappedX + wrappedY * worldX );
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
      atom1->setX( ( x2 + worldX ) % worldX );
      atom1->setY( ( y2 + worldY ) % worldY );
   }
   if( atom2 != NULL )
   {
      atom2->setDx( atom2->getDx() + ( x1 - x2 ) );
      atom2->setDy( atom2->getDy() + ( y1 - y2 ) );
      atom2->setX( ( x1 + worldX ) % worldX );
      atom2->setY( ( y1 + worldY ) % worldY );
   }
   world[ getWorldIndex( x1, y1 ) ] = atom2;
   world[ getWorldIndex( x2, y2 ) ] = atom1;
}

