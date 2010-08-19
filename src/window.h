/* window.h
 */

#ifndef WINDOW_H
#define WINDOW_H 
#ifdef HAVE_QT

#include <QtGui>
#include "options.h"
#include "plot.h"
#include "sim.h"
#include "viewer.h"

class Window : public QMainWindow
{
   Q_OBJECT

   public:
      // Constructor
      Window( Options* initOptions, Sim* initSim, QWidget* parent = 0, Qt::WindowFlags flags = 0 );

   protected:
      void closeEvent( QCloseEvent* event );

   public slots:
      void updateButton();
      void runSim();

   signals:
      void iterDone();

   private:
      // GUI attributes
      Options* o;
      Sim* sim;
      bool running;
      bool closing;

      // GUI components
      Viewer* viewer;
      Plot* plot;
      QPushButton* button;

      bool shouldSave();
      bool saveFiles();
};

#endif /* HAVE_QT */
#endif /* WINDOW_H */

