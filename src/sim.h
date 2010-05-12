/* sim.h
 */

#ifndef SIM_H
#define SIM_H 

#include <map>
#include <stdint.h>
#include <vector>
#include "atom.h"
#include "element.h"
#include "options.h"
#include "reaction.h"

typedef std::map<std::string,Element*> ElementMap;
typedef std::map<int,Reaction*> ReactionMap;
typedef std::vector<Element*> ElementVector;

class Sim
{
   public:
      // Constructor
      Sim( Options* newOptions );

      // Public engine methods
      int iterate();
      int getCurrentIter();

      // Public I/O methods
      void writeConfig();
      void takeCensus();
      void dumpAtoms();
      void printWorld();
      void printElements();
      void printReactions();

   private:
      // Sim attributes
      Options* o;
      int currentIter;

      int elesLoaded;
      int rxnsLoaded;
      int initsLoaded;

      ElementMap periodicTable;
      ReactionMap rxnTable;
      ElementVector initialTypes;

      Atom** world;
      uint8_t* claimed;
      unsigned int* positions;
      
      // RNG parameters
      int randNums_length_in_64_bit_ints;
      uint64_t* randNums;

      // Private engine methods
      void initRNG( int initSeed );
      void generateRandNums();
      void shufflePositions();
      void shuffleWorld();

      void moveAtoms();
      int* dirdx;
      int* dirdy;

      void executeRxns();

      ElementVector ev( int elementCount, ... );
      int getWorldIndex( int x, int y );

      // Private I/O methods
      void loadChemistry();
};

#endif /* SIM_H */
