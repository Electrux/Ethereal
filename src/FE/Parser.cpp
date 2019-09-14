/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include <algorithm>

#include "Parser/Internal.hpp"
#include "Parser.hpp"
#include "Ethereal.hpp"

bool check_type_correct_parent( const int tok_type, const GrammarTypes parent );

// NOTE: all the parse_* functions return at the point of their respective ending tokens
parse_tree_t * parse( src_t & src, parse_helper_t * pre_ph, std::vector< GrammarTypes > parent_stack, const int end )
{
	parse_tree_t * ptree = new parse_tree_t();
	parse_helper_t * ph = pre_ph != nullptr ? pre_ph : new parse_helper_t( src.toks );

	while( ph->peak()->type != TOK_INVALID && ( end == -1 || ph->tok_ctr() < end ) ) {
		if( !parent_stack.empty() && !check_type_correct_parent( ph->peak()->type, parent_stack.back() ) ) {
			PARSE_FAIL( "cannot have %s in %s's block",
				    TokStrs[ ph->peak()->type ],
				    GrammarTypeStrs[ parent_stack.back() ] );
			goto fail;
		}
		stmt_base_t * res = nullptr;
		if( ph->peak()->type == TOK_ENUM || ph->peak()->type == TOK_ENUM_MASK ) {
			res = parse_enum( src, ph );
		} else if( ph->peak()->type == TOK_LDMOD ) {
			res = parse_ldmod( src, ph );
		} else if( ph->peak()->type == TOK_IMPORT ) {
			res = parse_import( src, ph );
		} else if( ph->peak()->type == TOK_STRUCT ) {
			res = parse_struct( src, ph );
		} else if( ph->peak()->type == TOK_FN || ph->peak()->type == TOK_MFN ) {
			res = parse_func( src, ph );
		} else if( ph->peak()->type == TOK_IF ) {
			res = parse_if( src, ph, parent_stack );
		} else if( ph->peak()->type == TOK_FOR ) {
			if( ph->peak( 1 )->type == TOK_IDEN && ph->peak( 2 )->type == TOK_IN ) {
				res = parse_foreach( src, ph, parent_stack );
			} else {
				res = parse_for( src, ph, parent_stack );
			}
		} else if( ph->peak()->type == TOK_RETURN ) {
			if( std::find( parent_stack.begin(), parent_stack.end(), GRAM_FUNC ) == parent_stack.end() ) {
				PARSE_FAIL( "keyword 'return' can only be used inside a function" );
				goto fail;
			}
			res = parse_return( src, ph );
		} else if( ph->peak()->type == TOK_CONTINUE ) {
			// TODO: convert two find calls to a single
			if( std::find( parent_stack.begin(), parent_stack.end(), GRAM_FOR ) == parent_stack.end() &&
			    std::find( parent_stack.begin(), parent_stack.end(), GRAM_FOREACH ) == parent_stack.end() ) {
				PARSE_FAIL( "keyword 'continue' can only be used inside a loop" );
				goto fail;
			}
			res = new stmt_continue_t( ph->tok_ctr() );
			ph->next();
		} else if( ph->peak()->type == TOK_BREAK ) {
			if( std::find( parent_stack.begin(), parent_stack.end(), GRAM_FOR ) == parent_stack.end() &&
			    std::find( parent_stack.begin(), parent_stack.end(), GRAM_FOREACH ) == parent_stack.end() ) {
				PARSE_FAIL( "keyword 'break' can only be used inside a loop" );
				goto fail;
			}
			res = new stmt_break_t( ph->tok_ctr() );
			ph->next();
		} else if( ph->peak()->type == TOK_LBRACE ) {
			// simple block
			res = parse_block( src, ph, parent_stack );
		} else {
			// just expressions remain
			expr_res_t expr = parse_expr( src, ph );
			if( expr.res != 0 ) {
				PARSE_FAIL( "failed parsing expression" );
				goto fail;
			}
			res = expr.expr;
			if( res == nullptr ) {
				PARSE_FAIL( "encountered invalid token" );
				goto fail;
			}
		}

		if( res == nullptr ) goto fail;
		ptree->push_back( res );
		ph->next();
	}

	if( pre_ph == nullptr ) delete ph;
	return ptree;
fail:
	for( auto & base : * ptree ) {
		delete base;
	}
	delete ptree;
	if( pre_ph == nullptr ) delete ph;
	return nullptr;
}

bool check_type_correct_parent( const int tok_type, const GrammarTypes parent )
{
	//if( parent == GRAM_INVALID ) return true;
	if( ( tok_type == TOK_ENUM ||
	      tok_type == TOK_STRUCT ||
	      tok_type == TOK_FN ) &&
	     parent != GRAM_INVALID ) return false;
	return true;
}
