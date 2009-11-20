/* reaction.h
 */

#ifndef REACTION_H
#define REACTION_H 

#include "element.h"

class Reaction
{
   public:
      // Constructor
      Reaction( Element** initReactants, int initNumReactants, Element** initProducts, int initNumProducts, double initRate );
      
      // Static functions
      static Reaction* getReaction( int key );
      static void initList();
      static void printList();

      // Get and set functions
      int getKey();
      Element** getReactants();
      int getNumReactants();
      Element** getProducts();
      int getNumProducts();
      void setProducts( Element** newProducts, int newNumProducts );
      double getRate();
      void setRate( double newRate );

   private:
      // Reaction attributes
      int key;
      Element** reactants;
      int numReactants;
      Element** products;
      int numProducts;
      double rate;

      // Static members
      static Reaction** list;
};

#endif /* REACTION_H */
