/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../Vars/Base.hpp"
#include "../Core.hpp"

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

var_base_t * print( std::vector< var_base_t * > & vars )
{
	for( auto & v : vars ) {
		fprintf( stdout, "%s", v->to_str().c_str() );
	}
	fflush( stdout );
	return nullptr;
}

var_base_t * println( std::vector< var_base_t * > & vars )
{
	for( auto & v : vars ) {
		fprintf( stdout, "%s", v->to_str().c_str() );
	}
	fprintf( stdout, "\n" );
	return nullptr;
}

var_base_t * cprint( std::vector< var_base_t * > & vars )
{
	for( auto & v : vars ) {
		std::string data = v->to_str();
		apply_colors( data );
		fprintf( stdout, "%s", data.c_str() );
	}
	fflush( stdout );
	return nullptr;
}

var_base_t * cprintln( std::vector< var_base_t * > & vars )
{
	for( auto & v : vars ) {
		std::string data = v->to_str();
		apply_colors( data );
		fprintf( stdout, "%s", data.c_str() );
	}
	fprintf( stdout, "\n" );
	return nullptr;
}

var_base_t * scan( std::vector< var_base_t * > & vars )
{
	if( vars.size() > 0 ) fprintf( stdout, "%s", vars[ 0 ]->to_str().c_str() );
	char str[ MAX_C_STR_LEN ];
	fgets( str, MAX_C_STR_LEN, stdin );
	std::string res( str );
	while( res.back() == '\n' ) res.pop_back();
	while( res.back() == '\r' ) res.pop_back();
	return new var_str_t( res, vars[ 0 ]->parse_ctr() );
}

var_base_t * type( std::vector< var_base_t * > & vars )
{
	return new var_str_t( vars[ 0 ]->type_str(), vars[ 0 ]->parse_ctr() );
}

REGISTER_MODULE( core )
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////// CORE ////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	vm.funcs.add( { "print",   1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = print }, false } );
	vm.funcs.add( { "println", 0, -1, { "_whatever_" }, FnType::MODULE, { .modfn = println }, false } );
	vm.funcs.add( { "cprint",   1, -1, { "_whatever_" }, FnType::MODULE, { .modfn = cprint }, false } );
	vm.funcs.add( { "cprintln", 0, -1, { "_whatever_" }, FnType::MODULE, { .modfn = cprintln }, false } );
	vm.funcs.add( { "scan",    0,  1, { "_whatever_" }, FnType::MODULE, { .modfn = scan }, true } );

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
	vm.funcs.add( { "==", 2, 2, { "int", "int" }, FnType::MODULE, { .modfn = eqi }, true } );
	vm.funcs.add( { "!=", 2, 2, { "int", "int" }, FnType::MODULE, { .modfn = nei }, true } );
	vm.funcs.add( { "<",  2, 2, { "int", "int" }, FnType::MODULE, { .modfn = lti }, true } );
	vm.funcs.add( { "<=", 2, 2, { "int", "int" }, FnType::MODULE, { .modfn = lei }, true } );
	vm.funcs.add( { ">",  2, 2, { "int", "int" }, FnType::MODULE, { .modfn = gti }, true } );
	vm.funcs.add( { ">=", 2, 2, { "int", "int" }, FnType::MODULE, { .modfn = gei }, true } );
	vm.funcs.add( { "!", 1, 1, { "int" }, FnType::MODULE, { .modfn = not_oper }, true } );

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
	vm.funcs.add( { "==", 2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = eqf }, true } );
	vm.funcs.add( { "!=", 2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = nef }, true } );
	vm.funcs.add( { "<",  2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = ltf }, true } );
	vm.funcs.add( { "<=", 2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = lef }, true } );
	vm.funcs.add( { ">",  2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = gtf }, true } );
	vm.funcs.add( { ">=", 2, 2, { "flt", "flt" }, FnType::MODULE, { .modfn = gef }, true } );
	vm.funcs.add( { "!", 1, 1, { "flt" }, FnType::MODULE, { .modfn = not_operf }, true } );

	// cool arithmetic
	vm.funcs.add( { "**",  2, 2, { "flt", "int" }, FnType::MODULE, { .modfn = powerf }, true } );

	// unary
	vm.funcs.add( { "u-",  1, 1, { "flt" }, FnType::MODULE, { .modfn = unary_subf }, true } );

	// other types to flt
	vm.funcs.add( { "flt", 1, 1, { "_any_" }, FnType::MODULE, { .modfn = flt }, true } );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////// BOOL ///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// basic arithmetic
	vm.funcs.add( { "&&",   2, 2, { "bool", "bool" }, FnType::MODULE, { .modfn = log_and }, true } );
	vm.funcs.add( { "||",   2, 2, { "bool", "bool" }, FnType::MODULE, { .modfn = log_or }, true } );
	vm.funcs.add( { "==",   2, 2, { "bool", "bool" }, FnType::MODULE, { .modfn = log_eq }, true } );
	vm.funcs.add( { "!=",   2, 2, { "bool", "bool" }, FnType::MODULE, { .modfn = log_ne }, true } );
	vm.funcs.add( { "!", 1, 1, { "bool" }, FnType::MODULE, { .modfn = not_operb }, true } );

	// other types to bool
	vm.funcs.add( { "bool", 1, 1, { "_any_" }, FnType::MODULE, { .modfn = bool_create }, true } );

	// global object functions
	functions_t & anyfns = vm.typefuncs[ "_any_" ];
	anyfns.add( { "type", 0, 0, {}, FnType::MODULE, { .modfn = type }, true } );
}