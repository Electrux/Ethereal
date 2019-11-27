/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../../src/FE/Env.hpp"
#include "../../src/VM/Core.hpp"
#include "../../src/VM/LoadFile.hpp"

#include "Core/Int.hpp"
#include "Core/Flt.hpp"
#include "Core/Bool.hpp"

#include "Core/Range/Int.hpp"
#include "Core/Range/Flt.hpp"

const int MAX_C_STR_LEN = 1025;

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

var_base_t * scan( vm_state_t & vm, func_call_data_t & fcd )
{
	if( fcd.args.size() > 0 ) fprintf( stdout, "%s", fcd.args[ 0 ]->to_str().c_str() );
	char str[ MAX_C_STR_LEN ];
	fgets( str, MAX_C_STR_LEN, stdin );
	std::string res( str );
	while( res.back() == '\n' ) res.pop_back();
	while( res.back() == '\r' ) res.pop_back();
	return new var_str_t( res );
}

var_base_t * type( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_str_t( fcd.args[ 0 ]->type_str() );
}

var_base_t * to_str( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_str_t( fcd.args[ 0 ]->to_str() );
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
	vm.exit_called = true;
	vm.exit_status = E_ASSERT_LVL2;
	return nullptr;
	src_t & src = * vm.srcstack.back();
	int line = src.bcode[ fcd.bcodectr ].line;
	int col = src.bcode[ fcd.bcodectr ].col;
	std::string op;
	int sz = fcd.args.size();
	for( int i = 1; i < sz; ++i ) {
		op += fcd.args[ i ]->to_str();
	}
	src_fail( src.file, src.code[ line - 1 ], line, col, "assertion failed: %s", op.c_str() );
	vm.exit_called = true;
	vm.exit_status = 1;
	return nullptr;
}
var_base_t * assert_eth_fail( vm_state_t & vm, func_call_data_t & fcd )
{
	vm.exit_called = true;
	vm.exit_status = E_ASSERT_LVL2;
	int sz = fcd.args.size();
	vm.fail_msg.clear();
	for( int i = 0; i < sz; ++i ) {
		vm.fail_msg += fcd.args[ i ]->to_str();
	}
	return nullptr;
}

var_base_t * var_exists( vm_state_t & vm, func_call_data_t & fcd )
{
	return TRUE_FALSE( vm.vars->exists( fcd.args[ 0 ]->to_str() ) );
}

var_base_t * var_mfn_exists( vm_state_t & vm, func_call_data_t & fcd )
{
	if( vm.typefuncs.find( fcd.args[ 0 ]->type_str() ) == vm.typefuncs.end() ) return vm.vars->get( "false" );
	return TRUE_FALSE( vm.typefuncs[ fcd.args[ 0 ]->type_str() ].exists_name( fcd.args[ 1 ]->to_str() ) );
}

var_base_t * var_ref_count( vm_state_t & vm, func_call_data_t & fcd )
{
	if( !vm.vars->exists( fcd.args[ 0 ]->to_str() ) ) return new var_int_t( -1 );
	return new var_int_t( vm.vars->get( fcd.args[ 0 ]->to_str() )->ref() );
}

var_base_t * add_inc_dirs( vm_state_t & vm, func_call_data_t & fcd )
{
	for( auto & _dir : fcd.args ) {
		std::string dir = _dir->to_str();
		DirFormat( dir );
		AS_VEC( vm.inc_dirs )->get().insert(
			AS_VEC( vm.inc_dirs )->get().begin(),
			new var_str_t( dir, fcd.parse_ctr )
		);
	}
	return nullptr;
}

var_base_t * add_lib_dirs( vm_state_t & vm, func_call_data_t & fcd )
{
	for( auto & _dir : fcd.args ) {
		std::string dir = _dir->to_str();
		DirFormat( dir );
		AS_VEC( vm.lib_dirs )->get().insert(
			AS_VEC( vm.lib_dirs )->get().begin(),
			new var_str_t( dir, fcd.parse_ctr )
		);
	}
	return nullptr;
}

var_base_t * load_module( vm_state_t & vm, func_call_data_t & fcd )
{
	std::string name = fcd.args[ 0 ]->to_str();
	auto last_slash = name.find_last_of( '/' );
	name.insert( last_slash + 1, "lib" );

	std::string file = name + LIB_EXT;
	std::string init_fn_str = name.substr( name.find_last_of( '/' ) + 4 );

	src_t & src = * vm.srcstack.back();
	int line = src.bcode[ fcd.bcodectr ].line;
	int col = src.bcode[ fcd.bcodectr ].col;

	init_fnptr_t init_fn = nullptr;

	if( !mod_exists( file, vm.lib_dirs ) ) {
		src_fail( src.file, src.code[ line - 1 ], line, col,
			  "could not find module '%s' for loading",
			  name.c_str() );
		fprintf( stderr, "checked the following paths:\n" );
		for( auto & loc : AS_VEC( vm.lib_dirs )->get() ) {
			fprintf( stderr, "-> %s\n", ( loc->to_str() + "/" + file ).c_str() );
		}
		goto fail;
	}
	if( vm.dlib->load( file ) == nullptr ) goto fail;
	init_fn = ( init_fnptr_t ) vm.dlib->get( file, "init_" + init_fn_str );
	if( init_fn == nullptr ) {
		src_fail( src.file, src.code[ line - 1 ], line, col,
			  "failed to find init function '%s' in module '%s'",
			  init_fn_str.c_str(), file.c_str() );
		goto fail;
	}
	init_fn( vm );
	return new var_int_t( E_OK );
fail:
	return new var_int_t( E_FAIL );
}

