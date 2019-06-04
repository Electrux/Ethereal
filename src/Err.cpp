/*
	Copyright (c) 2018, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Err.hpp"

const char * ErrStr( Errors code )
{
	switch( code ) {
	case ALLOC_FAIL:
		return "memory allocation failed";
	case LEX_FAIL:
		return "lexical analyzer failed";
	case PARSE_FAIL:
		return "parser failed";
	case BYTECODE_FAIL:
		return "bytecode failed";
	case FILE_EMPTY:
		return "empty file";
	case FAIL: // fall through
	default:
		return "unknown error";
	}
}
