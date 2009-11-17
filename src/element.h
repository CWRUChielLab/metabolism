/* element.h
 */

#ifndef ELEMENT_H
#define ELEMENT_H 

class Element
{
   public:
      // Static functions
      static void addElement( const char *initName, int initColor, int initCharge = 0 );
      static void initList();
      static void printList();
      static void countElements();
      
      // Get and set functions
      int getKey();
      const char* getName();
      void setName( const char *newName );
      int getColor();
      void setColor( int newColor );
      int getCharge();
      void setCharge( int newCharge );
   
   private:
      // Constructor
      Element( const char *initName, int initColor, int initCharge = 0 );

      // Element attributes
      int key;
      const char *name;
      int color;
      int charge;

      // Static members
      static Element **list;
      static int lastPrime;
};

#endif /* ELEMENT_H */
