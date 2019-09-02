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

var_base_t * _abs( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & a = AS_INT( fcd.args[ 0 ] )->get();
	return new var_int_t( a >= 0 ? a : -a );
}

var_base_t * _sqrt( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & a = AS_INT( fcd.args[ 0 ] )->get();
	return new var_flt_t( sqrt( mpf_class( a ) ) );
}

REGISTER_MODULE( math )
{
	vm.funcs.add( { "abs", 1, 1, { "int" }, FnType::MODULE, { .modfn = _abs }, true } );
	vm.funcs.add( { "sqrt", 1, 1, { "int" }, FnType::MODULE, { .modfn = _sqrt }, true } );
}
