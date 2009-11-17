/* element.h
 */

#ifndef ELEMENT_H
#define ELEMENT_H 

class Element
{
   public:
      // Static Functions
      static void newElement( const char *initName, int initColor, int initCharge = 0 );
      static void initList();
      static void printList();
      static void countElements();
      
      // Get and Set Functions
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
      static int lastPrime;

      // Element Attributes
      int key;
      const char *name;
      int color;
      int charge;

      // Table of Elements
      static Element **list;
};

#endif /* EMELEMT_H */
