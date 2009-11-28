/* sim.h
 */

#ifndef SIM_H
#define SIM_H 

#include <map>
#include <vector>
#include "atom.h"
#include "element.h"
#include "reaction.h"

typedef std::map<std::string,Element*> ElementMap;
typedef std::map<int,Reaction*> ReactionMap;
typedef std::vector<Element*> ElementVector;

class Sim
{
   public:
      // Constructor
      Sim();

      // Methods
      void initialize();
      void printElements();
      void printReactions();
      void printWorld();
      void iterate();

   private:
      // Sim attributes
      ElementMap periodicTable;
      ReactionMap rxnTable;
      Atom** world;
      int* claimed;
      int worldX;
      int worldY;
      
      // SFMT stuff
      unsigned long int direction_sz64;
      unsigned char* direction;
      int dx( int x, int y );
      int dy( int x, int y );

      // Private methods
      void generateRandNums();
      void moveAtoms();
      ElementVector ev( int elementCount, ... );
      int getWorldIndex( int x, int y );
      void swapAtoms( int x1, int y1, int x2, int y2 );
};

#endif /* SIM_H */
