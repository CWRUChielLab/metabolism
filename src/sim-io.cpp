/* sim-io.cpp
 */

#include <cassert>
#include <fstream>
#include <iomanip> // setw
#include <iostream>
#ifndef _NO_NCURSES
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
   elesLoaded = 0;
   rxnsLoaded = 0;
   initsLoaded = 0;

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
               int n;
               char c;
               std::string firstEle, secondEle;
               double firstProb, secondProb;
               ElementVector reactants, firstProducts, secondProducts;

               load >> firstProb >> n;
               switch( n )
               {
                  case 1:
                     load >> firstEle;
                     reactants = ev( 1, firstEle.c_str() );
                     break;
                  case 2:
                     load >> firstEle >> secondEle;
                     reactants = ev( 2, firstEle.c_str(), secondEle.c_str() );
                     break;
                  default:
                     std::cout << "Load settings: " << n << " is not an acceptable number of reactants!" << std::endl;
                     assert(0);
                     break;
               }
               load >> n;
               switch( n )
               {
                  case 1:
                     load >> firstEle;
                     firstProducts = ev( 1, firstEle.c_str() );
                     break;
                  case 2:
                     load >> firstEle >> secondEle;
                     firstProducts = ev( 2, firstEle.c_str(), secondEle.c_str() );
                     break;
                  default:
                     std::cout << "Load settings: " << n << " is not an acceptable number of products!" << std::endl;
                     assert(0);
                     break;
               }
               while( load.peek() == ' ' && load.good() )
               {
                  c = load.get();
               }
               c = load.peek();
               if( c == '0' || c == '1' )
               {
                  load >> secondProb >> n;
                  switch( n )
                  {
                     case 1:
                        load >> firstEle;
                        secondProducts = ev( 1, firstEle.c_str() );
                        break;
                     case 2:
                        load >> firstEle >> secondEle;
                        secondProducts = ev( 2, firstEle.c_str(), secondEle.c_str() );
                        break;
                     default:
                        std::cout << "Load settings: " << n << " is not an acceptable number of products!" << std::endl;
                        assert(0);
                        break;
                  }
                  tempRxn = safeNew( Reaction( reactants, firstProducts, firstProb, secondProducts, secondProb ) );
               }
               else
               {
                  tempRxn = safeNew( Reaction( reactants, firstProducts, firstProb ) );
               }
               rxnTable[ tempRxn->getKey() ] = tempRxn;
               rxnsLoaded++;
            }
            else
            {
               if( keyword == "init" )
               {
                  int n;
                  std::string firstEle, secondEle, thirdEle, fourthEle;

                  load >> n;
                  switch( n )
                  {
                     case 1:
                        load >> firstEle;
                        initialTypes = ev( 1, firstEle.c_str() );
                        break;
                     case 2:
                        load >> firstEle >> secondEle;
                        initialTypes = ev( 2, firstEle.c_str(), secondEle.c_str() );
                        break;
                     case 3:
                        load >> firstEle >> secondEle >> thirdEle;
                        initialTypes = ev( 3, firstEle.c_str(), secondEle.c_str(), thirdEle.c_str() );
                        break;
                     case 4:
                        load >> firstEle >> secondEle >> thirdEle >> fourthEle;
                        initialTypes = ev( 4, firstEle.c_str(), secondEle.c_str(), thirdEle.c_str(), fourthEle.c_str() );
                        break;
                     default:
                        std::cout << "Load settings: only allowed between 1 and 4 initial types!" << std::endl;
                        assert(0);
                        break;
                  }
                  initsLoaded++;
                  assert( initsLoaded < 2 );
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
   for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
   {
      Element* ele = i->second;
      if( ele->getName() != "Solvent" )
      {
         configFile <<  "ele " << ele->getName() << " " << ele->getSymbol() << " " << ele->getColor() << " " << ele->getCharge() << std::endl;
      }
   }
   configFile << std::endl;

   // Write Reactions to file
   for( ReactionMap::iterator i = rxnTable.begin(); i != rxnTable.end(); i++ )
   {
      Reaction* rxn = i->second;
      if( rxn != NULL )
      {
         configFile << "rxn " << rxn->getFirstProb() << " " << rxn->getReactants().size() << " ";
         for( unsigned int j = 0; j < rxn->getReactants().size(); j++ )
         {
            configFile << rxn->getReactants()[j]->getName() << " ";
         }
         configFile << rxn->getFirstProducts().size() << " ";
         for( unsigned int j = 0; j < rxn->getFirstProducts().size(); j++ )
         {
            configFile << rxn->getFirstProducts()[j]->getName() << " ";
         }
         if( !rxn->getSecondProducts().empty() )
         {
            configFile << rxn->getSecondProb() << " " << rxn->getSecondProducts().size() << " ";
            for( unsigned int j = 0; j < rxn->getSecondProducts().size(); j++ )
            {
               configFile << rxn->getSecondProducts()[j]->getName() << " ";
            }
         }
         configFile << std::endl;
      }
   }
   configFile << std::endl;

   // Write initialTypes to file
   configFile << "init " << initialTypes.size() << " ";
   for( unsigned int i = 0; i < initialTypes.size(); i++ )
   {
      configFile << initialTypes[i]->getName() << " ";
   }
   configFile << std::endl << std::endl;

   configFile.close();
}


// Records important information about the state
// of the world and writes it to file.
void
Sim::takeCensus()
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
Sim::dumpAtoms()
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
#ifndef _NO_NCURSES
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
Sim::printElements()
{
   if( o->useGUI )
   // **********************
   // Print using ncurses
   // **********************
   {
#ifndef _NO_NCURSES
      printw( "There are %d elements.\n", periodicTable.size() );
      for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
      {
         Element* ele = i->second;
         printw( "periodicTable[\"%s\"] has key:\t%d\n", ele->getName().c_str(), periodicTable[ele->getName()]->getKey() );
      }
      refresh();
#endif
   }
   else
   // **********************
   // Print using cout
   // **********************
   {
      std::cout << "There are " << periodicTable.size() << " elements." << std::endl;
      for( ElementMap::iterator i = periodicTable.begin(); i != periodicTable.end(); i++ )
      {
         Element* ele = i->second;
         std::cout << "periodicTable[\"" << ele->getName() << "\"] has key:\t" << periodicTable[ele->getName()]->getKey() << std::endl;
      }
   }
}


void
Sim::printReactions()
{
   // Loop through the rxnTable, printing each Reaction
   for( ReactionMap::iterator i = rxnTable.begin(); i != rxnTable.end(); i++ )
   {
      std::map<char,int> reactantCount;
      std::map<char,int> firstProductCount;
      std::map<char,int> secondProductCount;
      Reaction* rxn = i->second;

      if( rxn != NULL )
      {
         // Count up the number of each type of reactant and product
         for( unsigned int j = 0; j < rxn->getReactants().size(); j++ )
         {
            reactantCount[ rxn->getReactants()[j]->getSymbol() ]++;
         }
         for( unsigned int j = 0; j < rxn->getFirstProducts().size(); j++ )
         {
            firstProductCount[ rxn->getFirstProducts()[j]->getSymbol() ]++;
         }
         if( !rxn->getSecondProducts().empty() )
         {
            for( unsigned int j = 0; j < rxn->getSecondProducts().size(); j++ )
            {
               secondProductCount[ rxn->getSecondProducts()[j]->getSymbol() ]++;
            }
         }

         if( o->useGUI )
         // **********************************
         // Print first reaction using ncurses
         // **********************************
         {
#ifndef _NO_NCURSES
            printw( "Key: %d  Prob: %f  \t", rxn->getKey(), rxn->getFirstProb() );

            // Print the reactants, grouping copies of one type together
            // with stoichiometric coefficients
            for( std::map<char,int>::iterator j = reactantCount.begin(); j != reactantCount.end(); j++ )
            {
               int coefficient = j->second;
               if( coefficient == 1 )
               {
                  if( j == reactantCount.begin() )
                     printw( "%c", j->first );
                  else
                     printw( " + %c", j->first );
               }
               else
               {
                  if( j == reactantCount.begin() )
                     printw( "%d%c", j->second, j->first );
                  else
                     printw( " + %d%c", j->second, j->first );
               }
            }

            printw( " -> " );

            // Print the first products, grouping copies of one type together
            // with stoichiometric coefficients
            for( std::map<char,int>::iterator j = firstProductCount.begin(); j != firstProductCount.end(); j++ )
            {
               int coefficient = j->second;
               if( coefficient == 1 )
               {
                  if( j == firstProductCount.begin() )
                     printw( "%c", j->first );
                  else
                     printw( " + %c", j->first );
               }
               else
               {
                  if( j == firstProductCount.begin() )
                     printw( "%d%c", j->second, j->first );
                  else
                     printw( " + %d%c", j->second, j->first );
               }
            }

            printw( "\n" );


            if( !rxn->getSecondProducts().empty() )
            // ***********************************
            // Print second reaction using ncurses
            // ***********************************
            {
               printw( "Key: %d  Prob: %f  \t", rxn->getKey(), rxn->getSecondProb() );

               // Print the reactants, grouping copies of one type together
               // with stoichiometric coefficients
               for( std::map<char,int>::iterator j = reactantCount.begin(); j != reactantCount.end(); j++ )
               {
                  int coefficient = j->second;
                  if( coefficient == 1 )
                  {
                     if( j == reactantCount.begin() )
                        printw( "%c", j->first );
                     else
                        printw( " + %c", j->first );
                  }
                  else
                  {
                     if( j == reactantCount.begin() )
                        printw( "%d%c", j->second, j->first );
                     else
                        printw( " + %d%c", j->second, j->first );
                  }
               }

               printw( " -> " );

               // Print the second products, grouping copies of one type together
               // with stoichiometric coefficients
               for( std::map<char,int>::iterator j = secondProductCount.begin(); j != secondProductCount.end(); j++ )
               {
                  int coefficient = j->second;
                  if( coefficient == 1 )
                  {
                     if( j == secondProductCount.begin() )
                        printw( "%c", j->first );
                     else
                        printw( " + %c", j->first );
                  }
                  else
                  {
                     if( j == secondProductCount.begin() )
                        printw( "%d%c", j->second, j->first );
                     else
                        printw( " + %d%c", j->second, j->first );
                  }
               }

               printw( "\n" );
            }
#endif
         }
         else
         // *******************************
         // Print first reaction using cout
         // *******************************
         {
            std::cout << "Key: " << rxn->getKey() << "  Prob: " << rxn->getFirstProb() << "  \t";

            // Print the reactants, grouping copies of one type together
            // with stoichiometric coefficients
            for( std::map<char,int>::iterator j = reactantCount.begin(); j != reactantCount.end(); j++ )
            {
               int coefficient = j->second;
               if( coefficient == 1 )
               {
                  if( j == reactantCount.begin() )
                     std::cout << j->first;
                  else
                     std::cout << " + " << j->first;
               }
               else
               {
                  if( j == reactantCount.begin() )
                     std::cout << j->second << j->first;
                  else
                     std::cout << " + " << j->second << j->first;
               }
            }

            std::cout << " -> ";

            // Print the first products, grouping copies of one type together
            // with stoichiometric coefficients
            for( std::map<char,int>::iterator j = firstProductCount.begin(); j != firstProductCount.end(); j++ )
            {
               int coefficient = j->second;
               if( coefficient == 1 )
               {
                  if( j == firstProductCount.begin() )
                     std::cout << j->first;
                  else
                     std::cout << " + " << j->first;
               }
               else
               {
                  if( j == firstProductCount.begin() )
                     std::cout << j->second << j->first;
                  else
                     std::cout << " + " << j->second << j->first;
               }
            }

            std::cout << std::endl;


            if( !rxn->getSecondProducts().empty() )
            // ********************************
            // Print second reaction using cout
            // ********************************
            {
               std::cout << "Key: " << rxn->getKey() << "  Prob: " << rxn->getSecondProb() << "  \t";

               // Print the reactants, grouping copies of one type together
               // with stoichiometric coefficients
               for( std::map<char,int>::iterator j = reactantCount.begin(); j != reactantCount.end(); j++ )
               {
                  int coefficient = j->second;
                  if( coefficient == 1 )
                  {
                     if( j == reactantCount.begin() )
                        std::cout << j->first;
                     else
                        std::cout << " + " << j->first;
                  }
                  else
                  {
                     if( j == reactantCount.begin() )
                        std::cout << j->second << j->first;
                     else
                        std::cout << " + " << j->second << j->first;
                  }
               }

               std::cout << " -> ";

               // Print the second products, grouping copies of one type together
               // with stoichiometric coefficients
               for( std::map<char,int>::iterator j = secondProductCount.begin(); j != secondProductCount.end(); j++ )
               {
                  int coefficient = j->second;
                  if( coefficient == 1 )
                  {
                     if( j == secondProductCount.begin() )
                        std::cout << j->first;
                     else
                        std::cout << " + " << j->first;
                  }
                  else
                  {
                     if( j == secondProductCount.begin() )
                        std::cout << j->second << j->first;
                     else
                        std::cout << " + " << j->second << j->first;
                  }
               }

               std::cout << std::endl;
            }
         }
      }
   }
}

