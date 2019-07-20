/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_CORE_HPP
#define VM_CORE_HPP

#include "../FE/Ethereal.hpp"
#include "DynLib.hpp"
#include "Consts.hpp"
#include "VMStack.hpp"
#include "unistd.h"

typedef std::vector< src_t * > src_stack_t;

typedef std::unordered_map< std::string, functions_t > type_funcs_t;

struct vm_state_t
{
	size_t flags;

	src_stack_t srcstack;
	srcs_t srcs;

	vars_t * vars;

	dyn_lib_t * dlib;
	consts_t * consts;
	functions_t funcs;
	vm_stack_t * stack;

	type_funcs_t typefuncs;

	vm_state_t();
	~vm_state_t();
};

#define VM_FAIL( ... ) src_fail( src.name, src.code[ ins.line - 1 ], \
				 ins.line, ins.col, __VA_ARGS__ )

#define VM_FAIL_TOK_CTR( tok_ctr, ... ) src_fail( src.name, src.code[ src.toks[ tok_ctr ].line - 1 ],	\
						  src.toks[ tok_ctr ].line, src.toks[ tok_ctr ].col,	\
						  __VA_ARGS__ )

bool set_init_mods( vm_state_t & vm );

#define _STRINGIZE(x) #x
#define STRINGIFY(x) _STRINGIZE(x)

#endif // VM_CORE_HPP