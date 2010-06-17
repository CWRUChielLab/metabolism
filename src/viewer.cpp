/* viewer.cpp
 */

#ifdef HAVE_QT

#include <iostream>
#include "viewer.h"


// Constructor
Viewer::Viewer( Options* newOptions, Sim* newSim, QWidget *parent ) 
	: QGLWidget( parent )
{
   // Copy constructor arguments
   o = newOptions;
   sim = newSim;

   // Let the GUI know that the simulation is
   // not yet running
   running = 0;

   // Degrees to rotate the coordinate space
   // counterclockwise around the associated axis
   rotationX = 0.0;
   rotationY = 0.0;
   rotationZ = 0.0;

   if( 0 )
   {
      zoomXRange  = 64;    // lattice sqaures wide
      zoomYRange  = 64;    // lattice squares high
      zoomXWindow = 210;   // pixels wide
      zoomYWindow = 210;   // pixels high
   }

   adjustPaintRegion();
}


// Set up view range
void
Viewer::adjustPaintRegion()
{
   if( 1 )
   {
      zoomXRange  = o->worldX;
      zoomYRange  = o->worldY;
      zoomXWindow = o->worldX;
      zoomYWindow = o->worldY;
   }

   minX = 0;
   maxX = o->worldX;
   minY = 0;
   maxY = o->worldY;

   // Force the widget to have exactly one
   // pixel per lattice square
   setFixedSize( maxX-minX, maxY-minY );
}


// Set the gui to viewer mode
void
Viewer::startPaint()
{
   running = 1;
   updateGL();
}


// Set the gui to previewer mode
void
Viewer::resetPaint()
{
   running = 0;
   updateGL();
}


// Mark an Atom as tracked when the mouse is
// pressed on or near its position
void
Viewer::mousePressEvent( QMouseEvent *event )
{
   int mouseX, mouseY, x, y;
   mouseX = event->x() * ( zoomXRange ) / ( zoomXWindow ) + minX;
   mouseY = ( zoomYWindow - 1 - event->y() ) * ( zoomYRange ) / ( zoomYWindow ) + minY;

   /*
   if( !running )
   {
      event->ignore();
      return;
   } else {
   */
      event->accept();
   /*
   }
   */

   x = mouseX;
   y = mouseY;

   if( sim->world[ sim->getWorldIndex( x, y ) ] == NULL || sim->world[ sim->getWorldIndex( x, y ) ]->isTracked() )
   {
      int offset = 1, done = 0;
      double offsetMax = 5.0 * (double)zoomXWindow / (double)zoomXRange;

      while( !done && offset < offsetMax )
      {
         for( x = mouseX - offset; x <= mouseX + offset && !done; x++ )
         {
            for( y = mouseY - offset; y <= mouseY + offset && !done; y++ )
            {
               if( sim->world[ sim->getWorldIndex( x, y ) ] != NULL && !sim->world[ sim->getWorldIndex( x, y ) ]->isTracked() )
               {
                  done = 1;
               }
            }
         }
         offset++;
      }
   }

   x--;
   y--;

   if( sim->world[ sim->getWorldIndex( x, y ) ] != NULL )
   {
      sim->world[ sim->getWorldIndex( x, y ) ]->setTracked(1);
   } else {
      event->ignore();
   }

   updateGL();
}



// Sets up the OpenGL rendering context, defines
// display lists, etc.; gets called once before
// the first time resizeGL() or paintGL() is called
void
Viewer::initializeGL()
{
   // Set background color
   qglClearColor( Qt::black );

   // Flat or smooth pixel shading,
   // indistinguishable for points
   glShadeModel( GL_FLAT );

   // Use the z-buffer, i.e., check to see
   // whether or not a pixel will be hidden
   // behind other pixels and therefore should
   // not be drawn
   glEnable( GL_DEPTH_TEST );

   // Use facet culling, i.e., remove facets
   // of polygons that are not facing the
   // window
   glEnable( GL_CULL_FACE );
}


// Sets up the OpenGL viewport, projection, etc.;
// gets called whenever the widget has been resized
// (and also when it is shown for the first time
// because all newly created widgets get a resize
// event automatically)
void
Viewer::resizeGL( int width, int height )
{
   // Specify the position and size of the viewport
   glViewport( 0, 0, (GLint)width, (GLint)height );

   // Switch to manipulating the projection matrix
   // stack for camera manipulations
   glMatrixMode( GL_PROJECTION );

   // Reset the projection matrix to the identity
   // matrix
   glLoadIdentity();

   // Multiply the current matrix by a perspective
   // matrix (2D lattice points are drawn in the
   // plane z = -8.0)
   glFrustum( 0, 0.5, 0.0, 0.5, 4.0, 16.0 );

   // Switch back to manipulating the model view
   // matrix stack for scene manipulations
   glMatrixMode( GL_MODELVIEW );
}


