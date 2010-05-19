/* options.cpp
 */

#include <cassert>
#include <cstdlib> // exit
#include <fstream>
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
#ifdef HAVE_NCURSES
   useGUI = 1;
#else
   useGUI = 0;
#endif
   doRxns = 1;
   doShuffle = 0;
   sleep = 0;
   verbose = 0;
   progress = 1;
   loadFile = "";
   configFile = "config.out";
   censusFile = "census.out";
   diffusionFile = "diffusion.out";
   randFile = "rand.out";

   // Store command line names for options.
   // Order by short name.
   struct option long_options[] =
   {
   // { "long_option_name", "noarg(0), requiredarg(1), optarg(2)", NULL, retval }
      { "atoms",             1, 0, 'a' },
      { "files",             1, 0, 'f' },
      { "gui-off",           0, 0, 'g' },
      { "help",              0, 0, 'h' },
      { "iters",             1, 0, 'i' },
      { "load",              1, 0, 'l' },
      { "progress-off",      0, 0, 'p' },
      { "rxns-off",          0, 0, 'r' },
      { "rxns-on",           0, 0, 'R' },
      { "seed",              1, 0, 's' },
      { "shuffle",           0, 0, 'S' },
      { "shuffle-off",       0, 0, 't' },
      { "version",           0, 0, 'v' },
      { "verbose",           0, 0, 'V' },
      { "x",                 1, 0, 'x' },
      { "y",                 1, 0, 'y' },
      { "sleep",             1, 0, 'z' }
   };

   // The options_string passed to getopt_long lists each valid
   // short option name followed by a colon if an argument
   // is required, followed by two colons if an argument is
   // optional, or not followed by a colon if the option
   // cannot take arguments.  The leading hyphen allows
   // getopt_long to properly handle the multiple parameters
   // that can be passed to --files by assigning the second
   // and third parameters passed to --files with c=1.
   const char* options_string = "-a:f:ghi:l:priRs:StvVx:y:z:";

   int option_index = 0, c;
   int files_read_in_so_far = 0;
   std::ifstream load;
   std::string keyword;
   std::string onOrOff;

   // First pass through arguments to look for --load option
   while( 1 )
   {
      c = getopt_long( argc, argv, options_string, long_options, &option_index );
      if( c == -1 )
      {
         break;
      }

      switch( c )
      {
         case 'l':
            loadFile = optarg;
            load.open( loadFile.c_str() );
            while( load.good() )
            {
               load >> keyword;
               if( keyword == "version" || keyword == "ele" || keyword == "rxn" || keyword == "init" )
               {
                  load.ignore(1024,'\n');
               }
               else
               {
                  if( keyword == "atoms" )
                  {
                     load >> atomCount;
                  }
                  else
                  {
                     if( keyword == "iters" )
                     {
                        load >> maxIters;
                     }
                     else
                     {
                        if( keyword == "reactions" )
                        {
                           onOrOff = "";
                           load >> onOrOff;
                           if( onOrOff == "on" )
                           {
                              doRxns = 1;
                           }
                           else
                           {
                              if( onOrOff == "off" )
                              {
                                 doRxns = 0;
                              }
                              else
                              {
                                 std::cout << "Load settings: \"reactions\" must have value \"on\" or \"off\"!" << std::endl;
                                 exit( EXIT_FAILURE );
                              }
                           }
                        }
                        else
                        {
                           if( keyword == "seed" )
                           {
                              load >> seed;
                           }
                           else
                           {
                              if( keyword == "shuffle" )
                              {
                                 onOrOff = "";
                                 load >> onOrOff;
                                 if( onOrOff == "on" )
                                 {
                                    doShuffle = 1;
                                 }
                                 else
                                 {
                                    if( onOrOff == "off" )
                                    {
                                       doShuffle = 0;
                                    }
                                    else
                                    {
                                       std::cout << "Load settings: \"shuffle\" must have value \"on\" or \"off\"!" << std::endl;
                                       exit( EXIT_FAILURE );
                                    }
                                 }
                              }
                              else
                              {
                                 if( keyword == "x" )
                                 {
                                    load >> worldX;
                                 }
                                 else
                                 {
                                    if( keyword == "y" )
                                    {
                                       load >> worldY;
                                    }
                                    else
                                    {
                                       if( keyword == "" )
                                       {
                                          break;
                                       }
                                       else
                                       {
                                          std::cout << "Load settings: Unrecognized keyword \"" << keyword << "\"!" << std::endl;
                                          exit( EXIT_FAILURE );
                                       }
                                    }
                                 }
                              }
                           }
                        }
                     }
                  }
               }
               keyword = "";
            }
            break;
         default:
            break;
      }
   }

   // Reset argv index so that scanning of options can restart
   optind = 1;

   // Second pass through arguments to read in all other options
   while( 1 )
   {
      c = getopt_long( argc, argv, options_string, long_options, &option_index );
      if( c == -1 )
      {
         break;
      }

      switch( c )
      {
         case 1:
            // When more than one file is passed to --files, the first
            // file will have c='f', and the rest will have c=1.  For a
            // call such as "./metabolism -f file1 file2 file3 file4",
            // file1 will have c='f' and will be processed first, thereby
            // incrementing files_read_in_so_far to 1.  file2, file3, and
            // file4 will have c=1 and increment files_read_in_so_far to
            // 2, 3, and 4.
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
               case 3:
                  randFile = optarg;
                  files_read_in_so_far++;
                  break;
               default:
                  if( files_read_in_so_far >= 4 )
                  {
                     std::cout << "options: --files can accept at most four file names." << std::endl;
                     exit( EXIT_FAILURE );
                  } else {
                     assert( files_read_in_so_far >= 1 && files_read_in_so_far <= 3 );
                  }
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
            exit( EXIT_SUCCESS );
            break;
         case 'i':
            maxIters = safeStrtol( optarg );
            break;
         case 'l':
            break;
         case 'p':
            progress = 0;
            break;
         case 'r':
            doRxns = 0;
            break;
         case 'R':
            doRxns = 1;
            break;
         case 's':
            seed = safeStrtol( optarg );
            break;
         case 'S':
            doShuffle = 1;
            break;
         case 't':
            doShuffle = 0;
            break;
         case 'v':
            printVersion();
            exit( EXIT_SUCCESS );
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
            exit( EXIT_FAILURE );
            break;
      }
   }
}


