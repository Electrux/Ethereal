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

stmt_ldmod_t * parse_ldmod( const src_t & src, parse_helper_t * ph )
{
	int tok_ctr = ph->tok_ctr();

	tok_t * what = nullptr;

	NEXT_VALID2( TOK_IDEN, TOK_STR );
	what = ph->peak();

	NEXT_VALID( TOK_COLS );
	return new stmt_ldmod_t( what, tok_ctr );
}

bool stmt_ldmod_t::bytecode( src_t & src ) const
{
	src.bcode.push_back( { m_tok_ctr, m_what->line, m_what->col, IC_LDMOD, { OP_INT, m_what->data } } );
	return true;
}