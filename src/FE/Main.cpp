/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef AS_LIB

#include <cstdio>
#include <vector>
#include <string>

#include "CmdArgs.hpp"
#include "Env.hpp"
#include "FS.hpp"
#include "Parser.hpp"
#include "../VM/VM.hpp"

int main( int argc, char ** argv )
{
	std::vector< std::string > args;
	size_t flags = cmd_get_args( argc, ( const char ** )argv, args );
	if( args.size() < 1 ) {
		fprintf( stdout, "usage: %s [flags] <source file>\n", argv[ 0 ] );
		return E_FAIL;
	}

	auto last_slash_loc = args[ 0 ].find_last_of( '/' ) + 1;
	// change current dir to that of file
	std::string curr_dir = GetCWD();
	SetCWD( args[ 0 ].substr( 0, last_slash_loc ) );

	int err = E_OK;
	const std::string main_src_str = args[ 0 ].substr( last_slash_loc );

	src_t * main_src = new src_t( true );
	main_src->name = main_src_str;
	err = tokenize( * main_src );
	if( err != E_OK ) return err;

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
	parse_tree_t * ptree = parse( * main_src );

	if( ptree == nullptr ) { err = E_PARSE_FAIL; goto cleanup; }
	main_src->ptree = ptree;

	if( flags & OPT_P ) {
		fprintf( stdout, "Parse Tree:\n" );
		for( auto it = ptree->begin(); it != ptree->end(); ++it ) {
			( * it )->disp( it != ptree->end() - 1 );
		}
	}

	err = E_OK;
	for( auto & it : * ptree ) {
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
		err = vm_exec( main_src );
	} else {
		delete main_src;
	}
cleanup:
	// reset working dir
	SetCWD( curr_dir );
	return err;
}

#endif // AS_LIB
