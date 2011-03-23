/* plot.cpp
 */

#ifdef HAVE_QT

#include "plot.h"


// Constructor
Plot::Plot( Options* initOptions, Sim* initSim, QWidget* parent )
   : QwtPlot( parent )
{
   // Copy constructor arguments
   o = initOptions;
   sim = initSim;

   // Set up the plot
   setTitle( "" );
   setAxisTitle( 0, "Density" );
   setAxisTitle( 2, "Time (iters)" );
}


// Update the data arrays and redraw the plot
void
Plot::update()
{
   static bool initialized = false;

   if( !initialized )
   {
      // Create and initialize the array of x-coordinate values
      arrayLength = o->maxIters + 1;
      iterData = new double [ arrayLength ];
      iterData[ sim->getItersCompleted() ] = sim->getItersCompleted();

      // For each Element...
      for( ElementMap::iterator i = sim->periodicTable.begin(); i != sim->periodicTable.end(); i++ )
      {  
         Element* ele = i->second;
         if( ele != sim->periodicTable[ "Solvent" ] )
         {
            // Create and initialize the array of y-coordinate values for this
            // Element
            density[ ele->getName() ] = new double [ arrayLength ];
            density[ ele->getName() ][ sim->getItersCompleted() ] = (double)ele->count / (double)(o->worldX * o->worldY);

            // Create and initialize the curve for this Element
            curves[ ele->getName() ] = new QwtPlotCurve( ele->getName().c_str() );
            curves[ ele->getName() ]->setPen( QColor( ele->getColor().c_str() ) );
            curves[ ele->getName() ]->setData( iterData, density[ ele->getName() ], sim->getItersCompleted() + 1 );
            curves[ ele->getName() ]->attach( this );
         }
      }

      initialized = true;
   } else {
      // Grow the data arrays if necessary
      if( sim->getItersCompleted() + 1 > arrayLength )
         growArrays();

      // Update the array of x-coordinate values
      iterData[ sim->getItersCompleted() ] = sim->getItersCompleted();

      // For each Element...
      for( ElementMap::iterator i = sim->periodicTable.begin(); i != sim->periodicTable.end(); i++ )
      {
         Element* ele = i->second;
         if( ele != sim->periodicTable[ "Solvent" ] )
         {
            // Update the array of y-coordinate values for this Element
            density[ ele->getName() ][ sim->getItersCompleted() ] = (double)ele->count / (double)(o->worldX * o->worldY);

            // Update the curve for this Element
            curves[ ele->getName() ]->setPen( QColor( ele->getColor().c_str() ) );
            curves[ ele->getName() ]->setData( iterData, density[ ele->getName() ], sim->getItersCompleted() + 1 );
         }
      }
   }

   // Redraw the plot with the updated curves
   replot();
}


// Increase the size of the data arrays
void
Plot::growArrays()
{
   int newArrayLength = arrayLength * 2;

   // Grow the array of x-coordinate values
   double* tempIterData = new double [ newArrayLength ];
   for( int i = 0; i < arrayLength; i++ )
   {
      tempIterData[ i ] = iterData[ i ];
   }
   delete iterData;
   iterData = tempIterData;

   // For each Element...
   for( ElementMap::iterator i = sim->periodicTable.begin(); i != sim->periodicTable.end(); i++ )
   {
      Element* ele = i->second;
      if( ele != sim->periodicTable[ "Solvent" ] )
      {
         // Grow the array of y-coordinate values for this Element
         double* tempDensity = new double [ newArrayLength ];
         for( int j = 0; j < arrayLength; j++ )
         {
            tempDensity[ j ] = density[ ele->getName() ][ j ];
         }
         delete density[ ele->getName() ];
         density[ ele->getName() ] = tempDensity;
      }
   }

   arrayLength = newArrayLength;
}

#endif /* HAVE_QT */

