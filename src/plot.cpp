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
   setMinimumWidth( 350 );
   setTitle( "" );
   setAxisTitle( 0, "Density" );
   setAxisTitle( 2, "Time (iters)" );

   // Create and initialize the array of x-coordinate values
   iterData = new double [ o->maxIters + 1 ];
   iterData[ sim->getItersCompleted() ] = sim->getItersCompleted();

   for( ElementMap::iterator i = sim->periodicTable.begin(); i != sim->periodicTable.end(); i++ )
   {  
      Element* ele = i->second;
      if( ele != sim->periodicTable[ "Solvent" ] )
      {
         // Create and initialize the array of y-coordinate
         // values for this Element
         density[ ele->getName() ] = new double [ o->maxIters + 1 ];
         density[ ele->getName() ][ sim->getItersCompleted() ] = (double)ele->count / (double)(o->worldX * o->worldY);

         // Create and initialize the curve for this Element
         curves[ ele->getName() ] = new QwtPlotCurve( ele->getName().c_str() );
         curves[ ele->getName() ]->setPen( QColor( ele->getColor().c_str() ) );
         curves[ ele->getName() ]->setData( iterData, density[ ele->getName() ], sim->getItersCompleted() + 1 );
         curves[ ele->getName() ]->attach( this );
      }
   }
}


// Update the data arrays and redraw the plot
void
Plot::update()
{
   // Update the array of x-coordinates values
   iterData[ sim->getItersCompleted() ] = sim->getItersCompleted();

   for( ElementMap::iterator i = sim->periodicTable.begin(); i != sim->periodicTable.end(); i++ )
   {
      Element* ele = i->second;
      if( ele != sim->periodicTable[ "Solvent" ] )
      {
         // Update the array of y-coordinate values
         // for this Element
         density[ ele->getName() ][ sim->getItersCompleted() ] = (double)ele->count / (double)(o->worldX * o->worldY);

         // Update the curve for this Element
         curves[ ele->getName() ]->setData( iterData, density[ ele->getName() ], sim->getItersCompleted() + 1 );
      }
   }

   replot();
}

#endif /* HAVE_QT */

