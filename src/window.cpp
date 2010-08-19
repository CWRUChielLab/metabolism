/* window.cpp
 */

#ifdef HAVE_QT

#include <cassert>
#include <QDir>
#include "safecalls.h"
#include "window.h"
using namespace SafeCalls;


// Constructor
Window::Window( Options* initOptions, Sim* initSim, QWidget* parent, Qt::WindowFlags flags )
   : QMainWindow( parent, flags )
{
   // Copy constructor arguments
   o = initOptions;
   sim = initSim;

   running = false;
   closing = false;

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
}


// Called when the gui window is closed;
// ensures a clean exit
void
Window::closeEvent( QCloseEvent* event )
{
   sim->end();
   closing = true;

   if( running )
   {
      event->ignore();
   }
   else
   {
      // Prompt the user asking if temporary files
      // should be saved in a permanent location; if the
      // user choses to save the files, prompt for a
      // directory repeatedly until the files are copied
      // successfully
      if( shouldSave() )
         while( !saveFiles() ) {}
      event->accept();
   }
}


// Prompt the user asking if temporary files
// should be saved in a permanent location;
// returns true if the user chose to save and
// false otherwise; temporary files are deleted
// if save is not chosen
bool
Window::shouldSave()
{
   // Grab the temporary files
   QFile configFile( o->configFilePath.c_str() );
   QFile censusFile( o->censusFilePath.c_str() );
   QFile diffusionFile( o->diffusionFilePath.c_str() );
   QFile randFile( o->randFilePath.c_str() );

   int choice = QMessageBox::question( this, "Save simulation output?",
      "Would you like to save the output for this simulation?",
      QMessageBox::Save | QMessageBox::Discard,
      QMessageBox::Save );

   switch( choice )
   {
      case QMessageBox::Save:
         return true;
         break;
      case QMessageBox::Discard:
         configFile.remove();
         censusFile.remove();
         diffusionFile.remove();
         randFile.remove();
         return false;
         break;
      default:
         assert(0);
         break;
   }
}


// Prompt the user for a directory in which to
// copy the temporary files as permanent files;
// returns true if all copies were successful or
// if the user changed his or her mind and
// false otherwise; temporary files are deleted
// if the function returns true
bool
Window::saveFiles()
{
   // Grab the temporary files
   QFile configFile( o->configFilePath.c_str() );
   QFile censusFile( o->censusFilePath.c_str() );
   QFile diffusionFile( o->diffusionFilePath.c_str() );
   QFile randFile( o->randFilePath.c_str() );

   // Prompt the user for a directory
   QString dirPath = QFileDialog::getExistingDirectory( this, "Select a directory" );

   if( dirPath == NULL )
   {
      int choice = QMessageBox::warning( this, "Cancel save?",
         "Are you sure you do not want to save the simulation output?",
         QMessageBox::Save | QMessageBox::Discard,
         QMessageBox::Save );

      switch( choice )
      {
         case QMessageBox::Save:
            return false;
            break;
         case QMessageBox::Discard:
            configFile.remove();
            censusFile.remove();
            diffusionFile.remove();
            randFile.remove();
            return true;
            break;
         default:
            assert(0);
            break;
      }
   }
   else
   {
      // Delete any files in the target directory with the target
      // names so that copying can proceed
      QFile::remove( QDir( dirPath ).filePath( "config.out" ) );
      QFile::remove( QDir( dirPath ).filePath( "census.out" ) );
      QFile::remove( QDir( dirPath ).filePath( "diffusion.out" ) );
      QFile::remove( QDir( dirPath ).filePath( "rand.out" ) );

      // Copy the temporary files to the chosen directory; remove the
      // temporary files and return true if all copy steps are successful,
      // otherwise remove any files that were successfully copied and
      // return false
      if( configFile.copy( QDir( dirPath ).filePath( "config.out" ) ) &&
          censusFile.copy( QDir( dirPath ).filePath( "census.out" ) ) &&
          diffusionFile.copy( QDir( dirPath ).filePath( "diffusion.out" ) ) &&
          randFile.copy( QDir( dirPath ).filePath( "rand.out" ) ) )
      {
         configFile.remove();
         censusFile.remove();
         diffusionFile.remove();
         randFile.remove();
         return true;
      }
      else
      {
         QFile::remove( QDir( dirPath ).filePath( "config.out" ) );
         QFile::remove( QDir( dirPath ).filePath( "census.out" ) );
         QFile::remove( QDir( dirPath ).filePath( "diffusion.out" ) );
         QFile::remove( QDir( dirPath ).filePath( "rand.out" ) );

         int choice = QMessageBox::warning( this, "Problem encountered!",
            "There was a problem writing to the directory you selected. Perhaps you don't have permission to write to this directory. Would you like to try saving the simulation output in a different directory?",
            QMessageBox::Save | QMessageBox::Discard,
            QMessageBox::Save );

         switch( choice )
         {
            case QMessageBox::Save:
               return false;
               break;
            case QMessageBox::Discard:
               configFile.remove();
               censusFile.remove();
               diffusionFile.remove();
               randFile.remove();
               return true;
               break;
            default:
               assert(0);
               break;
         }
      }
   }
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
   if( !running )
   {
      running = true;

      while( sim->iterate() )
      {
         // Update gui components
         updateButton();
         plot->update();
         viewer->updateGL();

         // Check for Qt signals and events
         QCoreApplication::processEvents();
      }

      running = false;
      if( closing )
         close();
   }
}

#endif /* HAVE_QT */

