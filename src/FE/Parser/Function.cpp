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

stmt_func_t * parse_func( src_t & src, parse_helper_t * ph )
{
	int tok_ctr = ph->tok_ctr();

	bool is_member_func = false;
	if( ph->peak()->type == TOK_MFN ) is_member_func = true;

	std::vector< GrammarTypes > parents;

	expr_res_t mem_type = { 0, nullptr };
	if( is_member_func ) {
		NEXT_VALID( TOK_LT );
		int mem_type_end;
		int err = find_next_of( ph, mem_type_end, { TOK_GT } );
		if( err < 0 ) {
			if( err == -1 ) {
				PARSE_FAIL( "could not find the right angular brace for member function type" );
			} else if( err == -2 ) {
				PARSE_FAIL( "found end of statement (;) before ending of member function type" );
			}
			return nullptr;
		}
		ph->next();
		mem_type = parse_expr( src, ph, mem_type_end );
		if( mem_type.res != 0 ) return nullptr;
		ph->set_tok_ctr( mem_type_end );
	}

	const tok_t * name = nullptr;
	NEXT_VALID( TOK_IDEN );
	name = ph->peak();

	NEXT_VALID( TOK_LPAREN );

	expr_res_t arg_expr = { 0, nullptr };
	int arg_expr_end;
	int err;
	stmt_block_t * block = nullptr;

	NEXT_VALID2( TOK_RPAREN, TOK_IDEN );

	if( ph->peak()->type == TOK_RPAREN ) goto end_args;
	err = find_next_of( ph, arg_expr_end, { TOK_RPAREN }, TOK_LPAREN );
	if( err < 0 ) {
		if( err == -1 ) {
			PARSE_FAIL( "could not find the right parenthesis for function '%s'",
				    name->data.c_str() );
		} else if( err == -2 ) {
			PARSE_FAIL( "found beginning of block (left braces) before the ')' for arguments of function '%s'",
				    name->data.c_str() );
		}
		goto fail;
	}
	arg_expr = parse_expr( src, ph, arg_expr_end );
	if( arg_expr.res != 0 ) goto fail;
	ph->set_tok_ctr( arg_expr_end );
end_args:
	NEXT_VALID_FAIL( TOK_LBRACE );
	parents.push_back( GRAM_FUNC );
	block = parse_block( src, ph, parents );
	parents.pop_back();
	if( block == nullptr ) goto fail;
	return new stmt_func_t( new stmt_simple_t( SIMPLE_TOKEN, name, tok_ctr + 1 ),
				arg_expr.expr, block, mem_type.expr, tok_ctr );
fail:
	if( arg_expr.expr ) delete arg_expr.expr;
	if( block ) delete block;
	if( mem_type.expr ) delete mem_type.expr;
	return nullptr;
}

bool stmt_func_t::bytecode( const toks_t & toks, bytecode_t & bcode ) const
{
	return true;
}