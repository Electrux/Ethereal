/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "VM/ExecInternal.hpp"
#include "VM.hpp"

int exec_byte_code( src_t & main_src )
{
	vm_state_t vmstate;
	vmstate.mods[ main_src.name ] = new var_mod_t( main_src.name, main_src );
	vmstate.srcs.push_back( { main_src.name, main_src } );
	var_base_t * res = exec_internal( vmstate );
	vmstate.srcs.pop_back();
	if( res == nullptr ) return 0;
	int r = res->to_int().get_si();
	VAR_DREF( res );
	return r;
}