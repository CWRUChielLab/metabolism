/* options.cpp
 */

#include <cassert>
#include <cstdlib> // exit
#include "options.h"
#include "safecalls.h"
using namespace SafeCalls;


// Constructor
Options::Options( int argc, char* argv[] )
{
   // Set default values
   seed = time(NULL);
   maxIters = 1000000;
   worldX = 250;
   worldY = 250;
#ifdef HAVE_QT
   gui = GUI_QT;
#else
#ifdef HAVE_NCURSES
   gui = GUI_NCURSES;
#else
   gui = GUI_OFF;
#endif
#endif
   doRxns = true;
   doShuffle = false;
   sleep = 0;
   verbose = false;
   progress = true;

   filePaths = std::vector<std::string>( N_FILES );
   filePaths[ FILE_CONFIG ] = "config.out";
   filePaths[ FILE_CENSUS ] = "census.out";
   filePaths[ FILE_DIFFUSION ] = "diffusion.out";
   filePaths[ FILE_RAND ] = "rand.out";

   // Options that take only long-opt form should be indexed here.
   // In order to not clash with single-letter options, start from
   // 'z'+1 (recall that 'z' > 'Z').
   enum
   {
      OPT_GUI_NCURSES = 'z' + 1,
      OPT_RXNS_ON,
      OPT_SHUFFLE_OFF
   };

   // Any options that take long-opt form should be stored here.
   // Order by short name.
   struct option long_options[] =
   {
   // { "long_option_name", "no_argument(0), required_argument(1), optional_argument(2)", NULL, retval }
      { "files",        required_argument, NULL, 'f' },
#if defined(HAVE_QT) | defined(HAVE_NCURSES)
      { "gui-off",      no_argument,       NULL, 'g' },
#endif
      { "help",         no_argument,       NULL, 'h' },
      { "iters",        required_argument, NULL, 'i' },
      { "load",         required_argument, NULL, 'l' },
      { "progress-off", no_argument,       NULL, 'p' },
      { "rxns-off",     no_argument,       NULL, 'r' },
      { "seed",         required_argument, NULL, 's' },
      { "shuffle",      no_argument,       NULL, 'S' },
      { "version",      no_argument,       NULL, 'v' },
      { "verbose",      no_argument,       NULL, 'V' },
      { "width",        required_argument, NULL, 'x' },
      { "height",       required_argument, NULL, 'y' },
      { "sleep",        required_argument, NULL, 'z' },
#if defined(HAVE_QT) & defined(HAVE_NCURSES)
      { "gui-ncurses",  no_argument,       NULL, OPT_GUI_NCURSES },
#endif
      { "rxns-on",      no_argument,       NULL, OPT_RXNS_ON },
      { "shuffle-off",  no_argument,       NULL, OPT_SHUFFLE_OFF }
   };

   // Any options that take short-opt form should be listed here.
   // The options_string passed to getopt_long lists each valid
   // short option name followed by a colon if an argument
   // is required, followed by two colons if an argument is
   // optional, or nothing if the option cannot take arguments.
   // The leading hyphen allows getopt_long to properly handle
   // the multiple parameters that can be passed to --files by
   // assigning the second and third parameters passed to
   // --files with c=1.
#if defined(HAVE_QT) | defined(HAVE_NCURSES)
   const char* options_string = "-f:ghi:l:p::rs:SvVx:y:z:";
#else
   // -g option is not available when no gui is compiled
   const char* options_string = "-f:hi:l:p::rs:SvVx:y:z:";
#endif

   int option_index = 0, c;
   int files_read_in_so_far = 0;
   std::ifstream load;
   std::string keyword;
   std::string onOrOff;

   // First pass through arguments to look for --load option
   while( true )
   {
      c = getopt_long( argc, argv, options_string, long_options, &option_index );
      if( c == -1 )
      {
         break;
      }

      switch( c )
      {
         case 'l':
            loadFile.open( optarg );
            if( loadFile.fail() )
            {
               std::cout << "options: unable to open file \"" << optarg << "\"!" << std::endl;
               exit( EXIT_FAILURE );
            }
            while( loadFile.good() )
            {
               loadFile >> keyword;
               if( keyword == "version" || keyword == "ele" || keyword == "rxn" || keyword == "extinct" )
               {
                  loadFile.ignore(1024,'\n');
               }
               else
               {
                  if( keyword == "iters" )
                  {
                     loadFile >> maxIters;
                  }
                  else
                  {
                     if( keyword == "reactions" )
                     {
                        onOrOff = "";
                        loadFile >> onOrOff;
                        if( onOrOff == "on" )
                        {
                           doRxns = true;
                        }
                        else
                        {
                           if( onOrOff == "off" )
                           {
                              doRxns = false;
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
                           loadFile >> seed;
                        }
                        else
                        {
                           if( keyword == "shuffle" )
                           {
                              onOrOff = "";
                              loadFile >> onOrOff;
                              if( onOrOff == "on" )
                              {
                                 doShuffle = true;
                              }
                              else
                              {
                                 if( onOrOff == "off" )
                                 {
                                    doShuffle = false;
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
                                 loadFile >> worldX;
                              }
                              else
                              {
                                 if( keyword == "y" )
                                 {
                                    loadFile >> worldY;
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
               keyword = "";
            }

            // When all is done, reload the file so
            // that loadChemistry can scan it again
            loadFile.close();
            loadFile.open( optarg );
            break;
         default:
            break;
      }
   }

   // Reset argv index so that scanning of options can restart
   optind = 1;

   // Second pass through arguments to read in all other options
   while( true )
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
                  filePaths[ FILE_CENSUS ] = optarg;
                  files_read_in_so_far++;
                  break;
               case 2:
                  filePaths[ FILE_DIFFUSION ] = optarg;
                  files_read_in_so_far++;
                  break;
               case 3:
                  filePaths[ FILE_RAND ] = optarg;
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
         case 'f':
            // The first parameter read in for --files will
            // have c='f'. All others will have c=1.
            assert( files_read_in_so_far == 0 );
            filePaths[ FILE_CONFIG ] = optarg;
            files_read_in_so_far++;
            break;
#if defined(HAVE_QT) | defined(HAVE_NCURSES)
         case 'g':
            gui = GUI_OFF;
            break;
#endif
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
            // When running the application on a Mac through Finder or "open"
            // in Terminal, an extra option is added that looks something like
            // "-psn_0_58333".  This is the process serial number.  We don't
            // have any use for it yet.  If optarg is nonzero (indicating a
            // value is present), then ignore it.  Otherwise, turn on the
            // progress bar.
            if(!optarg)
               progress = false;
            break;
         case 'r':
            doRxns = false;
            break;
         case 's':
            seed = safeStrtol( optarg );
            break;
         case 'S':
            doShuffle = true;
            break;
         case 'v':
            printVersion();
            exit( EXIT_SUCCESS );
            break;
         case 'V':
            verbose = true;
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
#if defined(HAVE_QT) & defined(HAVE_NCURSES)
         case OPT_GUI_NCURSES:
            gui = GUI_NCURSES;
            break;
#endif
         case OPT_RXNS_ON:
            doRxns = true;
            break;
         case OPT_SHUFFLE_OFF:
            doShuffle = false;
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
#if defined(HAVE_QT) & defined(HAVE_NCURSES)
   std::cout << "Compiled at " << __TIME__ << " on " << __DATE__ << " with Qt and ncurses."   << std::endl;
#else
#ifdef HAVE_QT
   std::cout << "Compiled at " << __TIME__ << " on " << __DATE__ << " with Qt."               << std::endl;
#else
#ifdef HAVE_NCURSES
   std::cout << "Compiled at " << __TIME__ << " on " << __DATE__ << " with ncurses."          << std::endl;
#else
   std::cout << "Compiled at " << __TIME__ << " on " << __DATE__ << "."                       << std::endl;
#endif
#endif
#endif
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
#ifdef HAVE_QT
   std::cout << "-f, --files         Specify the names of the four output files. Ignored if"  << std::endl;
   std::cout << "                      the Qt GUI is used."                                   << std::endl;
   std::cout << "                      Default: config.out census.out diffusion.out rand.out" << std::endl;
#else
   std::cout << "-f, --files         Specify the names of the four output files."             << std::endl;
   std::cout << "                      Default: config.out census.out diffusion.out rand.out" << std::endl;
#endif
#if defined(HAVE_QT) & defined(HAVE_NCURSES)
   std::cout << "-g, --gui-off       Disable the Qt GUI or use the ncurses text-based GUI"    << std::endl;
   std::cout << "    --gui-ncurses     instead."                                              << std::endl;
#else
#ifdef HAVE_QT
   std::cout << "-g, --gui-off       Disable the Qt GUI."                                     << std::endl;
#else
#ifdef HAVE_NCURSES
   std::cout << "-g, --gui-off       Disable the text-based ncurses GUI."                     << std::endl;
#else
   // -g option is not available when no gui is compiled
#endif
#endif
#endif
   std::cout << "-h, --help          Display this information."                               << std::endl;
   std::cout << "-i, --iters         Number of iterations. Default: 1000000"                  << std::endl;
   std::cout << "-l, --load          Specify the name of a config file to load settings"      << std::endl;
   std::cout << "                      from. Any other options specified will override"       << std::endl;
   std::cout << "                      loaded options."                                       << std::endl;
   std::cout << "-p, --progress-off  Disable simulation progress reporting (percent"          << std::endl;
   std::cout << "                      complete)."                                            << std::endl;
   std::cout << "-r, --rxns-off      Disable or enable the execution of chemical reactions."  << std::endl;
   std::cout << "    --rxns-on         Reactions are enabled by default."                     << std::endl;
   std::cout << "-s, --seed          Seed for the random number generator. Initialized using" << std::endl;
   std::cout << "                      the system time by default."                           << std::endl;
   std::cout << "-S, --shuffle       Enable or disable shuffling of the positions of atoms"   << std::endl;
   std::cout << "    --shuffle-off     in the world each iteration. Shuffling is disabled by" << std::endl;
   std::cout << "                      default."                                              << std::endl;
   std::cout << "-v, --version       Display version information."                            << std::endl;
   std::cout << "-V, --verbose       Write to screen detailed information for debugging."     << std::endl;
   std::cout << "-x, --width         Width of the world. Default: 250"                        << std::endl;
   std::cout << "-y, --height        Height of the world. Default: 250"                       << std::endl;
   std::cout << "-z, --sleep         Number of milliseconds to sleep between iterations."     << std::endl;
   std::cout << "                      Default: 0"                                            << std::endl;
   std::cout << "---------------------------------------------------------------------------" << std::endl;
}

