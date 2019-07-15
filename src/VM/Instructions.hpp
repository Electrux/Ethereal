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

	IC_JUMP,		// args: bcode index (int) on which to jump to
	IC_JUMP_TRUE,		// args: bcode index (int) on which to jump to
	IC_JUMP_FALSE,		// args: bcode index (int) on which to jump to

	IC_BUILD_ENUM,		// args: count of elements to take from stack
	IC_BUILD_VEC,		// args: count of elements to take from stack
	IC_BUILD_MAP,		// args: count of elements to take from stack

	IC_ADD_SCOPE,		// args: none
	IC_REM_SCOPE,		// args: none

	IC_LDMOD,		// args: count: 1 = what, 2 = what + as
	IC_IMPORT,		// args: count: 1 = what

	IC_FN_CALL,		// args: count of args (will take name by default)
	IC_STRUCT_DECL,		// args: same as IC_FN_CALL

	IC_RETURN,		// args: bool - returns something or not (not = returns OP_NONE)

	IC_SUBSCR,		// args: name of variable

	IC_STRUCT_FIELD,	// args: name of the field, takes one from stack as default value

	IC_STRUCT_BUILD,	// args: count of fields

	_IC_LAST,
};

extern const char * InstrCodeStrs[ _IC_LAST ];

enum OperTypes
{
	OP_CONST,
	OP_STR,
	OP_INT,
	OP_FLT,
	OP_BOOL,

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
	bool is_mem;
};

typedef std::vector< instr_t > bytecode_t;

#endif // VM_INSTRUCTIONS_HPP