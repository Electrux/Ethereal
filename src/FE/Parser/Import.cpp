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

	std::vector< std::string > parents = { "" };
	std::vector< std::string > full_names = { "" };
	std::vector< tok_t * > whats = { nullptr };

begin:
	NEXT_VALID2( TOK_IDEN, TOK_STR );
	whats.back() = ph->peak();
	full_names.back() += ph->peak()->data;
	NEXT_VALID4( TOK_COMMA, TOK_DOT, TOK_COLS, TOK_RPAREN );
comma:
	if( ph->peak()->type == TOK_COMMA ) {
		// add the previous identifer/str to whats as it is complete
		whats.push_back( nullptr );
		full_names.push_back( parents.back() );
		goto begin;
	}

dot:
	if( ph->peak()->type == TOK_DOT ) {
		full_names.back() += '/';
		if( ph->peak( 1 )->type != TOK_LPAREN ) goto begin;
		ph->next();
		// pass to lparen
	}

lparen:
	if( ph->peak()->type == TOK_LPAREN ) {
		parents.push_back( full_names.back() );
		goto begin;
	}

rparen:
	if( ph->peak()->type == TOK_RPAREN ) {
		if( parents.size() <= 1 ) {
			PARSE_FAIL( "no matching opening parentheses for this closing one" );
			return nullptr;
		}
		parents.pop_back();
		NEXT_VALID2( TOK_COMMA, TOK_COLS );
		if( ph->peak()->type == TOK_COMMA ) goto comma;
	}

end:
	return new stmt_import_t( whats, full_names, tok_ctr );
}

bool stmt_import_t::bytecode( src_t & src ) const
{
	for( size_t i = 0; i < m_full_names.size(); ++i ) {
		src.bcode.push_back( { m_tok_ctr, m_whats[ i ]->line, m_whats[ i ]->col,
				       IC_IMPORT, { OP_CONST, m_full_names[ i ] } } );
	}
	return true;
}
