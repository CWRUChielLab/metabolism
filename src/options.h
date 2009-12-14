/* options.h
 */

#ifndef OPTIONS_H
#define OPTIONS_H

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
      int useGUI;
      int sleep;
      int verbose;
      int progress;
      std::string configFile;
      std::string censusFile;
      std::string diffusionFile;
};

#endif /* OPTIONS_H */
