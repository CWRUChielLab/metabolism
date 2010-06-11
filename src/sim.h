/* sim.h
 */

#ifndef SIM_H
#define SIM_H 

#include <map>
#include <ostream>
#include <stdint.h>
#include <vector>
#include "atom.h"
#include "element.h"
#include "options.h"
#include "reaction.h"

typedef std::map<std::string,Element*> ElementMap;
typedef std::map<int,Reaction*> ReactionMap;
typedef std::map<std::string,int> StringCounter;
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
      void finalizeIO();
      void reportProgress();
      void forceReportProgress();
      void writeCensus();
      void printWorld();

      Atom** world;
      ElementMap periodicTable;
      int getWorldIndex( int x, int y );

   private:
      // Sim attributes
      Options* o;
      int currentIter;

      int elesLoaded;
      int rxnsLoaded;
      int initsLoaded;

      ReactionMap rxnTable;
      ElementVector initialTypes;

      uint8_t* claimed;
      unsigned int* positions;

      // I/O attributes
      int scrX;
      int scrY;
      int lastProgressUpdate;
      
      // RNG parameters
      int randNums_length_in_64_bit_ints;
      uint64_t* randNums;

      // Private engine methods
      void initializeEngine();
      void initRNG( int initSeed );
      void generateRandNums();
      void shufflePositions();
      void shuffleWorld();

      void moveAtoms();
      int* dirdx;
      int* dirdy;

      void executeRxns();

      ElementVector ev( int elementCount, ... );

      // Private I/O methods
      void initializeIO();
      void loadChemistry();
      void writeConfig();
      void writeDiffusion();
      void printEles( std::ostream* out );
      void printRxns( std::ostream* out );
      void printInits( std::ostream* out );
};

#endif /* SIM_H */
