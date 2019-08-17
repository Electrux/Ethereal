/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../src/VM/Core.hpp"

#include "Core/Int.hpp"
#include "Core/Flt.hpp"
#include "Core/Bool.hpp"

const int MAX_C_STR_LEN = 1025;

static std::unordered_map< std::string, const char * > COL = {
	{ "0", "\033[0m" },

	{ "r", "\033[0;31m" },
	{ "g", "\033[0;32m" },
	{ "y", "\033[0;33m" },
	{ "b", "\033[0;34m" },
	{ "m", "\033[0;35m" },
	{ "c", "\033[0;36m" },
	{ "w", "\033[0;37m" },

	{ "br", "\033[1;31m" },
	{ "bg", "\033[1;32m" },
	{ "by", "\033[1;33m" },
	{ "bb", "\033[1;34m" },
	{ "bm", "\033[1;35m" },
	{ "bc", "\033[1;36m" },
	{ "bw", "\033[1;37m" },
};

void apply_colors( std::string & str )
{
	for( auto it = str.begin(); it != str.end(); ) {
		if( * it == '{' && ( it == str.begin() || ( * ( it - 1 ) != '$' && * ( it - 1 ) != '%' && * ( it - 1 ) != '#' && * ( it - 1 ) != '\\' ) ) ) {
			it = str.erase( it );
			if( it != str.end() && * it == '{' ) {
				++it;
				continue;
			}

			std::string var;

			while( it != str.end() && * it != '}' ) {
				var += * it;
				it = str.erase( it );
			}

			// Remove the ending brace
			if( it != str.end() ) it = str.erase( it );

			if( var.empty() ) continue;

			std::string val = COL[ var ];

			it = str.insert( it, val.begin(), val.end() );
			it += val.size();
		}
		else {
			++it;
		}
	}
}

var_base_t * flush_out( vm_state_t & vm, func_call_data_t & fcd )
{
	fflush( stdout );
	return nullptr;
}

var_base_t * flush_err( vm_state_t & vm, func_call_data_t & fcd )
{
	fflush( stderr );
	return nullptr;
}

var_base_t * print( vm_state_t & vm, func_call_data_t & fcd )
{
	for( auto & v : fcd.args ) {
		fprintf( stdout, "%s", v->to_str().c_str() );
	}
	return nullptr;
}

var_base_t * println( vm_state_t & vm, func_call_data_t & fcd )
{
	for( auto & v : fcd.args ) {
		fprintf( stdout, "%s", v->to_str().c_str() );
	}
	fprintf( stdout, "\n" );
	return nullptr;
}

var_base_t * dprint( vm_state_t & vm, func_call_data_t & fcd )
{
	for( auto & v : fcd.args ) {
		fprintf( stderr, "%s", v->to_str().c_str() );
	}
	return nullptr;
}

var_base_t * dprintln( vm_state_t & vm, func_call_data_t & fcd )
{
	for( auto & v : fcd.args ) {
		fprintf( stderr, "%s", v->to_str().c_str() );
	}
	fprintf( stderr, "\n" );
	return nullptr;
}

var_base_t * cprint( vm_state_t & vm, func_call_data_t & fcd )
{
	for( auto & v : fcd.args ) {
		std::string data = v->to_str();
		apply_colors( data );
		fprintf( stdout, "%s", data.c_str() );
	}
	return nullptr;
}

var_base_t * cprintln( vm_state_t & vm, func_call_data_t & fcd )
{
	for( auto & v : fcd.args ) {
		std::string data = v->to_str();
		apply_colors( data );
		fprintf( stdout, "%s", data.c_str() );
	}
	fprintf( stdout, "\n" );
	return nullptr;
}

var_base_t * scan( vm_state_t & vm, func_call_data_t & fcd )
{
	if( fcd.args.size() > 0 ) fprintf( stdout, "%s", fcd.args[ 0 ]->to_str().c_str() );
	char str[ MAX_C_STR_LEN ];
	fgets( str, MAX_C_STR_LEN, stdin );
	std::string res( str );
	while( res.back() == '\n' ) res.pop_back();
	while( res.back() == '\r' ) res.pop_back();
	return new var_str_t( res, fcd.args[ 0 ]->parse_ctr() );
}

var_base_t * type( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_str_t( fcd.args[ 0 ]->type_str(), fcd.args[ 0 ]->parse_ctr() );
}

var_base_t * to_str( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_str_t( fcd.args[ 0 ]->to_str(), fcd.args[ 0 ]->parse_ctr() );
}

var_base_t * exit_eth( vm_state_t & vm, func_call_data_t & fcd )
{
	vm.exit_called = true;
	vm.exit_status = fcd.args.size() == 0 ? 0 : fcd.args[ 0 ]->to_int().get_si();
	return nullptr;
}

