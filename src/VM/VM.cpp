/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "VM.hpp"
#include "ExecInternal.hpp"

int vm_exec( src_t * main_src )
{
	srcs_t srcmap;
	src_stack_t srcstack;
	vm_state_t vm;
	vm.srcmap[ main_src->name ] = main_src;
	vm.srcstack.push_back( main_src );
	if( !set_init_mods( vm ) ) return E_VM_FAIL;
	int res = exec_internal( vm );
	for( auto & src : srcmap ) {
		delete src.second;
	}
	return res;
}