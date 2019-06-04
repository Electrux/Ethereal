/*
	Copyright (c) 2018, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef ERR_HPP
#define ERR_HPP

enum Errors {
	OK,
	FAIL,

	FILE_IO_ERR,
	FILE_EMPTY,

	ALLOC_FAIL,
	LEX_FAIL,
	PARSE_FAIL,
	BYTECODE_FAIL,
};

const char * ErrStr( Errors code );

#endif // ERR_HPP
