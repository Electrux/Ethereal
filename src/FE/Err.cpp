/*
	Copyright (c) 2019, Electrux
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
	case E_ALLOC_FAIL:
		return "memory allocation failed";
	case E_LEX_FAIL:
		return "lexical analyzer failed";
	case E_PARSE_FAIL:
		return "parser failed";
	case E_BYTECODE_FAIL:
		return "bytecode failed";
	case E_FILE_EMPTY:
		return "empty file";
	case E_VM_FAIL:
		return "vm failed";
	case E_FAIL: // fall through
	default:
		return "unknown error";
	}
}
