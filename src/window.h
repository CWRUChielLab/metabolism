/* window.h
 */

#ifndef WINDOW_H
#define WINDOW_H 
#ifdef HAVE_QT

#include <QtGui>
#include "viewer.h"
#include "options.h"
#include "sim.h"

class Window : public QMainWindow
{
   Q_OBJECT

   public:
      // Constructor
      Window( Options* newOptions, Sim* newSim, QWidget* parent = 0, Qt::WindowFlags flags = 0 );

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

      // GUI components
      Viewer* viewer;
      QPushButton* button;
      QVBoxLayout* mainLayout;
      QWidget* mainWidget;
};

#endif /* HAVE_QT */
#endif /* WINDOW_H */
