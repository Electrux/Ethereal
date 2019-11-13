/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <cmath>

#include "../../src/VM/Core.hpp"

var_base_t * func_count( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_int_t( vm.funcs.count() );
}

REGISTER_MODULE( runtime )
{
	functions_t & runtimefns = vm.typefuncs[ "_runtime_t" ];
	runtimefns.add( { "fn_count", 0, 0, {}, FnType::MODULE, { .modfn = func_count }, true } );
}
