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

bool stmt_func_struct_subscr_call_t::bytecode( src_t & src ) const
{
	int args = 0;
	if( m_args ) {
		m_args->bytecode( src );
		int child_cc = 0;
		child_comma_count( m_args, child_cc );
		if( child_cc == 0 ) args = 1;
		else args = child_cc + 1;
	}
	if( m_ctype == CT_SUBSCR ) {
		src.bcode.push_back( { m_tok_ctr, m_name->m_val->line, m_name->m_val->col,
				       IC_SUBSCR, { OP_INT, m_name->m_val->data } } );
		return true;
	}

	src.bcode.push_back( { m_tok_ctr, m_name->m_val->line, m_name->m_val->col,
			       IC_PUSH, { OP_CONST, m_name->m_val->data } } );
	if( m_ctype == CT_FUNC ) {
		src.bcode.push_back( { m_tok_ctr, m_name->m_val->line, m_name->m_val->col,
				       m_post_dot ? IC_FN_MEM_CALL : IC_FN_CALL,
				       { OP_INT, std::to_string( args ) } } );
	} else if( m_ctype == CT_STRUCT ) {
		src.bcode.push_back( { m_tok_ctr, m_name->m_val->line, m_name->m_val->col,
				       m_post_dot ? IC_STRUCT_MEM_DECL : IC_STRUCT_DECL,
				       { OP_INT, std::to_string( args ) } } );
	}
	return true;
}