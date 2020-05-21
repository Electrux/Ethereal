/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef PARSER_IO_HPP
#define PARSER_IO_HPP

#include <cstdio>

namespace IO
{
	void tab_add( const bool show );
	void tab_rem( const size_t num = 1 );
	void print( const bool has_next, const char * fmt, ... );
}

#endif // PARSER_IO_HPP