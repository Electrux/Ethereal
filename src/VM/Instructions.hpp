/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_INSTRUCTIONS_HPP
#define VM_INSTRUCTIONS_HPP

#include <vector>
#include <string>

enum InstrCode
{
	IC_PUSH,		// Push on the VM stack
	IC_POP,			// Pop from the VM stack
	IC_STORE,		// args: none, optionally - count of elements to store
	IC_STORE_LOAD,		// same as IC_STORE + push the result back on stack
	IC_STORE_NO_COPY,	// args: none
	IC_STORE_LOAD_NO_COPY,	// args: none
	IC_STORE_STACK,		// args: none, optionally - count of elements to store
	IC_STORE_LOAD_STACK,	// same as IC_STORE_STACK + push the result back on stack

	IC_JUMP,		// args: bcode index (int) on which to jump to
	IC_JUMP_TRUE,		// args: bcode index (int) on which to jump to
	IC_JUMP_FALSE,		// args: bcode index (int) on which to jump to
	IC_JUMP_TRUE_NO_POP,	// args: bcode index (int) on which to jump to
	IC_JUMP_FALSE_NO_POP,	// args: bcode index (int) on which to jump to

	IC_BUILD_ENUM,		// args: count of elements to take from stack
	IC_BUILD_ENUM_MASK,	// args: same as IC_BUILD_ENUM
	IC_BUILD_VEC,		// args: count of elements to take from stack
	IC_BUILD_MAP,		// args: count of elements to take from stack
	IC_BUILD_STRUCT,	// args: count of fields
	IC_BUILD_FN,		// args: func name
	IC_BUILD_MFN,		// args: func name

	IC_ADD_SCOPE,		// args: count of scopes to add
	IC_REM_SCOPE,		// args: count of scopes to remove

	IC_LDMOD,		// args: count: 1 = what, 2 = what + as
	IC_IMPORT,		// args: count: 1 = what

	IC_FN_CALL,		// args: count of args (will take name by default)
	IC_MFN_CALL,		// args: same as IC_FN_CALL

	IC_STRUCT_DECL,		// args: same as IC_FN_CALL
	IC_ATTR,		// args: same as IC_FN_CALL

	IC_BLOCK_TILL,		// args: index of bcode uptil where block lasts
	IC_ARGS_TILL,

	IC_RETURN,		// args: count of scopes to remove
	IC_RETURN_EMPTY,	// args: count of scopes to remove

	_IC_LAST,
};

extern const char * InstrCodeStrs[ _IC_LAST ];

enum OperTypes
{
	OP_CONST,
	OP_STR,
	OP_INT,
	OP_FLT,

	OP_NONE,

	_OP_LAST,
};

extern const char * OperTypeStrs[ _OP_LAST ];

struct oper_t
{
	OperTypes type;
	std::string val;
};

struct instr_t
{
	int parse_ctr;
	int line, col;
	InstrCode opcode;
	oper_t oper;
};

typedef std::vector< instr_t > bytecode_t;

#endif // VM_INSTRUCTIONS_HPP