var_base_t * import_module( vm_state_t & vm, func_call_data_t & fcd )
{
	std::string name = fcd.args[ 0 ]->to_str();
	std::string file = name + ".et";

	src_t & src = * vm.srcstack.back();
	int line = src.bcode[ fcd.bcodectr ].line;
	int col = src.bcode[ fcd.bcodectr ].col;

	int ret = E_OK;

	if( !mod_exists( file, vm.inc_dirs ) ) {
		src_fail( src.file, src.code[ line - 1 ], line, col, "could not find file '%s' for importing", name.c_str() );
		fprintf( stderr, "checked the following paths:\n" );
		for( auto & loc : AS_VEC( vm.inc_dirs )->get() ) {
			fprintf( stderr, "-> %s\n", ( loc->to_str() + "/" + file ).c_str() );
		}
		ret = E_FAIL;
		goto fail;
	}

	ret = load_src( vm, file );
	if( ret != E_OK ) {
		src_fail( src.file, src.code[ line - 1 ], line, col,
			  "could not import '%s', see the error above; aborting",
			  file.c_str() );
		goto fail;
	}
	return new var_int_t( E_OK );
fail:
	return new var_int_t( ret );
}

var_base_t * set_float_precision( vm_state_t & vm, func_call_data_t & fcd )
{
	update_float_precision( fcd.args[ 0 ]->to_int().get_ui() );
	return nullptr;
}

var_base_t * get_float_precision( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_int_t( get_float_precision() );
}

var_base_t * get_float_precision_num( vm_state_t & vm, func_call_data_t & fcd )
{
	return new var_flt_t( "0." + std::string( get_float_precision() - 1, '0' ) + "1" );
}

var_base_t * nil_eq( vm_state_t & vm, func_call_data_t & fcd )
{
	return TRUE_FALSE( fcd.args[ 1 ]->type() == fcd.args[ 0 ]->type() );
}

var_base_t * nil_ne( vm_state_t & vm, func_call_data_t & fcd )
{
	return TRUE_FALSE( fcd.args[ 1 ]->type() != fcd.args[ 0 ]->type() );
}

