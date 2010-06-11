/* gui-mainwindow.h
 */

#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H 
#ifdef HAVE_QT

#include <QtGui>
#include "gui-view.h"
#include "options.h"
#include "sim.h"

class GuiMainWindow : public QMainWindow
{
   Q_OBJECT

   public:
      // Constructor
      GuiMainWindow( Options* newOptions, Sim* newSim, QWidget* parent = 0, Qt::WindowFlags flags = 0 );

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
      GuiView* view;
      QPushButton* button;
      QVBoxLayout* mainLayout;
      QWidget* mainWidget;
};

#endif /* HAVE_QT */
#endif /* GUI_MAINWINDOW_H */
