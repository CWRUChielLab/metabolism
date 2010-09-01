/* window.cpp
 */

#ifdef HAVE_QT

#include <cstdlib> // exit
#include <iostream>
#include <QDir>
#include "window.h"


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
   QHBoxLayout* mainLayout = new QHBoxLayout();
   mainLayout->addWidget( createCtrl() );
   mainLayout->addWidget( createViewer() );
   mainLayout->addWidget( createPlot(), 1 );
   setStatusBar( createStatusBar() );

   QWidget* mainWidget = new QWidget();
   mainWidget->setLayout( mainLayout );
   setCentralWidget( mainWidget );
   setWindowTitle( "Chemical Metabolism Simulator" );

   // Set the initial keyboard focus to the start button
   startBtn->setFocus();
}


// Set up the widgets and connections for the
// control panel
QFrame*
Window::createCtrl()
{
   QFrame* ctrlFrame = new QFrame();
   QVBoxLayout* ctrlLayout = new QVBoxLayout();
   ctrlFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
   ctrlFrame->setLayout( ctrlLayout );

   // Header label
   QLabel* ctrlHeader = new QLabel( "<b>Control Panel</b>" );
   ctrlHeader->setAlignment( Qt::AlignHCenter );
   ctrlLayout->addWidget( ctrlHeader );

   // Iterations controller
   itersLbl = new QLabel( "&Iterations" );
   itersLbl->setToolTip( "Iterations" );
   itersLbl->setMinimumSize( itersLbl->sizeHint() );

   itersSlider = new QSlider( Qt::Horizontal );
   itersSlider->setMinimumWidth( 100 );
   itersSlider->setRange( 1, 1000000 );
   itersSlider->setPageStep( 1000 );
   itersSlider->setValue( o->maxIters );
   itersSlider->setToolTip( "Iterations" );

   itersLbl->setBuddy( itersSlider );

   itersVal = new QLabel( QString::number( itersSlider->maximum() ) );
   itersVal->setAlignment( Qt::AlignRight );
   itersVal->setMinimumSize( itersVal->sizeHint() );
   itersVal->setText( QString::number( o->maxIters ) );

   QHBoxLayout* itersLayout = new QHBoxLayout();
   itersLayout->addWidget( itersLbl );
   itersLayout->addWidget( itersSlider );
   itersLayout->addWidget( itersVal );
   ctrlLayout->addLayout( itersLayout );

   connect( itersSlider, SIGNAL( valueChanged( int ) ), this, SLOT( updateIters( int ) ) );

   // Lattice width controller
   xLbl = new QLabel( "&Width" );
   xLbl->setToolTip( "Width" );
   xLbl->setMinimumSize( itersLbl->sizeHint() );

   xSlider = new QSlider( Qt::Horizontal );
   xSlider->setMinimumWidth( 100 );
   xSlider->setRange( 1, 500 );
   xSlider->setPageStep( 100 );
   xSlider->setValue( o->worldX );
   xSlider->setToolTip( "Width" );

   xLbl->setBuddy( xSlider );

   xVal = new QLabel( QString::number( o->worldX ) );
   xVal->setAlignment( Qt::AlignRight );
   xVal->setMinimumSize( itersVal->sizeHint() );

   QHBoxLayout* xLayout = new QHBoxLayout();
   xLayout->addWidget( xLbl );
   xLayout->addWidget( xSlider );
   xLayout->addWidget( xVal );
   ctrlLayout->addLayout( xLayout );

   connect( xSlider, SIGNAL( valueChanged( int ) ), this, SLOT( updateWidth( int ) ) );

   // Lattice height controller
   yLbl = new QLabel( "&Height" );
   yLbl->setToolTip( "Height" );
   yLbl->setMinimumSize( itersLbl->sizeHint() );

   ySlider = new QSlider( Qt::Horizontal );
   ySlider->setMinimumWidth( 100 );
   ySlider->setRange( 1, 500 );
   ySlider->setPageStep( 100 );
   ySlider->setValue( o->worldX );
   ySlider->setToolTip( "Height" );

   yLbl->setBuddy( ySlider );

   yVal = new QLabel( QString::number( o->worldX ) );
   yVal->setAlignment( Qt::AlignRight );
   yVal->setMinimumSize( itersVal->sizeHint() );

   QHBoxLayout* yLayout = new QHBoxLayout();
   yLayout->addWidget( yLbl );
   yLayout->addWidget( ySlider );
   yLayout->addWidget( yVal );
   ctrlLayout->addLayout( yLayout );

   connect( ySlider, SIGNAL( valueChanged( int ) ), this, SLOT( updateHeight( int ) ) );

   // Seed controller
   seedLbl = new QLabel( "See&d" );
   seedLbl->setToolTip( "Seed" );

   seedVal = new QLineEdit( QString::number( o->seed ) );
   seedVal->setValidator( new QIntValidator( this ) );
   seedVal->setToolTip( "Seed" );

   seedLbl->setBuddy( seedVal );

   seedBtn = new QPushButton( "Get &New" );
   seedBtn->setToolTip( "Seed" );

   QHBoxLayout* seedLayout = new QHBoxLayout();
   seedLayout->addWidget( seedLbl );
   seedLayout->addWidget( seedVal );
   seedLayout->addWidget( seedBtn );
   ctrlLayout->addLayout( seedLayout );

   connect( seedVal, SIGNAL( textChanged( QString ) ), this, SLOT( updateSeed( QString ) ) );
   connect( seedBtn, SIGNAL( clicked() ), this, SLOT( generateSeed() ) );

   // Element controllers
   QSignalMapper* removeEleBtnMapper = new QSignalMapper();
   QSignalMapper* colorChipMapper = new QSignalMapper();
   QSignalMapper* eleNameMapper = new QSignalMapper();
   QSignalMapper* concSliderMapper = new QSignalMapper();

   int i = 0;
   for( ElementMap::iterator j = sim->periodicTable.begin(); j != sim->periodicTable.end(); j++ )
   {
      Element* thisEle = j->second;
      if( thisEle != sim->periodicTable[ "Solvent" ] )
      {
         eles.push_back( thisEle );

         removeEleBtns.push_back( new QPushButton( QApplication::style()->standardIcon( QStyle::SP_TrashIcon ), "" ) );
         removeEleBtns[ i ]->setFixedHeight( removeEleBtns[ i ]->sizeHint().height() );
         removeEleBtns[ i ]->setFixedWidth( removeEleBtns[ i ]->sizeHint().height() );

         connect( removeEleBtns[ i ], SIGNAL( clicked() ), removeEleBtnMapper, SLOT( map() ) );
         removeEleBtnMapper->setMapping( removeEleBtns[ i ], i );

         colorChips.push_back( new QPushButton() );
         colorChips[ i ]->setFixedHeight( colorChips[ i ]->sizeHint().height() );
         colorChips[ i ]->setFixedWidth( colorChips[ i ]->sizeHint().height() );
         colorChips[ i ]->setStyleSheet( "background: " + QString( thisEle->getColor().c_str() ) );

         connect( colorChips[ i ], SIGNAL( clicked() ), colorChipMapper, SLOT( map() ) );
         colorChipMapper->setMapping( colorChips[ i ], i );

         eleNames.push_back( new QLineEdit( thisEle->getName().c_str() ) );
         eleNames[ i ]->setMinimumWidth( 100 );
         eleNames[ i ]->setValidator( new QRegExpValidator( QRegExp( "\\S+" ), this ) );

         connect( eleNames[ i ], SIGNAL( textChanged( QString ) ), eleNameMapper, SLOT( map() ) );
         eleNameMapper->setMapping( eleNames[ i ], i );

         concSliders.push_back( new QSlider( Qt::Horizontal ) );
         concSliders[ i ]->setMinimumWidth( 100 );
         concSliders[ i ]->setRange( 0, 1000 );
         concSliders[ i ]->setPageStep( 100 );
         concSliders[ i ]->setValue( (int)(1000 * thisEle->getStartConc() + 0.5) );

         connect( concSliders[ i ], SIGNAL( valueChanged( int ) ), concSliderMapper, SLOT( map() ) );
         concSliderMapper->setMapping( concSliders[ i ], i );

         concVals.push_back( new QLabel( "0.999" ) );
         concVals[ i ]->setAlignment( Qt::AlignRight );
         concVals[ i ]->setFixedWidth( concVals[ i ]->sizeHint().width() );
         concVals[ i ]->setNum( thisEle->getStartConc() );

         i++;
      }
   }
   connect( removeEleBtnMapper, SIGNAL( mapped( int ) ), this, SLOT( removeElement( int ) ) );
   connect( colorChipMapper, SIGNAL( mapped( int ) ), this, SLOT( updateEleColor( int ) ) );
   connect( eleNameMapper, SIGNAL( mapped( int ) ), this, SLOT( updateEleName( int ) ) );
   connect( concSliderMapper, SIGNAL( mapped( int ) ), this, SLOT( updateEleConc( int ) ) );

   QGridLayout* eleLayout = new QGridLayout();
   for( unsigned int i = 0; i < eles.size(); i++ )
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
   startBtn = new QPushButton( "&Start" );
   pauseBtn = new QPushButton( "Pau&se" );
   resumeBtn = new QPushButton( "Re&sume" );

   stackedBtnLayout = new QStackedLayout();
   stackedBtnLayout->addWidget( startBtn );
   stackedBtnLayout->addWidget( pauseBtn );
   stackedBtnLayout->addWidget( resumeBtn );
   stackedBtnLayout->setCurrentWidget( startBtn );
   ctrlLayout->addLayout( stackedBtnLayout );

   connect( startBtn, SIGNAL( clicked() ), this, SLOT( startPauseResume() ) );
   connect( pauseBtn, SIGNAL( clicked() ), this, SLOT( startPauseResume() ) );
   connect( resumeBtn, SIGNAL( clicked() ), this, SLOT( startPauseResume() ) );

   // Quit button
   quitBtn = new QPushButton( "&Quit" );
   ctrlLayout->addWidget( quitBtn );

   connect( quitBtn, SIGNAL( clicked() ), this, SLOT( close() ) );

   ctrlFrame->setFixedWidth( ctrlFrame->sizeHint().width() );
   return ctrlFrame;
}