var_base_t * assert_eth( vm_state_t & vm, func_call_data_t & fcd )
{
	if( fcd.args[ 0 ]->to_bool() ) { return nullptr; }
	src_t & src = * vm.srcstack.back();
	int line = src.bcode[ vm.bcodectr.back() ].line;
	int col = src.bcode[ vm.bcodectr.back() ].col;
	std::string op;
	int sz = fcd.args.size();
	for( int i = 1; i < sz; ++i ) {
		op += fcd.args[ i ]->to_str();
	}
	src_fail( src.name, src.code[ line - 1 ], line, col, "assertion failed: %s", op.c_str() );
	vm.exit_called = true;
	vm.exit_status = 1;
	return nullptr;
}

var_base_t * var_exists( vm_state_t & vm, func_call_data_t & fcd )
{
	return TRUE_FALSE( vm.vars->exists( fcd.args[ 0 ]->to_str(), true ) );
}

var_base_t * var_ref_count( vm_state_t & vm, func_call_data_t & fcd )
{
	if( !vm.vars->exists( fcd.args[ 0 ]->to_str(), true ) ) return new var_int_t( -1, fcd.args[ 0 ]->parse_ctr() );
	return new var_int_t( vm.vars->get( fcd.args[ 0 ]->to_str() )->ref(), fcd.args[ 0 ]->parse_ctr() );
}

