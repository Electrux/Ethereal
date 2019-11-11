/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef ENV_HPP
#define ENV_HPP

#include <vector>
#include <string>

std::string GetEnv( const std::string & key );

std::string GetCWD();
void SetCWD( std::string dir );
std::string GetEtherealBinaryAbsoluteLoc( const std::string & arg0 );

void DirFormat( std::string & dir );

#endif // ENV_HPP
