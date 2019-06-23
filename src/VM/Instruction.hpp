/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_INSTRUCTION_HPP
#define VM_INSTRUCTION_HPP

#include <string>

#include "Vars/Base.hpp"

enum InstrCode
{
	IC_POP_STACK,
	IC_LD_CONST,
	IC_SAVE_VAR,
};

struct oper_t
{
	Types type;
	std::string val;
};

struct instr_t
{
	int tok_ctr;
	InstrCode opcode;
	oper_t oper;
};

typedef std::vector< instr_t > bytecode_t;

#endif // VM_INSTRUCTION_HPP