/* options.h
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#ifdef HAVE_QT
#include <QTemporaryFile>
#endif
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
      void openFiles();
      void closeFiles();
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

      std::vector<std::string> filePaths;
      std::vector<std::ostream*> out;
      std::vector<std::ofstream*> closableFiles;
#ifdef HAVE_QT
      std::vector<QTemporaryFile*> tempFiles;
#endif

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
