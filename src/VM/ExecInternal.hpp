/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef VM_EXEC_INTERNAL_HPP
#define VM_EXEC_INTERNAL_HPP

#include "VM.hpp"

int exec_internal( vm_state_t & vm, long begin = -1, long end = -1 );

#endif // VM_EXEC_INTERNAL_HPP