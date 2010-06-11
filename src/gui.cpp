/* gui.cpp
 */

#ifdef HAVE_QT

#include "gui.h"
#include "safecalls.h"
using namespace SafeCalls;


// Constructor
GUI::GUI( Options* newOptions, Sim* newSim, QWidget* parent, Qt::WindowFlags flags )
   : QMainWindow( parent, flags )
{
   // Copy constructor arguments
   o = newOptions;
   sim = newSim;

   // Set up GUI components
   button = safeNew( QPushButton( "Click to start" ) );

   // Set up GUI layout
   mainLayout = safeNew( QVBoxLayout() );
   mainLayout->addWidget( button );
   mainWidget = safeNew( QWidget() );
   mainWidget->setLayout( mainLayout );

   setCentralWidget( mainWidget );
   setWindowTitle( "testing 1 2 3" );

   // Signals
   connect( button, SIGNAL( clicked() ), this, SLOT( runSim() ) );
   connect( this, SIGNAL( iterDone() ), this, SLOT( updateButton() ) );
}


// ...
void
GUI::closeEvent( QCloseEvent* event )
{
   sim->finalizeIO();
   QWidget::closeEvent( event );
}


// ...
void
GUI::updateButton()
{
   button->setText( QString::number( sim->getCurrentIter() ) );
}


void
GUI::runSim()
{
   while( sim->iterate() )
   {
      QCoreApplication::processEvents();

      // Print out the progress of the simulation
      // at most once each second
      if( o->progress )
      {
         sim->reportProgress();
      }

      // Take a census of the atoms in the world
      // occasionally
      if( sim->getCurrentIter() % 8 == 0 )
      {
         sim->writeCensus();
      }

      // Sleep the simulation each iteration
      if( o->sleep != 0 )
      {
         usleep(o->sleep * 1000);
      }
      
      emit iterDone();
   }
}

#endif /* HAVE_QT */

