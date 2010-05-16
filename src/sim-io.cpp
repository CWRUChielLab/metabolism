/* sim-io.cpp
 */

#include <cassert>
#include <fstream>
#include <iomanip> // setw
#include <iostream>
#ifdef HAVE_NCURSES
#include <ncurses.h>
#endif
#include "safecalls.h"
#include "sim.h"
using namespace SafeCalls;


// Load periodicTable, rxnTable, and initialTypes
// if available from loadFile
void
Sim::loadChemistry()
{
   std::ifstream load;
   std::string keyword;
   Element* tempEle;
   Reaction* tempRxn;

   if( o->loadFile != "" )
   {
      load.open( o->loadFile.c_str() );
      while( load.good() )
      {
         load >> keyword;
         if( keyword == "ele")
         {
            std::string name;
            char symbol;
            int color, charge;

            load >> name >> symbol >> color >> charge;
            tempEle = safeNew( Element( name, symbol, color, charge ) );
            periodicTable[ name ] = tempEle;
            elesLoaded++;
         }
         else
         {
            if( keyword == "rxn")
            {
               std::string word;
               int n;
               double prob;
               StringCounter reactantCount;
               StringCounter productCount;
               ElementVector reactants, products;

               load.exceptions( std::ifstream::failbit );
               load >> prob;

               // Read in the names of reactants, adding
               // them up along the way
               word = "+";
               while( word == "+" )
               {
                  n = 1;
                  try
                  {
                     load >> n;
                  }
                  catch( std::ifstream::failure e )
                  {
                     load.clear();
                  }
                  load >> word;
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
                           assert(0);
                        }
                     }
                  }
                  if( load.peek() == '\n' )
                  {
                     break;
                  }
                  load >> word;
               }

               // Ensure that the reactants and products
               // are divided by a reaction arrow
               if( word != "->" )
               {
                  std::cout << "Loading rxn: confused by \"" << word << "\", was expecting \"->\"!" << std::endl;
                  assert(0);
               }

               // Read in the names of products, adding
               // them up along the way
               word = "+";
               while( word == "+" )
               {
                  n = 1;
                  try
                  {
                     load >> n;
                  }
                  catch( std::ifstream::failure e )
                  {
                     load.clear();
                  }
                  load >> word;
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
                           assert(0);
                        }
                     }
                  }
                  while( load.peek() == ' ' )
                  {
                     word = load.get();
                  }
                  if( load.peek() == '\n' )
                  {
                     break;
                  }
                  load >> word;
               }

               load.exceptions( std::ifstream::goodbit );

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

               // Create the reaction and store it in the rxnTable;
               // if a Reaction with the same reactants already exists,
               // make this its second set of products
               tempRxn = safeNew( Reaction( reactants, products, prob ) );
               if( rxnTable[ tempRxn->getKey() ] == NULL )
               {
                  rxnTable[ tempRxn->getKey() ] = tempRxn;
               }
               else
               {
                  rxnTable[ tempRxn->getKey() ]->setSecondProducts( products );
                  rxnTable[ tempRxn->getKey() ]->setSecondProb( prob );
               }

               rxnsLoaded++;
            }
            else
            {
               if( keyword == "init" )
               {
                  std::string word;
                  int n;

                  load >> n;
                  for( int i = 0; i < n; i++ )
                  {
                     load >> word;
                     if( periodicTable[ word ] != NULL )
                     {
                        initialTypes.push_back( periodicTable[ word ] );
                     }
                     else
                     {
                        std::cout << "Loading init: " << word << " is not a defined Element!" << std::endl;
                        assert(0);
                     }
                  }
                  initsLoaded++;
                  if( initsLoaded > 1 )
                  {
                     std::cout << "Loading init: only one init keyword is permitted!" << std::endl;
                     assert(0);
                  }
               }
            }
         }
         keyword = "";
      }
   }
}


// Output all important experimental parameters
void
Sim::writeConfig()
{
   static std::ofstream configFile;
   configFile.open( o->configFile.c_str() );

   // Write parameters to file
   configFile << "version "   << GIT_TAG << std::endl;
   configFile << "seed "      << o->seed << std::endl;
   configFile << "iters "     << o->maxIters << std::endl;
   configFile << "x "         << o->worldX << std::endl;
   configFile << "y "         << o->worldY << std::endl;
   configFile << "atoms "     << o->atomCount << std::endl;
   configFile << "reactions " << (o->doRxns ? "on" : "off") << std::endl;
   configFile << "shuffle "   << (o->doShuffle ? "on" : "off") << std::endl;
   configFile << std::endl;

   // Write Elements to file
   printEles( &configFile );
   configFile << std::endl;

   // Write Reactions to file
   printRxns( &configFile );
   configFile << std::endl;

   // Write initialTypes to file
   printInits( &configFile );
   configFile << std::endl;

   configFile.close();
}


