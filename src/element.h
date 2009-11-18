/* element.h
 */

#ifndef ELEMENT_H
#define ELEMENT_H 

class Element
{
   public:
      // Constructor
      Element( const char* initName, int initColor, int initCharge = 0 );

      // Static functions
      static Element* getElement( int key );
      static void initList();
      static void printList();
      
      // Get and set functions
      int getKey();
      const char* getName();
      void setName( const char *newName );
      int getColor();
      void setColor( int newColor );
      int getCharge();
      void setCharge( int newCharge );
   
   private:
      // Element attributes
      int key;
      const char *name;
      int color;
      int charge;

      // Static members
      static Element **list;
};

#endif /* ELEMENT_H */
