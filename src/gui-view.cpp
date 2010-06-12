/* gui-view.cpp
 */

#ifdef HAVE_QT

#include <iostream>
#include "gui-view.h"


// Constructor
GuiView::GuiView( Options* newOptions, Sim* newSim, QWidget *parent ) 
	: QGLWidget( parent )
{
   o = newOptions;
   sim = newSim;
   //running = 0;
   running = 1;

   setFormat( QGLFormat( QGL::DoubleBuffer | QGL::DepthBuffer ) );
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


void
GuiView::adjustPaintRegion()
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
   setFixedSize( o->worldX, o->worldY );
}


void
GuiView::startPaint()
{
   std::cout << "startPaint()" << std::endl;
   running = 1;
   update();
}


void
GuiView::resetPaint()
{
   running = 0;
   update();
}


void
GuiView::initializeGL()
{
   qglClearColor( Qt::black );
   glShadeModel( GL_FLAT );
   glEnable( GL_DEPTH_TEST );
   glEnable( GL_CULL_FACE );
}


void
GuiView::resizeGL( int width, int height )
{
   glViewport( 0, 0, width, height );
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   // GLfloat x = (GLfloat)(width) / height;
   // glFrustum( -x, x, -1.0, 1.0, 4.0, 15.0 );
   glFrustum( 0, 0.5, 0.0, 0.5, 4.0, 15.0 );
   glMatrixMode( GL_MODELVIEW );
}


void
GuiView::paintGL()
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   draw();
}


void
GuiView::draw()
{
   adjustPaintRegion();

   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();
   glTranslatef( 0.0, 0.0, -8.0 );
   glRotatef( rotationX, 1.0, 0.0, 0.0 );
   glRotatef( rotationY, 0.0, 1.0, 0.0 );
   glRotatef( rotationZ, 0.0, 0.0, 1.0 );

   double xWindowPerLatticeSquare = 1.0 / (double)o->worldX;
   double yWindowPerLatticeSquare = 1.0 / (double)o->worldY;

   double atomDiameterX        = 1.0 * xWindowPerLatticeSquare;      // in percentage of the window
   //double atomDiameterX        = 2.0 * xWindowPerLatticeSquare;      // in percentage of the window
   //double atomDiameterX        = (sim->getCurrentIter() / 100 + 1) * xWindowPerLatticeSquare;      // in percentage of the window
   double atomDiameterY        = 1.0 * yWindowPerLatticeSquare;      // in percentage of the window
   //double atomDiameterY        = 2.0 * yWindowPerLatticeSquare;      // in percentage of the window
   //double atomDiameterY        = (sim->getCurrentIter() / 100 + 1) * yWindowPerLatticeSquare;      // in percentage of the window
   double atomRadiusX          = atomDiameterX / 2.0;                // in percentage of the window
   double atomRadiusY          = atomDiameterY / 2.0;                // in percentage of the window

   double trackedAtomDiameterX = 5.0 * xWindowPerLatticeSquare;      // in percentage of the window
   double trackedAtomDiameterY = 5.0 * yWindowPerLatticeSquare;      // in percentage of the window
   double trackedAtomRadiusX   = trackedAtomDiameterX / 2.0;         // in percentage of the window
   double trackedAtomRadiusY   = trackedAtomDiameterY / 2.0;         // in percentage of the window

   // Realtime world visualization
   if( running )
   {
      glBegin( GL_POINTS );

      for( int y = minY; y < maxY; y++ )
      {
         for( int x = minX; x < maxX; x++ )
         {
            if( x >= 0 && x < o->worldX && y >= 0 && y < o->worldY )
            {
               int tracked = 0;

               if( sim->world[ sim->getWorldIndex( x, y ) ] != NULL )
               {
                  qglColor( QColor( sim->world[ sim->getWorldIndex( x, y ) ]->getType()->getColor().c_str() ) );
                     /*
                  switch( sim->world[ sim->getWorldIndex( x, y ) ]->getType()->getColor() )
                  {
                     case 0:
                        glColor3f( 1.f, 0.f, 0.f );  // Red
                        break;
                     default:
                        std::cout << "BAD!" << std::endl;
                        break;
                     case SOLVENT:
                        continue;
                     case ATOM_K:
                        if( o->electrostatics )
                        {
                           glColor3f( 1.f, 0.15f, 0.f );    // Red
                        } else {
                           glColor3f( 1.f, 0.64f, 0.57f );  // Pale red
                        }
                        break;
                     case ATOM_Na:
                        if( o->electrostatics )
                        {
                           glColor3f( 0.f, 0.f, 1.f );      // Blue
                        } else {
                           glColor3f( 0.57f, 0.57f, 1.f );  // Pale blue
                        }
                        break;
                     case ATOM_Cl:
                        if( o->electrostatics )
                        {
                           glColor3f( 0.f, 0.70f, 0.35f );  // Green
                        } else {
                           glColor3f( 0.57f, 0.87f, 0.72f );// Pale green
                        }
                        break;
                     case ATOM_K_TRACK:
                        glColor3f( 1.f, 0.15f, 0.f );    // Red
                        tracked = 1;
                        break;
                     case ATOM_Na_TRACK:
                        glColor3f( 0.f, 0.f, 1.f );      // Blue
                        tracked = 1;
                        break;
                     case ATOM_Cl_TRACK:
                        glColor3f( 0.f, 0.70f, 0.35f );  // Green
                        tracked = 1;
                        break;
                     case PORE_K:
                        if( o->selectivity )
                        {
                           glColor3f( 1.f, 0.64f, 0.57f );  // Pale red
                        } else {
                           glColor3f( 1.f, 1.f, 1.f );      // White
                        }
                        break;
                     case PORE_Na:
                        if( o->selectivity )
                        {
                           glColor3f( 0.57f, 0.57f, 1.f );  // Pale blue
                        } else {
                           glColor3f( 1.f, 1.f, 1.f );      // White
                        }
                        break;
                     case PORE_Cl:
                        if( o->selectivity )
                        {
                           glColor3f( 0.57f, 0.87f, 0.72f );// Pale green
                        } else {
                           glColor3f( 1.f, 1.f, 1.f );      // White
                        }
                        break;
                     case MEMBRANE:
                        glColor3f( 0.f, 0.f, 0.f );      // Black
                        break;
                     default:
                        glColor3f( 1.f, 1.f, 1.f );      // White
                        break;
                  }
                     */

                  if( tracked )
                  {
                     // Tracked ions
                     for( double xOff = -trackedAtomRadiusX; xOff < trackedAtomRadiusX; xOff += 1.0 / (double)zoomXWindow )
                     {
                        for( double yOff = -trackedAtomRadiusY; yOff < trackedAtomRadiusY; yOff += 1.0 / (double)zoomYWindow )
                        {
                           glVertex3f( (GLfloat)( ( x + 1 - minX ) / (double)zoomXRange + xOff ),
                                       (GLfloat)( ( maxY - y )     / (double)zoomYRange + yOff ),
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
                                       (GLfloat)( ( maxY - y )     / (double)zoomYRange + yOff ),
                                       (GLfloat)0.0 );
                        }
                     }
                  }
               }
            }
         }
      }
      glEnd();
   }
}

#endif /* HAVE_QT */

