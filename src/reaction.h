/* reaction.h
 */

#ifndef REACTION_H
#define REACTION_H 

#include <vector>
#include "element.h"

class Reaction
{
   public:
      // Constructors
      Reaction( std::vector<Element*> initReactants, std::vector<Element*> initFirstProducts, double initFirstProb );
      Reaction( std::vector<Element*> initReactants, std::vector<Element*> initFirstProducts, double initFirstProb, std::vector<Element*> initSecondProducts, double initSecondProb );
      
      // Get and set functions
      int getKey();
      std::vector<Element*> getReactants();
      std::vector<Element*> getFirstProducts();
      std::vector<Element*> getSecondProducts();
      void setFirstProducts( std::vector<Element*> newFirstProducts );
      void setSecondProducts( std::vector<Element*> newSecondProducts );
      double getFirstProb();
      double getSecondProb();
      void setFirstProb( double newFirstProb );
      void setSecondProb( double newSecondProb );

   private:
      // Reaction attributes
      int key;
      std::vector<Element*> reactants;
      std::vector<Element*> firstProducts;
      std::vector<Element*> secondProducts;
      double firstProb;
      double secondProb;
};

#endif /* REACTION_H */
