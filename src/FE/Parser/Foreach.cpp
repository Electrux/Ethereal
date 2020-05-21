/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the GNU GPL 3.0 license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Internal.hpp"
#include "../Ethereal.hpp"

static void set_continue_break_lines( bytecode_t & bcode, const int from,
				      const int to, const int cont, const int brk );

stmt_foreach_t * parse_foreach( src_t & src, parse_helper_t * ph, std::vector< GrammarTypes > & parents )
{
	int tok_ctr = ph->tok_ctr();

	tok_t * it_name = nullptr;
	expr_res_t expr = { 0, nullptr };
	stmt_block_t * block = nullptr;

	int err, expr_brace;

	// NOTE: for now, checks for iterator name and 'in' are redundant because that is done in Parser.cpp itself
	// iterator name
	ph->next();
	if( ph->peak()->type != TOK_IDEN ) {
		PARSE_FAIL( "expected identifier as the iterator variable" );
		goto fail;
	}
	it_name = ph->peak();

	// in
	ph->next();
	if( ph->peak()->type != TOK_IN ) {
		PARSE_FAIL( "expected 'in' here to signify the upcoming expression from which to iterate" );
		goto fail;
	}

	// expr
	ph->next();
	err = find_next_of( ph, expr_brace, { TOK_LBRACE }, TOK_RBRACE );
	if( err < 0 ) {
		if( err == -1 ) {
			PARSE_FAIL( "could not find the left brace for loop block" );
		} else if( err == -2 ) {
			PARSE_FAIL( "found end of statement (semicolon) before left braces for loop block" );
		}
		goto fail;
	}
	expr = parse_expr( src, ph, expr_brace, false );
	if( expr.res != E_OK ) goto fail;
	ph->set_tok_ctr( expr_brace );

blk:
	// block
	parents.push_back( GRAM_FOREACH );
	block = parse_block( src, ph, parents );
	parents.pop_back();
	if( block == nullptr ) goto fail;

	return new stmt_foreach_t( new stmt_simple_t( SIMPLE_TOKEN, it_name, tok_ctr + 1 ),
				   expr.expr, block, tok_ctr );
fail:
	if( expr.expr ) delete expr.expr;
	if( block ) delete block;
	return nullptr;
}

bool stmt_foreach_t::bytecode( src_t & src ) const
{
	int line = src.toks[ m_tok_ctr ].line;
	int col = src.toks[ m_tok_ctr ].col;

	ADD_SCOPE();
	// add first layer
	INC_SCOPE();

	// bytecode for expression
	if( !m_expr->bytecode( src ) ) return false;
	// provide for assignment
	src.bcode.push_back( { m_iter->m_tok_ctr, m_iter->m_val->line, m_iter->m_val->col,
			       IC_PUSH, { OP_CONST, "._" + m_iter->m_val->data } } );
	// store the expression result
	src.bcode.push_back( { m_iter->m_tok_ctr + 1, m_iter->m_val->line, m_iter->m_val->col,
			       IC_STORE, { OP_NONE, "" } } );

	int iter_end_loc = src.bcode.size();
	// add second layer
	INC_SCOPE();

	int expr_line = src.toks[ m_expr->m_tok_ctr ].line;
	int expr_col = src.toks[ m_expr->m_tok_ctr ].col;
	// assign the ._<iter>.next() values to iterator
	src.bcode.push_back( { m_expr->m_tok_ctr, expr_line, expr_col,
			       IC_PUSH, { OP_STR, "._" + m_iter->m_val->data } } );
	src.bcode.push_back( { m_expr->m_tok_ctr, expr_line, expr_col,
			       IC_PUSH, { OP_CONST, "next" } } );
	src.bcode.push_back( { m_expr->m_tok_ctr, expr_line, expr_col,
			       IC_MFN_CALL, { OP_INT, "0" } } );
	src.bcode.push_back( { m_iter->m_tok_ctr, m_iter->m_val->line, m_iter->m_val->col,
			       IC_PUSH, { OP_CONST, m_iter->m_val->data } } );
	src.bcode.push_back( { m_iter->m_tok_ctr, m_iter->m_val->line, m_iter->m_val->col,
			       IC_STORE_NO_COPY, { OP_NONE, "" } } );

	// compare the result with nil, break if it is nil
	src.bcode.push_back( { m_iter->m_tok_ctr, m_iter->m_val->line, m_iter->m_val->col,
			       IC_PUSH, { OP_STR, m_iter->m_val->data } } );
	src.bcode.push_back( { m_iter->m_tok_ctr, m_iter->m_val->line, m_iter->m_val->col,
			       IC_PUSH, { OP_STR, "nil" } } );
	src.bcode.push_back( { m_iter->m_tok_ctr, m_iter->m_val->line, m_iter->m_val->col,
			       IC_PUSH, { OP_CONST, "==" } } );
	src.bcode.push_back( { m_iter->m_tok_ctr, m_iter->m_val->line, m_iter->m_val->col,
			       IC_FN_CALL, { OP_INT, "2" } } );

	// jump out of loop if the above comparison results true
	int iter_comparison_loc = src.bcode.size();
	src.bcode.push_back( { m_iter->m_tok_ctr, m_iter->m_val->line, m_iter->m_val->col,
			       IC_JUMP_TRUE_NO_POP, { OP_INT, "" } } );

	int block_start_loc = src.bcode.size();

	int cont_brk_from = -1, cont_brk_to = -1;
	if( m_block != nullptr ) {
		cont_brk_from = src.bcode.size();
		if( !m_block->bytecode( src ) ) return false;
		cont_brk_to = src.bcode.size();
	}

	int continue_loc = src.bcode.size() + 1;

	DEC_SCOPE();
	// remove second layer

	src.bcode.push_back( { m_iter->m_tok_ctr, m_iter->m_val->line, m_iter->m_val->col,
			       IC_JUMP_FALSE, { OP_INT, std::to_string( iter_end_loc ) } } );

	DEC_SCOPE();
	// remove first layer
	REM_SCOPE();

	// - 3 to go before the 2 layers and the IC_JUMP_FALSE instruction above
	src.bcode[ iter_comparison_loc ].oper.val = std::to_string( src.bcode.size() - 3 );
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