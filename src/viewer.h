/* viewer.h
 */

#ifndef VIEWER_H
#define VIEWER_H 
#ifdef HAVE_QT

#include <QGLWidget>
#include <QMouseEvent>
#include "options.h"
#include "sim.h"

class Viewer : public QGLWidget
{
   Q_OBJECT

   public:
      // Constructor
      Viewer( Options* initOptions, Sim* initSim, QWidget *parent = 0 );

   public slots:
      void adjustPaintRegion();

   protected:
      void mousePressEvent( QMouseEvent *event );
      void initializeGL();
      void resizeGL( int width, int height );
      void paintGL();

   private:
      Options* o;
      Sim* sim;
      int minX;
      int minY;
      int maxX;
      int maxY;
      int zoomXRange;
      int zoomYRange;
      int zoomXWindow;
      int zoomYWindow;

      GLfloat rotationX;
      GLfloat rotationY;
      GLfloat rotationZ;
};

#endif /* HAVE_QT */
#endif /* VIEWER_H */

