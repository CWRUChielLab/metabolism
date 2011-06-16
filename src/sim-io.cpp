/* sim-io.cpp
 */

#ifdef HAVE_QT
#include <QDir>
#endif
#include <cassert>
#include <cstdlib> // exit
#include <fstream>
#include <iomanip> // setw
#include <iostream>
#ifdef HAVE_NCURSES
#include <ncurses.h>
#endif
#include "boost-devices.h"
#include "sim.h"


#ifdef HAVE_NCURSES
// Declare and initialize a stream using the
// ncurses sink device
boost::iostreams::stream<ncurses_ostream> screen(0);
#endif


// Start writing data to files, set up ncurses
// if necessary, and print simulation details
// if verbose is enabled
void
Sim::initializeIO()
{
   static bool initialized = false;
   if( !initialized )
   {
      initialized = true;

      // Take an initial census
      writeCensus();

      // Set the time for the most recent progress report
      // printout to "a long time ago and well overdue"
      // (i.e., never).
      lastProgressUpdate = 0;

      if( o->gui == Options::GUI_NCURSES )
      {
#ifdef HAVE_NCURSES
         // Initialize ncurses
         initscr();

         // Print extra information about the simulation
         if( o->verbose )
         {
            screen << "Press Ctrl-c to quit." << std::endl;
            screen << "------" << std::endl;
            printEles( &screen );
            screen << std::endl;
            printRxns( &screen );
            screen << std::endl;
            printExtincts( &screen );
            screen << "------" << std::endl;
         }

         scrX = 0;
         scrY = 0;
         getyx( stdscr, scrY, scrX );
         printWorld();
#endif
      } else {
         // Print extra information about the simulation
         if( o->verbose )
         {
            std::cout << "Press Ctrl-c to quit." << std::endl;
            std::cout << "------" << std::endl;
            printEles( &std::cout );
            std::cout << std::endl;
            printRxns( &std::cout );
            std::cout << std::endl;
            printExtincts( &std::cout );
            std::cout << "------" << std::endl;
         }
      }
   }
}


// Prepares output streams for writing
void
Sim::openFiles()
{
   out = std::vector<std::ostream*>( Options::N_FILES );
#ifdef HAVE_QT
   tempFiles = std::vector<QTemporaryFile*>( Options::N_FILES );
#endif

   // Ignore default file names or file names read-in as
   // arguments if the Qt gui is being used
   if( o->gui == Options::GUI_QT )
   {
#ifdef HAVE_QT
      tempFiles[ Options::FILE_CONFIG ]    = new QTemporaryFile( QDir::temp().filePath( "metabolism.config.XXXXXX" ) );
      tempFiles[ Options::FILE_CENSUS ]    = new QTemporaryFile( QDir::temp().filePath( "metabolism.census.XXXXXX" ) );
      tempFiles[ Options::FILE_DIFFUSION ] = new QTemporaryFile( QDir::temp().filePath( "metabolism.diffusion.XXXXXX" ) );
      tempFiles[ Options::FILE_RAND ]      = new QTemporaryFile( QDir::temp().filePath( "metabolism.rand.XXXXXX" ) );

      if( tempFiles[ Options::FILE_CONFIG ]->open() &&
          tempFiles[ Options::FILE_CENSUS ]->open() &&
          tempFiles[ Options::FILE_DIFFUSION ]->open() &&
          tempFiles[ Options::FILE_RAND ]->open() )
      {
         o->filePaths[ Options::FILE_CONFIG ]    = tempFiles[ Options::FILE_CONFIG ]->fileName().toStdString();
         o->filePaths[ Options::FILE_CENSUS ]    = tempFiles[ Options::FILE_CENSUS ]->fileName().toStdString();
         o->filePaths[ Options::FILE_DIFFUSION ] = tempFiles[ Options::FILE_DIFFUSION ]->fileName().toStdString();
         o->filePaths[ Options::FILE_RAND ]      = tempFiles[ Options::FILE_RAND ]->fileName().toStdString();
      }
      else
      {
         std::cerr << "openFiles: unable to open temporary files in \"" << QDir::tempPath().toStdString() << "\"!" << std::endl;
         exit( EXIT_FAILURE );
      }

      // Open the Boost streams for writing indirectly to temporary files
      out[ Options::FILE_CONFIG ]    = new boost::iostreams::stream<QFile_ostream>( tempFiles[ Options::FILE_CONFIG ] );
      out[ Options::FILE_CENSUS ]    = new boost::iostreams::stream<QFile_ostream>( tempFiles[ Options::FILE_CENSUS ] );
      out[ Options::FILE_DIFFUSION ] = new boost::iostreams::stream<QFile_ostream>( tempFiles[ Options::FILE_DIFFUSION ] );
      out[ Options::FILE_RAND ]      = new boost::iostreams::stream<QFile_ostream>( tempFiles[ Options::FILE_RAND ] );
#endif
   }
   else
   {
      // Open the ofstreams for writing directly to permanent files
      out[ Options::FILE_CONFIG ]    = new std::ofstream( o->filePaths[ Options::FILE_CONFIG ].c_str() );
      out[ Options::FILE_CENSUS ]    = new std::ofstream( o->filePaths[ Options::FILE_CENSUS ].c_str() );
      out[ Options::FILE_DIFFUSION ] = new std::ofstream( o->filePaths[ Options::FILE_DIFFUSION ].c_str() );
      out[ Options::FILE_RAND ]      = new std::ofstream( o->filePaths[ Options::FILE_RAND ].c_str() );
   }

   // Check that the streams opened properly
   if( out[ Options::FILE_CONFIG ]->fail() )
   {
         std::cerr << "openFiles: unable to open file \"" << o->filePaths[ Options::FILE_CONFIG ] << "\"!" << std::endl;
         exit( EXIT_FAILURE );
   }
   if( out[ Options::FILE_CENSUS ]->fail() )
   {
         std::cerr << "openFiles: unable to open file \"" << o->filePaths[ Options::FILE_CENSUS ] << "\"!" << std::endl;
         exit( EXIT_FAILURE );
   }
   if( out[ Options::FILE_DIFFUSION ]->fail() )
   {
         std::cerr << "openFiles: unable to open file \"" << o->filePaths[ Options::FILE_DIFFUSION ] << "\"!" << std::endl;
         exit( EXIT_FAILURE );
   }
   if( out[ Options::FILE_RAND ]->fail() )
   {
         std::cerr << "openFiles: unable to open file \"" << o->filePaths[ Options::FILE_RAND ] << "\"!" << std::endl;
         exit( EXIT_FAILURE );
   }
}


