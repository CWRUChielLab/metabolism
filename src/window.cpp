/* window.cpp
 */

#ifdef HAVE_QT

#include "window.h"
#include "safecalls.h"
using namespace SafeCalls;


// Constructor
Window::Window( Options* newOptions, Sim* newSim, QWidget* parent, Qt::WindowFlags flags )
   : QMainWindow( parent, flags )
{
   // Copy constructor arguments
   o = newOptions;
   sim = newSim;

   // Set up GUI components
   viewer = safeNew( Viewer( o, sim, this ) );
   button = safeNew( QPushButton( "Click to start" ) );

   // Set up GUI layout
   mainLayout = safeNew( QVBoxLayout() );
   mainLayout->addWidget( viewer );
   mainLayout->addWidget( button );
   mainWidget = safeNew( QWidget() );
   mainWidget->setLayout( mainLayout );

   setCentralWidget( mainWidget );
   setWindowTitle( "Chemical Metabolism Simulator" );

   // Connections with the same signal should be declared
   // in order of descending slot computational complexity
   connect( button, SIGNAL( clicked() ), viewer, SLOT( startPaint() ) );
   connect( button, SIGNAL( clicked() ), this, SLOT( runSim() ) );
   connect( this, SIGNAL( iterDone() ), this, SLOT( updateButton() ) );
   connect( this, SIGNAL( iterDone() ), viewer, SLOT( updateGL() ) );
}


// Called when the gui window is closed;
// ensures a clean exit
void
Window::closeEvent( QCloseEvent* event )
{
   sim->end();
   QWidget::closeEvent( event );
}


// Update the pushbutton to display the progress
void
Window::updateButton()
{
   button->setText( QString::number( sim->getItersCompleted() ) );
}


// The primary loop for running the simulation
void
Window::runSim()
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

