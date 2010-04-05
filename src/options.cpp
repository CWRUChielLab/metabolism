/* options.cpp
 */

#include <cassert>
#include <ctime>
#include <iostream>
#include "options.h"
#include "safecalls.h"
using namespace SafeCalls;


// Constructor
Options::Options( int argc, char* argv[] )
{
   // Set default values
   seed = time(NULL);
   maxIters = 100000;
   worldX = 16;
   worldY = 16;
   atomCount = 64;
   useGUI = 1;
   doRxns = 1;
   doShuffle = 0;
   sleep = 0;
   verbose = 0;
   progress = 1;
   configFile = "config.out";
   censusFile = "census.out";
   diffusionFile = "diffusion.out";

   // Store command line names for options.
   // Order by short name.
   struct option long_options[] =
   {
   // { "long_option_name", "noarg(0), requiredarg(1), optarg(2)", NULL, retval }
      { "atoms",             1, 0, 'a' },
      { "files",             1, 0, 'f' },
      { "no-gui",            0, 0, 'g' },
      { "help",              0, 0, 'h' },
      { "iters",             1, 0, 'i' },
      { "no-progress",       0, 0, 'p' },
      { "no-rxns",           0, 0, 'r' },
      { "seed",              1, 0, 's' },
      { "shuffle",           0, 0, 'S' },
      { "version",           0, 0, 'v' },
      { "verbose",           0, 0, 'V' },
      { "x",                 1, 0, 'x' },
      { "y",                 1, 0, 'y' },
      { "sleep",             1, 0, 'z' }
   };

   int option_index = 0, c;
   int files_read_in_so_far = 0;
   while( 1 )
   {
      // The string passed to getopt_long lists each valid
      // short option name followed by a colon if an argument
      // is required, followed by two colons if an argument is
      // optional, or not followed by a colon if the option
      // cannot take arguments.  The leading hyphen allows
      // getopt_long to properly handle the multiple parameters
      // that can be passed to --files by assigning the second
      // and third parameters passed to --files with c=1.
      c = getopt_long( argc, argv, "-a:f:ghi:prs:SvVx:y:z:", long_options, &option_index );
      if( c == -1 )
      {
         break;
      }

      switch( c )
      {
         case 1:
            // When more than one file is passed to --files, all but the
            // first file will have c=1 instead of c='f'.  For a call
            // such as "./metabolism -f file1 file2 file3", file1 will
            // have c='f' and will be processed first, thereby
            // incrementing files_read_in_so_far to 1.  file2 and file3
            // will have c=1 and increment files_read_in_so_far to 2 and
            // 3.
            switch( files_read_in_so_far )
            {
               case 1:
                  censusFile = optarg;
                  files_read_in_so_far++;
                  break;
               case 2:
                  diffusionFile = optarg;
                  files_read_in_so_far++;
                  break;
               default:
                  assert( files_read_in_so_far != 0 && files_read_in_so_far < 3 );
                  break;
            }
            break;
         case 'a':
            atomCount = safeStrtol( optarg );
            break;
         case 'f':
            // The first parameter read in for --files will
            // have c='f'. All others will have c=1.
            assert( files_read_in_so_far == 0 );
            configFile = optarg;
            files_read_in_so_far++;
            break;
         case 'g':
            useGUI = 0;
            break;
         case 'h':
            printHelp();
            exit( 0 );
            break;
         case 'i':
            maxIters = safeStrtol( optarg );
            break;
         case 'p':
            progress = 0;
            break;
         case 'r':
            doRxns = 0;
            break;
         case 's':
            seed = safeStrtol( optarg );
            break;
         case 'S':
            doShuffle = 1;
            break;
         case 'v':
            printVersion();
            exit( 0 );
            break;
         case 'V':
            verbose = 1;
            break;
         case 'x':
            worldX = safeStrtol( optarg );
            break;
         case 'y':
            worldY = safeStrtol( optarg );
            break;
         case 'z':
            sleep = safeStrtol( optarg );
            break;
         default:
            std::cout << "Unknown option.  Try --help for a full list." << std::endl;
            exit(-1);
            break;
      }
   }
}


void
Options::printVersion()
{
   // Output version information
   std::cout << "---------------------------------------------------------------------------" << std::endl;
   std::cout << "Chemical Metabolism Simulator"                                               << std::endl;
   std::cout << GIT_TAG                                                                       << std::endl;
   std::cout << "Compiled at " << __TIME__ << " on " << __DATE__ << "."                       << std::endl;
   std::cout << "---------------------------------------------------------------------------" << std::endl;
}


void
Options::printHelp()
{
   // Output help information
   std::cout << "---------------------------------------------------------------------------" << std::endl;
   std::cout << "Usage: metabolism [OPTION]..."                                               << std::endl;
   std::cout << "Example: metabolism -i 100 -x 256 -y 256 -z 1000"                            << std::endl;
   std::cout << "  Would run a 256x256 world for 100 iterations with 1 second pauses between" << std::endl;
   std::cout << "    each iteration."                                                         << std::endl;
   std::cout <<                                                                                  std::endl;
   std::cout << "-a, --atoms        Number of atoms in the world. Default: 64"                << std::endl;
   std::cout << "-f, --files        Specify the names of the three output files."             << std::endl;
   std::cout << "                     Default: config.out census.out diffusion.out"           << std::endl;
   std::cout << "-g, --no-gui       Disable the GUI. The GUI is enabled by default."          << std::endl;
   std::cout << "-h, --help         Display this information."                                << std::endl;
   std::cout << "-i, --iters        Number of iterations. Default: 100000"                    << std::endl;
   std::cout << "-p, --progress     Disable simulation progress reporting (percent"           << std::endl;
   std::cout << "                     complete)."                                             << std::endl;
   std::cout << "-r, --no-rxns      Disable the execution of chemical reactions. Reactions"   << std::endl;
   std::cout << "                     are enabled by default."                                << std::endl;
   std::cout << "-s, --seed         Seed for the random number generator. Initialized using"  << std::endl;
   std::cout << "                     the system time by default."                            << std::endl;
   std::cout << "-S, --shuffle      Shuffle the positions of the atoms in the world each"     << std::endl;
   std::cout << "                     iteration. Shuffling disabled by default."              << std::endl;
   std::cout << "-v, --version      Display version information."                             << std::endl;
   std::cout << "-V, --verbose      Write to screen detailed information for debugging."      << std::endl;
   std::cout << "-x, --x            Width of the world. Default: 16"                          << std::endl;
   std::cout << "-y, --y            Height of the world. Default: 16"                         << std::endl;
   std::cout << "-z, --sleep        Number of milliseconds to sleep between iterations."      << std::endl;
   std::cout << "                     Default: 0"                                             << std::endl;
   std::cout << "---------------------------------------------------------------------------" << std::endl;
}

