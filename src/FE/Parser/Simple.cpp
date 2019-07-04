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

bool stmt_simple_t::bytecode( bytecode_t & bcode ) const
{
	if( m_stype == SIMPLE_KEYWORD || m_stype == SIMPLE_OPER ) {
		bcode.push_back( { m_tok_ctr, m_val->line, m_val->col, IC_PUSH, { OP_CONST, TokStrs[ m_val->type ] } } );
	} else if( m_val->type == TOK_INT ) {
		bcode.push_back( { m_tok_ctr, m_val->line, m_val->col, IC_PUSH, { OP_INT, m_val->data } } );
	} else if( m_val->type == TOK_FLT ) {
		bcode.push_back( { m_tok_ctr, m_val->line, m_val->col, IC_PUSH, { OP_FLT, m_val->data } } );
	} else if( m_val->type == TOK_TRUE || m_val->type == TOK_FALSE ) {
		bcode.push_back( { m_tok_ctr, m_val->line, m_val->col, IC_PUSH, { OP_BOOL, m_val->data } } );
	} else if( m_val->type == TOK_STR ) {
		bcode.push_back( { m_tok_ctr, m_val->line, m_val->col, IC_PUSH, { OP_CONST, m_val->data } } );
	} else {
		bcode.push_back( { m_tok_ctr, m_val->line, m_val->col, IC_PUSH, { OP_STR, m_val->data } } );
	}
	return true;
}