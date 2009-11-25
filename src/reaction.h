/* reaction.h
 */

#ifndef REACTION_H
#define REACTION_H 

#include <map>
#include <vector>
#include "element.h"

class Reaction
{
   typedef std::map<int,Reaction*> ReactionMap;
   typedef std::vector<Element*> ElementVector;

   public:
      // Constructor
      Reaction( ElementVector initReactants, ElementVector initProducts, double initRate );
      
      // Static functions
      static Reaction* getReaction( int key );
      static void initList();
      static void printList();

      // Get and set functions
      int getKey();
      ElementVector getReactants();
      int getNumReactants();
      ElementVector getProducts();
      int getNumProducts();
      void setProducts( ElementVector newProducts );
      double getRate();
      void setRate( double newRate );

   private:
      // Reaction attributes
      int key;
      ElementVector reactants;
      ElementVector products;
      double rate;

      // Static members
      static ReactionMap list;
};

#endif /* REACTION_H */
