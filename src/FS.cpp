/*
	Copyright (c) 2018, Electrux
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

int read_file( eth_t & eth )
{
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen( eth.src_stack.back().c_str(), "r" );
	if( fp == NULL ) {
		fprintf( stdout, "failed to open source file: %s\n", eth.src_stack.back().c_str() );
		return FILE_IO_ERR;
	}

	std::vector< std::string > lines;

	while( ( read = getline( & line, & len, fp ) ) != -1 ) {
		lines.push_back( line );
	}

	fclose( fp );
	if( line ) free( line );

	if( lines.empty() ) return FILE_EMPTY;

	eth.srcs[ eth.src_stack.back() ].src = lines;
	return OK;
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