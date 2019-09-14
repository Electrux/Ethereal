/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Instructions.hpp"

const char * InstrCodeStrs[ _IC_LAST ] = {
	"PUSH",
	"POP",
	"STORE",
	"STORE_LOAD",
	"STORE_NO_COPY",
	"STORE_LOAD_NO_COPY",
	"STORE_STACK",
	"STORE_LOAD_STACK",

	"JUMP",
	"JUMP_TRUE",
	"JUMP_FALSE",
	"JUMP_TRUE_NO_POP",
	"JUMP_FALSE_NO_POP",

	"BUILD_ENUM",
	"BUILD_ENUM_MASK",
	"BUILD_VEC",
	"BUILD_MAP",
	"BUILD_STRUCT",
	"BUILD_FUNC",
	"BUILD_MEM_FUNC",

	"ADD_SCOPE",
	"REM_SCOPE",

	"LOAD_MOD",
	"IMPORT",

	"CALL_FUNC",
	"CALL_MEM_FUNC",
	"STRUCT_DECL",
	"OBJ_ATTR",

	"BLOCK_TILL",
	"ARGS_TILL",

	"RETURN",
	"RETURN_EMPTY",
};

const char * OperTypeStrs[ _OP_LAST ] = {
	"const",
	"string",
	"int",
	"float",

	"none",
};