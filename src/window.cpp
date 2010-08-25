/* window.cpp
 */

#ifdef HAVE_QT

#include <cstdlib> // exit
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

   // Initialize program state flags
   started = false;
   paused = false;
   finished = false;
   quitting = false;

   // Set up GUI components
   viewer = safeNew( Viewer( o, sim, this ) );
   plot = safeNew( Plot( o, sim, this ) );

   startBtn = safeNew( QPushButton( "&Start" ) );
   pauseBtn = safeNew( QPushButton( "&Pause" ) );
   continueBtn = safeNew( QPushButton( "&Continue" ) );

   // Set up GUI layout
   QFrame* frame = safeNew( QFrame() );
   frame->setFrameStyle( QFrame::Box | QFrame::Sunken );

   QHBoxLayout* frameLayout = safeNew( QHBoxLayout() );
   frame->setLayout( frameLayout );
   frameLayout->addWidget( viewer );
   frameLayout->addWidget( plot );

   stackedBtnLayout = safeNew( QStackedLayout() );
   stackedBtnLayout->addWidget( startBtn );
   stackedBtnLayout->addWidget( pauseBtn );
   stackedBtnLayout->addWidget( continueBtn );
   stackedBtnLayout->setCurrentWidget( startBtn );

   QVBoxLayout* mainLayout = safeNew( QVBoxLayout() );
   mainLayout->addWidget( frame );
   mainLayout->addLayout( stackedBtnLayout );

   QWidget* mainWidget = safeNew( QWidget() );
   mainWidget->setLayout( mainLayout );

   setCentralWidget( mainWidget );
   setWindowTitle( "Chemical Metabolism Simulator" );

   // Connections with the same signal should be declared
   // in order of descending slot computational complexity
   connect( startBtn, SIGNAL( clicked() ), viewer, SLOT( startPaint() ) );
   connect( startBtn, SIGNAL( clicked() ), this, SLOT( runSim() ) );
}


// The primary loop for running the simulation
void
Window::runSim()
{
   started = true;

   if( !finished )
   {
      while( sim->iterate() )
      {
         // Update gui components
         updateButton();
         plot->update();
         viewer->updateGL();

         // Check for Qt signals and events
         QCoreApplication::processEvents();
      }

      finished = true;
      if( quitting )
         close();
   }
}


// Called when the gui window is closed;
// ensures a clean exit
void
Window::closeEvent( QCloseEvent* event )
{
   sim->end();
   quitting = true;

   if( !started || finished )
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
   else
   {
      event->ignore();
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
         o->tempFiles[ Options::FILE_CONFIG ]->remove();
         o->tempFiles[ Options::FILE_CENSUS ]->remove();
         o->tempFiles[ Options::FILE_DIFFUSION ]->remove();
         o->tempFiles[ Options::FILE_RAND ]->remove();
         return false;
         break;
      default:
         std::cout << "shouldSave: Unknown dialog return value!" << std::endl;
         exit( EXIT_FAILURE );
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
   // Prompt the user for a directory
   QString dirPath = QFileDialog::getExistingDirectory( this, "Select a directory" );

   if( dirPath != NULL )
   {
      // Delete any files in the target directory with the target
      // names so that copying can proceed
      QFile::remove( QDir( dirPath ).filePath( "config.out" ) );
      QFile::remove( QDir( dirPath ).filePath( "census.out" ) );
      QFile::remove( QDir( dirPath ).filePath( "diffusion.out" ) );
      QFile::remove( QDir( dirPath ).filePath( "rand.out" ) );

      // Copy the temporary files to the chosen directory; remove the
      // temporary files and return true if all copy steps are successful;
      // otherwise remove any files that were successfully copied and
      // prompt for another attempt
      if( o->tempFiles[ Options::FILE_CONFIG ]->copy( QDir( dirPath ).filePath( "config.out" ) ) &&
          o->tempFiles[ Options::FILE_CENSUS ]->copy( QDir( dirPath ).filePath( "census.out" ) ) &&
          o->tempFiles[ Options::FILE_DIFFUSION ]->copy( QDir( dirPath ).filePath( "diffusion.out" ) ) &&
          o->tempFiles[ Options::FILE_RAND ]->copy( QDir( dirPath ).filePath( "rand.out" ) ) )
      {
         o->tempFiles[ Options::FILE_CONFIG ]->remove();
         o->tempFiles[ Options::FILE_CENSUS ]->remove();
         o->tempFiles[ Options::FILE_DIFFUSION ]->remove();
         o->tempFiles[ Options::FILE_RAND ]->remove();
         return true;
      }
      else
      {
         QFile::remove( QDir( dirPath ).filePath( "config.out" ) );
         QFile::remove( QDir( dirPath ).filePath( "census.out" ) );
         QFile::remove( QDir( dirPath ).filePath( "diffusion.out" ) );
         QFile::remove( QDir( dirPath ).filePath( "rand.out" ) );

         int choice = QMessageBox::warning( this, "Problem encountered!",
            "There was a problem writing to the directory you selected. Perhaps you don't have permission to write to that directory. Would you like to try saving the simulation output in a different directory?",
            QMessageBox::Save | QMessageBox::Discard,
            QMessageBox::Save );

         switch( choice )
         {
            case QMessageBox::Save:
               return false;
               break;
            case QMessageBox::Discard:
               o->tempFiles[ Options::FILE_CONFIG ]->remove();
               o->tempFiles[ Options::FILE_CENSUS ]->remove();
               o->tempFiles[ Options::FILE_DIFFUSION ]->remove();
               o->tempFiles[ Options::FILE_RAND ]->remove();
               return true;
               break;
            default:
               std::cout << "saveFiles: Unknown dialog return value!" << std::endl;
               exit( EXIT_FAILURE );
               break;
         }
      }
   }
   else
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
            o->tempFiles[ Options::FILE_CONFIG ]->remove();
            o->tempFiles[ Options::FILE_CENSUS ]->remove();
            o->tempFiles[ Options::FILE_DIFFUSION ]->remove();
            o->tempFiles[ Options::FILE_RAND ]->remove();
            return true;
            break;
         default:
            std::cout << "saveFiles: Unknown dialog return value!" << std::endl;
            exit( EXIT_FAILURE );
            break;
      }
   }
   std::cout << "saveFiles: Control reached end of non-void function (Mac warning)!" << std::endl;
   return false;
}


void
Window::updateButton()
{
   stackedBtnLayout->setCurrentWidget( pauseBtn );
}

#endif /* HAVE_QT */

