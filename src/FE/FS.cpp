/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
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

int read_file( src_t & src )
{
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen( ( src.dir + "/" + src.file ).c_str(), "r" );
	if( fp == NULL ) {
		fprintf( stdout, "failed to open source file: %s\n", src.file.c_str() );
		return E_FILE_IO_ERR;
	}

	std::vector< std::string > lines;

	while( ( read = getline( & line, & len, fp ) ) != -1 ) {
		lines.push_back( line );
	}

	fclose( fp );
	if( line ) free( line );

	if( lines.empty() ) return E_FILE_EMPTY;

	src.code = lines;
	return E_OK;
}

bool fexists( const std::string & file )
{
	return access( file.c_str(), F_OK ) != -1;
}