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

stmt_import_t * parse_import( const src_t & src, parse_helper_t * ph )
{
	int tok_ctr = ph->tok_ctr();

	std::vector< tok_t * > what;
	tok_t * as = nullptr;

beg_what:
	NEXT_VALID2( TOK_IDEN, TOK_STR );
	what.push_back( ph->peak() );

	if( ph->peak( 1 )->type == TOK_AS ) {
		ph->next();
		as = ph->next();
	} else if( ph->peak( 1 )->type == TOK_DOT ) {
		ph->next();
		goto beg_what;
	}

	NEXT_VALID( TOK_COLS );
	return new stmt_import_t( what, as, tok_ctr );
}