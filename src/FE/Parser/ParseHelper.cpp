/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "ParseHelper.hpp"

parse_helper_t::parse_helper_t( toks_t & toks )
	: m_toks( toks ), m_token_ctr( 0 ),
	  m_invalid( -1, -1, TOK_INVALID, "" ) {}

tok_t * parse_helper_t::peak( const int num )
{
	if( m_token_ctr + num >= ( int )m_toks.size() || m_token_ctr + num < 0 ) return & m_invalid;
	return & m_toks[ m_token_ctr + num ];
}

tok_t * parse_helper_t::next()
{
	++m_token_ctr;
	if( m_token_ctr >= ( int )m_toks.size() ) return & m_invalid;
	return & m_toks[ m_token_ctr ];
}

tok_t * parse_helper_t::prev()
{
	if( m_token_ctr < 0 ) return & m_invalid;
	--m_token_ctr;
	return & m_toks[ m_token_ctr ];
}

tok_t * parse_helper_t::at( const int idx )
{
	return & m_toks[ idx ];
}

bool parse_helper_t::has_next() const { return m_token_ctr + 1 < ( int )m_toks.size(); }

int parse_helper_t::tok_ctr() const { return m_token_ctr; }

void parse_helper_t::set_tok_ctr( const int tok_ctr ) { m_token_ctr = tok_ctr; }