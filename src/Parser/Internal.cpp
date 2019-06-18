/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Internal.hpp"

int oper_prec( const tok_t * tok )
{
	const TokType type = tok->type;

	if( type < TOK_ASSN || type > TOK_RBRACK ) return -1;
	if( type == TOK_COMMA ) return 0;
	if( token_is_one_of_assign( tok ) ) return 1;
	if( type == TOK_QUEST || type == TOK_COL ) return 2;
	if( type == TOK_OR ) return 3;
	if( type == TOK_AND ) return 4;
	if( type == TOK_BOR ) return 5;
	if( type == TOK_BXOR ) return 6;
	if( type == TOK_BAND ) return 7;
	if( type == TOK_EQ || type == TOK_NE ) return 8;
	if( type == TOK_LT || type == TOK_LE || type == TOK_GT || type == TOK_GE ) return 9;
	if( type == TOK_LSHIFT || type == TOK_RSHIFT ) return 10;
	if( type == TOK_ADD || type == TOK_SUB ) return 11;
	if( type == TOK_MUL || type == TOK_DIV || type == TOK_MOD || type == TOK_POW ) return 12;
	if( type == TOK_UADD || type == TOK_USUB ) return 13;
	if( type == TOK_NOT || type == TOK_BNOT ) return 14;
	if( type == TOK_DOT ) return 16;
	if( type == TOK_LPAREN || type == TOK_RPAREN || type == TOK_LBRACK || type == TOK_RBRACK || type == TOK_LBRACE || type == TOK_RBRACE ) return 16;

	return -1;
}

OperAssoc oper_assoc( const tok_t * tok )
{
	const TokType type = tok->type;
	if( type < TOK_ASSN || type > TOK_RSHIFT_ASSN ) return LTR;
	if( token_is_one_of_assign( tok ) ) return RTL;
	if( type == TOK_QUEST || type == TOK_COL ) return RTL;
	if( type == TOK_NOT || type == TOK_BNOT ) return RTL;
	if( type == TOK_DOT ) return LTR;

	// for everything else, it is LTR
	return LTR;
}

int oper_arg_count( const tok_t * tok )
{
	const TokType type = tok->type;

	if( type < TOK_ASSN || type > TOK_RBRACK ) return -1;
	if( type == TOK_COMMA ) return 2;
	if( token_is_one_of_assign( tok ) ) return 2;
	if( type == TOK_QUEST || type == TOK_COL ) return 2;
	if( type == TOK_OR || type == TOK_AND || type == TOK_BOR || type == TOK_BXOR || type == TOK_BAND ) return 2;
	if( type == TOK_EQ || type == TOK_NE || type == TOK_LT || type == TOK_LE || type == TOK_GT || type == TOK_GE ) return 2;
	if( type == TOK_LSHIFT || type == TOK_RSHIFT ) return 2;
	if( type == TOK_ADD || type == TOK_SUB ) return 2;
	if( type == TOK_MUL || type == TOK_DIV || type == TOK_MOD || type == TOK_POW ) return 2;
	if( type == TOK_UADD || type == TOK_USUB ) return 1;
	if( type == TOK_NOT || type == TOK_BNOT ) return 1;
	if( type == TOK_DOT ) return 2;
	//
	if( type == TOK_LBRACK ) return 2;
	if( type == TOK_LPAREN || type == TOK_RPAREN || type == TOK_LBRACK || type == TOK_RBRACK || type == TOK_LBRACE || type == TOK_RBRACE ) return 0;

	return -1;
}

int find_next_of( parse_helper_t * ph, int & loc, const TokType type,
		  const TokType eq, const bool bypass_semicol )
{
	int ctr = 1;
	int equi = 1;
	while( ph->peak( ctr )->type != TOK_INVALID ) {
		if( ph->peak( ctr )->type == TOK_COLS && type != TOK_COLS && !bypass_semicol ) { loc = ctr; return -2; }
		if( ph->peak( ctr )->type == type ) {
			if( eq != TOK_INVALID ) {
				--equi;
				if( equi == 0 ) { loc = ph->tok_ctr() + ctr; return 0; };
			} else {
				loc = ph->tok_ctr() + ctr;
				return 0;
			}
		}
		if( ph->peak( ctr )->type == eq ) ++equi;
		++ctr;
	}
	loc = -1;
	return -1;
}