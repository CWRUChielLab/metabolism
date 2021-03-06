/* sim.h
 */

#ifndef SIM_H
#define SIM_H 

#ifdef HAVE_QT
#include <QTemporaryFile>
#endif
#include <list>
#include <map>
#include <ostream>
#include <stdint.h>
#include <vector>
#include "atom.h"
#include "element.h"
#include "options.h"
#include "reaction.h"

typedef std::map<std::string,Element*> ElementMap;
typedef std::multimap<int,Reaction*> ReactionMap;
typedef std::map<std::string,int> StringCounter;
typedef std::vector<Element*> ElementVector;

class Sim
{
   public:
      // Constructor
      Sim( Options* initOptions );

      // Public engine methods
      void destroyWorld();
      void buildWorld();
      bool iterate();
      void end();
      void cleanup();
      int getItersCompleted();

      // Public I/O methods
      void reportProgress();
      void forceProgressReport();
      void finishProgressReport();
      void writeCensus();
      void printWorld();

      Atom** world;
      ElementMap periodicTable;
      ReactionMap rxnTable;
      int getWorldIndex( int x, int y );

      // File management
      std::vector<std::ostream*> out;
#ifdef HAVE_QT
      std::vector<QTemporaryFile*> tempFiles;
#endif

      static const unsigned int MAX_RXNS_PER_SET_OF_REACTANTS = 2;
      static const unsigned int MAX_ELES_NOT_INCLUDING_SOLVENT = 8;

   private:
      // Sim attributes
      Options* o;
      int itersCompleted;

      bool elesLoaded;
      bool rxnsLoaded;
      bool extinctsLoaded;

      std::list<ElementVector> extinctionTypes;

      uint8_t* claimed;
      unsigned int* positions;
      int maxPositions[ MAX_ELES_NOT_INCLUDING_SOLVENT ];
      bool positionSetReserved[ MAX_ELES_NOT_INCLUDING_SOLVENT ];
      StringCounter positionSets;

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
      void reservePositionSet( Element* ele );
      void reservePositionSet( Element* ele, int set );
      void shuffleWorld();

      void moveAtoms();
      int* dirdx;
      int* dirdy;

      void executeRxns();

      ElementVector ev( int elementCount, ... );

      // Private I/O methods
      void initializeIO();
      void openFiles();
      void killncurses();
      void loadChemistry();
      void writeConfig();
      void writeDiffusion();
      void printEles( std::ostream* out );
      void printRxns( std::ostream* out );
      void printExtincts( std::ostream* out );
};

#endif /* SIM_H */
