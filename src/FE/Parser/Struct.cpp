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

stmt_struct_t * parse_struct( const src_t & src, parse_helper_t * ph )
{
	int tok_ctr = ph->tok_ctr();
	tok_t * name = nullptr;
	NEXT_VALID( TOK_IDEN );
	name = ph->peak();

	NEXT_VALID( TOK_LBRACE );

	tok_t * fname = nullptr;
	int fname_tok_ctr;
	expr_res_t fdef_val = { 0, nullptr };
	std::vector< struct_field_t > fields;
	int semicol_loc = -1;
	int err = 0;
field_begin:
	NEXT_VALID_FAIL( TOK_IDEN );
	fname = ph->peak();
	fname_tok_ctr = ph->tok_ctr();
	NEXT_VALID_FAIL( TOK_ASSN );
	ph->next();
	err = find_next_of( ph, semicol_loc, { TOK_COLS } );
	if( err < 0 ) {
		if( err == -1 ) {
			PARSE_FAIL( "could not find the semicolon for field value expression" );
		} else if( err == -2 ) {
			PARSE_FAIL( "found end of statement (semicolon) before the equivalent ending brace for map" );
		}
		goto fail;
	}
	fdef_val = parse_expr( src, ph, semicol_loc, false );
	if( fdef_val.res != 0 ) goto fail;
	fields.push_back( { new stmt_simple_t( SIMPLE_TOKEN, fname, fname_tok_ctr ), fdef_val.expr } );
	fname = nullptr;
	fdef_val = { 0, nullptr };
	ph->set_tok_ctr( semicol_loc );
	if( ph->peak( 1 )->type != TOK_RBRACE ) {
		if( ph->peak( 1 )->type != TOK_IDEN ) {
			ph->next();
			PARSE_FAIL( "expected end of struct '}' or another struct field 'IDEN', found '%s'",
				    ph->peak()->data.c_str() );
			goto fail;
		}
		goto field_begin;
	}
	ph->next();
	// now at RBRACE
	return new stmt_struct_t( new stmt_simple_t( SIMPLE_TOKEN, name, tok_ctr + 1 ), fields, tok_ctr );
fail:
	for( auto & f : fields ) {
		delete f.name;
		delete f.def_val;
	}
	return nullptr;
}

bool stmt_struct_t::bytecode( src_t & src ) const
{
	for( auto & f : m_fields ) {
		f.def_val->bytecode( src );
		src.bcode.push_back( { m_tok_ctr, f.name->m_val->line, f.name->m_val->col,
				       IC_PUSH, { OP_CONST, f.name->m_val->data } } );
	}
	src.bcode.push_back( { m_tok_ctr, m_name->m_val->line, m_name->m_val->col,
			       IC_PUSH, { OP_CONST, m_name->m_val->data } } );
	src.bcode.push_back( { m_tok_ctr, m_name->m_val->line, m_name->m_val->col,
			       IC_BUILD_STRUCT, { OP_INT, std::to_string( m_fields.size() ) } } );
	return true;
}