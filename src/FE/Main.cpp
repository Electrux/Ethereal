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
#include "Ethereal.hpp"
#include "Parser.hpp"

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

	src_t main_src( true );
	main_src.name = main_src_str;
	err = tokenize( main_src );
	if( err != E_OK ) return err;

	if( flags & OPT_T ) {
		fprintf( stdout, "Tokens:\n" );
		auto & toks = main_src.toks;
		for( size_t i = 0; i < toks.size(); ++i ) {
			auto & tok = toks[ i ];
			fprintf( stdout, "ID: %zu\tType: %s\tLine: %d[%d]\tSymbol: %s\n",
				 i, TokStrs[ tok.type ], tok.line, tok.col, tok.data.c_str() );
		}
	}

	err = E_OK;
	parse_tree_t * ptree = parse( main_src );

	if( ptree == nullptr ) { err = E_PARSE_FAIL; goto cleanup; }
	main_src.ptree = ptree;

	if( flags & OPT_P ) {
		fprintf( stdout, "Parse Tree:\n" );
		for( auto it = ptree->begin(); it != ptree->end(); ++it ) {
			( * it )->disp( it != ptree->end() - 1 );
		}
	}

	err = E_OK;
	for( auto & it : * ptree ) {
		if( !it->bytecode( main_src ) ) { err = E_BYTECODE_FAIL; goto cleanup; }
	}

	if( flags & OPT_B ) {
		fprintf( stdout, "Byte Code:\n" );
		for( size_t i = 0; i < main_src.bcode.size(); ++i ) {
			auto & ins = main_src.bcode[ i ];
			fprintf( stdout, "%-*zu %-*s%d[%s]\n",
				 5, i, 20, InstrCodeStrs[ ins.opcode ], ins.oper.type, ins.oper.val.c_str() );
		}
	}
cleanup:
	if( ptree != nullptr ) {
		for( auto & stmt : * ptree ) delete stmt;
		delete ptree;
	}

	if( err != E_OK ) return err;
/*

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
	// reset working dir
	SetCWD( curr_dir );
	return err;
}

#endif // AS_LIB
