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

stmt_enum_t * parse_enum( const src_t & src, parse_helper_t * ph )
{
	int tok_ctr = ph->tok_ctr();

	bool is_mask = false;
	if( ph->peak()->type == TOK_ENUM_MASK ) is_mask = true;

	const tok_t * name = nullptr;

	NEXT_VALID( TOK_IDEN );
	name = ph->peak();

	std::vector< tok_t * > vals;

	NEXT_VALID( TOK_LBRACE );
	NEXT_VALID( TOK_IDEN );
val_begin:
	vals.push_back( ph->peak() );

	if( ph->peak( 1 )->type == TOK_COMMA ) {
		ph->next();
		if( ph->peak( 1 )->type == TOK_IDEN ) {
			ph->next();
			goto val_begin;
		}
	}

	NEXT_VALID( TOK_RBRACE );
	// now at RBRACE
	return new stmt_enum_t( name, vals, is_mask, tok_ctr );
}

bool stmt_enum_t::bytecode( src_t & src ) const
{
	for( auto v = m_vals.rbegin(); v != m_vals.rend(); ++v ) {
		src.bcode.push_back( { m_tok_ctr, ( * v )->line, ( * v )->col, IC_PUSH, { OP_CONST, ( * v )->data } } );
	}
	src.bcode.push_back( { m_tok_ctr, m_name->line, m_name->col, IC_PUSH, { OP_CONST, m_name->data } } );
	src.bcode.push_back( { m_tok_ctr, m_name->line, m_name->col, m_is_mask ? IC_BUILD_ENUM_MASK : IC_BUILD_ENUM,
			       { OP_INT, std::to_string( m_vals.size() ) } } );
	return true;
}