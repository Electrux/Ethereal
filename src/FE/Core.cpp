/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <string>
#include <cstdarg>

#include "Core.hpp"

void src_fail( const std::string & src, const std::string & line_str, const int line, const int col, const char * msg, ... )
{
	fprintf( stderr,
		 "%s %d[%d]: error: ",
		 src.c_str(), line, col );
	va_list vargs;
	va_start( vargs, msg );
	vfprintf( stderr, msg, vargs );
	fprintf( stderr, "\n" );
	va_end( vargs );
	fprintf( stderr, "%s\n", line_str.c_str() );
	std::string spcs;
	int tab_count = 0;
	for( auto & ch : line_str ) {
		if( ch == '\t' ) ++tab_count;
	}
	for( int i = 0; i < tab_count; ++i ) spcs += '\t';
	for( int i = 0; i < col - 1 - tab_count; ++i ) {
		spcs += " ";
	}
	fprintf( stderr, "%s^\n", spcs.c_str() );
}