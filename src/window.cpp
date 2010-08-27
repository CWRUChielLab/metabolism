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
   simStarted = false;
   simPaused = false;
   quitRequested = false;

   // Set up GUI components
   viewer = safeNew( Viewer( o, sim, this ) );
   plot = safeNew( Plot( o, sim, this ) );

   startBtn = safeNew( QPushButton( "&Start" ) );
   pauseBtn = safeNew( QPushButton( "&Pause" ) );
   resumeBtn = safeNew( QPushButton( "&Resume" ) );

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
   stackedBtnLayout->addWidget( resumeBtn );
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
   connect( startBtn, SIGNAL( clicked() ), this, SLOT( execStackedBtn() ) );
   connect( startBtn, SIGNAL( clicked() ), viewer, SLOT( startPaint() ) );
   connect( startBtn, SIGNAL( clicked() ), this, SLOT( runSim() ) );
   connect( pauseBtn, SIGNAL( clicked() ), this, SLOT( execStackedBtn() ) );
   connect( resumeBtn, SIGNAL( clicked() ), this, SLOT( execStackedBtn() ) );
   connect( resumeBtn, SIGNAL( clicked() ), this, SLOT( runSim() ) );
}


// The primary loop for running the simulation
void
Window::runSim()
{
   while( sim->iterate() )
   {
      // Update gui components
      plot->update();
      viewer->updateGL();

      // Check for Qt signals and events
      QCoreApplication::processEvents();

      // Check for pausing
      if( simPaused )
         return;
   }

   if( o->progress )
      sim->forceProgressReport();
}


// Called when the gui window is closed;
// ensures a clean exit
void
Window::closeEvent( QCloseEvent* event )
{
   quitRequested = true;
   if( o->progress )
      sim->forceProgressReport();

   if( !simStarted )
   {
      // Remove the temporary files permanently
      o->tempFiles[ Options::FILE_CONFIG ]->remove();
      o->tempFiles[ Options::FILE_CENSUS ]->remove();
      o->tempFiles[ Options::FILE_DIFFUSION ]->remove();
      o->tempFiles[ Options::FILE_RAND ]->remove();

      // Allow the application to close
      event->accept();
   }
   else
   {
      // Ask the user if and to where the temporary output
      // files should be copied as permanent files
      save();

      // Allow the application to close if the user has not
      // decided to cancel
      if( quitRequested )
         event->accept();
      else
         event->ignore();
   }
}


void
Window::execStackedBtn()
{
   switch( stackedBtnLayout->currentIndex() )
   {
      case 0: // startBtn
         simStarted = true;
         simPaused = false;
         stackedBtnLayout->setCurrentWidget( pauseBtn );
         break;
      case 1: // pauseBtn
         simStarted = true;
         simPaused = true;
         stackedBtnLayout->setCurrentWidget( resumeBtn );
         if( o->progress )
            sim->forceProgressReport();
         break;
      case 2: // resumeBtn
         simStarted = true;
         simPaused = false;
         stackedBtnLayout->setCurrentWidget( pauseBtn );
         break;
      default:
         std::cout << "updateStackedBtn: current widget unknown!" << std::endl;
         exit( EXIT_FAILURE );
         break;
   }
}


// Ask the user if and to where the temporary output
// files should be copied as permanent files
void
Window::save()
{
   int choice;
   QString savePath;

   // Ask the user if files should be saved
   choice = QMessageBox::question( this, "Save simulation output?",
      "Would you like to save the output for this simulation?",
      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
      QMessageBox::Cancel );

   while( true )
   {
      switch( choice )
      {
         // If the user chose to save the files...
         case QMessageBox::Save:
            // Ask the user for a directory
            savePath = QFileDialog::getExistingDirectory( this, "Select a directory" );

            // If a valid directory was selected...
            if( savePath != NULL )
            {
               // Clean up the simulation
               sim->cleanup();

               // Delete any files in the target directory with the target
               // names so that copying can proceed
               QFile::remove( QDir( savePath ).filePath( "config.out" ) );
               QFile::remove( QDir( savePath ).filePath( "census.out" ) );
               QFile::remove( QDir( savePath ).filePath( "diffusion.out" ) );
               QFile::remove( QDir( savePath ).filePath( "rand.out" ) );

               // Copy the temporary files to the chosen directory
               if( o->tempFiles[ Options::FILE_CONFIG ]->copy( QDir( savePath ).filePath( "config.out" ) ) &&
                   o->tempFiles[ Options::FILE_CENSUS ]->copy( QDir( savePath ).filePath( "census.out" ) ) &&
                   o->tempFiles[ Options::FILE_DIFFUSION ]->copy( QDir( savePath ).filePath( "diffusion.out" ) ) &&
                   o->tempFiles[ Options::FILE_RAND ]->copy( QDir( savePath ).filePath( "rand.out" ) ) )
               {
                  // If all copy steps were successful, remove the temporary
                  // files and return
                  o->tempFiles[ Options::FILE_CONFIG ]->remove();
                  o->tempFiles[ Options::FILE_CENSUS ]->remove();
                  o->tempFiles[ Options::FILE_DIFFUSION ]->remove();
                  o->tempFiles[ Options::FILE_RAND ]->remove();
                  return;
               }
               else
               {
                  // If some copy steps were NOT successful, remove any partial
                  // copies and alert the user to the problem
                  QFile::remove( QDir( savePath ).filePath( "config.out" ) );
                  QFile::remove( QDir( savePath ).filePath( "census.out" ) );
                  QFile::remove( QDir( savePath ).filePath( "diffusion.out" ) );
                  QFile::remove( QDir( savePath ).filePath( "rand.out" ) );

                  choice = QMessageBox::warning( this, "Problem encountered!",
                     "There was a problem writing to the directory you selected. Perhaps you don't have permission to write to that directory. Would you like to try saving the simulation output in a different directory?",
                     QMessageBox::Save | QMessageBox::Discard,
                     QMessageBox::Save );
                  break;
               }
            }
            // If a valid directory was NOT selected...
            else
            {
               choice = QMessageBox::warning( this, "Cancel save?",
                  "Are you sure you do not wish to save the simulation output?",
                  QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                  QMessageBox::Save );
               break;
            }
            break;

         // If the user chose NOT to save the files...
         case QMessageBox::Discard:
            // Clean up the simulation
            sim->cleanup();

            // Remove the temporary files permanently
            o->tempFiles[ Options::FILE_CONFIG ]->remove();
            o->tempFiles[ Options::FILE_CENSUS ]->remove();
            o->tempFiles[ Options::FILE_DIFFUSION ]->remove();
            o->tempFiles[ Options::FILE_RAND ]->remove();
            return;
            break;

         // If the user chose to cancel closing the app...
         case QMessageBox::Cancel:
            // Reset the quitting flag
            quitRequested = false;
            return;
            break;

         default:
            std::cout << "save: Unknown dialog return value!" << std::endl;
            exit( EXIT_FAILURE );
            break;
      }
   }
}

#endif /* HAVE_QT */

