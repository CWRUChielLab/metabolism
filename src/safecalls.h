/* safecalls.h
 */

#ifndef SAFECALLS_H
#define SAFECALLS_H

#include <cstdlib>  // exit, EXIT_FAILURE
#include <iostream> // cerr

namespace SafeCalls
{
   long int _safeStrtol( const char *str, const char* file, const int line );
   #define safeStrtol(X) \
	   _safeStrtol(X, __FILE__, __LINE__)
   double   _safeStrtod( const char *str, const char* file, const int line );
   #define safeStrtod(X) \
   	_safeStrtod(X, __FILE__, __LINE__)

   template <class T> T* 
   AllocCheck(T* p, const char* file, int line);

   #define safeNew(X) \
	   _safeNew(new (std::nothrow) X, __FILE__, __LINE__)

   // Note:  Because templates are compiled when required, this forces a 
   // restriction for multi-file projects: the implementation (definition) 
   // of a template class or function must be in the same file as its declaration. 
   // That means that we cannot separate the interface in a separate header file, 
   // and that we must include both interface and implementation in any file that 
   // uses the templates.
   //
   // Since no code is generated until a template is instantiated when required, 
   // compilers are prepared to allow the inclusion more than once of the same 
   // template file with both declarations and definitions in a project without 
   // generating linkage errors.
   //
   // http://www.cplusplus.com/doc/tutorial/templates.html

   template <class T> T* 
   _safeNew(T* p, const char* file, const int line)
   {
      if (p == NULL)
      {
         std::cerr << "Call to new resulted in null ptr.  Failed at " << file
            << "::" << line << std::endl;
         exit(EXIT_FAILURE);
      }
      else
      {
         return p;
      }
   }
};

#endif /* SAFECALLS_H */
