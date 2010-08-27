/* options.h
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#include <fstream>
#include <getopt.h>
#include <string>
#include <vector>

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
      int gui;
      bool doRxns;
      bool doShuffle;
      int sleep;
      bool verbose;
      bool progress;
      std::vector<std::string> filePaths;

      std::ifstream loadFile;

      // Options attribute values
      enum
      {
         FILE_CONFIG = 0,
         FILE_CENSUS,
         FILE_DIFFUSION,
         FILE_RAND,
         N_FILES, // number of files (list after
                  // all files for auto-enum)
         GUI_OFF,
         GUI_QT,
         GUI_NCURSES
      };
};

#endif /* OPTIONS_H */
