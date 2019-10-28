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

bool stmt_simple_t::bytecode( src_t & src ) const
{
	if( m_stype == SIMPLE_OPER ) {
		src.bcode.push_back( { m_tok_ctr, m_val->line, m_val->col, IC_PUSH, { OP_CONST, TokStrs[ m_val->type ] } } );
	} else if( m_val->type == TOK_INT ) {
		src.bcode.push_back( { m_tok_ctr, m_val->line, m_val->col, IC_PUSH, { OP_INT, m_val->data } } );
	} else if( m_val->type == TOK_FLT ) {
		src.bcode.push_back( { m_tok_ctr, m_val->line, m_val->col, IC_PUSH, { OP_FLT, m_val->data } } );
	} else if( m_val->type == TOK_STR || m_val->type == TOK_TDOT || src.bcode_as_const || m_post_dot ) {
		src.bcode.push_back( { m_tok_ctr, m_val->line, m_val->col, IC_PUSH, { OP_CONST, m_val->data } } );
	} else {
		src.bcode.push_back( { m_tok_ctr, m_val->line, m_val->col, IC_PUSH, { OP_STR, m_val->data } } );
	}
	return true;
}