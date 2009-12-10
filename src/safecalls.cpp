/* safecalls.cpp
 */

#include <cerrno>
#include <stdio.h>
#include "safecalls.h"


namespace SafeCalls{
long int
_safeStrtol( const char *str, const char* file, const int line ) {
	//Handle error checking on strtol.
	long int val;
	char *endptr;
	char msg[1024];
	errno = 0;
	val = strtol( str, &endptr, 10 );

	if( errno ){

		snprintf(msg, 1023, "strtol failed at %s::%d", file, line);
		perror( msg );
		exit( EXIT_FAILURE );

	}else if( endptr == str ) {

		fprintf( stderr, "No digits were found.  %s::%d.\n", file, line );
		exit( EXIT_FAILURE );
	}

	return val;
}

double
_safeStrtod( const char *str, const char* file, const int line ){
	//Handle error checking on strtol.
	double val;
	char *endptr;
	char msg[1024];
	errno = 0;
	val = strtod( str, &endptr );

	if( errno ){

		snprintf(msg, 1023, "strtod failed at %s::%d", file, line);
		perror( msg );
		exit( EXIT_FAILURE );

	}else if( endptr == str ) {

		fprintf( stderr, "No digits were found.  %s::%d.\n", file, line );
		exit( EXIT_FAILURE );
	}

	return val;
}
}
