/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#include "Parser/Internal.hpp"

bool check_type_correct_parent( const int tok_type, const GrammarTypes parent );

// NOTE: all the parse_* functions return at the point of their respective ending tokens
std::vector< stmt_base_t * > * parse( src_t & src, const GrammarTypes parent )
{
	std::vector< stmt_base_t * > * ptree = new std::vector< stmt_base_t * >();
	parse_helper_t ph( src.toks );

	while( ph.peak()->type != TOK_INVALID ) {
		if( !check_type_correct_parent( ph.peak()->type, parent ) ) {
			PARSE_FAIL( "cannot have %s in %s's block",
				    TokStrs[ ph.peak()->type ],
				    GrammarTypeStrs[ parent ] );
			goto fail;
		}
		stmt_base_t * res = nullptr;
		if( ph.peak()->type == TOK_ENUM ) {
			res = parse_enum( src, ph );
		} else if( ph.peak()->type == TOK_LDMOD ) {
			res = parse_ldmod( src, ph );
		} else if( ph.peak()->type == TOK_IMPORT ) {
			res = parse_import( src, ph );
		} else if( ph.peak()->type == TOK_STRUCT ) {
			// parse_struct_def( ph );
		} else if( ph.peak()->type == TOK_FN ) {
			// parse_fn_def( ph );
		} else if( ph.peak()->type == TOK_MFN ) {
			// parse_mfn_def( ph );
		} else if( ph.peak()->type == TOK_IF ) {
			// parse_if( ph );
		} else if( ph.peak()->type == TOK_FOR ) {
			// parse_for( ph );
		}

		// just expressions remain
		res = parse_expr( src, ph );

		if( res == nullptr ) goto fail;
		ptree->push_back( res );
		ph.next();
	}

	return ptree;
fail:
	for( auto & base : * ptree ) {
		delete base;
	}
	delete ptree;
	return nullptr;
}

bool check_type_correct_parent( const int tok_type, const GrammarTypes parent )
{
	//if( parent == GRAM_INVALID ) return true;

	if( parent == GRAM_ENUM ) {
	}
	return true;
}
