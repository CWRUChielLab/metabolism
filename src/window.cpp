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

   // Create the gui components
   QHBoxLayout* mainLayout = safeNew( QHBoxLayout() );
   mainLayout->addWidget( createCtrl() );
   mainLayout->addWidget( createViewer() );
   mainLayout->addWidget( createPlot(), 1 );
   createStatusBar();

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

   // Set the initial keyboard focus for the window
   // to the start button
   startBtn->setFocus();
}


// Set up the widgets and connections for the
// control panel
QFrame*
Window::createCtrl()
{
   QFrame* ctrlFrame = safeNew( QFrame() );
   QVBoxLayout* ctrlLayout = safeNew( QVBoxLayout() );
   ctrlFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
   ctrlFrame->setLayout( ctrlLayout );

   // Header label
   QLabel* ctrlHeader = safeNew( QLabel( "<b>Control Panel</b>" ) );
   ctrlHeader->setAlignment( Qt::AlignHCenter );
   ctrlLayout->addWidget( ctrlHeader );

   // Iterations controller
   QLabel* itersLbl = safeNew( QLabel( "&Iterations" ) );
   itersLbl->setToolTip( "Iterations" );
   itersLbl->setMinimumSize( itersLbl->sizeHint() );

   QSlider* itersSlider = safeNew( QSlider( Qt::Horizontal ) );
   itersSlider->setMinimumWidth( 100 );
   itersSlider->setRange( 1, 1000000 );
   itersSlider->setPageStep( 1000 );
   itersSlider->setValue( o->maxIters );
   itersSlider->setToolTip( "Iterations" );

   itersLbl->setBuddy( itersSlider );

   QLabel* itersVal = safeNew( QLabel( QString::number( itersSlider->maximum() ) ) );
   itersVal->setAlignment( Qt::AlignRight );
   itersVal->setMinimumSize( itersVal->sizeHint() );
   itersVal->setText( QString::number( o->maxIters ) );

   QHBoxLayout* itersLayout = safeNew( QHBoxLayout() );
   itersLayout->addWidget( itersLbl );
   itersLayout->addWidget( itersSlider );
   itersLayout->addWidget( itersVal );
   ctrlLayout->addLayout( itersLayout );

   connect( itersSlider, SIGNAL( valueChanged( int ) ), itersVal, SLOT( setNum( int ) ) );

   // Lattice width controller
   QLabel* xLbl = safeNew( QLabel( "&Width" ) );
   xLbl->setToolTip( "Width" );
   xLbl->setMinimumSize( itersLbl->sizeHint() );

   QSlider* xSlider = safeNew( QSlider( Qt::Horizontal ) );
   xSlider->setMinimumWidth( 100 );
   xSlider->setRange( 1, 1000 );
   xSlider->setPageStep( 100 );
   xSlider->setValue( o->worldX );
   xSlider->setToolTip( "Width" );

   xLbl->setBuddy( xSlider );

   QLabel* xVal = safeNew( QLabel( QString::number( o->worldX ) ) );
   xVal->setAlignment( Qt::AlignRight );
   xVal->setMinimumSize( itersVal->sizeHint() );

   QHBoxLayout* xLayout = safeNew( QHBoxLayout() );
   xLayout->addWidget( xLbl );
   xLayout->addWidget( xSlider );
   xLayout->addWidget( xVal );
   ctrlLayout->addLayout( xLayout );

   connect( xSlider, SIGNAL( valueChanged( int ) ), xVal, SLOT( setNum( int ) ) );

   // Lattice height controller
   QLabel* yLbl = safeNew( QLabel( "&Height" ) );
   yLbl->setToolTip( "Height" );
   yLbl->setMinimumSize( itersLbl->sizeHint() );

   QSlider* ySlider = safeNew( QSlider( Qt::Horizontal ) );
   ySlider->setMinimumWidth( 100 );
   ySlider->setRange( 1, 1000 );
   ySlider->setPageStep( 100 );
   ySlider->setValue( o->worldX );
   ySlider->setToolTip( "Height" );

   yLbl->setBuddy( ySlider );

   QLabel* yVal = safeNew( QLabel( QString::number( o->worldX ) ) );
   yVal->setAlignment( Qt::AlignRight );
   yVal->setMinimumSize( itersVal->sizeHint() );

   QHBoxLayout* yLayout = safeNew( QHBoxLayout() );
   yLayout->addWidget( yLbl );
   yLayout->addWidget( ySlider );
   yLayout->addWidget( yVal );
   ctrlLayout->addLayout( yLayout );

   connect( ySlider, SIGNAL( valueChanged( int ) ), yVal, SLOT( setNum( int ) ) );

   // Seed controller
   QLabel* seedLbl = safeNew( QLabel( "See&d" ) );
   seedLbl->setToolTip( "Seed" );

   QLineEdit* seedVal = safeNew( QLineEdit( QString::number( o->seed ) ) );
   seedVal->setValidator( safeNew( QIntValidator() ) );
   seedVal->setToolTip( "Seed" );

   seedLbl->setBuddy( seedVal );

   QPushButton* seedBtn = safeNew( QPushButton( "Get &New" ) );
   seedBtn->setToolTip( "Seed" );

   QHBoxLayout* seedLayout = safeNew( QHBoxLayout() );
   seedLayout->addWidget( seedLbl );
   seedLayout->addWidget( seedVal );
   seedLayout->addWidget( seedBtn );
   ctrlLayout->addLayout( seedLayout );

   // Element controllers
   std::vector<Element*> eles;
   std::vector<QPushButton*> removeEleBtns;
   std::vector<QLineEdit*> eleNames;
   std::vector<QPushButton*> colorChips;
   std::vector<QSlider*> concSliders;
   std::vector<QLabel*> concVals;

   int i = 0;
   for( ElementMap::iterator j = sim->periodicTable.begin(); j != sim->periodicTable.end(); j++ )
   {
      Element* thisEle = j->second;
      if( thisEle != sim->periodicTable[ "Solvent" ] )
      {
         eles.push_back( thisEle );

         removeEleBtns.push_back( safeNew( QPushButton( "-" ) ) );
         removeEleBtns[ i ]->setFixedHeight( removeEleBtns[ i ]->sizeHint().height() );
         removeEleBtns[ i ]->setFixedWidth( removeEleBtns[ i ]->sizeHint().height() );

         colorChips.push_back( safeNew( QPushButton() ) );
         //colorChips[ i ]->setFlat( true );
         colorChips[ i ]->setStyleSheet( "background-color : " + QColor( thisEle->getColor().c_str() ).name() );
         colorChips[ i ]->setFixedHeight( colorChips[ i ]->sizeHint().height() );
         colorChips[ i ]->setFixedWidth( colorChips[ i ]->sizeHint().height() );

         eleNames.push_back( safeNew( QLineEdit( thisEle->getName().c_str() ) ) );
         eleNames[ i ]->setMinimumWidth( 100 );

         concSliders.push_back( safeNew( QSlider( Qt::Horizontal ) ) );
         concSliders[ i ]->setMinimumWidth( 100 );
         concSliders[ i ]->setRange( 0, 1000 );
         concSliders[ i ]->setPageStep( 100 );
         concSliders[ i ]->setValue( (int)(1000 * thisEle->getStartConc()) );

         concVals.push_back( safeNew( QLabel( QString::number( thisEle->getStartConc() ) ) ) );
         concVals[ i ]->setAlignment( Qt::AlignRight );

         //connect( concSliders[ i ], SIGNAL( valueChanged( int ) ), concVals[ i ], SLOT( setNum( int ) ) );

         i++;
      }
   }

   QGridLayout* eleLayout = safeNew( QGridLayout() );
   for( unsigned int i = 0; i < sim->periodicTable.size() - 1; i++ )
   {
      eleLayout->addWidget( removeEleBtns[ i ], i, 0 );
      eleLayout->addWidget( colorChips[ i ], i, 1 );
      eleLayout->addWidget( eleNames[ i ], i , 2 );
      eleLayout->addWidget( concSliders[ i ], i , 3 );
      eleLayout->addWidget( concVals[ i ], i , 4 );
   }
   ctrlLayout->addLayout( eleLayout );

   // Vertical stretch
   ctrlLayout->addStretch( 1 );

   // Start / Pause / Resume buttons
   startBtn = safeNew( QPushButton( "&Start" ) );
   pauseBtn = safeNew( QPushButton( "Pau&se" ) );
   resumeBtn = safeNew( QPushButton( "Re&sume" ) );

   stackedBtnLayout = safeNew( QStackedLayout() );
   stackedBtnLayout->addWidget( startBtn );
   stackedBtnLayout->addWidget( pauseBtn );
   stackedBtnLayout->addWidget( resumeBtn );
   stackedBtnLayout->setCurrentWidget( startBtn );
   ctrlLayout->addLayout( stackedBtnLayout );

   // Quit button
   QPushButton* quitBtn = safeNew( QPushButton( "&Quit" ) );
   ctrlLayout->addWidget( quitBtn );

   connect( quitBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

   return ctrlFrame;
}


// Set up the widgets and connections for the
// viewer
QFrame*
Window::createViewer()
{
   QFrame* viewerFrame = safeNew( QFrame() );
   QVBoxLayout* viewerLayout = safeNew( QVBoxLayout() );
   viewerFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
   viewerFrame->setLayout( viewerLayout );

   // Viewer widget
   viewer = safeNew( Viewer( o, sim, this ) );
   viewerLayout->addWidget( viewer );

   return viewerFrame;
}


// Set up the widgets and connections for the
// plot
QFrame*
Window::createPlot()
{
   QFrame* plotFrame = safeNew( QFrame() );
   QVBoxLayout* plotLayout = safeNew( QVBoxLayout() );
   plotFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
   plotFrame->setLayout( plotLayout );

   // Plot widget
   plot = safeNew( Plot( o, sim, this ) );
   plotLayout->addWidget( plot );

   return plotFrame;
}


// Set up the widgets and connections for the
// status bar
void
Window::createStatusBar()
{
   statusLbl = safeNew( QLabel( "Ready" ) );
   statusLbl->show();
   statusBar()->addWidget( statusLbl );
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
      statusLbl->setText( "Iteration: " + QString::number( sim->getItersCompleted() ) + " of " + QString::number( o->maxIters )
            + " | " + QString::number( (int)( 100 * (double)sim->getItersCompleted() / (double)o->maxIters ) ) + "\% complete" );

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
      sim->tempFiles[ Options::FILE_CONFIG ]->remove();
      sim->tempFiles[ Options::FILE_CENSUS ]->remove();
      sim->tempFiles[ Options::FILE_DIFFUSION ]->remove();
      sim->tempFiles[ Options::FILE_RAND ]->remove();

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
               if( sim->tempFiles[ Options::FILE_CONFIG ]->copy( QDir( savePath ).filePath( "config.out" ) ) &&
                   sim->tempFiles[ Options::FILE_CENSUS ]->copy( QDir( savePath ).filePath( "census.out" ) ) &&
                   sim->tempFiles[ Options::FILE_DIFFUSION ]->copy( QDir( savePath ).filePath( "diffusion.out" ) ) &&
                   sim->tempFiles[ Options::FILE_RAND ]->copy( QDir( savePath ).filePath( "rand.out" ) ) )
               {
                  // If all copy steps were successful, remove the temporary
                  // files and return
                  sim->tempFiles[ Options::FILE_CONFIG ]->remove();
                  sim->tempFiles[ Options::FILE_CENSUS ]->remove();
                  sim->tempFiles[ Options::FILE_DIFFUSION ]->remove();
                  sim->tempFiles[ Options::FILE_RAND ]->remove();
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
            sim->tempFiles[ Options::FILE_CONFIG ]->remove();
            sim->tempFiles[ Options::FILE_CENSUS ]->remove();
            sim->tempFiles[ Options::FILE_DIFFUSION ]->remove();
            sim->tempFiles[ Options::FILE_RAND ]->remove();
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