// Records important information about the state
// of the world and writes it to file.
void
Sim::writeCensus()
{
   int colwidth = 12;
   static int initialized = 0;
   static std::ofstream censusFile;
   int totalAtoms = 0;

   if( !initialized )
   {
      initialized = 1;
      censusFile.open( o->censusFile.c_str() );
      censusFile.flags(std::ios::left);
      censusFile << std::setw(colwidth) << "iter";
      for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
      {
         Element* ele = i->second;
         if( ele != periodicTable[ "Solvent" ] )
         {
            censusFile << std::setw(colwidth) << ele->getName();
         }
      }
      censusFile << std::setw(colwidth) << "total" << std::endl;
   }

   censusFile << std::setw(colwidth) << currentIter;
   for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
   {
      Element* ele = i->second;
      if( ele != periodicTable[ "Solvent" ] )
      {
         censusFile << std::setw(colwidth) << ele->getCount();
      }
      totalAtoms += ele->getCount();
   }
   censusFile << std::setw(colwidth) << totalAtoms << std::endl;
}


// Writes important information about the state
// of the world to file.  To be called when the
// simulation ends.
void
Sim::writeDiffusion()
{
   int colwidth = 12;
   std::ofstream diffusionFile;
   diffusionFile.open( o->diffusionFile.c_str() );
   diffusionFile.flags(std::ios::left);
   diffusionFile << std::setw(colwidth) <<
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
            diffusionFile << std::setw(colwidth) <<
                             thisAtom->getType()->getName() << std::setw(colwidth) <<
                             thisAtom->getDxActual() << std::setw(colwidth) <<
                             thisAtom->getDyActual() << std::setw(colwidth) <<
                             thisAtom->getDxIdeal() << std::setw(colwidth) <<
                             thisAtom->getDyIdeal() << std::setw(colwidth) <<
                             thisAtom->getCollisions() << std::endl;
         }
      }
   }
   diffusionFile.close();
}


void
Sim::printWorld()
{
#ifdef HAVE_NCURSES
   for( int y = 0; y < o->worldY; y++ )
   {
      for( int x = 0; x < o->worldX; x++ )
      {
         if( world[ getWorldIndex(x,y) ] != NULL )
            printw( "%c ", world[ getWorldIndex(x,y) ]->getType()->getSymbol() );
         else
            printw( ". " );
      }
      printw( "\n" );
   }
   printw( "\n" );
   refresh();
#endif
}


void
Sim::printEles( std::ostream* out )
{
   for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
   {
      Element* ele = i->second;
      if( ele->getName() != "Solvent" )
      {
         if( out == (std::ostream*)(NULL) )
         {
#ifdef HAVE_NCURSES
            printw( "ele %s %c %d %d\n", ele->getName().c_str(), ele->getSymbol(), ele->getColor(), ele->getCharge() );
#endif
         }
         else
         {
            *out <<  "ele " << ele->getName() << " " << ele->getSymbol() << " " << ele->getColor() << " " << ele->getCharge() << std::endl;
         }
      }
   }
}


