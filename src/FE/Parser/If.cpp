/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Internal.hpp"
#include "../Ethereal.hpp"

stmt_if_t * parse_if( src_t & src, parse_helper_t * ph, std::vector< GrammarTypes > & parents )
{
	int tok_ctr = ph->tok_ctr();

	expr_res_t expr = { 0, nullptr };
	stmt_block_t * block = nullptr;
	std::vector< condition_t > conds;

	int err, start_brace;
begin_cond:
	ph->next();
	err = find_next_of( ph, start_brace, { TOK_LBRACE }, TOK_RBRACE );
	if( err < 0 ) {
		if( err == -1 ) {
			PARSE_FAIL( "could not find the left brace for if block" );
		} else if( err == -2 ) {
			PARSE_FAIL( "found end of statement (semicolon) before left braces for if block" );
		}
		goto fail;
	}

	expr = parse_expr( src, ph, start_brace, false );
	if( expr.res != 0 ) goto fail;
	ph->set_tok_ctr( start_brace );

begin_block:
	parents.push_back( GRAM_IF );
	block = parse_block( src, ph, parents );
	parents.pop_back();
	if( block == nullptr ) goto fail;
	conds.push_back( { expr.expr, block } );
	expr = { 0, nullptr };
	block = nullptr;
	if( ph->peak( 1 )->type == TOK_ELIF ) {
		ph->next();
		goto begin_cond;
	} else if( ph->peak( 1 )->type == TOK_ELSE ) {
		ph->next();
		NEXT_VALID_FAIL( TOK_LBRACE );
		goto begin_block;
	}

	return new stmt_if_t( conds, tok_ctr );

fail:
	for( auto & cond : conds ) {
		if( cond.cond ) delete cond.cond;
		delete cond.block;
	}
	if( expr.expr ) delete expr.expr;
	if( block ) delete block;
	return nullptr;
}

bool stmt_if_t::bytecode( src_t & src ) const
{
	std::vector< size_t > unconditional_jumps;
	for( size_t i = 0; i < m_conds.size(); ++i ) {
		auto & cond = m_conds[ i ];
		int jump_loc = -1;
		if( cond.cond != nullptr ) {
			if( !cond.cond->bytecode( src ) ) return false;
			src.bcode.push_back( { src.bcode.back().parse_ctr, src.bcode.back().line, src.bcode.back().col,
					       IC_JUMP_FALSE, { OP_INT, "<placeholder>" } } );
		}
		int orig_bcode_size = src.bcode.size();
		jump_loc = orig_bcode_size - 1;
		int block_size = -1;
		if( !cond.block->bytecode( src ) ) return false;
		if( cond.cond != nullptr && i != m_conds.size() - 1 ) {
			src.bcode.push_back( { src.bcode.back().parse_ctr, src.bcode.back().line, src.bcode.back().col,
					       IC_JUMP, { OP_INT, "<placeholder>" } } );
			unconditional_jumps.push_back( src.bcode.size() - 1 );
		}
		block_size = src.bcode.size() - orig_bcode_size + 1;
		src.bcode[ jump_loc ].oper.val = std::to_string( jump_loc + block_size );
	}
	for( auto & ucj : unconditional_jumps ) {
		src.bcode[ ucj ].oper.val = std::to_string( src.bcode.size() );
	}
	return true;
}