// Clean up ncurses
void
Sim::killncurses()
{
   if( o->gui == Options::GUI_NCURSES )
   {
#ifdef HAVE_NCURSES
      endwin();
#endif
   }
}


// Load periodicTable, rxnTable, and initialTypes
// if available from loadFile
void
Sim::loadChemistry()
{
   std::string keyword;
   Element* tempEle;
   Reaction* tempRxn;

   if( o->loadFile.is_open() )
   {
      while( o->loadFile.good() )
      {
         // Remove line breaks before checking for comments
         while( o->loadFile.peek() == '\n' )
         {
            o->loadFile.ignore(1);
         }

         // Ignore the line if it begins with a '#' character
         if( o->loadFile.peek() == '#' )
         {
            o->loadFile.ignore(1024,'\n');
            continue;
         }

         o->loadFile >> keyword;

         // Read in Elements
         if( keyword == "ele")
         {
            std::string word;
            std::string name;
            char symbol;
            std::string color;
            double startConc;
            int set;

            o->loadFile >> name >> symbol >> color >> startConc;
            tempEle = new Element( name, symbol, color, startConc );
            periodicTable[ name ] = tempEle;
            elesLoaded = true;
            while( o->loadFile.peek() == ' ' )
            {
               word = o->loadFile.get();
            }
            if( o->loadFile.peek() != '\n' )
            {
               o->loadFile >> set;
               reservePositionSet( tempEle, set );
            }
            else
            {
               reservePositionSet( tempEle );
            }
         }

         // Read in Reactions
         if( keyword == "rxn")
         {
            std::string word;
            int n;
            double prob;
            StringCounter reactantCount;
            StringCounter productCount;
            ElementVector reactants, products;

            o->loadFile.exceptions( std::ifstream::failbit );
            o->loadFile >> prob;

            // Read in the names of reactants, adding
            // them up along the way
            word = "+";
            while( word == "+" )
            {
               n = 1;
               try
               {
                  o->loadFile >> n;
               }
               catch( std::ifstream::failure e )
               {
                  o->loadFile.clear();
                  n = 1;
               }
               o->loadFile >> word;
               for( int i = 0; i < n; i++ )
               {
                  if( word != "*" && word != "Solvent" )
                  {
                     if( periodicTable[ word ] != NULL )
                     {
                        reactantCount[ word ]++;
                     }
                     else
                     {
                        std::cerr << "Loading rxn: " << word << " is not a defined Element!" << std::endl;
                        exit( EXIT_FAILURE );
                     }
                  }
               }
               while( o->loadFile.peek() == ' ' )
               {
                  word = o->loadFile.get();
               }
               if( o->loadFile.peek() == '\n' )
               {
                  std::cerr << "Loading rxn: premature line-break, was expecting \"->\"!" << std::endl;
                  exit( EXIT_FAILURE );
                  break;
               }
               o->loadFile >> word;
            }

            // Ensure that the reactants and products
            // are divided by a reaction arrow
            if( word != "->" )
            {
               std::cerr << "Loading rxn: confused by \"" << word << "\", was expecting \"->\"!" << std::endl;
               exit( EXIT_FAILURE );
            }

            // Read in the names of products, adding
            // them up along the way
            word = "+";
            while( word == "+" )
            {
               n = 1;
               try
               {
                  o->loadFile >> n;
               }
               catch( std::ifstream::failure e )
               {
                  o->loadFile.clear();
                  n = 1;
               }
               o->loadFile >> word;
               for( int i = 0; i < n; i++ )
               {
                  if( word != "*" && word != "Solvent" )
                  {
                     if( periodicTable[ word ] != NULL )
                     {
                        productCount[ word ]++;
                     }
                     else
                     {
                        std::cerr << "Loading rxn: " << word << " is not a defined Element!" << std::endl;
                        exit( EXIT_FAILURE );
                     }
                  }
               }
               while( o->loadFile.peek() == ' ' )
               {
                  word = o->loadFile.get();
               }
               if( o->loadFile.peek() == '\n' )
               {
                  break;
               }
               o->loadFile >> word;
            }

            o->loadFile.exceptions( std::ifstream::goodbit );

            // Store the reactants and products in the ElementVectors,
            // adding Solvent as placeholders to balance the reaction
            // if necessary
            for( StringCounter::iterator i = reactantCount.begin(); i != reactantCount.end(); i++ )
            {
               word = i->first;
               n = i->second;
               for( int j = 0; j < n; j++ )
                  reactants.push_back( periodicTable[ word ] );
            }
            for( StringCounter::iterator i = productCount.begin(); i != productCount.end(); i++ )
            {
               word = i->first;
               n = i->second;
               for( int j = 0; j < n; j++ )
                  products.push_back( periodicTable[ word ] );
            }
            while( products.size() > reactants.size() )
            {
               reactants.push_back( periodicTable[ "Solvent" ] );
            }
            while( reactants.size() > products.size() )
            {
               products.push_back( periodicTable[ "Solvent" ] );
            }

            // Create the reaction and store it in the rxnTable
            tempRxn = new Reaction( reactants, products, prob );
            if( rxnTable.count( tempRxn->getKey() ) < MAX_RXNS_PER_SET_OF_REACTANTS )
            {
               rxnTable.insert( std::pair<int,Reaction*>( tempRxn->getKey(), tempRxn ) );
               rxnsLoaded = true;
            }
            else
            {
               std::cerr << "Loading rxn: the limit of " << MAX_RXNS_PER_SET_OF_REACTANTS <<
                  " reactions per set of reactants has been exceeded!" << std::endl;
               exit( EXIT_FAILURE );
            }
         }

         // Read in extincts
         if( keyword == "extinct" )
         {
            ElementVector eleVector;
            std::string word;
            int n;

            o->loadFile >> n;
            for( int i = 0; i < n; i++ )
            {
               o->loadFile >> word;
               if( periodicTable[ word ] != NULL )
               {
                  eleVector.push_back( periodicTable[ word ] );
               }
               else
               {
                  std::cerr << "Loading extinct: " << word << " is not a defined Element!" << std::endl;
                  exit( EXIT_FAILURE );
               }
            }
            extinctionTypes.push_back( eleVector );
            extinctsLoaded = true;
         }

         keyword = "";
      }
   }
}


