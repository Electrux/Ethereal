/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_EXEC_INTERNAL_HPP
#define VM_EXEC_INTERNAL_HPP

#include "VM.hpp"

// TODO: Perhaps use `union` type for operand value of `instr_code_t` to avoid using conversions like `std::stoi` in ExecInternal
int exec_internal( vm_state_t & vm, long begin = -1, long end = -1, var_base_t * ret = nullptr );

#endif // VM_EXEC_INTERNAL_HPP