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

void DirFormat( std::string & dir )
{
	if( dir.empty() ) { dir = "."; return; }
	if( dir.front() != '/' && dir.front() != '.' && dir.front() != '~') {
		dir.insert( dir.begin(), '/' );
		dir.insert( dir.begin(), '.' );
	}
	if( dir.front() == '~' ) {
		dir.erase( dir.begin() );
		std::string home = GetEnv( "HOME" );
		dir.insert( dir.begin(), home.begin(), home.end() );
	}
	if( dir.size() > 1 && dir.back() == '/' ) dir.pop_back();
	return;
}

void format_file_str( std::string & file, const FormatFileType ftype )
{
	if( file.front() != '~' && file.front() != '/' && file.front() != '.' ) {
		std::string type;
		if( ftype == FormatFileType::INC ) type = "include";
		else if( ftype == FormatFileType::LIB ) type = "lib";
		file = STRINGIFY( BUILD_PREFIX_DIR ) "/" + type + "/ethereal/" + file;
	} else if( file.front() == '~' ) {
		file.erase( file.begin() );
		std::string home = GetEnv( "HOME" );
		file.insert( file.begin(), home.begin(), home.end() );
	}
	if( ftype == FormatFileType::INC ) file += ".et";
}