REGISTER_MODULE( core )
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////// CORE ////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	vm.funcs.add( { "flush_out",  0,  0, {}, FnType::MODULE, { .modfn = flush_out }, false } );
	vm.funcs.add( { "flush_err",  0,  0, {}, FnType::MODULE, { .modfn = flush_err }, false } );
	vm.funcs.add( { "print",      1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = print }, false } );
	vm.funcs.add( { "println",    0, -1, { "_whatever_" }, FnType::MODULE, { .modfn = println }, false } );
	vm.funcs.add( { "dprint",     1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = dprint }, false } );
	vm.funcs.add( { "dprintln",   0, -1, { "_whatever_" }, FnType::MODULE, { .modfn = dprintln }, false } );
	vm.funcs.add( { "cprint",     1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = cprint }, false } );
	vm.funcs.add( { "cprintln",   0, -1, { "_whatever_" }, FnType::MODULE, { .modfn = cprintln }, false } );
	vm.funcs.add( { "scan",       0,  1, { "_whatever_" }, FnType::MODULE, { .modfn = scan }, true } );
	vm.funcs.add( { "exit",       0,  1, { "_any_" }, FnType::MODULE, { .modfn = exit_eth }, false } );
	vm.funcs.add( { "assert",     2, -1, { "_any_", "_whatever_" }, FnType::MODULE, { .modfn = assert_eth }, false } );
	vm.funcs.add( { "var_exists", 1,  1, { "str" }, FnType::MODULE, { .modfn = var_exists }, false } );
	vm.funcs.add( { "var_ref_count", 1,  1, { "_any_" }, FnType::MODULE, { .modfn = var_ref_count }, true } );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////// INT ////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// basic arithmetic
	vm.funcs.add( { "+",   2, 2, { "int", "int" }, FnType::MODULE, { .modfn = add }, true } );
	vm.funcs.add( { "-",   2, 2, { "int", "int" }, FnType::MODULE, { .modfn = sub }, true } );
	vm.funcs.add( { "*",   2, 2, { "int", "int" }, FnType::MODULE, { .modfn = mul }, true } );
	vm.funcs.add( { "/",   2, 2, { "int", "int" }, FnType::MODULE, { .modfn = div }, true } );
	vm.funcs.add( { "%",   2, 2, { "int", "int" }, FnType::MODULE, { .modfn = mod }, true } );

	// arithmetic assignments
	vm.funcs.add( { "+=",  2, 2, { "int", "int" }, FnType::MODULE, { .modfn = add_assn }, false } );
	vm.funcs.add( { "-=",  2, 2, { "int", "int" }, FnType::MODULE, { .modfn = sub_assn }, false } );
	vm.funcs.add( { "*=",  2, 2, { "int", "int" }, FnType::MODULE, { .modfn = mul_assn }, false } );
	vm.funcs.add( { "/=",  2, 2, { "int", "int" }, FnType::MODULE, { .modfn = div_assn }, false } );
	vm.funcs.add( { "%=",  2, 2, { "int", "int" }, FnType::MODULE, { .modfn = mod_assn }, false } );

	// comparisons
	vm.funcs.add( { "==", 2, 2, { "int", "int" }, FnType::MODULE, { .modfn = eqi }, false } );
	vm.funcs.add( { "!=", 2, 2, { "int", "int" }, FnType::MODULE, { .modfn = nei }, false } );
	vm.funcs.add( { "<",  2, 2, { "int", "int" }, FnType::MODULE, { .modfn = lti }, false } );
	vm.funcs.add( { "<=", 2, 2, { "int", "int" }, FnType::MODULE, { .modfn = lei }, false } );
	vm.funcs.add( { ">",  2, 2, { "int", "int" }, FnType::MODULE, { .modfn = gti }, false } );
	vm.funcs.add( { ">=", 2, 2, { "int", "int" }, FnType::MODULE, { .modfn = gei }, false } );
	vm.funcs.add( { "!",  1, 1, { "int" }, FnType::MODULE, { .modfn = not_oper }, false } );

	// bitshift
	vm.funcs.add( { "<<", 2, 2, { "int", "int" }, FnType::MODULE, { .modfn = lshift }, true } );
	vm.funcs.add( { ">>", 2, 2, { "int", "int" }, FnType::MODULE, { .modfn = rshift }, true } );

	// bitwise assignments
	vm.funcs.add( { "<<=", 2, 2, { "int", "int" }, FnType::MODULE, { .modfn = lshift_assn }, false } );
	vm.funcs.add( { ">>=", 2, 2, { "int", "int" }, FnType::MODULE, { .modfn = rshift_assn }, false } );

	// bitwise
	vm.funcs.add( { "&", 2, 2, { "int", "int" }, FnType::MODULE, { .modfn = andi }, true } );
	vm.funcs.add( { "|", 2, 2, { "int", "int" }, FnType::MODULE, { .modfn = ori }, true } );
	vm.funcs.add( { "~", 1, 1, { "int" }, FnType::MODULE, { .modfn = not_oper_bitwise }, true } );

	// cool arithmetic
	vm.funcs.add( { "**",  2, 2, { "int", "int" }, FnType::MODULE, { .modfn = power }, true } );

	// unary
	vm.funcs.add( { "u-",  1, 1, { "int" }, FnType::MODULE, { .modfn = unary_sub }, true } );

	// other types to int
	vm.funcs.add( { "num", 1, 1, { "_any_" }, FnType::MODULE, { .modfn = num }, true } );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////// FLT ////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// basic arithmetic
	vm.funcs.add( { "+",   2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = addf }, true } );
	vm.funcs.add( { "-",   2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = subf }, true } );
	vm.funcs.add( { "*",   2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = mulf }, true } );
	vm.funcs.add( { "/",   2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = divf }, true } );

	// arithmetic assignments
	vm.funcs.add( { "+=",  2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = add_assnf }, false } );
	vm.funcs.add( { "-=",  2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = sub_assnf }, false } );
	vm.funcs.add( { "*=",  2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = mul_assnf }, false } );
	vm.funcs.add( { "/=",  2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = div_assnf }, false } );

	// comparisons
	vm.funcs.add( { "==", 2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = eqf }, false } );
	vm.funcs.add( { "!=", 2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = nef }, false } );
	vm.funcs.add( { "<",  2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = ltf }, false } );
	vm.funcs.add( { "<=", 2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = lef }, false } );
	vm.funcs.add( { ">",  2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = gtf }, false } );
	vm.funcs.add( { ">=", 2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = gef }, false } );
	vm.funcs.add( { "!",  1, 1, { "flt" }, FnType::MODULE, { .modfn = not_operf }, false } );

	// cool arithmetic
	vm.funcs.add( { "**",  2, 2, { "int", "flt" }, FnType::MODULE, { .modfn = powerf }, true } );

	// unary
	vm.funcs.add( { "u-",  1, 1, { "flt" }, FnType::MODULE, { .modfn = unary_subf }, true } );

	// other types to flt
	vm.funcs.add( { "flt", 1, 1, { "_any_" }, FnType::MODULE, { .modfn = flt }, true } );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////// BOOL ///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// basic logical
	vm.funcs.add( { "==", 2, 2, { "bool", "bool" }, FnType::MODULE, { .modfn = log_eq }, false } );
	vm.funcs.add( { "!=", 2, 2, { "bool", "bool" }, FnType::MODULE, { .modfn = log_ne }, false } );
	vm.funcs.add( { "!",  1, 1, { "bool" }, FnType::MODULE, { .modfn = not_operb }, false } );

	// other types to bool
	vm.funcs.add( { "bool", 1, 1, { "_any_" }, FnType::MODULE, { .modfn = bool_create }, false } );

	// global object functions
	functions_t & anyfns = vm.typefuncs[ "_any_" ];
	anyfns.add( { "type",   0, 0, {}, FnType::MODULE, { .modfn = type }, true } );
	anyfns.add( { "to_str", 0, 0, {}, FnType::MODULE, { .modfn = to_str }, true } );
}