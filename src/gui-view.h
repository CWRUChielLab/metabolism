/* gui-view.h
 */

#ifndef GUI_VIEW_H
#define GUI_VIEW_H 
#ifdef HAVE_QT

#include <QGLWidget>
#include "options.h"
#include "sim.h"

class GuiView : public QGLWidget
{
   Q_OBJECT

   public:
      // Constructor
      GuiView( Options* newOptions, Sim* newSim, QWidget *parent = 0 );

   public slots:
      void adjustPaintRegion();
      void startPaint();
      void resetPaint();

   protected:
      void initializeGL();
      void resizeGL( int width, int height );
      void paintGL();

   private:
      Options* o;
      Sim* sim;
      int running;
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
      void draw();
};

#endif /* HAVE_QT */
#endif /* GUI_VIEW_H */

