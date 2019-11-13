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
	mpz_class & a = AS_INT( fcd.args[ 1 ] )->get();
	return new var_int_t( a >= 0 ? a : -a );
}

var_base_t * _sqrt( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & a = AS_INT( fcd.args[ 1 ] )->get();
	return new var_flt_t( sqrt( mpf_class( a ) ) );
}

var_base_t * _log( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class num = AS_INT( fcd.args[ 1 ] )->get();
	int base = fcd.args.size() > 2 ? AS_INT( fcd.args[ 2 ] )->get().get_si() : 10;
	// TODO: return infinite if number or base is <= 0
	mpz_class digits = 0;
	mpz_class tmp = num;
	num = 0;
	while( tmp > 0 ) {
		if( digits < 16 ) num += ( ( tmp % 10 ) * std::pow( 10, digits.get_si() ) );
		++digits;
		tmp /= 10;
	}

	double data = num.get_d();
	mpf_class res = std::log( data ) / std::log( base );
	if( digits <= 15 ) digits = 1;
	else digits -= 15;
	res *= digits;
	return new var_flt_t( res );
}

var_base_t * _ceil( vm_state_t & vm, func_call_data_t & fcd )
{
	mpf_class & a = AS_FLT( fcd.args[ 1 ] )->get();
	return new var_int_t( mpz_class( a ) + ( a != mpf_class( mpz_class( a ) ) ) );
}

REGISTER_MODULE( math )
{
	functions_t & mathfns = vm.typefuncs[ "_math_t" ];
	mathfns.add( { "abs", 1, 1, { "int" }, FnType::MODULE, { .modfn = _abs }, true } );
	mathfns.add( { "sqrt", 1, 1, { "int" }, FnType::MODULE, { .modfn = _sqrt }, true } );
	mathfns.add( { "log", 1, 2, { "int", "int" }, FnType::MODULE, { .modfn = _log }, true } );
	mathfns.add( { "ceil", 1, 1, { "flt" }, FnType::MODULE, { .modfn = _ceil }, true } );
}