// Write to file all experimental parameters
void
Sim::writeConfig()
{
   // Write parameters to file
   *(out[ Options::FILE_CONFIG ]) << "version "   << GIT_TAG << std::endl;
   *(out[ Options::FILE_CONFIG ]) << "seed "      << o->seed << std::endl;
   *(out[ Options::FILE_CONFIG ]) << "iters "     << itersCompleted << std::endl;
   *(out[ Options::FILE_CONFIG ]) << "x "         << o->worldX << std::endl;
   *(out[ Options::FILE_CONFIG ]) << "y "         << o->worldY << std::endl;
   *(out[ Options::FILE_CONFIG ]) << "reactions " << (o->doRxns ? "on" : "off") << std::endl;
   *(out[ Options::FILE_CONFIG ]) << "shuffle "   << (o->doShuffle ? "on" : "off") << std::endl;
   *(out[ Options::FILE_CONFIG ]) << std::endl;

   // Write Elements to file
   printEles( out[ Options::FILE_CONFIG ] );
   *(out[ Options::FILE_CONFIG ]) << std::endl;

   // Write Reactions to file
   printRxns( out[ Options::FILE_CONFIG ] );
   *(out[ Options::FILE_CONFIG ]) << std::endl;

   // Write extinctionTypes to file
   printExtincts( out[ Options::FILE_CONFIG ] );
   *(out[ Options::FILE_CONFIG ]) << std::endl;
}


