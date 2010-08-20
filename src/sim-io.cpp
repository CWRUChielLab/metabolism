/* sim-io.cpp
 */

#include <boost/iostreams/stream.hpp>
#include <cassert>
#include <cstdlib> // exit
#include <fstream>
#include <iomanip> // setw
#include <iostream>
#ifdef HAVE_NCURSES
#include <ncurses.h>
#endif
#include "safecalls.h"
#include "sim.h"
using namespace SafeCalls;


#ifdef HAVE_NCURSES
// Define a sink device for sending input to the
// ncurses function addnstr() which writes to the
// screen (the constructor requires something be
// passed as an argument)
class ncurses_stream : public boost::iostreams::sink
{
   public:
      ncurses_stream(int) {};
      std::streamsize write( const char* s, std::streamsize n )
      {
         addnstr( s, n );
         return n;
      }
};

// Declare and initialize a stream using the
// ncurses sink device
boost::iostreams::stream<ncurses_stream> screen(0);
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

      // Open the census file and take an initial survey
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
            printInits( &screen );
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
            printInits( &std::cout );
            std::cout << std::endl;
            printExtincts( &std::cout );
            std::cout << "------" << std::endl;
         }
      }
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
         o->loadFile >> keyword;

         // Read in Elements
         if( keyword == "ele")
         {
            std::string name;
            char symbol;
            std::string color;

            o->loadFile >> name >> symbol >> color;
            tempEle = safeNew( Element( name, symbol, color ) );
            periodicTable[ name ] = tempEle;
            elesLoaded = true;
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
                        std::cout << "Loading rxn: " << word << " is not a defined Element!" << std::endl;
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
                  std::cout << "Loading rxn: premature line-break, was expecting \"->\"!" << std::endl;
                  exit( EXIT_FAILURE );
                  break;
               }
               o->loadFile >> word;
            }

            // Ensure that the reactants and products
            // are divided by a reaction arrow
            if( word != "->" )
            {
               std::cout << "Loading rxn: confused by \"" << word << "\", was expecting \"->\"!" << std::endl;
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
                        std::cout << "Loading rxn: " << word << " is not a defined Element!" << std::endl;
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
            tempRxn = safeNew( Reaction( reactants, products, prob ) );
            if( rxnTable.count( tempRxn->getKey() ) < MAX_RXNS_PER_SET_OF_REACTANTS )
            {
               rxnTable.insert( std::pair<int,Reaction*>( tempRxn->getKey(), tempRxn ) );
               rxnsLoaded = true;
            }
            else
            {
               std::cout << "Loading rxn: the limit of " << MAX_RXNS_PER_SET_OF_REACTANTS <<
                  " reactions per set of reactants has been exceeded!" << std::endl;
               exit( EXIT_FAILURE );
            }
         }

         // Read in inits
         if( keyword == "init" )
         {
            std::string word;
            int n;

            o->loadFile >> n;
            for( int i = 0; i < n; i++ )
            {
               o->loadFile >> word;
               if( periodicTable[ word ] != NULL )
               {
                  initialTypes.push_back( periodicTable[ word ] );
               }
               else
               {
                  std::cout << "Loading init: " << word << " is not a defined Element!" << std::endl;
                  exit( EXIT_FAILURE );
               }
            }
            if( initsLoaded )
            {
               std::cout << "Loading init: only one init keyword is permitted!" << std::endl;
               exit( EXIT_FAILURE );
            }

            initsLoaded = true;
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
                  std::cout << "Loading extinct: " << word << " is not a defined Element!" << std::endl;
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


// Records important information about the state
// of the world and writes it to file
void
Sim::writeCensus()
{
   if( !o->censusFile.is_open() )
   {
      std::cout << "writeCensus: file not open!" << std::endl;
      exit( EXIT_FAILURE );
   }

   int colwidth = 12;
   int totalAtoms = 0;

   static bool initialized = false;
   if( !initialized )
   {
      initialized = true;

      o->censusFile.flags(std::ios::left);
      o->censusFile << std::setw(colwidth) << "iter";
      for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
      {
         Element* ele = i->second;
         if( ele != periodicTable[ "Solvent" ] )
         {
            o->censusFile << std::setw(colwidth) << ele->getName();
         }
      }
      o->censusFile << std::setw(colwidth) << "total" << std::endl;
   }

   o->censusFile << std::setw(colwidth) << itersCompleted;
   for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
   {
      Element* ele = i->second;
      if( ele != periodicTable[ "Solvent" ] )
      {
         o->censusFile << std::setw(colwidth) << ele->count;
      }
      totalAtoms += ele->count;
   }
   o->censusFile << std::setw(colwidth) << totalAtoms << std::endl;
}


// Write to file all experimental parameters
void
Sim::writeConfig()
{
   if( !o->configFile.is_open() )
   {
      std::cout << "writeConfig: file not open!" << std::endl;
      exit( EXIT_FAILURE );
   }

   // Write parameters to file
   o->configFile << "version "   << GIT_TAG << std::endl;
   o->configFile << "seed "      << o->seed << std::endl;
   o->configFile << "iters "     << itersCompleted << std::endl;
   o->configFile << "x "         << o->worldX << std::endl;
   o->configFile << "y "         << o->worldY << std::endl;
   o->configFile << "atoms "     << o->atomCount << std::endl;
   o->configFile << "reactions " << (o->doRxns ? "on" : "off") << std::endl;
   o->configFile << "shuffle "   << (o->doShuffle ? "on" : "off") << std::endl;
   o->configFile << std::endl;

   // Write Elements to file
   printEles( &(o->configFile) );
   o->configFile << std::endl;

   // Write Reactions to file
   printRxns( &(o->configFile) );
   o->configFile << std::endl;

   // Write initialTypes to file
   printInits( &(o->configFile) );
   o->configFile << std::endl;

   // Write extinctionTypes to file
   printExtincts( &(o->configFile) );
   o->configFile << std::endl;

   o->configFile.close();
}


// Writes important information about the state
// of the world to file; to be called when the
// simulation ends
void
Sim::writeDiffusion()
{
   if( !o->diffusionFile.is_open() )
   {
      std::cout << "writeDiffusion: file not open!" << std::endl;
      exit( EXIT_FAILURE );
   }

   int colwidth = 12;

   o->diffusionFile.flags(std::ios::left);
   o->diffusionFile << std::setw(colwidth) <<
      "type" << std::setw(colwidth) <<
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
            o->diffusionFile << std::setw(colwidth) <<
               thisAtom->getType()->getName() << std::setw(colwidth) <<
               thisAtom->dx_actual << std::setw(colwidth) <<
               thisAtom->dy_actual << std::setw(colwidth) <<
               thisAtom->dx_ideal << std::setw(colwidth) <<
               thisAtom->dy_ideal << std::setw(colwidth) <<
               thisAtom->collisions << std::endl;
         }
      }
   }
   o->diffusionFile.close();
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
Sim::forceReportProgress()
{
   lastProgressUpdate = 0;
   reportProgress();
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
template <typename OutputStream>
void
Sim::printEles( OutputStream* out )
{
   // Loop through the periodicTable, printing each Element
   for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
   {  
      Element* thisEle = i->second;
      if( thisEle->getName() != "Solvent" )
         *out <<  "ele " << thisEle->getName() << " " << thisEle->getSymbol() << " " << thisEle->getColor() << std::endl;
   }
}


// Print the list of Reactions to an output
// stream
template <typename OutputStream>
void
Sim::printRxns( OutputStream* out )
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
                  *out << name;
               else
                  *out << " + " << name;
            }
            else
            {
               if( j == reactantCount.begin() )
                  *out << coefficient << " " << name;
               else
                  *out << " + " << coefficient << " " << name;
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
                  *out << name;
               else
                  *out << " + " << name;
            }
            else
            {
               if( j == productCount.begin() )
                  *out << coefficient << " " << name;
               else
                  *out << " + " << coefficient << " " << name;
            }
         }
      }

      // End the line
      *out << std::endl;
   }
}


// Print the list of inits to an output
// stream
template <typename OutputStream>
void
Sim::printInits( OutputStream* out )
{
   *out << "init " << initialTypes.size() << " ";

   for( unsigned int i = 0; i < initialTypes.size(); i++ )
      *out << initialTypes[i]->getName() << " ";

   *out << std::endl;
}


// Print the list of extincts to an output
// stream
template <typename OutputStream>
void
Sim::printExtincts( OutputStream* out )
{
   for( std::list<ElementVector>::iterator i = extinctionTypes.begin(); i != extinctionTypes.end(); i++ )
   {
      ElementVector thisEleVector = *(i);
      *out << "extinct " << thisEleVector.size() << " ";

      for( unsigned int j = 0; j < thisEleVector.size(); j++ )
         *out << thisEleVector[j]->getName() << " ";

      *out << std::endl;
   }
}