// Set up the widgets and connections for the
// viewer
QFrame*
Window::createViewer()
{
   QFrame* viewerFrame = new QFrame();
   QVBoxLayout* viewerLayout = new QVBoxLayout();
   viewerFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
   viewerFrame->setLayout( viewerLayout );

   // Viewer widget
   viewer = new Viewer( o, sim, this );
   viewerLayout->addWidget( viewer );

   return viewerFrame;
}


// Set up the widgets and connections for the
// plot
QFrame*
Window::createPlot()
{
   QFrame* plotFrame = new QFrame();
   QVBoxLayout* plotLayout = new QVBoxLayout();
   plotFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
   plotFrame->setLayout( plotLayout );

   // Plot widget
   plot = new Plot( o, sim, this );
   plot->setMinimumWidth( 350 );
   plotLayout->addWidget( plot );

   return plotFrame;
}


// Set up the widgets and connections for the
// status bar
QStatusBar*
Window::createStatusBar()
{
   QStatusBar* statusBar = new QStatusBar();

   statusLbl = new QLabel( "Ready" );
   statusLbl->show();
   statusBar->addWidget( statusLbl );

   return statusBar;
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

   startPauseResume();
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


// ...
void
Window::startPauseResume()
{
   switch( stackedBtnLayout->currentIndex() )
   {
      case 0: // startBtn
         simStarted = true;
         simPaused = false;

         itersLbl->setEnabled( false );
         itersSlider->setEnabled( false );
         itersVal->setEnabled( false );

         xLbl->setEnabled( false );
         xSlider->setEnabled( false );
         xVal->setEnabled( false );

         yLbl->setEnabled( false );
         ySlider->setEnabled( false );
         yVal->setEnabled( false );

         seedLbl->setEnabled( false );
         seedVal->setEnabled( false );
         seedBtn->setEnabled( false );

         for( unsigned int i = 0; i < eles.size(); i++ )
         {
            removeEleBtns[ i ]->setEnabled( false );
            eleNames[ i ]->setEnabled( false );
            concSliders[ i ]->setEnabled( false );
            concVals[ i ]->setEnabled( false );
         }

         stackedBtnLayout->setCurrentWidget( pauseBtn );

         viewer->startPaint();
         plot->startup();
         runSim();

         break;
      case 1: // pauseBtn
         simStarted = true;
         simPaused = true;

         itersLbl->setEnabled( true );
         itersSlider->setEnabled( true );
         itersSlider->setMinimum( sim->getItersCompleted() );
         itersVal->setEnabled( true );

         stackedBtnLayout->setCurrentWidget( resumeBtn );

         if( o->progress )
            sim->forceProgressReport();

         break;
      case 2: // resumeBtn
         simStarted = true;
         simPaused = false;

         itersLbl->setEnabled( false );
         itersSlider->setEnabled( false );
         itersVal->setEnabled( false );

         stackedBtnLayout->setCurrentWidget( pauseBtn );

         runSim();

         break;
      default:
         std::cerr << "updateStackedBtn: current widget unknown!" << std::endl;
         exit( EXIT_FAILURE );
         break;
   }
}


// ...
void
Window::updateIters( int newVal )
{
   itersVal->setNum( newVal );
   o->maxIters = newVal;
}


// ...
void
Window::updateWidth( int newVal )
{
   xVal->setNum( newVal );
   sim->destroyWorld();
   o->worldX = newVal;
   sim->buildWorld();
   viewer->updateGL();
}


// ...
void
Window::updateHeight( int newVal )
{
   yVal->setNum( newVal );
   sim->destroyWorld();
   o->worldY = newVal;
   sim->buildWorld();
   viewer->updateGL();
}


// ...
void
Window::updateSeed( QString newVal )
{
   o->seed = newVal.toInt();
   sim->destroyWorld();
   sim->buildWorld();
   viewer->updateGL();
}


// ...
void
Window::generateSeed()
{
   seedVal->setText( QString::number( time(NULL) ) );
}


// ...
void
Window::removeElement( int eleIndex )
{
   std::cout << "remove " << eleIndex << std::endl;
}


// ...
void
Window::updateEleColor( int eleIndex )
{
   QColor newColor = QColorDialog::getColor( QColor( eles[ eleIndex ]->getColor().c_str() ), this,
      "Select a color for \"" + QString( eles[ eleIndex ]->getName().c_str() ) + "\"" );

   if( newColor.isValid() )
   {
      eles[ eleIndex ]->setColor( newColor.name().toStdString() );
      colorChips[ eleIndex ]->setStyleSheet( "background: " + QString( eles[ eleIndex ]->getColor().c_str() ) );
      viewer->updateGL();
      if( sim->getItersCompleted() > 0 )
         plot->update();
   }
}


// ...
void
Window::updateEleName( int eleIndex )
{
   std::string newName = eleNames[ eleIndex ]->text().toStdString();
   Element* thisEle = eles[ eleIndex ];
   sim->periodicTable.erase( thisEle->getName() );
   thisEle->setName( newName );
   sim->periodicTable[ thisEle->getName() ] = thisEle;
}


// ...
void
Window::updateEleConc( int eleIndex )
{
   double newConc = (double)concSliders[ eleIndex ]->value() / (double)1000;
   concVals[ eleIndex ]->setNum( newConc );

   eles[ eleIndex ]->setStartConc( newConc );
   sim->destroyWorld();
   sim->buildWorld();
   viewer->updateGL();
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
            std::cerr << "save: Unknown dialog return value!" << std::endl;
            exit( EXIT_FAILURE );
            break;
      }
   }
}

#endif /* HAVE_QT */