// Records important information about the state
// of the world and writes it to file
void
Sim::writeCensus()
{
   int colwidth = 12;
   int totalAtoms = 0;

   static bool initialized = false;
   if( !initialized )
   {
      initialized = true;

      out[ Options::FILE_CENSUS ]->flags(std::ios::left);
      *(out[ Options::FILE_CENSUS ]) << std::setw(colwidth) << "iter";
      for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
      {
         Element* ele = i->second;
         if( ele != periodicTable[ "Solvent" ] )
         {
            *(out[ Options::FILE_CENSUS ]) << std::setw(colwidth) << ele->getName().c_str();
         }
      }
      *(out[ Options::FILE_CENSUS ]) << std::setw(colwidth) << "total" << std::endl;
   }

   *(out[ Options::FILE_CENSUS ]) << std::setw(colwidth) << itersCompleted;
   for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
   {
      Element* ele = i->second;
      if( ele != periodicTable[ "Solvent" ] )
      {
         *(out[ Options::FILE_CENSUS ]) << std::setw(colwidth) << ele->count;
      }
      totalAtoms += ele->count;
   }
   *(out[ Options::FILE_CENSUS ]) << std::setw(colwidth) << totalAtoms << std::endl;
}


// Writes important information about the state
// of the world to file; to be called when the
// simulation ends
void
Sim::writeDiffusion()
{
   int colwidth = 12;

   out[ Options::FILE_DIFFUSION ]->flags(std::ios::left);
   *(out[ Options::FILE_DIFFUSION ]) << std::setw(colwidth) <<
      "type" << std::setw(colwidth) <<
      "x" << std::setw(colwidth) <<
      "y" << std::setw(colwidth) <<
      "dx_actual" << std::setw(colwidth) <<
      "dy_actual" << std::setw(colwidth) <<
      "dx_ideal" << std::setw(colwidth) <<
      "dy_ideal" << std::setw(colwidth) <<
      "collisions" << std::endl;
   for( int x = 0; x < o->worldX; x++ )
   {
      for( int y = 0; y < o->worldY; y++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
         {
            Atom* thisAtom = world[ getWorldIndex(x,y) ];
            *(out[ Options::FILE_DIFFUSION ]) << std::setw(colwidth) <<
               thisAtom->getType()->getName().c_str() << std::setw(colwidth) <<
               thisAtom->x << std::setw(colwidth) <<
               thisAtom->y << std::setw(colwidth) <<
               thisAtom->dx_actual << std::setw(colwidth) <<
               thisAtom->dy_actual << std::setw(colwidth) <<
               thisAtom->dx_ideal << std::setw(colwidth) <<
               thisAtom->dy_ideal << std::setw(colwidth) <<
               thisAtom->collisions << std::endl;
         }
      }
   }
}


// Print out the progress of the simulation
// at most once each second
void
Sim::reportProgress()
{
   if( time(NULL) - lastProgressUpdate > 0 )
   {
      lastProgressUpdate = time(NULL);
      if( o->gui == Options::GUI_NCURSES )
      {
#ifdef HAVE_NCURSES
         screen << "Iteration: " << itersCompleted << " of " << o->maxIters << " | ";
         screen << (int)( 100 * (double)itersCompleted / (double)o->maxIters ) << "\% complete" << std::endl;
         refresh();
#endif
      } else {
         std::cout << "                                                                          \r" << std::flush;
         std::cout << "Iteration: " << itersCompleted << " of " << o->maxIters << " | ";
         std::cout << (int)( 100 * (double)itersCompleted / (double)o->maxIters ) << "\% complete\r" << std::flush;
      }
   }
}


// Force a progress report
void
Sim::forceProgressReport()
{
   lastProgressUpdate = 0;
   reportProgress();
}


// Force a progress report and break the line
void
Sim::finishProgressReport()
{
   if( o->gui != Options::GUI_NCURSES )
   {
      forceProgressReport();
      std::cout << std::endl;
   }
}


