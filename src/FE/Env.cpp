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
#include <unistd.h>

#include "Env.hpp"
#include "FS.hpp"

#define MAX_PATH 2048

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
	chdir( dir.c_str() );
}

std::string GetEtherealBinaryAbsoluteLoc( const std::string & arg0 )
{
	auto last_slash_loc = arg0.find_last_of( '/' );
	if( last_slash_loc == std::string::npos ) return arg0;
	std::string bin_dir = arg0.substr( 0, last_slash_loc );
	std::string bin_name = arg0.substr( last_slash_loc + 1 );
	DirFormat( bin_dir );
	return bin_dir + "/" + bin_name;
}

void DirFormat( std::string & dir )
{
	std::string cwd = GetCWD();
	SetCWD( dir );
	std::string res = GetCWD();
	SetCWD( cwd );
	dir = res;
}
