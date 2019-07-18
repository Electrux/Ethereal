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
#include "../FS.hpp"

#define _STRINGIZE(x) #x
#define STRINGIFY(x) _STRINGIZE(x)

stmt_import_t * parse_import( const src_t & src, parse_helper_t * ph )
{
	int tok_ctr = ph->tok_ctr();

	tok_t * what;
	tok_t * as = nullptr;

	NEXT_VALID2( TOK_IDEN, TOK_STR );
	what = ph->peak();

	if( ph->peak( 1 )->type == TOK_AS ) {
		ph->next();
		as = ph->next();
	}

	NEXT_VALID( TOK_COLS );
	return new stmt_import_t( what, as, tok_ctr );
}

int format_file_str( std::string & file );

bool stmt_import_t::bytecode( src_t & src ) const
{
	std::string file = m_what->data;
	int res = format_file_str( file );
	if( res != 0 ) {
		src_fail( src.code[ m_what->line - 1 ], m_what->line, m_what->col,
			  "could not find file '%s' for importing", file.c_str() );
		return false;
	}
	src.bcode.push_back( { m_tok_ctr, m_what->line, m_what->col, IC_PUSH, { OP_CONST, file } } );
	if( m_as != nullptr ) {
		src.bcode.push_back( { m_tok_ctr, m_as->line, m_as->col, IC_PUSH, { OP_CONST, m_as->data } } );
	}
	src.bcode.push_back( { m_tok_ctr, m_what->line, m_what->col, IC_IMPORT, { OP_INT, m_as == nullptr ? "1" : "2" } } );
	return true;
}

int format_file_str( std::string & file )
{
	if( file.front() != '~' && file.front() != '/' && file.front() != '.' ) {
		file = STRINGIFY( BUILD_PREFIX_DIR ) "/include/ethereal/" + file;
	} else if( file.front() == '~' ) {
		file.erase( file.begin() );
		std::string home = GetEnv( "HOME" );
		file.insert( file.begin(), home.begin(), home.end() );
	}
	file += ".et";
	return ( int )!fexists( file );
}