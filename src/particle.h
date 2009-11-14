/* particle.h
 */

#ifndef PARTICLE_H
#define PARTICLE_H 

class Particle
{
   public:
      // Constuctors
      Particle( char *initName, int initColor, int initCharge = 0 );
      
      // Get and Set Functions
      char *getName();
      void  setName( char *newName );
      int   getColor();
      void  setColor( int newColor );
      int   getCharge();
      void  setCharge( int newCharge );

   private:
      // Particle Attributes
      char *name;
      int   color;
      int   charge;
};

#endif /* PARTICLE_H */
