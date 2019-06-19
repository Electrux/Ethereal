/*
	Copyright (c) 2019, Electrux
	All rights reserved.
	Using the BSD 3-Clause license for the project,
	main LICENSE file resides in project's root directory.
	Please read that file and understand the license terms
	before using or altering the project.
*/

#ifndef PARSER_INTERNAL_HPP
#define PARSER_INTERNAL_HPP

#include "Stmts.hpp"

#define NEXT_VALID( tok )							\
do {										\
	if( ph->peak( 1 )->type == TOK_INVALID ) {				\
		PARSE_FAIL( "expected token '%s', but found <EOF>",		\
			    TokStrs[ tok ] );					\
		return nullptr;							\
	}									\
	ph->next();								\
	if( ph->peak()->type != tok ) {						\
		PARSE_FAIL( "expected token '%s', but found '%s'",		\
			    TokStrs[ tok ], TokStrs[ ph->peak()->type ] );	\
		return nullptr;							\
	}									\
} while( 0 )

#define NEXT_VALID2( tok1, tok2 )								\
do {												\
	if( ph->peak( 1 )->type == TOK_INVALID ) {						\
		PARSE_FAIL( "expected token '%s' or '%s', but found <EOF>",			\
			    TokStrs[ tok1 ], TokStrs[ tok2 ] );					\
		return nullptr;									\
	}											\
	ph->next();										\
	if( ph->peak()->type != tok1 && ph->peak()->type != tok2 ) {				\
		PARSE_FAIL( "expected token '%s' or '%s', but found '%s'",			\
			    TokStrs[ tok1 ], TokStrs[ tok2 ], TokStrs[ ph->peak()->type ] );	\
		return nullptr;									\
	}											\
} while( 0 )

#define NEXT_VALID3( tok1, tok2, tok3 )								\
do {												\
	if( ph->peak( 1 )->type == TOK_INVALID ) {						\
		PARSE_FAIL( "expected token '%s' or '%s' or '%s', but found <EOF>",		\
			    TokStrs[ tok1 ], TokStrs[ tok2 ], TokStrs[ tok3 ] );		\
		return nullptr;									\
	}											\
	ph->next();										\
	if( ph->peak()->type != tok1 && ph->peak()->type != tok2 && ph->peak()->type != tok3 ) {\
		PARSE_FAIL( "expected token '%s' or '%s' or '%s', but found '%s'",		\
			    TokStrs[ tok1 ], TokStrs[ tok2 ],					\
			    TokStrs[ tok3 ], TokStrs[ ph->peak()->type ] );			\
		return nullptr;									\
	}											\
} while( 0 )

#define NEXT_VALID_FAIL( tok )							\
do {										\
	if( ph->peak( 1 )->type == TOK_INVALID ) {				\
		PARSE_FAIL( "expected token '%s', but found <EOF>",		\
			    TokStrs[ tok ] );					\
		goto fail;							\
	}									\
	ph->next();								\
	if( ph->peak()->type != tok ) {						\
		PARSE_FAIL( "expected token '%s', but found '%s'",		\
			    TokStrs[ tok ], TokStrs[ ph->peak()->type ] );	\
		goto fail;							\
	}									\
} while( 0 )

#define NEXT_VALID2_FAIL( tok1, tok2 )								\
do {												\
	if( ph->peak( 1 )->type == TOK_INVALID ) {						\
		PARSE_FAIL( "expected token '%s' or '%s', but found <EOF>",			\
			    TokStrs[ tok1 ], TokStrs[ tok2 ] );					\
		goto fail;									\
	}											\
	ph->next();										\
	if( ph->peak()->type != tok1 && ph->peak()->type != tok2 ) {				\
		PARSE_FAIL( "expected token '%s' or '%s', but found '%s'",			\
			    TokStrs[ tok1 ], TokStrs[ tok2 ], TokStrs[ ph->peak()->type ] );	\
		goto fail;									\
	}											\
} while( 0 )

#define NEXT_VALID3_FAIL( tok1, tok2, tok3 )							\
do {												\
	if( ph->peak( 1 )->type == TOK_INVALID ) {						\
		PARSE_FAIL( "expected token '%s' or '%s' or '%s', but found <EOF>",		\
			    TokStrs[ tok1 ], TokStrs[ tok2 ], TokStrs[ tok3 ] );		\
		goto fail;									\
	}											\
	ph->next();										\
	if( ph->peak()->type != tok1 && ph->peak()->type != tok2 && ph->peak()->type != tok3 ) {\
		PARSE_FAIL( "expected token '%s' or '%s' or '%s', but found '%s'",		\
			    TokStrs[ tok1 ], TokStrs[ tok2 ],					\
			    TokStrs[ tok3 ], TokStrs[ ph->peak()->type ] );			\
		goto fail;									\
	}											\
} while( 0 )

stmt_enum_t * parse_enum( const src_t & src, parse_helper_t * ph );
stmt_ldmod_t * parse_ldmod( const src_t & src, parse_helper_t * ph );
stmt_import_t * parse_import( const src_t & src, parse_helper_t * ph );
stmt_expr_t * parse_expr( const src_t & src, parse_helper_t * ph,
			  const int end = -1, const ExprType type = EXPR_BASIC, const bool is_top = true );
stmt_struct_t * parse_struct( const src_t & src, parse_helper_t * ph );
stmt_block_t * parse_block( src_t & src, parse_helper_t * ph, GrammarTypes parent );
stmt_func_t * parse_func( src_t & src, parse_helper_t * ph );

// precedence of a operator (lex type) in ascending order
int oper_prec( const tok_t * tok );

enum OperAssoc
{
	LTR,
	RTL,
};

// LTR => left->right assoc
// RTL => right->left assoc
OperAssoc oper_assoc( const tok_t * tok );

int oper_arg_count( const tok_t * tok );

int find_next_of( parse_helper_t * ph, int & loc, const std::vector< TokType > & types,
		  const TokType eq = TOK_INVALID, const bool bypass_breaker = false,
		  const TokType breaker = TOK_COLS );

#endif // PARSER_INTERNAL_HPP