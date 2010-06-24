/* plot.cpp
 */

#ifdef HAVE_QT

#include "plot.h"
#include "safecalls.h"
using namespace SafeCalls;


// Constructor
Plot::Plot( Options* newOptions, Sim* newSim, QWidget* parent )
   : QwtPlot( parent )
{
   // Copy constructor arguments
   o = newOptions;
   sim = newSim;

   // Set up the plot
   setMinimumWidth( 350 );
   setTitle( "" );
   setAxisTitle( 0, "Density" );
   setAxisTitle( 2, "Time (iters)" );

   // Create and initialize the array of x-coordinate values
   iterData = safeNew( double [ o->maxIters ] );
   iterData[ sim->getItersCompleted() ] = sim->getItersCompleted();

   for( ElementMap::iterator i = sim->periodicTable.begin(); i != sim->periodicTable.end(); i++ )
   {  
      Element* ele = i->second;
      if( ele != sim->periodicTable[ "Solvent" ] )
      {
         // Create and initialize the array of y-coordinate
         // values for this Element
         density[ ele->getName() ] = safeNew( double [ o->maxIters ] );
         density[ ele->getName() ][ sim->getItersCompleted() ] = (double)ele->getCount() / (double)(o->worldX * o->worldY);

         // Create and initialize the curve for this Element
         curves[ ele->getName() ] = safeNew( QwtPlotCurve( ele->getName().c_str() ) );
         curves[ ele->getName() ]->setPen( QColor( ele->getColor().c_str() ) );
         curves[ ele->getName() ]->setData( iterData, density[ ele->getName() ], sim->getItersCompleted() );
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
         density[ ele->getName() ][ sim->getItersCompleted() ] = (double)ele->getCount() / (double)(o->worldX * o->worldY);

         // Update the curve for this Element
         curves[ ele->getName() ]->setData( iterData, density[ ele->getName() ], sim->getItersCompleted() );
      }
   }

   replot();
}

#endif /* HAVE_QT */

