/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Stmts.hpp"

const char * GrammarTypeStrs[ _GRAM_LAST ] = {
	"Enum",
	"Ldmod",
};

stmt_base_t::stmt_base_t( const GrammarTypes type, const int line, const int col )
	: m_type( type ), m_line( line ), m_col( col ) {}
stmt_base_t::~stmt_base_t() {}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Enum //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// ldmod /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

stmt_ldmod_t::stmt_ldmod_t( const tok_t * what, const tok_t * as,
			  const int line, const int col )
	: stmt_base_t( GRAM_LDMOD, line, col ), m_what( what ), m_as( as ) {}
stmt_ldmod_t::~stmt_ldmod_t() {}

void stmt_ldmod_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, "Load Module at: %x\n", this );

	IO::tab_add( false );
	IO::print( true, "Name: %s\n", m_what->data.c_str() );
	IO::print( false, "As: %s\n", m_as == nullptr ? "(none)" : m_as->data.c_str() );
	IO::tab_rem( 2 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// import /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

stmt_import_t::stmt_import_t( const std::vector< tok_t * > & what, const tok_t * as,
			  const int line, const int col )
	: stmt_base_t( GRAM_IMPORT, line, col ), m_what( what ), m_as( as ) {}
stmt_import_t::~stmt_import_t() {}

void stmt_import_t::disp( const bool has_next ) const
{
	IO::tab_add( has_next );
	IO::print( has_next, "Import at: %x\n", this );

	IO::tab_add( false );
	IO::print( true, "Name: " );
	for( auto & what : m_what ) {
		fprintf( stdout, "%s.", what->data.c_str() );
	}
	fprintf( stdout, "\b \b\n" );
	IO::print( false, "As: %s\n", m_as == nullptr ? "(none)" : m_as->data.c_str() );
	IO::tab_rem( 2 );
}