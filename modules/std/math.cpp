/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../../src/VM/Core.hpp"

static mpz_class factorial( const mpz_class & of );
static mpfr::mpreal high_precision_logn( const mpz_class & num, mpfr::mpreal precision );

var_base_t * flt_is_nan( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal & num = AS_FLT( fcd.args[ 0 ] )->get();
	return TRUE_FALSE( mpfr::isnan( num ) );
}

var_base_t * _abs_int( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & a = AS_INT( fcd.args[ 1 ] )->get();
	return new var_int_t( a >= 0 ? a : -a );
}

var_base_t * _abs_flt( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal & a = AS_FLT( fcd.args[ 1 ] )->get();
	return new var_flt_t( a >= 0.0 ? a : -a );
}

var_base_t * _ceil( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal & a = AS_FLT( fcd.args[ 1 ] )->get();
	return new var_int_t( mpfr::ceil( a ) );
}

var_base_t * _floor( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal & a = AS_FLT( fcd.args[ 1 ] )->get();
	return new var_int_t( mpfr::floor( a ) );
}

var_base_t * _sqrt( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal & a = AS_FLT( fcd.args[ 1 ] )->get();
	return new var_flt_t( mpfr::sqrt( a ) );
}

var_base_t * _log( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal & num = AS_FLT( fcd.args[ 1 ] )->get();
	mpfr::mpreal base = fcd.args.size() > 2 ? AS_FLT( fcd.args[ 2 ] )->get() : 10;

	if( num < 1 || base < 2 ) {
		mpfr::mpreal tmp;
		tmp.setNan();
		return new var_flt_t( tmp );
	}

	return new var_flt_t( mpfr::log( num ) / mpfr::log( base ) );
}

var_base_t * _sin( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal & num = AS_FLT( fcd.args[ 1 ] )->get();
	return new var_flt_t( mpfr::sin( num ) );
}

var_base_t * _cos( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal & num = AS_FLT( fcd.args[ 1 ] )->get();
	return new var_flt_t( mpfr::cos( num ) );
}

var_base_t * calc_e( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal precision = "0." + std::string( get_float_precision() + 1, '0' ) + "1";

	mpfr::mpreal res = 0.0;
	// 1/0! + 1/1! + 1/2! + 1/3! + 1/4! ...
	mpfr::mpreal delta = 1;
	for( mpz_class i = 1; delta > precision; ++i ) {
		res += delta;
		delta /= i.get_mpz_t();
	}
	return new var_flt_t( res );
}

var_base_t * calc_pi( vm_state_t & vm, func_call_data_t & fcd )
{
	mpfr::mpreal precision = "0." + std::string( get_float_precision() + 1, '0' ) + "1";

	mpfr::mpreal res = 0.0;
	// 1 - 1/3 + 1/5 - 1/7 + 1/9 ...
	mpfr::mpreal delta = 1;
	for( mpfr::mpreal i = 1; delta > precision; i += 2 ) {
		res += 1.0 / i;
		delta /= i;
	}
	return new var_flt_t( res );
}

REGISTER_MODULE( math )
{
	vm.funcs.add( { "math_calc_e",  0, 0, {}, FnType::MODULE, { .modfn = calc_e  }, true } );
	vm.funcs.add( { "math_calc_pi", 0, 0, {}, FnType::MODULE, { .modfn = calc_pi }, true } );

	functions_t & fltfns = vm.typefuncs[ "flt" ];
	fltfns.add( { "is_nan", 0, 0, {}, FnType::MODULE, { .modfn = flt_is_nan }, false } );

	functions_t & mathfns = vm.typefuncs[ "_math_t" ];
	mathfns.add( { "abs",    1, 1, { "int" }, FnType::MODULE, { .modfn = _abs_int }, true } );
	mathfns.add( { "abs",    1, 1, { "flt" }, FnType::MODULE, { .modfn = _abs_flt }, true } );
	mathfns.add( { "ceil",   1, 1, { "flt" }, FnType::MODULE, { .modfn = _ceil }, true } );
	mathfns.add( { "floor",  1, 1, { "flt" }, FnType::MODULE, { .modfn = _floor }, true } );
	mathfns.add( { "sqrt",   1, 1, { "flt" }, FnType::MODULE, { .modfn = _sqrt }, true } );
	mathfns.add( { "log",    1, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = _log }, true } );
	mathfns.add( { "sin",    1, 1, { "flt" }, FnType::MODULE, { .modfn = _sin }, true } );
	mathfns.add( { "cos",    1, 1, { "flt" }, FnType::MODULE, { .modfn = _cos }, true } );

}

static mpz_class factorial( const mpz_class & of )
{
	mpz_class fact = 1;
	for( mpz_class i = 2; i <= of; ++i ) fact *= i;
	return fact;
}
