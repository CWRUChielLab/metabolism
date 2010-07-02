/* options.h
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#include <fstream>
#include <getopt.h>
#include <string>

class Options
{
   public:
      // Constructor
      Options( int argc, char* argv[] );

      // Options output methods
      void printVersion();
      void printHelp();

      // Options attributes
      int seed;
      int maxIters;
      int worldX;
      int worldY;
      int atomCount;
      int gui;
      bool doRxns;
      bool doShuffle;
      int sleep;
      bool verbose;
      bool progress;
      std::ifstream loadFile;
      std::ofstream configFile;
      std::ofstream censusFile;
      std::ofstream diffusionFile;
      std::ofstream randFile;

      // Options attribute values
      enum
      {
         GUI_OFF,
         GUI_QT,
         GUI_NCURSES
      };
};

#endif /* OPTIONS_H */
