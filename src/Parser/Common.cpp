/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Common.hpp"

const char * GrammarTypeStrs[ _GRAM_LAST ] = {
	"Enum",
};

stmt_base_t::stmt_base_t( const GrammarTypes type, const int line, const int col )
	: m_type( type ), m_line( line ), m_col( col ) {}
stmt_base_t::~stmt_base_t() {}

stmt_enum_t::stmt_enum_t( const tok_t * name, const std::vector< tok_t * > & vals,
			  const int line, const int col )
	: stmt_base_t( GRAM_ENUM, line, col ), m_name( name ), m_vals( vals ) {}
stmt_enum_t::~stmt_enum_t() {}

void stmt_enum_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, "Enum at: %x\n", this );

	IO::tab_add( false );
	IO::print( true, "Name: %s\n", m_name->data.c_str() );
	IO::print( false, "Values:\n" );
	IO::tab_add( false );
	for( size_t i = 0; i < m_vals.size(); ++i ) {
		IO::print( i != m_vals.size() - 1, "%s\n", m_vals[ i ]->data.c_str() );
	}
	IO::tab_rem( 3 );
}







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
	return & m_toks[ m_token_ctr ];
}

bool parse_helper_t::has_next() const { return m_token_ctr + 1 < ( int )m_toks.size(); }

int parse_helper_t::tok_ctr() const { return m_token_ctr; }