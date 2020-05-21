/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef ETHEREAL_HPP
#define ETHEREAL_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "Parser.hpp"
#include "../VM/Instructions.hpp"
#include "../VM/Vars.hpp"

struct src_t
{
	// id is unique and used for mapping variables to sources
	// file is the name (no dir part) of the source file and is used
	// as key for srcs map.
	int id;
	std::string file;
	std::string dir;
	std::vector< std::string > code;
	toks_t toks;
	std::vector< std::vector< int > > block_depth;
	parse_tree_t * ptree;
	bool is_main_src;
	bool found_assn;
	bool bcode_as_const;

	// for VM
	bytecode_t bcode;

	src_t( const bool _is_main_src );
	~src_t();
};

// for parser
#define ADD_FUNC()									\
	src.block_depth.push_back( { 0 } )
#define ADD_SCOPE()									\
	src.block_depth.back().push_back( 0 )
#define INC_SCOPE()									\
	src.bcode.push_back( { m_tok_ctr, line, col, IC_ADD_SCOPE, { OP_INT, "1" } } );	\
	if( src.block_depth.back().size() > 0 ) ++src.block_depth.back().back()

#define REM_FUNC()									\
	src.block_depth.pop_back()
#define REM_SCOPE()									\
	src.block_depth.back().pop_back()
#define DEC_SCOPE()									\
	src.bcode.push_back( { m_tok_ctr, line, col, IC_REM_SCOPE, { OP_INT, "1" } } );	\
	if( src.block_depth.back().size() > 0 ) --src.block_depth.back().back()

typedef std::unordered_map< int, src_t * > srcs_t;

#endif // ETHEREAL_HPP