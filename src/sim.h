/* sim.h
 */

#ifndef SIM_H
#define SIM_H 

#include <map>
#include <vector>
#include "atom.h"
#include "element.h"
#include "reaction.h"

typedef std::map<int,Element*> ElementMap;
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
      void moveAtoms();

   private:
      // Sim attributes
      ElementMap periodicTable;
      ReactionMap rxnTable;
      Atom** world;
      int* claimed;
      int worldX;
      int worldY;

      // Private methods
      int getWorldIndex( int x, int y );
};

#endif /* SIM_H */
