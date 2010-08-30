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
      void startPauseResume();
      void runSim();

   signals:
      void iterDone();

   private:
      // GUI attributes
      Options* o;
      Sim* sim;
      bool simStarted;
      bool simPaused;
      bool quitRequested;

      // GUI components
      Viewer* viewer;
      Plot* plot;
      QLabel* statusLbl;

      QLabel* itersLbl;
      QSlider* itersSlider;
      QLabel* itersVal;

      QLabel* xLbl;
      QSlider* xSlider;
      QLabel* xVal;

      QLabel* yLbl;
      QSlider* ySlider;
      QLabel* yVal;

      QLabel* seedLbl;
      QLineEdit* seedVal;
      QPushButton* seedBtn;

      std::vector<Element*> eles;
      std::vector<QPushButton*> removeEleBtns;
      std::vector<QPushButton*> colorChips;
      std::vector<QLineEdit*> eleNames;
      std::vector<QSlider*> concSliders;
      std::vector<QLabel*> concVals;

      QStackedLayout* stackedBtnLayout;
      QPushButton* startBtn;
      QPushButton* pauseBtn;
      QPushButton* resumeBtn;
      QPushButton* quitBtn;

      QFrame* createCtrl();
      QFrame* createViewer();
      QFrame* createPlot();
      QStatusBar* createStatusBar();
      void save();
};

#endif /* HAVE_QT */
#endif /* WINDOW_H */

