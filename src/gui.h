/* gui.h
 */

#ifndef GUI_H
#define GUI_H 
#ifdef HAVE_QT

#include <QGLWidget>
#include <QtGui>
#include "options.h"
#include "sim.h"

class GUI : public QMainWindow
{
   Q_OBJECT

   public:
      // Constructor
      GUI( Options* newOptions, Sim* newSim, QWidget* parent = 0, Qt::WindowFlags flags = 0 );

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
      QPushButton* button;
      QVBoxLayout* mainLayout;
      QWidget* mainWidget;
};

#endif /* HAVE_QT */
#endif /* GUI_H */
