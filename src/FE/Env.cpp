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
	std::string curr_dir = GetCWD();
	std::string bin_dir = arg0.substr( 0, last_slash_loc );
	std::string bin_name = arg0.substr( last_slash_loc + 1 );
	SetCWD( bin_dir );
	std::string res = GetCWD();
	SetCWD( curr_dir );
	return res + "/" + bin_name;
}

void DirFormat( std::string & dir )
{
	if( dir.empty() ) { dir = GetCWD(); return; }
	if( dir.front() != '/' && dir.front() != '~' ) {
		dir = GetCWD() + "/" + dir;
	}
	if( dir.front() == '~' ) {
		dir.erase( dir.begin() );
		std::string home = GetEnv( "HOME" );
		dir.insert( dir.begin(), home.begin(), home.end() );
	}
	if( dir.size() > 1 && dir.back() == '/' ) dir.pop_back();
}
