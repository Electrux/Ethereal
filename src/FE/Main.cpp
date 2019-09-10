/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <cstdio>
#include <vector>
#include <string>

#include "CmdArgs.hpp"
#include "Env.hpp"
#include "FS.hpp"
#include "Parser.hpp"
#include "../VM/VM.hpp"

#ifndef VERSION_MAIN
#error "VERSION_MAIN is not defined"
#endif
#ifndef VERSION_SUB
#error "VERSION_SUB is not defined"
#endif
#ifndef VERSION_PATCH
#error "VERSION_PATCH is not defined"
#endif

int main( int argc, char ** argv )
{
	std::vector< std::string > args;
	size_t flags = cmd_get_args( argc, ( const char ** )argv, args );

	if( flags & OPT_V ) {
		fprintf( stdout, "Ethereal: %d.%d.%d\n", VERSION_MAIN, VERSION_SUB, VERSION_PATCH );
		return E_OK;
	}

	if( args.size() < 1 ) {
		fprintf( stderr, "usage: %s [flags] <source file>\n", argv[ 0 ] );
		return E_FAIL;
	}

	auto last_slash_loc = args[ 0 ].find_last_of( '/' ) + 1;
	// change current dir to that of file
	std::string curr_dir = GetCWD();
	std::string src_dir = args[ 0 ].substr( 0, last_slash_loc );
	DirFormat( src_dir );
	SetCWD( src_dir );

	int err = E_OK;
	const std::string main_src_str = args[ 0 ].substr( last_slash_loc );

	src_t * main_src = new src_t( true );
	main_src->name = main_src_str;
	main_src->id = "main_src_" + main_src->name;
	main_src->dir = GetCWD();
	err = tokenize( * main_src );
	if( err != E_OK ) goto cleanup;

	if( flags & OPT_T ) {
		fprintf( stdout, "Tokens:\n" );
		auto & toks = main_src->toks;
		for( size_t i = 0; i < toks.size(); ++i ) {
			auto & tok = toks[ i ];
			fprintf( stdout, "ID: %zu\tType: %s\tLine: %d[%d]\tSymbol: %s\n",
				 i, TokStrs[ tok.type ], tok.line, tok.col, tok.data.c_str() );
		}
	}

	err = E_OK;
	main_src->ptree = parse( * main_src );

	if( main_src->ptree == nullptr ) { err = E_PARSE_FAIL; goto cleanup; }

	if( flags & OPT_P ) {
		fprintf( stdout, "Parse Tree:\n" );
		for( auto it = main_src->ptree->begin(); it != main_src->ptree->end(); ++it ) {
			( * it )->disp( it != main_src->ptree->end() - 1 );
		}
	}

	err = E_OK;
	for( auto & it : * main_src->ptree ) {
		if( !it->bytecode( * main_src ) ) { err = E_BYTECODE_FAIL; goto cleanup; }
	}

	if( flags & OPT_B ) {
		fprintf( stdout, "Byte Code:\n" );
		for( size_t i = 0; i < main_src->bcode.size(); ++i ) {
			auto & ins = main_src->bcode[ i ];
			fprintf( stdout, "%-*zu %-*s%-*s[%s]\n",
				 5, i, 20, InstrCodeStrs[ ins.opcode ], 7, OperTypeStrs[ ins.oper.type ], ins.oper.val.c_str() );
		}
	}

	if( err != E_OK ) goto cleanup;

	// actual code execution
	if( !( flags & OPT_C ) && !( flags & OPT_D ) ) {
		vm_state_t vm;
		vm.flags = flags;
		vm.srcstack.push_back( main_src );
		vm.srcs[ main_src->id ] = main_src;
		if( !set_init_mods( vm ) ) { err = E_VM_FAIL; goto end; }
		std::vector< var_base_t * > arg_vec;
		for( auto & v : args ) {
			arg_vec.push_back( new var_str_t( v, 0 ) );
		}
		vm.vars->add( "__PROG__", new var_str_t( argv[ 0 ], 0 ) );
		vm.vars->add( "args", new var_vec_t( arg_vec, 0 ) );
		vm.vars->add( "true", new var_bool_t( true, 0 ) );
		vm.vars->add( "false", new var_bool_t( false, 0 ) );
		VAR_IREF( vm.nil );
		vm.vars->add( "nil", vm.nil );
		err = vm_exec( vm );
		vm.srcstack.pop_back();
		goto end;
	}
cleanup:
	delete main_src;
end:
	// reset working dir
	SetCWD( curr_dir );
	return err;
}