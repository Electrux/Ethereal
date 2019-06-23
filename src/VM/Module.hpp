/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_MODULE_HPP
#define VM_MODULE_HPP

#include "Consts.hpp"
#include "Vars.hpp"
#include "Functions.hpp"

struct module_t
{
	consts_t consts;
	vars_t vars;
	funcs_t funcs;
	modules_t mods;
};

#endif // VM_MODULE_HPP