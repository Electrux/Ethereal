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
	tok_t * as = nullptr;

	NEXT_VALID2( TOK_IDEN, TOK_STR );
	what = ph->peak();

	if( ph->peak( 1 )->type == TOK_AS ) {
		ph->next();
		NEXT_VALID( TOK_IDEN );
		as = ph->peak();
	}

	NEXT_VALID( TOK_COLS );
	return new stmt_ldmod_t( what, as, tok_ctr );
}