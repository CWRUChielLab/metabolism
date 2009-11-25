/* element.h
 */

#ifndef ELEMENT_H
#define ELEMENT_H 

#include <map>
#include <string>

class Element
{
   typedef std::map<int,Element*> ElementMap;

   public:
      // Constructor
      Element( std::string initName, int initColor, int initCharge = 0 );

      // Static functions
      static Element* getElement( int key );
      static void initList();
      static void printList();
      
      // Get and set functions
      int getKey();
      std::string getName();
      void setName( std::string newName );
      int getColor();
      void setColor( int newColor );
      int getCharge();
      void setCharge( int newCharge );
   
   private:
      // Element attributes
      int key;
      std::string name;
      int color;
      int charge;

      // Static members
      static ElementMap list;
};

#endif /* ELEMENT_H */
