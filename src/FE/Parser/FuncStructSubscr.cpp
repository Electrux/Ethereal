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

bool stmt_func_struct_subscr_call_t::bytecode( src_t & src ) const
{
	int line = src.toks[ m_tok_ctr ].line;
	int col = src.toks[ m_tok_ctr ].col;

	if( m_ctype == CT_SUBSCR ) {
		bool bcodeasconst = src.bcode_as_const;
		bool found_assn = src.found_assn;
		src.bcode_as_const = false;
		src.found_assn = false;
		if( m_name ) {
			if( m_post_dot ) src.bcode_as_const = true;
			m_name->bytecode( src );
			if( m_post_dot ) {
				src.bcode_as_const = false;
				src.bcode.push_back( { m_name->m_tok_ctr, line, col, IC_ATTR, { OP_NONE, "" } } );
			}
		}
		for( auto & arg : m_args ) {
			if( arg == nullptr ) continue;
			arg->bytecode( src );
			src.bcode.push_back( { m_tok_ctr, m_name ? m_name->m_val->line : line , m_name ? m_name->m_val->col : col,
					       IC_PUSH, { OP_CONST, "[]" } } );
			src.bcode.push_back( { m_tok_ctr, m_name ? m_name->m_val->line : line , m_name ? m_name->m_val->col : col,
					       IC_MFN_CALL, { OP_INT, "1" } } );
		}
		src.bcode_as_const = bcodeasconst;
		src.found_assn = found_assn;
		return true;
	}

	int args = 0;
	if( m_args.size() > 0 && m_args[ 0 ] != nullptr ) {
		m_args[ 0 ]->bytecode( src );
		int child_cc = 0;
		child_comma_count( m_args[ 0 ], child_cc );
		if( child_cc == 0 ) args = 1;
		else args = child_cc + 1;
	}

	src.bcode.push_back( { m_tok_ctr, m_name->m_val->line, m_name->m_val->col,
			       IC_PUSH, { OP_CONST, m_name->m_val->data } } );
	if( m_ctype == CT_FUNC ) {
		src.bcode.push_back( { m_tok_ctr, m_name->m_val->line, m_name->m_val->col,
				       m_post_dot ? IC_MFN_CALL : IC_FN_CALL,
				       { OP_INT, std::to_string( args ) } } );
	} else if( m_ctype == CT_STRUCT ) {
		src.bcode.push_back( { m_tok_ctr, m_name->m_val->line, m_name->m_val->col,
				       IC_STRUCT_DECL, { OP_INT, std::to_string( args ) } } );
	}
	return true;
}