// Renders the OpenGL scene; gets called whenever
// the widget needs to be updated
void
Viewer::paintGL()
{
   // Do something important
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // Set up view range
   adjustPaintRegion();

   // Switch to manipulating the model view matrix
   // stack for scene manipulations
   glMatrixMode( GL_MODELVIEW );

   // Reset the model view matrix to the identity
   // matrix
   glLoadIdentity();

   // Translate the coordinate space to z = -8.0
   glTranslatef( 0.0, 0.0, -8.0 );

   // Rotate the coordinate space rotationX degrees
   // counterclockwise around the x-axis, etc.
   glRotatef( rotationX, 1.0, 0.0, 0.0 );
   glRotatef( rotationY, 0.0, 1.0, 0.0 );
   glRotatef( rotationZ, 0.0, 0.0, 1.0 );

   // Percentage of the widget window belonging to
   // each lattice square
   double xWindowPerLatticeSquare = 1.0 / (double)o->worldX;
   double yWindowPerLatticeSquare = 1.0 / (double)o->worldY;

   double atomDiameterX        = 1.0 * xWindowPerLatticeSquare;      // in percentage of the window
   double atomDiameterY        = 1.0 * yWindowPerLatticeSquare;      // in percentage of the window
   double atomRadiusX          = atomDiameterX / 2.0;                // in percentage of the window
   double atomRadiusY          = atomDiameterY / 2.0;                // in percentage of the window

   double trackedAtomDiameterX = 5.0 * xWindowPerLatticeSquare;      // in percentage of the window
   double trackedAtomDiameterY = 5.0 * yWindowPerLatticeSquare;      // in percentage of the window
   double trackedAtomRadiusX   = trackedAtomDiameterX / 2.0;         // in percentage of the window
   double trackedAtomRadiusY   = trackedAtomDiameterY / 2.0;         // in percentage of the window

   // Realtime world visualization
   //if( running )
   if( 1 )
   {
      // Start specifying points as vertices
      glBegin( GL_POINTS );

      for( int y = minY; y < maxY; y++ )
      {
         for( int x = minX; x < maxX; x++ )
         {
            if( x >= 0 && x < o->worldX && y >= 0 && y < o->worldY )
            {
               if( sim->world[ sim->getWorldIndex( x, y ) ] != NULL )
               {
                  // Set the pen color to the Atom's Element's color
                  qglColor( QColor( sim->world[ sim->getWorldIndex( x, y ) ]->getType()->getColor().c_str() ) );

                  // Create a vertex for the Atom
                  if( sim->world[ sim->getWorldIndex( x, y ) ]->isTracked() )
                  {
                     // Tracked ions
                     for( double xOff = -trackedAtomRadiusX; xOff < trackedAtomRadiusX; xOff += 1.0 / (double)zoomXWindow )
                     {
                        for( double yOff = -trackedAtomRadiusY; yOff < trackedAtomRadiusY; yOff += 1.0 / (double)zoomYWindow )
                        {
                           glVertex3f( (GLfloat)( ( x + 1 - minX ) / (double)zoomXRange + xOff ),
                                       (GLfloat)( ( y + 1 - minY ) / (double)zoomYRange + yOff ),
                                       (GLfloat)0.0 );
                        }
                     }
                  } else {
                     // Nontracked ions
                     for( double xOff = -atomRadiusX; xOff < atomRadiusX; xOff += 1.0 / (double)zoomXWindow )
                     {
                        for( double yOff = -atomRadiusY; yOff < atomRadiusY; yOff += 1.0 / (double)zoomYWindow )
                        {
                           glVertex3f( (GLfloat)( ( x + 1 - minX ) / (double)zoomXRange + xOff ),
                                       (GLfloat)( ( y + 1 - minY ) / (double)zoomYRange + yOff ),
                                       (GLfloat)0.0 );
                        }
                     }
                  }
               }
            }
         }
      }
      // Finish specifying points as vertices
      glEnd();
   }
   else
   // World preview visualization
   {
   }
}

#endif /* HAVE_QT */

