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

static void set_continue_break_lines( bytecode_t & bcode, const int from,
				      const int to, const int cont, const int brk );

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
	int line = src.toks[ m_tok_ctr ].line;
	int col = src.toks[ m_tok_ctr ].col;

	src.bcode.push_back( { m_tok_ctr, line, col, IC_ADD_SCOPE, { OP_NONE, "" } } );

	if( m_init != nullptr ) {
		if( !m_init->bytecode( src ) ) return false;
	}
	int cond_end_loc = -1;
	if( m_cond != nullptr ) {
		if( !m_cond->bytecode( src ) ) return false;
		src.bcode.push_back( { src.bcode.back().parse_ctr, src.bcode.back().line,
				       src.bcode.back().col, IC_JUMP_FALSE, { OP_INT, "<placeholder>" } } );
		cond_end_loc = src.bcode.size() - 1;
	}

	int block_start_loc = src.bcode.size();

	src.bcode.push_back( { m_tok_ctr, line, col, IC_ADD_SCOPE, { OP_NONE, "" } } );

	int cont_brk_from = -1, cont_brk_to = -1;
	if( m_block != nullptr ) {
		cont_brk_from = src.bcode.size();
		if( !m_block->bytecode( src ) ) return false;
		cont_brk_to = src.bcode.size();
	}

	int continue_loc = -1;
	if( m_step != nullptr ) {
		continue_loc = src.bcode.size();
		if( !m_step->bytecode( src ) ) return false;
	}

	if( m_cond != nullptr ) {
		if( continue_loc == -1 ) continue_loc = src.bcode.size();
		if( !m_cond->bytecode( src ) ) return false;
	}

	// remove second layer
	src.bcode.push_back( { m_tok_ctr, line, col, IC_REM_SCOPE, { OP_NONE, "" } } );
	if( continue_loc == -1 ) continue_loc = src.bcode.size() - 1;

	if( m_cond == nullptr ) {
		src.bcode.push_back( { m_tok_ctr, src.bcode.back().line, src.bcode.back().col,
				       IC_JUMP, { OP_INT, std::to_string( block_start_loc ) } } );
	} else {
		src.bcode.push_back( { m_tok_ctr, src.bcode.back().line, src.bcode.back().col,
				       IC_JUMP_TRUE, { OP_INT, std::to_string( block_start_loc ) } } );
	}
	if( cond_end_loc >= 0 ) {
		src.bcode[ cond_end_loc ].oper.val = std::to_string( src.bcode.size() );
	}

	// remove first layer
	src.bcode.push_back( { m_tok_ctr, line, col, IC_REM_SCOPE, { OP_NONE, "" } } );

	// set continue and break locations
	if( m_block != nullptr ) {
		// continue to the beginning of step, or condition, or removal of second layer
		// break to removal of first layer
		set_continue_break_lines( src.bcode, cont_brk_from, cont_brk_to, continue_loc, src.bcode.size() - 1 );
	}
	return true;
}

static void set_continue_break_lines( bytecode_t & bcode, const int from,
				      const int to, const int cont, const int brk )
{
	for( int i = from; i < to; ++i ) {
		if( bcode[ i ].oper.val == "<continue-placeholder>" ) {
			bcode[ i ].oper.val = std::to_string( cont );
		} else if( bcode[ i ].oper.val == "<break-placeholder>" ) {
			bcode[ i ].oper.val = std::to_string( brk );
		}
	}
}