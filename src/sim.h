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
      Sim( Options* initOptions );

      // Public engine methods
      bool iterate();
      void end();
      int getItersCompleted();

      // Public I/O methods
      void reportProgress();
      void forceReportProgress();
      void writeCensus();
      void printWorld();

      Atom** world;
      ElementMap periodicTable;
      ReactionMap rxnTable;
      int getWorldIndex( int x, int y );

   private:
      // Sim attributes
      Options* o;
      int itersCompleted;

      bool elesLoaded;
      bool rxnsLoaded;
      bool initsLoaded;

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
      void cleanup();
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
      void killncurses();
      void loadChemistry();
      void writeConfig();
      void writeDiffusion();
      void printEles( std::ostream* out );
      void printRxns( std::ostream* out );
      void printInits( std::ostream* out );
};

#endif /* SIM_H */
