/* particle.h
 */

#ifndef PARTICLE_H
#define PARTICLE_H 

class Particle
{
   public:
      // Constuctors
      Particle( const char *initName, int initColor, int initCharge = 0 );
      
      // Get and Set Functions
      int getKey();
      const char* getName();
      void setName( const char *newName );
      int getColor();
      void setColor( int newColor );
      int getCharge();
      void setCharge( int newCharge );

   private:
      // Particle Attributes
      int key;
      const char *name;
      int color;
      int charge;
};

#endif /* PARTICLE_H */
