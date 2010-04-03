/* reaction.h
 */

#ifndef REACTION_H
#define REACTION_H 

#include <vector>
#include "element.h"

class Reaction
{
   public:
      // Constructor
      Reaction( std::vector<Element*> initReactants, std::vector<Element*> initProducts, double initProb );
      
      // Get and set functions
      int getKey();
      std::vector<Element*> getReactants();
      std::vector<Element*> getProducts();
      void setProducts( std::vector<Element*> newProducts );
      double getProb();
      void setProb( double newProb );

   private:
      // Reaction attributes
      int key;
      std::vector<Element*> reactants;
      std::vector<Element*> products;
      double prob;
};

#endif /* REACTION_H */