// Print the world using ncurses
void
Sim::printWorld()
{
#ifdef HAVE_NCURSES
   // Move the cursor to the appropriate position
   // for printing with ncurses
   move( scrY, scrX );

   // Print top border
   for( int x = -1; x < o->worldX + 1; x++ )
      screen << ". ";
   screen << std::endl;

   // Print with origin at bottom left
   for( int y = o->worldY - 1; y >= 0; y-- )
   {
      // Print left border
      screen << ". ";

      // Print contents of world
      for( int x = 0; x < o->worldX; x++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
            screen << world[ getWorldIndex(x,y) ]->getType()->getSymbol() << " ";
         else
            screen << "  ";
      }
      
      // Print right border
      screen << ". ";
      screen << std::endl;
   }

   // Print bottom border
   for( int x = -1; x < o->worldX + 1; x++ )
      screen << ". ";
   screen << std::endl << std::endl;

   refresh();
#endif
}


// Print the list of Elements to an output
// stream
void
Sim::printEles( std::ostream* out )
{
   // Loop through the periodicTable, printing each Element
   for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
   {  
      Element* thisEle = i->second;
      if( thisEle->getName() != "Solvent" )
         *out <<  "ele " << thisEle->getName().c_str() << " " << thisEle->getSymbol() << " " << thisEle->getColor().c_str() << " " << thisEle->getStartConc() << " " << positionSets[ thisEle->getName() ] << std::endl;
   }
}


// Print the list of Reactions to an output
// stream
void
Sim::printRxns( std::ostream* out )
{
   // Loop through the rxnTable, printing each Reaction
   for( ReactionMap::iterator i = rxnTable.begin(); i != rxnTable.end(); i++ )
   {
      Reaction* thisRxn = i->second;

      // Count up the number of each type of reactant and product
      StringCounter reactantCount;
      StringCounter productCount;
      for( unsigned int j = 0; j < thisRxn->getReactants().size(); j++ )
      {
         if( thisRxn->getReactants()[j]->getName() != "Solvent" )
            reactantCount[ thisRxn->getReactants()[j]->getName() ]++;
      }
      for( unsigned int j = 0; j < thisRxn->getProducts().size(); j++ )
      {
         if( thisRxn->getProducts()[j]->getName() != "Solvent" )
            productCount[ thisRxn->getProducts()[j]->getName() ]++;
      }

      // Print the "rxn" keyword and the probability
      *out << "rxn " << thisRxn->getProb() << " ";

      // Print the reactants, grouping copies of one type together
      // with stoichiometric coefficients
      int coefficient;
      std::string name;
      if( reactantCount.empty() )
      {
         *out << "*";
      }
      else
      {
         for( StringCounter::iterator j = reactantCount.begin(); j != reactantCount.end(); j++ )
         {
            coefficient = j->second;
            name = j->first;
            if( coefficient == 1 )
            {
               if( j == reactantCount.begin() )
                  *out << name.c_str();
               else
                  *out << " + " << name.c_str();
            }
            else
            {
               if( j == reactantCount.begin() )
                  *out << coefficient << " " << name.c_str();
               else
                  *out << " + " << coefficient << " " << name.c_str();
            }
         }
      }

      // Print the reaction arrow, separating reactants from products
      *out << " -> ";

      // Print the products, grouping copies of one type together
      // with stoichiometric coefficients
      if( productCount.empty() )
      {
         *out << "*";
      }
      else
      {
         for( StringCounter::iterator j = productCount.begin(); j != productCount.end(); j++ )
         {
            coefficient = j->second;
            name = j->first;
            if( coefficient == 1 )
            {
               if( j == productCount.begin() )
                  *out << name.c_str();
               else
                  *out << " + " << name.c_str();
            }
            else
            {
               if( j == productCount.begin() )
                  *out << coefficient << " " << name.c_str();
               else
                  *out << " + " << coefficient << " " << name.c_str();
            }
         }
      }

      // End the line
      *out << std::endl;
   }
}


// Print the list of extincts to an output
// stream
void
Sim::printExtincts( std::ostream* out )
{
   for( std::list<ElementVector>::iterator i = extinctionTypes.begin(); i != extinctionTypes.end(); i++ )
   {
      ElementVector thisEleVector = *(i);
      *out << "extinct " << thisEleVector.size() << " ";

      for( unsigned int j = 0; j < thisEleVector.size(); j++ )
         *out << thisEleVector[j]->getName().c_str() << " ";

      *out << std::endl;
   }
}

