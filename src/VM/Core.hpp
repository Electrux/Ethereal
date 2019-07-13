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

typedef std::vector< src_t * > src_stack_t;

struct vm_state_t
{
	srcs_t srcmap;
	src_stack_t srcstack;

	dyn_lib_t dlib;
	consts_t consts;
	vm_stack_t * stack;
	vm_state_t();
	~vm_state_t();
};

#define VM_FAIL( ... ) src_fail( src.code[ ins.line - 1 ], \
				    ins.line, ins.col, __VA_ARGS__ )

#endif // VM_CORE_HPP