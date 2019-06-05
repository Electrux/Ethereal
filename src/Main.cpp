/*
	Copyright (c) 2018, Electrux
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
#include "Ethereal.hpp"
//#include "Parser.hpp"

//#include "../VM/Core.hpp"
//#include "../VM/VM.hpp"

int main( int argc, char ** argv )
{
	std::vector< std::string > args;
	size_t flags = cmd_get_args( argc, ( const char ** )argv, args );
	if( args.size() < 1 ) {
		fprintf( stdout, "usage: %s [flags] <source file>\n", argv[ 0 ] );
		return FAIL;
	}

	auto last_slash_loc = args[ 0 ].find_last_of( '/' ) + 1;
	// change current dir to that of file
	std::string curr_dir = GetCWD();
	SetCWD( args[ 0 ].substr( 0, last_slash_loc ) );

	int err = OK;
	const std::string main_src = args[ 0 ].substr( last_slash_loc );

	eth_t eth;
	eth.srcs.insert( { main_src, {} } );
	eth.srcs[ main_src ].is_main_src = true;
	eth.srcs[ main_src ].name = main_src;
	eth.src_stack.push_back( main_src );
	err = tokenize( eth );
	if( err != OK ) return err;

	if( flags & OPT_T ) {
		fprintf( stdout, "Tokens:\n" );
		auto & toks = eth.srcs[ main_src ].toks;
		for( size_t i = 0; i < toks.size(); ++i ) {
			auto & tok = toks[ i ];
			fprintf( stdout, "ID: %zu\tType: %s\tLine: %d[%d]\tSymbol: %s\n",
				 i, TokStrs[ tok.type ], tok.line, tok.col, tok.data.c_str() );
		}
	}
	/*

	int err = OK;
	result_t< std::vector< stmt_base_t * > > parse_res = parse( * toks );
	std::vector< stmt_base_t * > * ptree = nullptr;
	std::vector< Instruction > bcode;

	if( parse_res.code != OK ) { err = parse_res.code; goto cleanup; }
	ptree = parse_res.data;

	if( flags & OPT_P ) {
		fprintf( stdout, "Parse Tree:\n" );
		for( auto it = ptree->begin(); it != ptree->end(); ++it ) {
			( * it )->disp( it != ptree->end() - 1 );
		}
	}

	err = OK;
	for( auto & it : * ptree ) {
		err = it->byte_code( bcode );
		if( err != OK ) goto cleanup;
	}

cleanup:
	if( ptree != nullptr ) {
		for( auto & stmt : * ptree ) delete stmt;
		delete ptree;
	}
	if( toks != nullptr ) {
		delete toks;
	}
	if( err != OK ) return err;

	if( flags & OPT_B ) {
		fprintf( stdout, "Byte Code:\n" );
		for( size_t i = 0; i < bcode.size(); ++i ) {
			auto & ins = bcode[ i ];
			fprintf( stdout, "%zu\t%d[%d]\t%s\t%d[%s]\n",
				 i, ins.line, ins.col, InstructionCodeStrs[ ins.opcode ], ins.operand.type, ins.operand.val.c_str() );
		}
	}

	if( !( flags & OPT_C ) && !( flags & OPT_D ) ) {
		fprintf( stdout, "Executing Byte Code:\n" );
		ExecData ex_dat;
		ex_dat.flags = flags;
		// set exec dir
		std::string exec_loc = argv[ 0 ];
		if( exec_loc[ 0 ] == '/' || exec_loc[ 0 ] == '~' ) {
			ex_dat.exec_dir = exec_loc.substr( 0, exec_loc.find_last_of( '/' ) );
		} else {
			if( exec_loc.front() == '.' ) {
				exec_loc.erase( 0 );
				if( exec_loc.front() == '/' ) {
					exec_loc.erase( 0 );
				}
			}
			ex_dat.exec_dir = curr_dir + "/" + exec_loc.substr( 0, exec_loc.find_last_of( '/' ) );
		}
		ex_dat.module_stack.push_back( "<global>" );
		ex_dat.modules[ ex_dat.module_stack.back() ] = bcode;
		err = VM::Run( bcode, ex_dat );
	}
*/
	eth.src_stack.pop_back();
	// reset working dir
	SetCWD( curr_dir );
	return err;
}

#endif // AS_LIB
