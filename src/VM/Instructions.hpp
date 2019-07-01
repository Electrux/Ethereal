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

	IC_BUILD_ENUM,		// args: count of elements to take from stack (it will take enum's name by default)
	IC_LDMOD,		// args: count: 1 = what, 2 = what + as

	IC_FN_CALL,		// args: count of args (will take name by default)

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
};

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