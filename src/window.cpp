/* window.cpp
 */

#ifdef HAVE_QT

#include "safecalls.h"
#include "window.h"
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
   plot = safeNew( Plot( o, sim, this ) );
   button = safeNew( QPushButton( "Click to start" ) );

   // Set up GUI layout
   QFrame* frame = safeNew( QFrame() );
   frame->setFrameStyle( QFrame::Box | QFrame::Sunken );

   QHBoxLayout* frameLayout = safeNew( QHBoxLayout() );
   frame->setLayout( frameLayout );
   frameLayout->addWidget( viewer );
   frameLayout->addWidget( plot );

   QVBoxLayout* mainLayout = safeNew( QVBoxLayout() );
   mainLayout->addWidget( frame );
   mainLayout->addWidget( button );

   QWidget* mainWidget = safeNew( QWidget() );
   mainWidget->setLayout( mainLayout );

   setCentralWidget( mainWidget );
   setWindowTitle( "Chemical Metabolism Simulator" );

   // Connections with the same signal should be declared
   // in order of descending slot computational complexity
   connect( button, SIGNAL( clicked() ), viewer, SLOT( startPaint() ) );
   connect( button, SIGNAL( clicked() ), this, SLOT( runSim() ) );
   connect( this, SIGNAL( iterDone() ), this, SLOT( updateButton() ) );
   connect( this, SIGNAL( iterDone() ), plot, SLOT( update() ) );
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

