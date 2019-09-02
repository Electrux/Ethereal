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

stmt_ldmod_t * parse_ldmod( const src_t & src, parse_helper_t * ph )
{
	int tok_ctr = ph->tok_ctr();

	std::string full_name;
	tok_t * what = nullptr;

repeat:
	NEXT_VALID2( TOK_IDEN, TOK_STR );
	if( what == nullptr ) what = ph->peak();
	full_name += ph->peak()->data;

	if( ph->peak( 1 )->type == TOK_DOT ) {
		full_name += '/';
		ph->next();
		goto repeat;
	}

	auto last_slash = full_name.find_last_of( '/' );
	full_name.insert( last_slash + 1, "lib" );

	NEXT_VALID( TOK_COLS );
	return new stmt_ldmod_t( what, full_name, tok_ctr );
}

bool stmt_ldmod_t::bytecode( src_t & src ) const
{
	std::string file = m_full_name;
	int res = format_file_str( file, FormatFileType::LIB );
	if( res != 0 ) {
		src_fail( src.name, src.code[ m_what->line - 1 ], m_what->line, m_what->col,
			  "could not find module file '%s.so' for loading", file.c_str() );
		return false;
	}
	src.bcode.push_back( { m_tok_ctr, m_what->line, m_what->col, IC_LDMOD, { OP_CONST, file } } );
	return true;
}