void
Options::printVersion()
{
   // Print version information
   std::cout << "---------------------------------------------------------------------------" << std::endl;
   std::cout << "Chemical Metabolism Simulator"                                               << std::endl;
   std::cout << GIT_TAG                                                                       << std::endl;
   std::cout << "Compiled at " << __TIME__ << " on " << __DATE__ << "."                       << std::endl;
   std::cout << "---------------------------------------------------------------------------" << std::endl;
}


void
Options::printHelp()
{
   // Print help information
   std::cout << "---------------------------------------------------------------------------" << std::endl;
   std::cout << "Usage: metabolism [OPTION]..."                                               << std::endl;
   std::cout << "Example: metabolism -i 100 -x 256 -y 256 -z 1000"                            << std::endl;
   std::cout << "  Would run a 256x256 world for 100 iterations with 1 second pauses between" << std::endl;
   std::cout << "    each iteration."                                                         << std::endl;
   std::cout <<                                                                                  std::endl;
   std::cout << "-a, --atoms         Number of atoms in the world. Default: 64"               << std::endl;
   std::cout << "-f, --files         Specify the names of the four output files."             << std::endl;
   std::cout << "                      Default: config.out census.out diffusion.out rand.out" << std::endl;
#ifdef HAVE_NCURSES
   std::cout << "-g, --gui-off       Disable the GUI. The GUI is enabled by default."         << std::endl;
#endif
   std::cout << "-h, --help          Display this information."                               << std::endl;
   std::cout << "-i, --iters         Number of iterations. Default: 100000"                   << std::endl;
   std::cout << "-l, --load          Specify the name of a config file to load settings"      << std::endl;
   std::cout << "                      from. Any other options specified will override"       << std::endl;
   std::cout << "                      loaded options."                                       << std::endl;
   std::cout << "-p, --progress-off  Disable simulation progress reporting (percent"          << std::endl;
   std::cout << "                      complete)."                                            << std::endl;
   std::cout << "-r, --rxns-off      Disable the execution of chemical reactions. Reactions"  << std::endl;
   std::cout << "                      are enabled by default."                               << std::endl;
   std::cout << "-R, --rxns-on       Enable reactions."                                       << std::endl;
   std::cout << "-s, --seed          Seed for the random number generator. Initialized using" << std::endl;
   std::cout << "                      the system time by default."                           << std::endl;
   std::cout << "-S, --shuffle       Shuffle the positions of the atoms in the world each"    << std::endl;
   std::cout << "                      iteration. Shuffling disabled by default."             << std::endl;
   std::cout << "-t, --shuffle-off   Disable shuffling."                                      << std::endl;
   std::cout << "-v, --version       Display version information."                            << std::endl;
   std::cout << "-V, --verbose       Write to screen detailed information for debugging."     << std::endl;
   std::cout << "-x, --x             Width of the world. Default: 16"                         << std::endl;
   std::cout << "-y, --y             Height of the world. Default: 16"                        << std::endl;
   std::cout << "-z, --sleep         Number of milliseconds to sleep between iterations."     << std::endl;
   std::cout << "                      Default: 0"                                            << std::endl;
   std::cout << "---------------------------------------------------------------------------" << std::endl;
}

