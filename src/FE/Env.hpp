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

#include <string>

std::string GetEnv( const std::string & key );

std::string GetCWD();
void SetCWD( std::string dir );

void DirFormat( std::string & dir );

enum class FormatFileType {
	INC,
	LIB,
};

void format_file_str( std::string & file, const FormatFileType ftype );

#endif // ENV_HPP