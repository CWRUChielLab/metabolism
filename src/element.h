/* element.h
 */

#ifndef ELEMENT_H
#define ELEMENT_H 

#include <string>

class Element
{
   public:
      // Constructor
      Element( std::string initName, int initColor, int initCharge = 0 );

      // Get and set functions
      int getKey();
      std::string getName();
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
};

#endif /* ELEMENT_H */
