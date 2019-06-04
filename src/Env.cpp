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
#include <unistd.h>

#include "Env.hpp"

#define MAX_PATH 1024

std::string GetEnv( const std::string & key )
{
	const char * env = std::getenv( key.c_str() );
	return env == NULL ? "" : env;
}

std::string GetCWD()
{
	char cwd[ MAX_PATH ];
	if( getcwd( cwd, sizeof( cwd ) ) != NULL ) {
		return cwd;
	}
	return "";
}

void SetCWD( std::string dir )
{
	if( dir.empty() ) return;
	if( dir.front() != '/' && dir.front() != '.' && dir.front() != '~') {
		dir.insert( dir.begin(), '/' );
		dir.insert( dir.begin(), '.' );
	}
	if( dir.front() == '~' ) {
		dir.erase( dir.begin() );
		std::string home = GetEnv( "HOME" );
		dir.insert( dir.begin(), home.begin(), home.end() );
	}
	chdir( dir.c_str() );
}