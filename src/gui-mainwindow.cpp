/* gui-mainwindow.cpp
 */

#ifdef HAVE_QT

#include "gui-mainwindow.h"
#include "safecalls.h"
using namespace SafeCalls;


// Constructor
GuiMainWindow::GuiMainWindow( Options* newOptions, Sim* newSim, QWidget* parent, Qt::WindowFlags flags )
   : QMainWindow( parent, flags )
{
   // Copy constructor arguments
   o = newOptions;
   sim = newSim;

   // Set up GUI components
   view = safeNew( GuiView( o, sim, this ) );
   button = safeNew( QPushButton( "Click to start" ) );

   // Set up GUI layout
   mainLayout = safeNew( QVBoxLayout() );
   mainLayout->addWidget( view );
   mainLayout->addWidget( button );
   mainWidget = safeNew( QWidget() );
   mainWidget->setLayout( mainLayout );

   setCentralWidget( mainWidget );
   setWindowTitle( "Chemical Metabolism Simulator" );

   // Connections with the same signal should be declared
   // in order of descending slot computational complexity
   connect( button, SIGNAL( clicked() ), view, SLOT( startPaint() ) );
   connect( button, SIGNAL( clicked() ), this, SLOT( runSim() ) );
   connect( this, SIGNAL( iterDone() ), this, SLOT( updateButton() ) );
   connect( this, SIGNAL( iterDone() ), view, SLOT( updateGL() ) );
}


// ...
void
GuiMainWindow::closeEvent( QCloseEvent* event )
{
   sim->end();
   QWidget::closeEvent( event );
}


// ...
void
GuiMainWindow::updateButton()
{
   button->setText( QString::number( sim->getItersCompleted() ) );
}


// ...
void
GuiMainWindow::runSim()
{
   while( sim->iterate() )
   {
      // Check for Qt signals and events
      QCoreApplication::processEvents();

      // Signal that an iteration has completed
      emit iterDone();
   }
}

#endif /* HAVE_QT */

