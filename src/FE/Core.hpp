/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef CORE_HPP
#define CORE_HPP

#include <cstdio>
#include <string>

#include "Err.hpp"

#ifdef _GNU_SOURCE
#define ATTR_FALLTHROUGH __attribute__ ( ( fallthrough ) )
#else
#define ATTR_FALLTHROUGH
#endif

template< typename T > struct res_t {
	int code;
	T data;
};

template< typename T > struct res_ptr_t {
	int code;
	T * data;
};

void src_fail( const std::string & src, const std::string & line_str, const int line, const int col, const char * msg, ... );

#endif // CORE_HPP