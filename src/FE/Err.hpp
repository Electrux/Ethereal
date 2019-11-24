/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef ERR_HPP
#define ERR_HPP

enum Errors
{
	E_OK,
	E_FAIL,

	E_FILE_IO_ERR,
	E_FILE_EMPTY,

	E_ALLOC_FAIL,
	E_LEX_FAIL,
	E_PARSE_FAIL,
	E_BYTECODE_FAIL,

	// special case for assertion
	E_ASSERT_LVL1,
	E_ASSERT_LVL2,

	E_VM_FAIL,
};

const char * ErrStr( Errors code );

#endif // ERR_HPP
