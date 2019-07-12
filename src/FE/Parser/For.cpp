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

stmt_for_t * parse_for( src_t & src, parse_helper_t * ph, std::vector< GrammarTypes > & parents )
{
	int tok_ctr = ph->tok_ctr();

	expr_res_t init = { 0, nullptr }, cond = { 0, nullptr }, step = { 0, nullptr };
	stmt_block_t * block = nullptr;

	int err, init_cols, cond_cols, step_brace;

	// initialization
	ph->next();
	err = find_next_of( ph, init_cols, { TOK_COLS } );
	if( err < 0 ) {
		if( err == -1 ) {
			PARSE_FAIL( "could not find the semicolon for the loop's init statement" );
		}
		goto fail;
	}
	init = parse_expr( src, ph, init_cols );
	if( init.res != 0 ) goto fail;
	ph->set_tok_ctr( init_cols );

	// condition
	ph->next();
	err = find_next_of( ph, cond_cols, { TOK_COLS } );
	if( err < 0 ) {
		if( err == -1 ) {
			PARSE_FAIL( "could not find the semicolon for the loop's init statement" );
		}
		goto fail;
	}
	cond = parse_expr( src, ph, cond_cols );
	if( cond.res != 0 ) goto fail;
	ph->set_tok_ctr( cond_cols );

	// step
	ph->next();
	err = find_next_of( ph, step_brace, { TOK_LBRACE }, TOK_RBRACE );
	if( err < 0 ) {
		if( err == -1 ) {
			PARSE_FAIL( "could not find the left brace for loop block" );
		} else if( err == -2 ) {
			PARSE_FAIL( "found end of statement (semicolon) before left braces for loop block" );
		}
		goto fail;
	}
	step = parse_expr( src, ph, step_brace );
	if( step.res != 0 ) goto fail;
	ph->set_tok_ctr( step_brace );

	// block
	parents.push_back( GRAM_FOR );
	block = parse_block( src, ph, parents );
	parents.pop_back();
	if( block == nullptr ) goto fail;

	return new stmt_for_t( init.expr, cond.expr, step.expr, block, tok_ctr );

fail:
	if( init.expr ) delete init.expr;
	if( cond.expr ) delete cond.expr;
	if( step.expr ) delete step.expr;
	if( block ) delete block;
	return nullptr;
}

bool stmt_for_t::bytecode( src_t & src ) const
{
	return true;
}