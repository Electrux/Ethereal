/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Internal.hpp"
#include "../Parser.hpp"
#include "../Ethereal.hpp"

stmt_block_t * parse_block( src_t & src, parse_helper_t * ph, std::vector< GrammarTypes > & parents )
{
	int tok_ctr = ph->tok_ctr();
	int end_brace;
	int err = find_next_of( ph, end_brace, { TOK_RBRACE }, TOK_LBRACE, true );
	std::vector< stmt_base_t * > * block = nullptr;
	if( err < 0 ) {
		PARSE_FAIL( "could not find the ending (right) brace for block" );
		return nullptr;
	}
	// without moving to next point, the parse() and parse_block() will
	// go on forever, thereby causing seg fault (stack overflow)
	ph->next();
	block = parse( src, ph, parents, end_brace );
	if( block == nullptr ) return nullptr;
	return new stmt_block_t( block, tok_ctr, parents.size() > 0 && parents.back() == GRAM_FUNC );
}

bool stmt_block_t::bytecode( src_t & src ) const
{
	int line = src.toks[ m_tok_ctr ].line;
	int col = src.toks[ m_tok_ctr ].col;
	INC_SCOPE();
	for( auto & stmt : * m_stmts ) {
		if( !stmt->bytecode( src ) ) return false;
	}
	if( m_in_func && src.bcode.back().opcode != IC_RETURN && src.bcode.back().opcode != IC_RETURN_EMPTY ) {
		src.bcode.push_back( { m_tok_ctr, line, col, IC_PUSH, { OP_NONE, "" } } );
	}
	DEC_SCOPE();
	return true;
}