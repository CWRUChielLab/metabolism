/* boost-devices.h
 */

#ifndef BOOST_DEVICES_H
#define BOOST_DEVICES_H 

#if defined(HAVE_QT) | defined(HAVE_NCURSES)
#include <boost/iostreams/stream.hpp>
#endif
#ifdef HAVE_NCURSES
#include <ncurses.h>
#endif
#ifdef HAVE_QT
#include <QFile>
#include <QTextStream>
#endif


#ifdef HAVE_NCURSES
// Define a sink device for sending input to the
// ncurses function addnstr() which writes to the
// screen (the constructor requires something be
// passed as an argument)
class ncurses_stream : public boost::iostreams::sink
{
   public:
      ncurses_stream(int) {}
      std::streamsize write( const char* s, std::streamsize n )
      {
         addnstr( s, n );
         return n;
      }
};
#endif


#ifdef HAVE_QT
// Define a sink device as a wrapper for a
// QTextStream associated with a QFile (needed so
// that the stream will accept all std and iomanip
// functions that std::ostream typically accepts)
class QFile_ostream : public boost::iostreams::sink
{
   public:
      QFile_ostream( QFile* file )
      {
         out = new QTextStream( file );
      }
      std::streamsize write( const char* s, std::streamsize n )
      {
         for( int i = 0; i < n; i++ )
         {
            *out << s[i];
         }
         return n;
      }
   private:
      QTextStream* out;
};
#endif

#endif /* BOOST_DEVICES_H */

