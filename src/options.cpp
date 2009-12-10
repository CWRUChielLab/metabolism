/* options.cpp
 */

#include <ctime>
#include <iostream>
#include <string>
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
   sleep = 200;

   // Store command line names for options.
   // Order by short name.
   struct option long_options[] =
   {
   // { "long_option_name", "noarg(0), requiredarg(1), optarg(2)", NULL, retval }
      { "atoms",             1, 0, 'a' },
      { "no-gui",            0, 0, 'g' },
      { "help",              0, 0, 'h' },
      { "iters",             1, 0, 'i' },
      { "sleep",             1, 0, 'l' },
      { "seed",              1, 0, 's' },
      { "version",           0, 0, 'v' },
      { "x",                 1, 0, 'x' },
      { "y",                 1, 0, 'y' }
   };

   int option_index = 0, c;
   while( 1 )
   {
      c = getopt_long( argc, argv, "a:ghi:l:s:vx:y:", long_options, &option_index );
      if( c == -1 )
      {
         break;
      }

      switch( c )
      {
         case 'a':
            atomCount = safeStrtol( optarg );
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
         case 'l':
            sleep = safeStrtol( optarg );
            break;
         case 's':
            seed = safeStrtol( optarg );
            break;
         case 'v':
            printVersion();
            exit( 0 );
            break;
         case 'x':
            worldX = safeStrtol( optarg );
            break;
         case 'y':
            worldY = safeStrtol( optarg );
            break;
         default:
            std::cout << "Unknown option.  Try --help for a full list." << std::endl;
            exit( -1 );
            break;
      }
   }
}


void
Options::printVersion()
{
   // Output version information
   std::cout << "---------------------------------------------------------------------------" << std::endl;
   std::cout << "This is our version information."                                            << std::endl;
   std::cout << "It fills up multiple lines."                                                 << std::endl;
   std::cout << "Compiled at " << __TIME__ << " on " << __DATE__ << "."                       << std::endl;
   std::cout << "---------------------------------------------------------------------------" << std::endl;
}


void
Options::printHelp()
{
   // Output help information
   std::cout << "---------------------------------------------------------------------------" << std::endl;
   std::cout << "-a, --atoms        Number of atoms in the world."                            << std::endl;
   std::cout << "-g, --no-gui       Disable the GUI."                                         << std::endl;
   std::cout << "-h, --help         Display this information."                                << std::endl;
   std::cout << "-i, --iters        Number of iterations."                                    << std::endl;
   std::cout << "-l, --sleep        Number of milliseconds to sleep between iterations."      << std::endl;
   std::cout << "-s, --seed         Seed for the random number generator."                    << std::endl;
   std::cout << "-v, --version      Display version information."                             << std::endl;
   std::cout << "-x, --x            Width of the world."                                      << std::endl;
   std::cout << "-y, --y            Height of the world."                                     << std::endl;
   std::cout << "---------------------------------------------------------------------------" << std::endl;
}


void
Options::dumpOptions()
{
   //
}

