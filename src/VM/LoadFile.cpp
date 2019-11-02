/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "../FE/CmdArgs.hpp"
#include "../FE/Env.hpp"

#include "LoadFile.hpp"

int load_src( vm_state_t & vm, const std::string & file, const std::string & alias )
{
	src_t & src = * vm.srcstack.back();
	auto last_slash_loc = file.find_last_of( '/' ) + 1;
	auto last_dot_loc = file.find_last_of( '.' );
	std::string mod_name = alias == "" ? file.substr( last_slash_loc, last_dot_loc - last_slash_loc ) : alias;

	const std::string new_src_str = file.substr( last_slash_loc );

	if( vm.srcs.find( new_src_str ) != vm.srcs.end() ) {
		return E_OK;
	}

	std::string curr_dir = GetCWD();
	std::string src_dir = file.substr( 0, last_slash_loc );
	DirFormat( src_dir );
	SetCWD( src_dir );
	int err = E_OK;

	parse_tree_t * ptree = nullptr;
	src_t * new_src = new src_t( false );
	new_src->name = new_src_str;
	new_src->id = new_src_str;
	new_src->dir = GetCWD();
	err = tokenize( * new_src );
	if( err != E_OK ) goto cleanup;

	if( vm.flags & OPT_T ) {
		fprintf( stdout, "Tokens:\n" );
		auto & toks = new_src->toks;
		for( size_t i = 0; i < toks.size(); ++i ) {
			auto & tok = toks[ i ];
			fprintf( stdout, "ID: %zu\tType: %s\tLine: %d[%d]\tSymbol: %s\n",
				 i, TokStrs[ tok.type ], tok.line, tok.col, tok.data.c_str() );
		}
	}

	err = E_OK;
	ptree = parse( * new_src );

	if( ptree == nullptr ) { err = E_PARSE_FAIL; goto cleanup; }
	new_src->ptree = ptree;

	if( vm.flags & OPT_P ) {
		fprintf( stdout, "Parse Tree:\n" );
		for( auto it = ptree->begin(); it != ptree->end(); ++it ) {
			( * it )->disp( it != ptree->end() - 1 );
		}
	}

	err = E_OK;
	for( auto & it : * ptree ) {
		if( !it->bytecode( * new_src ) ) { err = E_BYTECODE_FAIL; goto cleanup; }
	}

	if( vm.flags & OPT_B ) {
		fprintf( stdout, "Byte Code:\n" );
		for( size_t i = 0; i < new_src->bcode.size(); ++i ) {
			auto & ins = new_src->bcode[ i ];
			fprintf( stdout, "%-*zu %-*s%-*s[%s]\n",
				 5, i, 20, InstrCodeStrs[ ins.opcode ], 7, OperTypeStrs[ ins.oper.type ], ins.oper.val.c_str() );
		}
	}

	if( err != E_OK ) goto cleanup;

	// actual code execution
	if( !( vm.flags & OPT_C ) && !( vm.flags & OPT_D ) ) {
		vm.srcstack.push_back( new_src );
		vm.srcs[ new_src->id ] = new_src;
		err = vm_exec( vm );
		vm.srcstack.pop_back();
		if( err != E_OK ) vm.srcs.erase( new_src->id );
	}

cleanup:
	SetCWD( curr_dir );
	if( err != E_OK ) delete new_src;
	return err;
}