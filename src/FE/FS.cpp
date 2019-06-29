/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <unistd.h>

#include "Env.hpp"
#include "FS.hpp"

#define _STRINGIZE(x) #x
#define STRINGIFY(x) _STRINGIZE(x)

int read_file( src_t & src )
{
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen( src.name.c_str(), "r" );
	if( fp == NULL ) {
		fprintf( stdout, "failed to open source file: %s\n", src.name.c_str() );
		return E_FILE_IO_ERR;
	}

	std::vector< std::string > lines;

	while( ( read = getline( & line, & len, fp ) ) != -1 ) {
		lines.push_back( line );
		if( lines.back().size() > 0 && lines.back().back() == '\n' ) {
			lines.back().erase( lines.back().end() - 1 );
		}
	}

	fclose( fp );
	if( line ) free( line );

	if( lines.empty() ) return E_FILE_EMPTY;

	src.code = lines;
	return E_OK;
}

int get_internal_file_path( std::string & file, const std::string & type )
{
	if( file.front() != '~' && file.front() != '/' && file.front() != '.' ) {
		file = STRINGIFY( BUILD_PREFIX_DIR ) "/" + type + "/ethereal/" + file;
	} else if( file.front() == '~' ) {
		file.erase( file.begin() );
		std::string home = GetEnv( "HOME" );
		file.insert( file.begin(), home.begin(), home.end() );
	}
	file += ".et";
	return ( int )!fexists( file );
}

bool fexists( const std::string & file )
{
	return access( file.c_str(), F_OK ) != -1;
}