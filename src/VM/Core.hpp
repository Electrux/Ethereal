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

#include "DynLib.hpp"
#include "ExecStack.hpp"
#include "Consts.hpp"
#include "Vars.hpp"
#include "Functions.hpp"

struct vm_data_t
{
	dyn_lib_t dlib;
	exec_stack_t stack;
	modules_t mods;
};

struct module_t
{
	consts_t consts;
	vars_t vars;
	funcs_t funcs;
	modules_t mods;
};

#endif // VM_CORE_HPP