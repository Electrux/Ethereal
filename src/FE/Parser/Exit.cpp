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

stmt_exit_t * parse_exit( src_t & src, parse_helper_t * ph )
{
	int tok_ctr = ph->tok_ctr();
	ph->next();

	if( ph->peak()->type == TOK_COLS ) {
		return new stmt_exit_t( nullptr, tok_ctr );
	}

	expr_res_t expr = { 0, nullptr };
	int err, cols;
	err = find_next_of( ph, cols, { TOK_COLS } );
	if( err < 0 ) {
		if( err == -1 ) {
			PARSE_FAIL( "could not find the semicolon for the exit's value expression" );
		}
		goto fail;
	}

	expr = parse_expr( src, ph, cols, false );
	ph->set_tok_ctr( cols );
	if( expr.res != 0 ) goto fail;
	return new stmt_exit_t( expr.expr, tok_ctr );
fail:
	if( expr.expr ) delete expr.expr;
	return nullptr;
}

bool stmt_exit_t::bytecode( src_t & src ) const
{
	int line = src.toks[ m_tok_ctr ].line;
	int col = src.toks[ m_tok_ctr ].col;
	if( m_exit_val ) {
		if( !m_exit_val->bytecode( src ) ) return false;
	} else {
		src.bcode.push_back( { m_tok_ctr, line, col, IC_PUSH, { OP_INT, "0" } } );
	}
	src.bcode.push_back( { m_tok_ctr, line, col, IC_EXIT, { OP_NONE, "" } } );
	return true;
}