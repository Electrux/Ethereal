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
#include "../Env.hpp"

stmt_import_t * parse_import( const src_t & src, parse_helper_t * ph )
{
	int tok_ctr = ph->tok_ctr();

	std::string full_name;
	tok_t * what = nullptr;
	tok_t * as = nullptr;

repeat:
	NEXT_VALID2( TOK_IDEN, TOK_STR );
	if( what == nullptr ) what = ph->peak();
	full_name += ph->peak()->data;

	if( ph->peak( 1 )->type == TOK_AS ) {
		ph->next();
		NEXT_VALID2( TOK_IDEN, TOK_STR );
		as = ph->peak();
	} else if( ph->peak( 1 )->type == TOK_DOT ) {
		full_name += '/';
		ph->next();
		goto repeat;
	}

	NEXT_VALID( TOK_COLS );
	return new stmt_import_t( what, as, full_name, tok_ctr );
}

bool stmt_import_t::bytecode( src_t & src ) const
{
	std::string file = m_full_name;
	format_file_str( file, FormatFileType::INC );
	src.bcode.push_back( { m_tok_ctr, m_what->line, m_what->col, IC_PUSH, { OP_CONST, file } } );
	if( m_as != nullptr ) {
		src.bcode.push_back( { m_tok_ctr, m_as->line, m_as->col, IC_PUSH, { OP_CONST, m_as->data } } );
	}
	src.bcode.push_back( { m_tok_ctr, m_what->line, m_what->col, IC_IMPORT, { OP_INT, m_as == nullptr ? "1" : "2" } } );
	return true;
}
