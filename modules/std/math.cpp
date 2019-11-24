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
static mpf_class high_precision_logn( const mpz_class & num, mpf_class precision );

var_base_t * _abs_int( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & a = AS_INT( fcd.args[ 1 ] )->get();
	return new var_int_t( a >= 0 ? a : -a );
}

var_base_t * _abs_flt( vm_state_t & vm, func_call_data_t & fcd )
{
	mpf_class & a = AS_FLT( fcd.args[ 1 ] )->get();
	return new var_flt_t( a >= 0.0 ? a : -a );
}

var_base_t * _ceil( vm_state_t & vm, func_call_data_t & fcd )
{
	mpf_class & a = AS_FLT( fcd.args[ 1 ] )->get();
	if( a < 0 ) return new var_int_t( mpz_class( a ) );
	return new var_int_t( mpz_class( a ) + ( a != mpf_class( mpz_class( a ) ) ) );
}

var_base_t * _floor( vm_state_t & vm, func_call_data_t & fcd )
{
	mpf_class & a = AS_FLT( fcd.args[ 1 ] )->get();
	if( a < 0 ) return new var_int_t( mpz_class( a ) - ( a != mpf_class( mpz_class( a ) ) ) );
	return new var_int_t( mpz_class( a ) );
}

var_base_t * _sqrt( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & a = AS_INT( fcd.args[ 1 ] )->get();
	if( a < 0 ) return new var_flt_t( 0 );
	return new var_flt_t( sqrt( mpf_class( a ) ) );
}

var_base_t * _log( vm_state_t & vm, func_call_data_t & fcd )
{
	mpz_class & num = AS_INT( fcd.args[ 1 ] )->get();
	mpz_class base = fcd.args.size() > 2 ? AS_INT( fcd.args[ 2 ] )->get() : 10;
	mpf_class & precision = AS_FLT( AS_STRUCT( vm.vars->get( "math" ) )->get_val()[ "precision" ] )->get();

	if( num < 1 || base < 2 ) {
		return new var_flt_t( 0 );
	}

	return new var_flt_t( high_precision_logn( num, precision ) / high_precision_logn( base, precision ) );
}

var_base_t * _sin( vm_state_t & vm, func_call_data_t & fcd )
{
	mpf_class & num = AS_FLT( fcd.args[ 1 ] )->get();
	mpz_class & precision_iter_max = AS_INT( AS_STRUCT( vm.vars->get( "math" ) )->get_val()[ "precision_iter_max" ] )->get();
	// begin with result = x
	mpf_class res = num;
	// sine(x) = x - x^3/3! + x^5/5! ...
	mpf_class num_sq = num * num;
	mpf_class pow = num * num_sq;
	mpz_class fact_of = 3;
	mpz_class neg_pos = -1;
	for( mpz_class i = 0; i < precision_iter_max; ++i ) {
		res += neg_pos * ( pow / factorial( fact_of ) );
		neg_pos = -neg_pos;
		pow *= num_sq;
		fact_of += 2;
	}
	return new var_flt_t( res );
}

var_base_t * _cos( vm_state_t & vm, func_call_data_t & fcd )
{
	mpf_class & num = AS_FLT( fcd.args[ 1 ] )->get();
	mpz_class & precision_iter_max = AS_INT( AS_STRUCT( vm.vars->get( "math" ) )->get_val()[ "precision_iter_max" ] )->get();
	// begin with result = 1
	mpf_class res = 1;
	// cos(x) = 1 - x^2/2! + x^4/4! ...
	mpf_class num_sq = num * num;
	mpf_class pow = num_sq;
	mpz_class fact_of = 2;
	mpz_class neg_pos = -1;
	for( mpz_class i = 0; i < precision_iter_max; ++i ) {
		res += neg_pos * ( pow / factorial( fact_of ) );
		neg_pos = -neg_pos;
		pow *= num_sq;
		fact_of += 2;
	}
	return new var_flt_t( res );
}

REGISTER_MODULE( math )
{
	functions_t & mathfns = vm.typefuncs[ "_math_t" ];
	mathfns.add( { "abs",   1, 1, { "int" }, FnType::MODULE, { .modfn = _abs_int }, true } );
	mathfns.add( { "abs",   1, 1, { "flt" }, FnType::MODULE, { .modfn = _abs_flt }, true } );
	mathfns.add( { "ceil",  1, 1, { "flt" }, FnType::MODULE, { .modfn = _ceil }, true } );
	mathfns.add( { "floor", 1, 1, { "flt" }, FnType::MODULE, { .modfn = _floor }, true } );
	mathfns.add( { "sqrt",  1, 1, { "int" }, FnType::MODULE, { .modfn = _sqrt }, true } );
	mathfns.add( { "log",   1, 2, { "int", "int" }, FnType::MODULE, { .modfn = _log }, true } );
	mathfns.add( { "sin",   1, 1, { "flt" }, FnType::MODULE, { .modfn = _sin }, true } );
	mathfns.add( { "cos",   1, 1, { "flt" }, FnType::MODULE, { .modfn = _cos }, true } );
}

static mpz_class factorial( const mpz_class & of )
{
	mpz_class fact = 1;
	for( mpz_class i = 2; i <= of; ++i ) fact *= i;
	return fact;
}

static mpf_class high_precision_logn( const mpz_class & num, mpf_class precision )
{
	precision /= 1000;
	// formula:
	// ln(z) = 2 . Summation( k, 0, inf )[ (1/(2k+1)) * ((z-1)/(z+1))^(2k+1) ]
	mpf_class res = 0.0;
	mpf_class k2plus1 = 1;
	// start with (z-1)/(z+1)
	mpf_class zpow = mpf_class( num - 1 ) / mpf_class( num + 1 );
	// multiplicative iteration of ((z-1)/(z+1)) ^ 2
	mpf_class square_zminus1byzplus1 = zpow * zpow;

	mpz_class i = 1;
	mpf_class inter = ( 2 / k2plus1 ) * zpow;
	for( i = 1; inter > precision; ++i ) {
		res += inter;
		k2plus1 += 2;
		zpow *= square_zminus1byzplus1;
		inter = ( 2 / k2plus1 ) * zpow;
	}
	return res;
}
/*
static mpf_class high_precision_logn( const mpz_class & num, mpf_class precision )
{
	precision /= 1000;
	// formula:
	// ln(z) = 2 . Summation( k, 0, inf )[ (1/(2k+1)) * ((z-1)/(z+1))^(2k+1) ]
	mpf_class res = 0.0;
	mpf_class k2plus1 = 1;
	// start with (z-1)/(z+1)
	mpf_class zpow = mpf_class( num - 1 ) / mpf_class( num + 1 );
	// multiplicative iteration of ((z-1)/(z+1)) ^ 2
	mpf_class square_zminus1byzplus1 = zpow * zpow;

	mpz_class i = 1;
	mpf_class inter = ( 2 / k2plus1 ) * zpow;
	for( i = 1; inter > precision; ++i ) {
		res += inter;
		k2plus1 += 2;
		zpow *= square_zminus1byzplus1;
		inter = ( 2 / k2plus1 ) * zpow;
	}
	std::cout << std::setprecision( float_precision() ) << ( res ) << " in iterations: " << i << "\n";
	return res;
}*/