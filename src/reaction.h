/* reaction.h
 */

#ifndef REACTION_H
#define REACTION_H 

class Reaction
{
   public:
      // Constructor
      Reaction( int* initReactants, int initNumReactants, int* initProducts, int initNumProducts, double initRate );
      
      // Static functions
      static Reaction* getReaction( int key );
      static void initList();
      static void printList();

      // Get and set functions
      int getKey();
      int* getReactants();
      int getNumReactants();
      int* getProducts();
      int getNumProducts();
      void setProducts( int* newProducts, int newNumProducts );
      double getRate();
      void setRate( double newRate );

   private:
      // Reaction attributes
      int key;
      int *reactants;
      int numReactants;
      int *products;
      int numProducts;
      double rate;

      // Static members
      static Reaction **list;
};

#endif /* REACTION_H */