REGISTER_MODULE( core )
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////// CORE ////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	vm.funcs.add( { "flush_out",             0,  0, {}, FnType::MODULE, { .modfn = flush_out }, false } );
	vm.funcs.add( { "flush_err",             0,  0, {}, FnType::MODULE, { .modfn = flush_err }, false } );
	vm.funcs.add( { "print",                 1, -1, { "_any_", "_whatever_" }, FnType::MODULE, { .modfn = print }, false } );
	vm.funcs.add( { "println",               0, -1, { "_whatever_" }, FnType::MODULE, { .modfn = println }, false } );
	vm.funcs.add( { "dprint",                1, -1, { "_any_", "_whatever_" }, FnType::MODULE, { .modfn = dprint }, false } );
	vm.funcs.add( { "dprintln",              0, -1, { "_whatever_" }, FnType::MODULE, { .modfn = dprintln }, false } );
	vm.funcs.add( { "scan",                  0,  1, { "_whatever_" }, FnType::MODULE, { .modfn = scan }, true } );
	vm.funcs.add( { "exit",                  0,  1, { "_any_" }, FnType::MODULE, { .modfn = exit_eth }, false } );
	vm.funcs.add( { "assert_fail",           1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = assert_eth_fail }, false } );
	vm.funcs.add( { "var_exists",            1,  1, { "str" }, FnType::MODULE, { .modfn = var_exists }, false } );
	vm.funcs.add( { "var_mfn_exists",        2,  2, { "_any_", "str" }, FnType::MODULE, { .modfn = var_mfn_exists }, false } );
	vm.funcs.add( { "var_ref_count",         1,  1, { "_any_" }, FnType::MODULE, { .modfn = var_ref_count }, true } );
	vm.funcs.add( { "__add_incs__",	         1, -1, { "_any_", "_whatever_" }, FnType::MODULE, { .modfn = add_inc_dirs }, false } );
	vm.funcs.add( { "__add_libs__",	         1, -1, { "_any_", "_whatever_" }, FnType::MODULE, { .modfn = add_lib_dirs }, false } );
	vm.funcs.add( { "_ldmod_",	         1,  1, { "str" }, FnType::MODULE, { .modfn = load_module }, true } );
	vm.funcs.add( { "__import__",	         1,  1, { "str" }, FnType::MODULE, { .modfn = import_module }, true } );
	vm.funcs.add( { "set_flt_precision",     1,  1, { "int" }, FnType::MODULE, { .modfn = set_float_precision }, false } );
	vm.funcs.add( { "get_flt_precision",     0,  0, {}, FnType::MODULE, { .modfn = get_float_precision }, true } );
	vm.funcs.add( { "get_flt_precision_num", 0,  0, {}, FnType::MODULE, { .modfn = get_float_precision_num }, true } );

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
	vm.funcs.add( { "int", 1, 1, { "_any_" }, FnType::MODULE, { .modfn = _int }, true } );

	// hashing
	vm.typefuncs[ "int" ].add( { "hash", 0, 0, {}, FnType::MODULE, { .modfn = hash_int }, true } );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////// FLT ////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// basic arithmetic
	vm.funcs.add( { "+",   2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = addf }, true } );
	vm.funcs.add( { "-",   2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = subf }, true } );
	vm.funcs.add( { "*",   2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = mulf }, true } );
	vm.funcs.add( { "/",   2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = divf }, true } );
	// LHS is int
	vm.funcs.add( { "+",   2, 2, { "flt", "int" }, FnType::MODULE, { .modfn = addif }, true } );
	vm.funcs.add( { "-",   2, 2, { "flt", "int" }, FnType::MODULE, { .modfn = subif }, true } );
	vm.funcs.add( { "*",   2, 2, { "flt", "int" }, FnType::MODULE, { .modfn = mulif }, true } );
	vm.funcs.add( { "/",   2, 2, { "flt", "int" }, FnType::MODULE, { .modfn = divif }, true } );
	// RHS is int
	vm.funcs.add( { "+",   2, 2, { "int", "flt" }, FnType::MODULE, { .modfn = addfi }, true } );
	vm.funcs.add( { "-",   2, 2, { "int", "flt" }, FnType::MODULE, { .modfn = subfi }, true } );
	vm.funcs.add( { "*",   2, 2, { "int", "flt" }, FnType::MODULE, { .modfn = mulfi }, true } );
	vm.funcs.add( { "/",   2, 2, { "int", "flt" }, FnType::MODULE, { .modfn = divfi }, true } );

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

	// hashing
	vm.typefuncs[ "flt" ].add( { "hash", 0, 0, {}, FnType::MODULE, { .modfn = hash_flt }, true } );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////// BOOL ///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// basic logical
	vm.funcs.add( { "==", 2, 2, { "bool", "bool" }, FnType::MODULE, { .modfn = log_eq }, false } );
	vm.funcs.add( { "!=", 2, 2, { "bool", "bool" }, FnType::MODULE, { .modfn = log_ne }, false } );
	vm.funcs.add( { "!",  1, 1, { "bool" }, FnType::MODULE, { .modfn = not_operb }, false } );

	// other types to bool
	vm.funcs.add( { "bool", 1, 1, { "_any_" }, FnType::MODULE, { .modfn = bool_create }, false } );

	// hashing
	vm.typefuncs[ "bool" ].add( { "hash", 0, 0, {}, FnType::MODULE, { .modfn = hash_bool }, true } );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////// NIL ////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	vm.funcs.add( { "==", 2, 2, { "nil", "_any_" }, FnType::MODULE, { .modfn = nil_eq }, false } );
	vm.funcs.add( { "==", 2, 2, { "_any_", "nil" }, FnType::MODULE, { .modfn = nil_eq }, false } );
	vm.funcs.add( { "!=", 2, 2, { "nil", "_any_" }, FnType::MODULE, { .modfn = nil_ne }, false } );
	vm.funcs.add( { "!=", 2, 2, { "_any_", "nil" }, FnType::MODULE, { .modfn = nil_ne }, false } );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////// MISC ///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// range (int)
	functions_t & ritifns = vm.typefuncs[ "range_iter_int_t" ];
	ritifns.add(  { "next",  0, 0, {}, FnType::MODULE, { .modfn = next_int }, true } );
	vm.funcs.add( { "range", 2, 3, { "int", "int", "int" }, FnType::MODULE, { .modfn = range_int }, true } );
	// range (flt)
	functions_t & ritffns = vm.typefuncs[ "range_iter_flt_t" ];
	ritffns.add(  { "next",  0, 0, {}, FnType::MODULE, { .modfn = next_flt }, true } );
	vm.funcs.add( { "range", 2, 3, { "flt", "flt", "flt" }, FnType::MODULE, { .modfn = range_flt }, true } );

	// global object functions
	functions_t & anyfns = vm.typefuncs[ "_any_" ];
	anyfns.add( { "type",   0, 0, {}, FnType::MODULE, { .modfn = type }, true } );
	anyfns.add( { "to_str", 0, 0, {}, FnType::MODULE, { .modfn = to_str }, true } );
}