void
Sim::printRxns( std::ostream* out )
{
   // Loop through the rxnTable, printing each Reaction
   for( ReactionMap::iterator i = rxnTable.begin(); i != rxnTable.end(); i++ )
   {
      Reaction* rxn = i->second;
      if( rxn != NULL )
      {
         // Determine how many sets of products this reaction has
         int numProductSets;
         if( !rxn->getSecondProducts().empty() )
            numProductSets = 2;
         else
            numProductSets = 1;

         // Loop through each set of products, printing a separate
         // line for each reactant-product pair
         for( int j = 0; j < numProductSets; j++ )
         {
            double currentProb;
            ElementVector currentProducts;
            switch( j )
            {
               case 0:
                  currentProb = rxn->getFirstProb();
                  currentProducts = rxn->getFirstProducts();
                  break;
               case 1:
                  currentProb = rxn->getSecondProb();
                  currentProducts = rxn->getSecondProducts();
                  break;
               default:
                  assert(0);
            }

            // Count up the number of each type of reactant and product
            StringCounter reactantCount;
            StringCounter currentProductCount;
            for( unsigned int k = 0; k < rxn->getReactants().size(); k++ )
            {
               if( rxn->getReactants()[k]->getName() != "Solvent" )
                  reactantCount[ rxn->getReactants()[k]->getName() ]++;
            }
            for( unsigned int k = 0; k < currentProducts.size(); k++ )
            {
               if( currentProducts[k]->getName() != "Solvent" )
                  currentProductCount[ currentProducts[k]->getName() ]++;
            }

            // Print the "rxn" keyword and the probability
            if( out == (std::ostream*)(NULL) )
            {
#ifdef HAVE_NCURSES
               printw( "rxn %f ", currentProb );
#endif
            }
            else
            {
               *out << "rxn " << currentProb << " ";
            }

            // Print the reactants, grouping copies of one type together
            // with stoichiometric coefficients
            int coefficient;
            std::string name;
            if( reactantCount.empty() )
            {
               if( out == (std::ostream*)(NULL) )
               {
#ifdef HAVE_NCURSES
                  printw( "*" );
#endif
               }
               else
               {
                  *out << "*";
               }
            }
            else
            {
               for( StringCounter::iterator k = reactantCount.begin(); k != reactantCount.end(); k++ )
               {
                  coefficient = k->second;
                  name = k->first;
                  if( coefficient == 1 )
                  {
                     if( k == reactantCount.begin() )
                     {
                        if( out == (std::ostream*)(NULL) )
                        {
#ifdef HAVE_NCURSES
                           printw( "%s", name.c_str() );
#endif
                        }
                        else
                        {
                           *out << name;
                        }
                     }
                     else
                     {
                        if( out == (std::ostream*)(NULL) )
                        {
#ifdef HAVE_NCURSES
                           printw( " + %s", name.c_str() );
#endif
                        }
                        else
                        {
                           *out << " + " << name;
                        }
                     }
                  }
                  else
                  {
                     if( k == reactantCount.begin() )
                     {
                        if( out == (std::ostream*)(NULL) )
                        {
#ifdef HAVE_NCURSES
                           printw( "%d %s", coefficient, name.c_str() );
#endif
                        }
                        else
                        {
                           *out << coefficient << " " << name;
                        }
                     }
                     else
                     {
                        if( out == (std::ostream*)(NULL) )
                        {
#ifdef HAVE_NCURSES
                           printw( " + %d %s", coefficient, name.c_str() );
#endif
                        }
                        else
                        {
                           *out << " + " << coefficient << " " << name;
                        }
                     }
                  }
               }
            }

            // Print the reaction arrow, separating reactants from products
            if( out == (std::ostream*)(NULL) )
            {
#ifdef HAVE_NCURSES
               printw( " -> " );
#endif
            }
            else
            {
               *out << " -> ";
            }

            // Print the products, grouping copies of one type together
            // with stoichiometric coefficients
            if( currentProductCount.empty() )
            {
               if( out == (std::ostream*)(NULL) )
               {
#ifdef HAVE_NCURSES
                  printw( "*" );
#endif
               }
               else
               {
                  *out << "*";
               }
            }
            else
            {
               for( StringCounter::iterator k = currentProductCount.begin(); k != currentProductCount.end(); k++ )
               {
                  coefficient = k->second;
                  name = k->first;
                  if( coefficient == 1 )
                  {
                     if( k == currentProductCount.begin() )
                     {
                        if( out == (std::ostream*)(NULL) )
                        {
#ifdef HAVE_NCURSES
                           printw( "%s", name.c_str() );
#endif
                        }
                        else
                        {
                           *out << name;
                        }
                     }
                     else
                     {
                        if( out == (std::ostream*)(NULL) )
                        {
#ifdef HAVE_NCURSES
                           printw(" + %s", name.c_str() );
#endif
                        }
                        else
                        {
                           *out << " + " << name;
                        }
                     }
                  }
                  else
                  {
                     if( k == currentProductCount.begin() )
                     {
                        if( out == (std::ostream*)(NULL) )
                        {
#ifdef HAVE_NCURSES
                           printw( "%d %s", coefficient, name.c_str() );
#endif
                        }
                        else
                        {
                           *out << coefficient << " " << name;
                        }
                     }
                     else
                     {
                        if( out == (std::ostream*)(NULL) )
                        {
#ifdef HAVE_NCURSES
                           printw( " + %d %s", coefficient, name.c_str() );
#endif
                        }
                        else
                        {
                           *out << " + " << coefficient << " " << name;
                        }
                     }
                  }
               }
            }

            // End the line
            if( out == (std::ostream*)(NULL) )
            {
#ifdef HAVE_NCURSES
               printw( "\n" );
#endif
            }
            else
            {
               *out << std::endl;
            }
         }
      }
   }
}


void
Sim::printInits( std::ostream* out )
{
   if( out == (std::ostream*)(NULL) )
   {
#ifdef HAVE_NCURSES
      printw( "init %d ", initialTypes.size() );
#endif
   }
   else
   {
      *out << "init " << initialTypes.size() << " ";
   }

   for( unsigned int i = 0; i < initialTypes.size(); i++ )
   {
      if( out == (std::ostream*)(NULL) )
      {
#ifdef HAVE_NCURSES
         printw( "%s ", initialTypes[i]->getName().c_str() );
#endif
      }
      else
      {
         *out << initialTypes[i]->getName() << " ";
      }
   }

   if( out == (std::ostream*)(NULL) )
   {
#ifdef HAVE_NCURSES
      printw( "\n" );
#endif
   }
   else
   {
      *out << std::endl;
   }
}

